// filename:   'TimerCls.hpp'
// 
// Purpose:     This class is used to spimplify reporting of the time to complete the program
//
//              ToDo: Add Method Later for reporting an intermmediate Time.
// Usage:
//              TimerCls timer; <- Instanciate Class
//              timer.start();   <- Start the timer
//              your code her
//              timer.end();    <- End the timer
//
// By:          T. Sciple 8/13/2024

#include<iostream>  
#include<chrono>

class TimerCls {
public:
    // Constructor
    TimerCls() : start_time(), end_time(), is_started(false) {}

    // Start the timer
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        is_started = true;
    }

    // Stop the timer and display the elapsed time
    void end() {
        if (!is_started) {
            std::cerr << "Timer was not started." << std::endl;
            return;
        }

        end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end_time - start_time;

        // Output the duration
        std::cout << "\n\nProgram Execution Completed in " << duration.count() << " seconds" << std::endl;

        // Pause the console window before exiting (Windows-specific)
        system("pause");
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_started;
};