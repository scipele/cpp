#include <iostream>
#include <string>

struct vessel_data {
    std::string item_no;
    double des_press;
    double des_temp;
    double id_in;
    double len_tt_ft;
    double corr_allow;
    double joint_eff;
    double calc_thk;
    double act_thk;

};



int main(int argc, char const *argv[])
{
    vessel_data v;
            
    // get User Input
    std::cout << "Enter Item No: ";
    getline(std::cin, v.item_no);

    std::cout << "Enter Service Description: ";
    getline(std::cin, v.item_no);

    std::cout << "Design Pressure: ";
    std::cin >> v.des_press;

    std::cout << "Design Temperature: ";
    std::cin >> v.des_temp;

    std::cout << "Inside Diameter (Inches): ";
    std::cin >> v.id_in;

    std::cout << "Length Tan-Tan (Inches): ";
    std::cin >> v.len_tt_ft;
    

    /* code */
    return 0;
}
