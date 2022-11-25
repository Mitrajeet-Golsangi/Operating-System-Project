#include <cstring>
#include <fstream>
#include <iostream>

#include "cpu.hpp"
#include "io.hpp"

using namespace std;

// Keep Track of the current line in io device => Useful for reading the data from data cards
int line_count = 0;

/**
 * @brief Constructor for IOHandler class which reads the input and output files using initializer list
 *
 */
IOHandler::IOHandler() : input_file("../IO/inp.txt"), output_file("../IO/out.txt") {}

/**
 * @brief Destructor for IOHandler which closes the input and output files
 *
 */
IOHandler::~IOHandler()
{
    input_file.close();
    output_file.close();
}

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
    VM cpu = VM(this);
    bool program_card = true;
    const char *log;

    while (input_file >> buffer)
    {
        if (buffer.substr(0, 4) == "$AMJ")
        {
            output_file << "Started Job " << buffer.substr(4, 4) << "\n";
            cpu.clean();
        }
        else if (buffer.substr(0, 4) == "$DTA")
        {
            output_file << "Data Card Encountered ! Starting User Program Execution"
                        << "\n";
            line_count++;
            cpu.start_execution();
            program_card = false;
        }
        else if (buffer.substr(0, 4) == "$END")
        {
            output_file << "Ended Job " << buffer.substr(4, 4) << "\n\n";
        }
        else if (program_card)
        {
            output_file << "Loading Program to Main Memory ..."
                        << "\n";
            cpu.load_program(buffer, count);
            count = buffer.length() + count;
        }
        line_count++;
    }
}

/**
 * @brief Read a specific data line
 *
 * @return string : data read from the input card
 */
string IOHandler::read_data()
{
    string data;

    for (int i = 1; i != input_file.eof() && getline(input_file, buffer); i++)
    {
        if (i == line_count + 1)
        {
            data += buffer;
            line_count++;
            return data;
        }
    }
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

/**
 * @brief Write data to the output file
 *
 * @param data String data the has to be written
 */
void IOHandler::write(string data)
{
    for (size_t i = 0; i < data.length(); i++)
    {
        if (data[i] != '\000')
            output_file << data[i];
        else
            output_file << '*';
    }
    output_file << '\n';
}