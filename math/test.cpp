#include <iostream>
#include <vector>

int main() {
    float a = 3.14159;
    float b = 2.1;

    std::vector<float> myVect = { 4.14159, 6.1, 8.14159};

    float c;

    c = a + b;

    float d;
    d = myVect[0] + myVect[1];

    std::cout << "c = " << c << "\n";
    std::cout << "d = " << d;
   
    return 0;
}
