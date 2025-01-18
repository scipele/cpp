// filename:   'TimerCls.hpp'
// 
// Purpose:     This class is used to spimplify reporting of the time to complete the program
//
// Usage:
//              TimerCls timer; <- Instantiate Class
//              timer.start();   <- Start the timer
//                  [your code here]
//              timer.interval();   <- Show Interval Time
//                  [your code here]
//              timer.interval();   <- Show Interval Time
//              timer.end();    <- End the timer
//
// By:          T. Sciple 8/14/2024
#pragma once
#include<iostream>  
#include<chrono>
#include<vector>
#include<iomanip>

class TimerCls {
public:
    // Constructor
    TimerCls() : start_time(), prev_interval_time(), end_time(), is_started(false) {}

    // Start the timer
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        prev_interval_time = start_time; // Initialize previous interval time to start time
        is_started = true;
        durations.clear(); // Clear previous durations
    }

    // Calculate the interval times where this method is called
    void interval() {
        if (!is_started) {
            std::cerr << "Timer was not started." << std::endl;
            return;
        }

        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> cur_duration = current_time - prev_interval_time;

        // Store the current duration in the durations vector,
        // note that the .count converts the time duration object into a double
        durations.push_back(cur_duration.count());

        // Update the previous interval time for the next call
        prev_interval_time = current_time;
    }

    // Stop the timer and display the elapsed time
    void end() {
        if (!is_started) {
            std::cerr << "Timer was not started." << std::endl;
            return;
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end_time - start_time;

        // Output each intervals
        // Print the table header
        std::cout << "\n\nProgram Step Run Times:" << std::endl;
        std::cout << "+-------------+---------+" << std::endl;
        std::cout << "| Interval No | Dur (s) |" << std::endl;
        std::cout << "+-------------+---------+" << std::endl;
        for (int i = 0; i < durations.size(); i++) {
            std::cout   << "| " << std::left << std::setw(11) << i + 1 << " | " << std::setw(7)
                        << std::right << std::fixed << std::setprecision(5) << durations[i] << " |" << std::endl;
            //std::cout << "+-------------+---------+" << std::endl;
        }

        // Output the total duration
        std::cout   << "| Overall     | " << std::setw(7)
                    << std::fixed << std::setprecision(5) << duration.count() << " |" << std::endl;
        std::cout << "+-------------+---------+" << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point prev_interval_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_started;
    std::vector<double> durations; // Store durations of intervals
};