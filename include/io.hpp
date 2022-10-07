#include <iostream>
#include <memory>

class IOHandler
{
    //Buffer
    std::string buffer;        
    
    // Counter for loading the program cards
    int count = 0;        
    
public:
    void read_card();
    std::string read_data();

    void reset_buffer();
};