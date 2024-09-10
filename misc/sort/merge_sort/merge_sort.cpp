// filename:    merge_sort.cpp
//
// Purpose:     Implementation of merge sort alogorithm for random generated integers
//
// Dependencies:    Header Files            Class Name
//                  Timer.hpp               TimerCls
//
// By:  T.Sciple, 9/8/2024

#include <iostream>
#include <vector>
#include <random>
#include "../../Classes/Timer.hpp"

//Function Prototypes
void mergeSort(std::vector<int>& vec, int left, int right);
void merge(std::vector<int>& vec, int left, int mid, int right);
void printVector(std::vector<int>& vec);

int main()
{
    // Create a random device to seed the generator
    std::random_device rd;
    // Initialize a Mersenne Twister random number generator
    std::mt19937 gen(rd());
    // Define a uniform distribution between a given range, for example 1 to 10000
    std::uniform_int_distribution<> dist(1, 50000);

    // Generate 10000 random numbers for testing purposes
    std::vector<int> vec;
    for (int i = 0; i < 50000; ++i) {
        vec.push_back(dist(gen));
    }
    // print blank lines to clear screen
    for (int i=0; i<10; i++) std::cout << "\n";

    std::cout << "Given unsorted vector is \n";
    // printVector(vec);

    TimerCls timer;
    int n = vec.size();
    timer.start();
    mergeSort(vec, 0, n - 1);
    timer.interval();

    std::cout << "\nSorted vector is \n";
    // printVector(vec);
    timer.end();
    
    // Pause the console window before exiting
    system("pause");
    return 0;
}

// begin is for left index and end is right index
// of the sub-array of vec to be sorted
void mergeSort(std::vector<int>& vec, int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;
    mergeSort(vec, left, mid);
    mergeSort(vec, mid + 1, right);
    merge(vec, left, mid, right);
}

// Merges two subvectors of vec[].
// First subarray is vec[left..mid]
// Second subarray is vec[mid+1..right]
void merge(std::vector<int>& vec, int left, 
                     int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temp vectors
    std::vector<int> L(n1), R(n2);

    // Copy data to temp vectors L[] and R[]
    for (int i = 0; i < n1; i++) 
        L[i] = vec[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = vec[mid + 1 + j];

    int i = 0, j = 0;
    int k = left;

    // Merge the temp vectors back into vec[left..right]
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            vec[k] = L[i];
            i++;
        }
        else {
            vec[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        vec[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        vec[k] = R[j];
        j++;
        k++;
    }
}

// Function to print a vector
void printVector(std::vector<int>& vec)
{
    for (int i = 0; i < vec.size(); i++)
        std::cout << vec[i] << ",";
    std::cout << std::endl;
}