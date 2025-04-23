#include <iostream>
#include <string>
#include <vector>

struct parsed {
    std::string item;
    int loc;
};


void getConsecutiveNumsAndPosition(std::string& inp, std::vector<parsed>& prsd) {
    bool prev_is_num = false;   //set starting condition
    std::string strg;

    for (size_t i ; i < inp.length(); i++) {

        if( isdigit(inp[i]) ) {
            if (prev_is_num) {
                prsd.back().item = prsd.back().item + inp[i];
            } else {
                strg += inp[i];
                prsd.back( { strg, i } );
                strg.clear();        
            }
            prev_is_num = true;
        } else {
            prev_is_num = false;
        }
    }
}


int main(int argc, char const *argv[])
{
    std::string inp = "132'-5 7/16\"";
    std::vector<parsed> prsd;

    getConsecutiveNumsAndPosition(inp, prsd);

    std::cout << "\n" << std::endl;  // Insert Blank lines

    for (auto p : prsd) {
        std::cout << p.item << p.loc << "\n";
    }
    std::cout << std::endl;

    return 0;
}
