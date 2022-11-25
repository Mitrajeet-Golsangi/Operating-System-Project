#ifndef IO_HPP
#define IO_HPP

#include <iostream>
#include <memory>
#include <fstream>

class IOHandler
{
    //Buffer
    std::string buffer;
    

    // Counter for loading the program cards
    int count = 0;        
    
    // Input and Output Cards
    std::ifstream input_file;
    std::ofstream output_file;

public:
    // constructor
    IOHandler();

    // destructor
    ~IOHandler();

    // member methods
    void read_card();
    std::string read_data();

    void reset_buffer();
    
    // getter setters
    void write(std::string);
};

#endif