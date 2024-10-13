#include <iostream>
#include <string>

// *******************************************************************************************************************************
// **** The following portion of the class definition is typically placed in a header file 'ClassName.h' or 'ClassName.hpp' ****
// *******************************************************************************************************************************
// Provides the following:
//      - public, private, or protected access specifiers
//      - defines member variables
//      - provides constructor and destructor prototypes
//      - provides function prototypes of member functions
class SteelShapes
{
private:
    const std::string shape_desc;
    double depth_d_in;
    double web_thk_tw_in;
    double flg_width_bf_in;
    double flg_thk_tf_in;
    double k;
    double k1;
    double flange_bolt_gauge;
    double web_bolt_gauge;
    double surf_area_sf;
    double weight_per_foot;
public:
    // This is called a constructor protype which is basically similar to a function with the exact same name as the class that automatically runs
    // anytime that an instance of the Object 'SteelShapes' is created (aka instantiated).  
    SteelShapes(const std::string desc);
    
    // This is called a destructor which is basically a function with the exact same name as the class that automatically runs anytime that an instance
    // of the Object 'SteelShapes' is created.  This is called 'instantiated'
    ~SteelShapes();
};




// ***********************************************************************************************************
// **** Class implentation File ****
// ***********************************************************************************************************
// This class implimentation is typically placed in a separate file named 'ClassName.cpp'

// This is called a constructor protype which is basically similar to a function with the exact same name as the class that automatically runs
// anytime that an instance of the Object 'SteelShapes' is created (aka instantiated).  

SteelShapes::SteelShapes(const std::string desc) : shape_desc(desc) {
    std::cout << "I'm inside the constructor\n";
    std::cout << "The variable got set to the private one = " << shape_desc << "\n";
}

SteelShapes::~SteelShapes() {
    std::cout << "I'm inside the destructor where the object will be destroyed\n";
}



int main(int argc, char const *argv[])
{
    
    SteelShapes stl("W14x109");

    /* code */
    return 0;
}
