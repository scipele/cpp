// filename:    bubble_sort.cpp
//
// Purpose:     Implementation of bubble sort alogorithm for random generated integers
//
// Dependencies:    Header Files            Class Name
//                  Timer.hpp               TimerCls
//
// By:  T.Sciple, 9/8/2024

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "../../Classes/Timer.hpp"

//Function Prototypes

void bubbleSort(std::vector<int>& vec, int n);
void printVector(std::vector<int>& vec);

int main()
{
    // Create a random device to seed the generator
    std::random_device rd;
    // Initialize a Mersenne Twister random number generator
    std::mt19937 gen(rd());
    // Define a uniform distribution between a given range, for example 1 to 100
    std::uniform_int_distribution<> dist(1, 50000);

    // Generate 2000 random numbers for testing purposes
    std::vector<int> vec;
    for (int i = 0; i < 50000; ++i) {
        vec.push_back(dist(gen));
    }
    // print blank lines to clear screen
    for (int i=0; i<10; i++) std::cout << "\n";

    std::cout << "Given unsorted vector is \n";
    //printVector(vec);

    TimerCls timer;

    int n = vec.size();
    timer.start();
    bubbleSort(vec, n);
    timer.interval();

    std::cout << "\nSorted vector is \n";
    //printVector(vec);
    timer.end();

    // Pause the console window before exiting
    system("pause");

    return 0;
}


// An optimized version of Bubble Sort
void bubbleSort(std::vector<int>& vec, int n)
{
    int i, j;
    bool swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = false;
        for (j = 0; j < n - i - 1; j++) {
            if (vec[j] > vec[j + 1]) {
                std::swap(vec[j], vec[j + 1]);
                swapped = true;
            }
        }

        // If no two elements were swapped
        // by inner loop, then break
        if (swapped == false)
            break;
    }
}


// Function to print a vector
void printVector(std::vector<int>& vec)
{
    for (int i = 0; i < vec.size(); i++)
        std::cout << vec[i] << ",";
    std::cout << std::endl;
}