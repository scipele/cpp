#include <iostream>


long dl(double& num) {
    return static_cast<int>(num);
}

int main(int argc, char const *argv[])
{
    long num_lg;  
    double num_dbl = 534.25;

    //standard version
    num_lg = static_cast<int>(num_dbl);
    std::cout << num_lg << "\n\n";
    std::cout << num_lg << "\n";

    //shortend version using function
    num_lg = dl(num_dbl);
    std::cout << num_lg << "\n\n";


    return 0;
}
