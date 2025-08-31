// Function to return the pipe outside diameter from a user input size.  Map is defined for the sizes where the OD is different than the nominal pipe size
#include <iostream>
#include <map>

double getPipeOD(double nps);

int main()
{
    std::cout << "Enter Nom Pipe Size :";       
    double nps;  std::cin >> nps; 
    
    //call function getPipeOD to return a double with the pipe outside diameter
    std::cout << "Pipe OD = " << getPipeOD(nps) << "\n";    // output OD to screen

    return 0;
}

double getPipeOD(double nps) {
    std::map<double, double> pipOdMap = { {0.5,0.84},
                                          {0.75,1.05},
                                          {1,1.315},
                                          {1.25,1.66},
                                          {1.5,1.9},
                                          {2,2.375},
                                          {2.5,2.875},
                                          {3,3.5},
                                          {4,4.5},
                                          {5,5.625},
                                          {6,6.625},
                                          {8,8.625},
                                          {10,10.75},
                                          {12,12.75}
                                        };
        if(nps > 12.0) {
                return nps;       // For Pipe Sizes Larger than 12" the OD is equal to the nom pipe size (nps)
        } else {
            return pipOdMap[nps];  // Otherwise return the pipe OD from the Map related pair 
        }
}