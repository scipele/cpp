#include "PipeData.hpp"
#include <iostream>
#include <iostream>
#include <cctype> // For std::toupper

// Filename:  PipeData.cpp
//
// Purpose:   This is the class implementation file that defines all the member funcitons of the class
//
//

// This Constructor is used call other member functions
PipeData::PipeData()
{
    std::cout << "Object Constructed\n";
    this->nom_size = getSize();
    this->sched = getSched();
}

PipeData::~PipeData() {
}


std::string PipeData::getSize() {
    // Get Size from User    
    std::string size;
    std::cout << "Pipe Size:\n"
              << "    Enter nominal size in Decimal Format (0.5, 1.25, 2, 3, 4, 6, 8, 10, 12) --> ";
    std::getline(std::cin, size);
    return PipeData::toUpperCase(size);
}


std::string PipeData::getSched() {
    // Get Schedule from User    
    std::string sch;
    std::cout << "Enter Pipe Schedule (std, xs, xxs, 5, 10, 20, 30, 40, 60, 80, 120, 140, 160) --> ";
    std::getline(std::cin, sch); 
    return PipeData::toUpperCase(sch);
}


//function to convert string to uppercase
std::string PipeData::toUpperCase(const std::string str) {  
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}