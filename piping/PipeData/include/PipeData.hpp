// Filename:    PipeData.hpp
//
// Purpose:     This is the class header file that defines all the outline of the class
//
//  By:         T.Sciple, 8/31/2024

#pragma once
#ifndef PIPEDATA_HPP  // <- if "PIPEDATA_HPP" is not defined then the following define will be executed, otherwise compiler will skip to the #endif // PIPEDATA_HPP
#define PIPEDATA_HPP

#include <string>

class PipeData
{
protected:
    // Private member Variables
    std::string nom_size = "";
    std::string sched = "";
    double pipeOD = 0;
    double pipeThk = 0;

    // Function Prototypes for class member functions
    std::string get_size();
    std::string get_str_from_user(std::string user_msg);
    std::string get_sched();
    std::string toUpperCase(const std::string str);
    int get_od_and_thk();
    void print_any_errors(int result);

    
public:
    // Constructor Prototype
    PipeData();
    
    // Public Member Functions or methods
    void GetUserInput();
    void PrintPipeData();

    //Destructor Prototype
    ~PipeData();
};


class PipeDataExtended : public PipeData
{
private:
    // Additional properties in derived class
    double pipeID;
    double emptyWtPerFt;
    double waterWtPerFt;
    double pipeWtFullPerFt;
    void get_addn_data();

public:
    PipeDataExtended();         // Constructor Prototype
    ~PipeDataExtended();        // Destructor Prototype
    void PrintPipeDataExt();
};

#endif // PIPEDATA_HPP