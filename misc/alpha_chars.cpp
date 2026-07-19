/*
Linux Compile:
g++ -std=c++17 -o alpha_chars alpha_chars.cpp
*/

#include<iostream>

int main() {

    for (int i = 0; i < 26; i++) {
        char c = 'A' + i;
        std::cout << "## " << c << "\n";
    }

    return 0;
}