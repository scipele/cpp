#include <cstring> 
#include <iostream> 

// defining struct coordinates 
struct Coordinates { 
    int x; 
    int y; 
}; 
  
int main() 
{ 
  
    // Declare a fixed array of structures 
    const int ary_sz = 3; 
    Coordinates vertices[ary_sz]; 
  
    // Assign values to elements in the array 
    vertices[0].x = 2; 
    vertices[0].y = 4; 
  
    // Alternate Way to populate the Aarry of Struct
    vertices[1] = { 3, 6 }; 
    vertices[2] = { 4, 8 }; 
  
    // Displaying the values stored in the array of 
    // structures 
    for (int i = 0; i < ary_sz; ++i) { 
        std::cout << "Coordinates of ( x , y ) " << i + 1 
             << ": (" << vertices[i].x << ", "
             << vertices[i].y << ")" << std::endl; 
    } 
  
    return 0; 
}