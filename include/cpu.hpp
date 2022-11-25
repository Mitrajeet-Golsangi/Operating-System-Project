#ifndef CPU_HPP
#define CPU_HPP

#include "io.hpp"
#include <iostream>

class VM
{
    char r[4];              // General Purpose register
    char ir[4];             // Instruction Register
    int ic = 0;             // Instruction Counter
    bool c;                 // Toggle Register
    char mem[100][4] = {0}; // Memory of 100 words
    int SI = 0;             // Service Interrupt
    bool terminate = false; // termination flag

    IOHandler *io; // io card instance

public:
    // constructor
    VM(IOHandler *);

    void load_program(std::string, int);

    void start_execution();

    void MOS(int, std::string);

    void clean();

    void load_register(int);
    void store_register(int);
    void compare_register(int);
    void branch_on_true(int);
};

#endif