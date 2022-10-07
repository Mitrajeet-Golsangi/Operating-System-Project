#include <iostream>
#include <cpu.hpp>
#include <cstring>
#include <io.hpp>

using namespace std;

/**
 * @brief Load the Program into the main memory of the Virtual Machine
 *
 * @param data The Program Card from the Job
 */
void VM::load_program(string data, int start_addr)
{
    SI = 1;
    cout << "Program = " << data << endl;
    MOS(start_addr, data);
}

/**
 * @brief Executes the user program loaded in the main memory
 *
 */
void VM::start_execution()
{
    // Make Instruction counter zero
    ic = 0;

    // Fetch the instruction in instruction register
    for (int i = 0; i < 4; i++)
    {
        ir[i] = mem[ic][i];
    }

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
void VM::MOS(int mem_loc, string data = NULL)
{
    switch (SI)
    {
    case 1: // Case for GD
        ir[4] = 0;
        for (int i = 0; i <= data.length(); i = i + 4)
        {
            string d = data.substr(i, 4);

            for (int j = 0; j < d.length(); j++)
            {
                mem[mem_loc][j] = d[j];
            }
            mem_loc++;
        }
        break;
    case 2: // Case for PD
    case 3: // Case for H
    default:
        break;
    }
    SI = 0;
}

/**
 * @brief Clean the entire machine and make it ready for a new job
 * This method :
 * 1. Resets the Main memory
 * 2. Cleans the Instruction Register
 * 3. Sets the toggle register to false
 * 4. Sets the Instruction Counter to zero
 * 5. Resets the 40 byte buffer
 *
 */
void VM::clean()
{
    // 1. Reset the Main memory
    memset(mem, 0, sizeof(mem));

    // 2. Cleans the Instruction Register
    memset(ir, 0, sizeof(ir));

    // 3. Sets the toggle register to false
    c = false;

    // 4. Sets the Instruction Counter to zero
    ic = 0;

    // 5. Resets the 40 byte buffer
    IOHandler().reset_buffer();
}