#include <iostream>
#include <chrono>

int main() {
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

	for (int i=0; i<10000; i++) {
	    std::cout << i << "\n"; // loop operation to time	
	}
    
        // Record end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Output the duration
    std::cout << "Calculation Compleded in : " << duration.count() << " seconds" << std::endl;
    
    // Pause the console window before exiting
    system("pause");

    return 0;
}
