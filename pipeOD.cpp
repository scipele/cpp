// Looping with For and with an array
#include <iostream>

double getPipeOD(double nps) {
    // Array defines the pipe NPS and cooresponding od for pipe sizes 0.5" thru 12"
    double const pipeSzOD[13][2] = {
        {0.5,  0.84 },   // NPS 0.5" has OD 0.84
        {0.75, 1.05 },
        {1,    1.315},
        {1.25, 1.66 },
        {1.5,  1.9  },
        {2,    2.375},
        {2.5,  2.875},
        {3,    3.5  },
        {4,    4.5  },
        {6,    6.625},
        {8,    8.625},
        {10,   10.75}, 
        {12,   12.75}   
    };
        
    if (nps > 12) {
        return nps;  // For Pipe Sizes Larger than 12" the OD is equal to the nom pipe size (nps)
    } else {
        for (int i = 0; i<=12; i++) {   // loop thru array elements until finding a match 
            if (pipeSzOD[i][0] == nps ) {
                return pipeSzOD[i][1];
                break;
            }
        } 
    }
    // If the input doesn't match any known pipe size, return 0 value
    return 0.0; // Default value
}

int main()
{
    // message for user
    std::cout << "Enter Nom Pipe Size :";       
    double input;                               // declare 'input' as type double
    std::cin >> input;                          // get user input using the standard cin function
    double od = getPipeOD(input);               // declare 'od' as return from the getPipeOD Custom Function  
    std::cout << "Pipe OD = " << od << "\n";    // output OD to screen

    return 0;
}