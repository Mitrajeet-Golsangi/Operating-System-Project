#ifndef PCB_HPP
#define PCB_HPP

#include <iostream>

class PCB
{
public:
    int jobid;      // ID of the process for which PCB is generated
    int ttc = 0;    // Total Time Counter
    int llc = 0;    // Line Limit Counter
    int ttl;        // Total Time Limit
    int tll;        // Total Line Limit

    // constructors
    PCB() {}
    PCB(int ttl, int tll, int jobid)
    {
        this->ttl = ttl;
        this->tll = tll;
        this->jobid = jobid;
    }

    // increment the counters
    void increment_ttc() { ttc++; }
    void increment_llc() { llc++; }
};

#endif