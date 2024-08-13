// Filename:    test_timer.cpp
//
// Purpose:     This is an example of how full path can be shown in header files
//              so that the compliler/linker will locate the header file without any
//              special modification to the /json file
//
//              Alternative Method modify this line in :  
//                  "${file}",


// Testing of Timer class

#include<iostream>
#include<c:/dev/cpp/misc/Classes/TimerCls.hpp>

int main(int argc, char const *argv[])
{
    TimerCls timer;
    timer.start();
    

    for (int i=0; i< 1000; i++) {
        std::cout << "Testing Timer\n";
    }

    timer.end();
    
    return 0;
}

