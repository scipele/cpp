// Filename:        ProgressBar.hpp
//
// Purpose:         Class to provide a visual representation of percent complete during
//                  program execution
//
// Usage:           int total = 200;
//                  ProgressBar prg(200);  <- Instantiate the class with a 100% Complete Value of 200
//                  prg.Update(cur_prog);  <- pass integer number using the update method
//
// Dependencies:    None
//
//  By: T.Sciple    8/16/2024

#pragma once
#include <iostream>
#include <iomanip>

class ProgressBar {
private:
    //Define Private class variables used in the functions jbelow
    int barWidth;                   // Width of the progress bar
    int completed_progress_amount;  // Value for progress that represents 100% complete
    char progressChar;              // Character used to fill the progress bar
    char emptyChar;                 // Character used for the empty part of the bar

public:
    // Constructor to initialize the progress bar
    ProgressBar(int total, int width = 50, char progChar = '*', char emptyChar = '-')
        : completed_progress_amount(total), barWidth(width), progressChar(progChar), emptyChar(emptyChar) {}

    // Method to update and display the progress bar
    void Update(int cur_progress) {
         
         if (cur_progress > completed_progress_amount) cur_progress = completed_progress_amount;
        
        float pct_compl = (float)cur_progress / completed_progress_amount;
        int pos = barWidth * pct_compl;

        std::cout << "[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)
                std::cout << progressChar;
            else
                std::cout << emptyChar;
        }
        std::cout << "] " << int(pct_compl * 100.0) << " %\r";
        std::cout.flush();
        
        // Complete the bar if progress reaches total
        if (cur_progress == completed_progress_amount) {
            std::cout << std::endl;
        }
    }
};
