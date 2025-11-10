// Filename:        test_timer.cpp
//
// Purpose:         Example showing how the Timer Class can be utilized to report various
//                  execution times with your program
//
// Dependencies:    'TimerCls'
//
//      Option 1:   Indicate full path of the header file
//                  This option is an example of how full path can be shown in header files
//                  so that the compliler/linker will locate the header file without any
//                  special modification to the /json file
//                  #include "c:/dev/cpp/misc/Classes/TimerCls.hpp"
//
//      Option 2:   Alternative Method is to modify this line in .vscode {} tasks.json as follows which
//                  will compile all files in the current folder:  
//                  "${file}" , ->  "${fileDirname}\\*.cpp" ,
//                  include format name only -> #include "TimerCls.hpp"
//  By: T.Sciple    8/14/2024

#include<iostream>
#include "TimerCls.hpp"

int main(int argc, char const *argv[])
{
    TimerCls timer;
    timer.start();
        
    int a;
    for (long int i=0; i< 100000000; i++)  a = 1 + 1000;    // Do something to burn time for the example
    timer.interval();
    for (long int i=0; i< 1000000000; i++)  a = 1 + 1000; 
    timer.interval();
    for (long int i=0; i< 1000000000; i++)  a = 1 + 13564; 
    timer.interval();
    timer.end();
    
    return 0;
}

