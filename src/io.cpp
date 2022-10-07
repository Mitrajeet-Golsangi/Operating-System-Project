#include <iostream>
#include <fstream>
#include <io.hpp>
#include <cpu.hpp>
#include <cstring>

using namespace std;

// Keep Track of the current line in io device => Useful for reading the data from data cards
int line_count = 0;

/**
 * @brief Read the input from the provided Cards and perform required actions
 * Actions:
 * 1. Detect when a new job starts
 * 2. Detect when the Data card starts
 * 3. Detect when the Job Ends
 * 4. Detect when the Program card starts
 *
 */
void IOHandler::read_card()
{

    bool data_card = false;
    VM cpu = VM();

    ifstream input_file("../IO/inp.txt");

    while (input_file >> buffer)
    {
        if (buffer.substr(0, 4) == "$AMJ")
        {
            cout << "Started Job " << buffer.substr(4, 4) << endl;
            cpu.clean();
        }
        else if (buffer.substr(0, 4) == "$DTA")
        {
            cout << "Data Card Encountered ! Starting User Program Execution" << endl;
            data_card = true;
            line_count++;
            cpu.start_execution();
        }
        else if (buffer.substr(0, 4) == "$END")
        {
            cout << "Ended Job " << buffer.substr(4, 4) << endl;
            data_card = false;
        }
        else if (data_card)
        {
            cout << "Data" << endl;
        }
        else
        {
            cout << "Loading Program to Main Memory ..." << endl;
            cpu.load_program(buffer, count);
            count = buffer.length() + count;
        }
        line_count++;
    }

    input_file.close();
}

string IOHandler::read_data()
{
    ifstream input_file("../IO/inp.txt");
    
    for (int i = 0; i < line_count + 1 && getline(input_file, buffer); i++)
    {
        if (i == line_count)
        {
            line_count++;
            return buffer;
        }
    }
    input_file.close();
}

/**
 * @brief Reset the buffer in order to start a new job
 *
 */
void IOHandler::reset_buffer()
{
    buffer = "";
    count = 0;
}