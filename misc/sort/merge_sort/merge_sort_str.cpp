// filename:    merge_sort_str.cpp
//
// Purpose:     Implementation of merge sort alogorithm for strings
//              Case is ignored in comparison see toLowerCase() Function usage
// Dependencies:    Header Files            Class Name
//                  Timer.hpp               TimerCls
//
// By:  T.Sciple, 9/8/2024

#include <iostream>
#include <vector>
#include <random>
#include "../../Classes/Timer.hpp"

//Function Prototypes
void mergeSort(std::vector<std::string>& vec, int left, int right);
void merge(std::vector<std::string>& vec, int left, int mid, int right);
void printVector(std::vector<std::string>& vec);
std::string toLowerCase(const std::string& str);

int main()
{
    std::vector<std::string> vec = {
        "tiger", "horse", "cat", "dog", "elephant", "fish", "grape", "house", "igloo", 
        "sun", "train", "violin", "water", "xray", "yarn", "zoo", "word", "Cat", "Dog",
        "juice", "koala", "lamp", "moon", "nest", "orange", "penguin", "quilt", "rose", 
        "jungle", "kite", "lion", "mountain", "notebook", "octopus", "pencil", "QUEEN", 
        "Rabbit", "star", "apple", "Umbrella", "vase", "whale", "xylophone", "yogurt", 
        "juice", "koala", "lamp", "moon", "nest", "orange", "penguin", "quilt", "rose", 
        "zebra", "bird", "Banana", "cloud", "desk", "earth", "forest", "garden", "INSECT", 
        "tiger", "horse", "cat", "dog", "elephant", "fish", "grape", "house", "igloo", 
        "Rabbit", "star", "apple", "Umbrella", "vase", "whale", "xylophone", "yogurt", 
        "juice", "koala", "lamp", "moon", "nest", "orange", "penguin", "quilt", "rose", 
        "sun", "train", "violin", "water", "xray", "yarn", "zoo", "word", "Cat", "Dog"
    };

    // print blank lines to clear screen
    for (int i=0; i<20; i++) std::cout << "\n";

    std::cout << "Given unsorted vector is \n";
    printVector(vec);

    TimerCls timer;
    timer.start();

    int n = vec.size();
    mergeSort(vec, 0, n - 1);
    timer.interval();

    std::cout << "\nSorted vector is \n";
    printVector(vec);
    std::cout << "\n";

    timer.end();
    return 0;
}

// begin is for left index and end is right index
// of the sub-array of vec to be sorted
void mergeSort(std::vector<std::string>& vec, int left, int right)
{
    if (left >= right)
        return;
    int mid = left + (right - left) / 2;
    mergeSort(vec, left, mid);
    mergeSort(vec, mid + 1, right);
    merge(vec, left, mid, right);
}

// Merges two subvectors of vec[]
// First subarray is vec[left..mid]
// Second subarray is vec[mid+1..right]
void merge(std::vector<std::string>& vec, int left, 
                     int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temp vectors
    std::vector<std::string> L(n1), R(n2);

    // Copy data to temp vectors L[] and R[]
    for (int i = 0; i < n1; i++) 
        L[i] = vec[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = vec[mid + 1 + j];

    int i = 0, j = 0;
    int k = left;

    // Merge the temp vectors back into vec[left..right]
    while (i < n1 && j < n2) {
        // Following is where comparison and swap is done
        if (toLowerCase(L[i]) <= toLowerCase(R[j]) ) {   
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

// Function to convert a string to lowercase
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);  // replaces each character in the ByRef Operator '&'str
    }
    return result;
}

// Function to print a vector
void printVector(std::vector<std::string>& vec)
{
    int last = vec.size()- 1;
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec[i];
        if(i != last) std::cout << ",";
    }
    std::cout << std::endl;
}