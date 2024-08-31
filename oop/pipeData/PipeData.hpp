#pragma once
#ifndef PIPEDATA_HPP
#define PIPEDATA_HPP

#include <string>


class PipeData
{
private:
    // Private member Variables
    std::string nom_size = "";
    std::string sched = "";
    double pipeOD = 0;
    double pipeThk = 0;
    double pipeID = 0;
    double emptyWtPerFt = 0;
    double waterWtPerFt = 0;
    double pipeWtFullPerFt= 0;
    int sizeIndex = 0;
    int schIndex = 0;

    // Function Prototypes for class member functions
    std::string getSize();
    std::string getSched();
    std::string toUpperCase(const std::string str);
    
public:
    // Constructor Prototype
    PipeData();
    //Destructor Prototype
    ~PipeData();
};

#endif // PIPEDATA_HPP