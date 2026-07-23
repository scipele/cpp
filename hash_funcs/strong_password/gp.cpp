/*

| Item         | Main Program Documentation Notes                            |
|--------------|-------------------------------------------------------------|
| Filename     | gp.cpp                                                      |
| EntryPoint   | main                                                        |
| Purpose      | generate strong passwords                                   |
| Inputs       | varies                                                      |
| Outputs      | generated strong password                                   |
| Dependencies | none                                                        |
| By Name,Date | T.Sciple, 07/23/2026                                        |
Compile iOS/Linux with:
clang++ -std=c++17 -o gp gp.cpp
*/

#include <iostream>
#include <string>    // Required for std::string and std::stoi
#include <cstdlib>   // Required for rand() and srand()
#include <ctime>     // Required for time() to seed the random generator

std::string get_password(std::string length)
{
    int len;
    try {
        len = std::stoi(length);
    } catch (...) {
        std::cout << "Error: Invalid number entered.\n";
        return "";
    }

    if (len <= 0 || len > 20) {
        std::cout << "Error: Length must be between 1 and 20.\n";
        return "";
    }

    // iPhone-Friendly Pool: Skips confusing letters (O, 0, l, 1, I) 
    // and hides hard-to-find symbols (~, `, \, ^, |).
    std::string safe_chars = "abcdefghijkmnopqrstuvwxyz"
                             "ABCDEFGHJKLMNPQRSTUVWXYZ"
                             "23456789"
                             "!@#$%&*()+=[]{}?";

    std::string password;

    for (int i = 0; i < len; i++)
    {
        char c;
        do {
            // Pick randomly from our safe string length
            int index = rand() % safe_chars.length();
            c = safe_chars[index];
        } while (password.find(c) != std::string::npos);  
        
        password += c;
    }

    std::cout << "Generated Password:\n\n" << password << std::endl;
    return password;
}


int main(int argc, char const *argv[])
{
    // Safety Fix 3: Seed the random number generator using the current time.
    // Without this, rand() generates the exact same sequence of passwords every time you run the app.
    srand(static_cast<unsigned int>(time(nullptr)));

    std::cout << "Strong Password Generator:\n";
    std::cout << "Password length: ";
    std::string length;
    std::cin >> length;

    get_password(length);

    return 0;
}
