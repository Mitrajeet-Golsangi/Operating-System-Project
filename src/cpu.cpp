#include <cstring>
#include <iostream>
#include <random>

#include "cpu.hpp"
#include "io.hpp"
#include "pcb.hpp"

using namespace std;

/**
 * @brief Calculate the size of blocks needed for GD and PD operations
 *
 * @param length length of the data to be loaded in memory
 * @return size of block needed to store data
 */
int blocks_calculator(int length)
{
    for (int i = 40; i <= 100; i += 40)
    {
        if (length <= i)
            return i;
    }
    return 40;
}

/**
 * @brief Generate a random block from 0 - 29
 *
 * @return Randomly generated block
 */
int get_block()
{
    random_device rd;                        // obtain a random number from hardware
    mt19937 gen(rd());                       // seed the generator using current time
    uniform_int_distribution<> distr(0, 29); // define the range

    return distr(gen);
}

/**
 * @brief Constructor for VM class
 *
 * @param io instance of the IOHandler class
 */
VM::VM(IOHandler *io)
{
    this->io = io;
}

/**
 * @brief Load the Program into the main memory of the Virtual Machine
 *
 * @param data The Program Card from the Job
 */
void VM::load_program(string data, int logical_addr)
{
    io->write("Program = " + data);

    // allocate actual address for program card
    allocate(logical_addr);

    SI = 1;
    MOS(logical_addr, data);
}

/**
 * @brief Executes the user program loaded in the main memory
 *
 */
void VM::start_execution(PCB pcb)
{

    while (!terminate)
    {
        // Data storage for reading the data after data card
        string data;

        // Get the actual location from the main memory
        int loc = address_map(ic);

        // Fetch the instruction in instruction register
        for (int i = 0; i < 4; i++)
        {
            ir[i] = mem[loc][i];
        }

        // increment instruction counter
        ic++;

        // Decode the instruction
        string opcode{ir[0], ir[1]};
        // convert the character to a integer address
        int operand = (ir[2] - 48) * 10 + (ir[3] - 48);

        if (opcode == "GD")
        {
            SI = 1;
            data = IOHandler().read_data();
            MOS(operand, data);
        }
        else if (opcode == "PD")
        {
            // increment the line limit counter
            pcb.increment_llc();

            // Terminate program if TLL is exceeded
            if (pcb.llc > pcb.tll)
            {
                em = 2;
                terminate = true;
            }
            else
            {
                SI = 2;
                MOS(operand, data);
            }
        }
        else if (opcode == "LR")
            load_register(operand);

        else if (opcode == "SR")
            store_register(operand);

        else if (opcode == "CR")
            compare_register(operand);

        else if (opcode == "BT")
            branch_on_true(operand);
        else
        {
            if (ir[0] == 'H')
            {
                SI = 3;
                MOS(operand, data);
            }
            else
            {
                em = 1;
                MOS(operand, data);
                terminate = true;
            }
        }

        // increment the time limit counter
        pcb.increment_ttc();
        if (pcb.ttc > pcb.ttl)
        {
            em = 3;
            terminate = true;
        }
    }

    // Checking for error messages and displaying them
    display_error();
}

/**
 * @brief Change the machine state from slave to master mode
 * SI Modes
 * Mode 0 : Return to Slave mode. This is the default execution mode of the user programs
 * Mode 1 : Get Data from Card and write in Main Memory
 * Mode 2 : Write Data to Card from Main Memory
 * Mode 3 : Halt the execution of the Program
 *
 * TI Modes:
 * Mode 0 : Time Limit not exceeded, thus proceed as normal
 * Mode 2 : Time limit exceeded, thus halt processes
 *
 * PI Modes:
 * Mode 1 : Operation Error
 * Mode 2 : Operand Error
 * Mode 3 : Page Fault
 *
 * @param data The Data to be written (Required only in case of SI Mode 1)
 * @param mem_loc The memory Location of the main memory
 *
 */
void VM::MOS(int mem_loc, string data)
{
    // Decode the instruction
    string opcode{ir[0], ir[1]};

    string d;

    long long unsigned int sub = 0;
    int lim = blocks_calculator(data.length()) / 4;

    // convert the virtual memory location to physical memory location
    int va = mem_loc;
    mem_loc = address_map(va);

    // Allocate consecutive blocks in memory for the loading the data greater than 1 block
    if (lim > 10)
    {
        for (int i = 1; i < lim / 10; i++)
        {
            mem[ptr + i][0] = mem_loc / 10 + i;
        }
    }
    // Cases for normal execution if Time limit is not exceeded
    if (TI == 0)
    {
        // Process Interrupt Cases
        switch (PI)
        {
        case 1:
            em = 4;
            terminate = true;
            break;
        case 2:
            em = 5;
            terminate = true;
            break;
        case 3:

            // Implement valid page fault execution
            if (opcode == "GD" || opcode == "SR")
            {
                cout << "Valid Page Fault encountered for " << opcode << va << endl;
                mem_loc = allocate(va) * 10;
                PI = 0;
            }
            // If instruction is not GD or SR invoke invalid page fault
            else
            {
                em = 6;
                terminate = true;
            }
            break;

        default:
            break;
        }

        if (!terminate)
        {
            // Service Interrupt Cases
            switch (SI)
            {
            case 1: // Case for GD and program loading

                for (int i = mem_loc; i < (mem_loc + lim); i++)
                {

                    if (sub <= data.length())
                        d = data.substr(sub, 4);
                    else
                        d = "****";

                    for (int j = 0; j < 4; j++)
                    {
                        mem[i][j] = d[j];
                    };
                    sub += 4;
                }
                break;
            case 2: // Case for PD
                for (int i = mem_loc; i < (mem_loc + lim); i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        d += mem[i][j];
                    }
                }

                io->write(d);
                break;
            case 3:
                em = 0;
                terminate = true;
                break;

            default:
                break;
            }
            SI = 0;
        }
    }
    // Cases if Time limit is exceeded
    else
    {
        // Process Interrupt Cases
        switch (PI)
        {
        case 1:
            em = 3;
            em2 = 4;
            terminate = true;
            break;
        case 2:
            em = 3;
            em2 = 5;
            terminate = true;
            break;
        case 3:
            em = 3;
            terminate = true;
            break;

        default:
            break;
        }

        if (!terminate)
        {
            // Service Interrupt Cases
            switch (SI)
            {
            case 1: // TERMINATE
                em = 3;
                terminate = true;
                break;
            case 2: // WRITE, THEN TERMINATE
                for (int i = mem_loc; i < (mem_loc + lim); i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        d += mem[i][j];
                    }
                }
                io->write(d);

                em = 3;
                terminate = true;
                break;
            case 3: // TERMINATE
                em = 0;
                terminate = true;
                break;

            default:
                break;
            }
        }
    }
}

/**
 * @brief Clean the entire machine and make it ready for a new job
 *
 */
void VM::clean()
{
    // 1. Cleans the General Purpose Register
    memset(r, 0, sizeof(r));

    // 2. Cleans the Instruction Register
    memset(ir, 0, sizeof(ir));

    // 3. Sets the Instruction Counter to zero
    ic = 0;

    // 4. Sets the toggle register to false
    c = false;

    // 5. Reset the Main memory
    memset(mem, 0, sizeof(mem));

    // 6. Reset the SI interrupt
    SI = 0;

    // 7. Reset the PI interrupt
    PI = 0;

    // 8. Reset the TI interrupt
    TI = 0;

    // 9. Resets the termination condition set by previous program
    terminate = false;

    // 10. Reset the error message
    em = 0;
    em2 = 0;

    // 6. Resets the 40 byte buffer
    io->reset_buffer();
}

/**
 * @brief Load data from memory location to general purpose R register
 *
 * @param mem_loc The memory Location in the main memory
 *
 */
void VM::load_register(int mem_loc)
{
    mem_loc = address_map(mem_loc);
    for (int i = 0; i < 4; i++)
    {
        r[i] = mem[mem_loc][i];
    }
}

/**
 * @brief Store data from memory general purpose R register to memory location
 *
 * @param mem_loc The memory Location in the main memory
 *
 */
void VM::store_register(int mem_loc)
{
    mem_loc = address_map(mem_loc);
    for (int i = 0; i < 4; i++)
    {
        mem[mem_loc][i] = r[i];
    }
}

/**
 * @brief Compare the values in R register with the values in memory location and set the toggle register
 *
 * @param mem_loc The memory Location in the main memory
 *
 */
void VM::compare_register(int mem_loc)
{
    mem_loc = address_map(mem_loc);
    string a, b;
    for (int i = 0; i < 4; i++)
    {
        a += mem[mem_loc][i];
        b += r[i];
    }
    c = a == b;
}

/**
 * @brief Branch to given memory location if the toggle register is true
 *
 * @param mem_loc The memory Location in the main memory
 */
void VM::branch_on_true(int mem_loc)
{
    if (c)
        ic = mem_loc;
}

/**
 * @brief Display the required error message from the error codes
 *
 */
void VM::display_error()
{
    switch (em)
    {
    case 0:
        io->write("------------------------- No Error -------------------------");
        break;
    case 1:
        io->write("------------------------ Out of Data -----------------------");
        break;
    case 2:
        io->write("-------------------- Line Limit Exceeded -------------------");
        break;
    case 3:
        io->write("-------------------- Time Limit Exceeded -------------------");
        break;
    case 4:
        io->write("------------------- Operation Code Error -------------------");
        break;
    case 5:
        io->write("---------------------- Operand Error -----------------------");
        break;
    case 6:
        io->write("-------------------- Invalid Page Fault --------------------");
        break;
    default:
        break;
    }
    if (em2)
        switch (em2)
        {
        case 0:
            io->write("------------------------- No Error -------------------------");
            break;
        case 1:
            io->write("------------------------ Out of Data -----------------------");
            break;
        case 2:
            io->write("-------------------- Line Limit Exceeded -------------------");
            break;
        case 3:
            io->write("-------------------- Time Limit Exceeded -------------------");
            break;
        case 4:
            io->write("------------------- Operation Code Error -------------------");
            break;
        case 5:
            io->write("---------------------- Operand Error -----------------------");
            break;
        case 6:
            io->write("-------------------- Invalid Page Fault --------------------");
            break;
        default:
            break;
        }
}

/**
 * @brief Generate a random number from 0-29 and allocate that block with 'P' for page table
 * Also set the PTR to point at the starting address of the page table
 *
 */
void VM::initialize_page_table()
{
    int rnd = get_block();
    ptr = rnd * 10;

    for (int i = ptr; i < ptr + 10; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            mem[i][j] = 'P';
        }
    }
}

/**
 * @brief Get the real address from the virtual address
 * ! Note : the function raises a Page Fault or an Operand Error if there exists one
 *
 * @param va Virtual Address
 * @return Real Address
 */
int VM::address_map(int va)
{
    int pte = ptr + va / 10;

    if (mem[pte][0] == 'P')
    {
        PI = 3;
        return -1;
    }
    if (pte > 300)
    {
        PI = 1;
        return -1;
    }
    return mem[pte][0] * 10 + va % 10;
}

/**
 * @brief Allocate the block of logical memory a real memory block
 *
 * @param va Virtual Address for which memory needs to be allocated
 * @return the block of allocated real memory address
 */
int VM::allocate(int va)
{
    // Get a frame for loading program
    int block = get_block();

    // Counter in order to prevent infinite loop
    int cnt = 0;

    // check if the generated block already exists in the system if it does regenerate a block
    for (int i = 0; i < 10; i++)
    {
        if (block == mem[ptr][0])
        {
            block = get_block();
            cnt++;
            i = 0;
        }
        if (cnt == 100)
        {
            // Generate a invalid page fault error
            PI = 3;
            break;
        }
    }

    // store the block number in page table
    int pte = ptr + va / 10;

    // here block digit is stored as its ASCII equivalent in mem
    mem[pte][0] = block;
    return block;
}