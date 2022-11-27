#ifndef CPU_HPP
#define CPU_HPP

#include "io.hpp"
#include "pcb.hpp"
#include <iostream>

class VM
{
    char r[4];              // General Purpose register
    char ir[4];             // Instruction Register
    bool c;                 // Toggle Register
    bool terminate = false; // termination flag
    int ptr;                // Page Table register

    int ic = 0;             // Instruction Counter

    char mem[300][4];       // Memory of 300 words

    int SI = 0; // Service Interrupt
    int PI = 0; // Program Error detection interrupt
    int TI = 0; // Time Limit Exceeded error

    int em = 0;  // Default Error Message
    int em2 = 0; // Additional Error Message

    IOHandler *io; // io card instance

public:
    // constructor
    VM(IOHandler *);

    void load_program(std::string, int);

    void start_execution(PCB);

    void MOS(int, std::string);

    void clean();

    // Commands
    void load_register(int);
    void store_register(int);
    void compare_register(int);
    void branch_on_true(int);

    // Paging functions
    void initialize_page_table();
    int address_map(int);
    int allocate(int);
        
    // Error functions
    void display_error();

    // Getter Setters
    int get_ptr() { return ptr; }

    bool get_terminate() { return terminate; }
    void set_terminate(bool terminate) { this->terminate = terminate; }

    void set_em(int em) { this->em = em; }
};

#endif