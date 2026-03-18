#include <vector>
#include <iostream>

void consume(std::vector<int>&& data) {          // ← THIS IS THE CUSTOM && FUNCTION
                                                 //    (exactly what real apps use)
    std::cout << "Inside consume(std::vector<int>&&):\n";
    std::cout << "   data.data() address : " << static_cast<const void*>(data.data()) << '\n';
    std::cout << "   data.size()         : " << data.size() << '\n';
    std::cout << "   data.capacity()     : " << data.capacity() << '\n';

    // Real application work goes here — you now OWN this memory
    // (no copy ever happened)
    long long sum = 0;
    for (int x : data) sum += x;                 // example processing
    std::cout << "   (Processed — sum of all elements = " << sum << ")\n";

    // When the function ends, the vector is destroyed.
    // Because it came in as &&, the original temporary's memory
    // was directly bound here — zero copies, zero extra allocations.
}

int main() {
    std::cout << "=== Starting real-app style call ===\n";
    std::cout << "Creating a temporary vector and passing it directly...\n\n";

    std::vector<int> tempVec(999'999, 42); // ← This is the temporary vector we want to create and consume
    std::cout << "Created temporary vector with 999,999 elements.\n";
    // NO std::move() anywhere!
    // The temporary on the right is an rvalue → binds straight to the && parameter
    consume(std::move(tempVec));

    std::cout << "\n=== Back in main ===\n";
    std::cout << "The temporary was consumed and destroyed inside the function.\n";
    std::cout << "No duplicate data was ever created.\n\n";

    std::cout << "This pattern is used EVERY DAY in real applications:\n";
    std::cout << "   • processResults( computeHeavyVector() );\n";
    std::cout << "   • saveToDatabase( loadEntireFileIntoVector() );\n";
    std::cout << "   • logData( std::move would be needed only for named variables )\n";

    return 0;
}