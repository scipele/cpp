#include <iostream>
#include <string>

int main(int argc, char const *argv[])
{
    std::string fr_path = "c:\\from path";
    std::string fr_file = "c:\\from path\\subFolder\\exFileName.txt";
    std::string to_path = "c:\\destination example folder";

    int no_chars = fr_path.size();

    // strips specified number of chars from the left of the string
    std::string to_file =  fr_file.substr(no_chars + 1);
    std::cout << to_file << "\n";

    // next add new to_path to the beginning
    to_file = to_path + "\\" + to_file;
    std::cout << to_file << "\n";

    return 0;
}
