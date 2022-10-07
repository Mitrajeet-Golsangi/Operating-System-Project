#include <iostream>

class VM
{
    char r[4];        // General Purpose register
    char ir[4];       // Instruction Register
    int ic = 0;        // Instruction Counter
    bool c;           // Toggle Register
    char mem[100][4] = {0}; // Memory of 100 words
    int SI = 0;       // Service Interrupt

public:
    void load_program(std::string, int);

    void start_execution();

    void MOS(int, std::string);

    void clean();
};