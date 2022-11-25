#include <cstring>
#include <iostream>

#include "cpu.hpp"
#include "io.hpp"

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
void VM::load_program(string data, int start_addr)
{
    io->write("Program = " + data);

    SI = 1;
    MOS(start_addr, data);
}

/**
 * @brief Executes the user program loaded in the main memory
 *
 */
void VM::start_execution()
{
    while (!terminate)
    {
        // Fetch the instruction in instruction register
        for (int i = 0; i < 4; i++)
        {
            ir[i] = mem[ic][i];
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
            string data = IOHandler().read_data();
            MOS(operand, data);
        }
        else if (opcode == "PD")
        {
            SI = 2;
            MOS(operand, "");
        }
        else if (opcode == "LR")
            load_register(operand);

        else if (opcode == "SR")
            store_register(operand);

        else if (opcode == "CR")
            compare_register(operand);

        else if (opcode == "BT")
            branch_on_true(operand);

        if (ir[0] == 'H')
        {
            SI = 3;
            MOS(operand, "");
        }
    }
}

/**
 * @brief Change the machine state from slave to master mode
 * SI Modes
 * Mode 0 : Return to Slave mode. This is the default execution mode of the user programs
 * Mode 1 : Get Data from Card and write in Main Memory
 * Mode 2 : Write Data to Card from Main Memory
 * Mode 3 : Halt the execution of the Program
 *
 * @param data The Data to be written (Required only in case of SI Mode 1)
 * @param mem_loc The memory Location of the main memory
 *
 */
void VM::MOS(int mem_loc, string data)
{
    string d;

    int sub = 0;
    int lim = blocks_calculator(data.length()) / 4;

    switch (SI)
    {
    case 1: // Case for GD

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
        terminate = true;
        break;

    default:
        break;
    }
    SI = 0;
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

    // 7. Resets the termination condition set by previous program
    terminate = false;

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