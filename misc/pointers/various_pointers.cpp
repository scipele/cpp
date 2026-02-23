// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | x.cpp                                                       |
//| EntryPoint   | xxx                                                         |
//| Purpose      | compute estimate work hours for various                     |
//| Inputs       | varies                                                      |
//| Outputs      | number of work hours                                        |
//| Dependencies | Indicate if any libraries are used or none                  |
//| By Name,Date | T.Sciple, MM/DD/YYYY                                        |

#include <iostream>
#include <iomanip>

void pauseConsole() {
    // If Windows, use system("pause"); otherwise, use cin.get() to wait for user input
    #ifdef _WIN32
        system("pause");    
    #else
        std::cout << "Press Enter to continue...";
        std::cin.get();
    #endif
}


// example of using a void pointer in a function to print the value of different types of data
void printValue (void *ptr, const std::string &type) {
   
    if (type == "i") {
        std::cout << "\tInt Value: " << *(static_cast<int*>(ptr)) << "\n"; // Cast void pointer to int pointer and dereference to print value
    } else if (type == "d") {
        std::cout << "\tDouble Value: " << *(static_cast<double*>(ptr)) << "\n"; // Cast void pointer to double pointer and dereference to print value
    } else if (type == "c") {
        std::cout << "\tChar Value: " << *(static_cast<char*>(ptr)) << "\n"; // Cast void pointer to char pointer and dereference to print value
    } else if (type == "f") {
        std::cout << "\tFloat Value: " << *(static_cast<float*>(ptr)) << "\n"; // Cast void pointer to float pointer and dereference to print value
    } else {
        std::cout << "\tUnknown type\n"; // Handle unknown types
    }
}


int main() {

    // 1. Basic pointer usage
    // "*" is used to declare a pointer variable, which can hold the address of another variable.
    // "&" is the address-of operator, which gives the memory address of a variable.
    int a = 10; // An integer variable
    int *ptr = &a; // Pointer to an integer, initialized to the address of a

    std::cout << "Integer Pointer Example:\n";
    std::cout << std::setw(45) << std::left << "\tValue of integer a: " << a << "\n"; // Output the value of a
    std::cout << std::setw(45) << std::left << "\tSize of integer a: " << sizeof(a) << " bytes" << "\n"; // Output the size of a
    std::cout << std::setw(45) << std::left << "\tAddress of a using the & operator: " << &a << "\n"; // Output the address of a
    std::cout << std::setw(45) << std::left << "\tValue of ptr: (address of a) " << ptr << "\n"; // Output the value of ptr (address of a)
    std::cout << std::setw(45) << std::left << "\tSize of ptr: " << sizeof(ptr) << " bytes" << "\n"; // Output the size of ptr
    std::cout << std::setw(45) << std::left << "\tValue pointed to by ptr: " << *ptr << "\n\n"; // Output the value at the address stored in ptr (value of a)

    *ptr = 20; // Change the value of a through the pointer
    std::cout << "\tAfter changing value through pointer:\n";
    std::cout << std::setw(45) << std::left << "\tValue of integer a: " << a << "\n"; // Output the new value of 
    std::cout << std::setw(45) << std::left << "\tValue pointed to by ptr: " << *ptr << "\n"; // Output the new value at the address stored in ptr (value of a)
    std::cout << std::setw(45) << std::left << "\tAddress of a using the & operator: " << &a << "\n\n\n"; // Output the address of a again to show it hasn't changed
    

    std::cout << "Integer Array Pointer Example:\n";
    int arr[5] = {1, 2, 3, 5, 8}; // An array of integers
    int *arrPtr = arr; // Pointer to the first element of the array (equivalent to &arr[0])
    std::cout << std::setw(45) << std::left << "\tValue of arrPtr (address of first element): " << arrPtr << "\n"; // Output the value of arrPtr (address of first element)
    std::cout << std::setw(45) << std::left << "\tSize of arrPtr: " << sizeof(arrPtr) << " bytes" << "\n"; // Output the size of arrPtr
    std::cout << std::setw(45) << std::left << "\twhich is the Value of arr[0]: " << arr[0] << "\n"; // Output the value of the first element of the array to show it's the same as *arrPtr      
    std::cout << std::setw(45) << std::left << "\tValue pointed to by arrPtr: " << *arrPtr << "\n"; // Output the value at the address stored in arrPtr (first element of the array)
    std::cout << std::setw(45) << std::left << "\tValue pointed to by arrPtr+2: " << *(arrPtr + 2) << "\n"; // Output the value at the address stored in arrPtr (third element of the array)
    std::cout << std::setw(45) << std::left << "\tValue pointed to by arrPtr+5: " << *(arrPtr + 5) << "\n"; // Output the value at the address stored in arrPtr (sixth element of the array, out of bounds, will likely print garbage value or cause undefined behavior)
    
    // Loop through the array using the pointer to demonstrate pointer arithmetic and output each element's value and address
    for (auto elem : arr) {
        std::cout << std::setw(45) << std::left << "\tValue of element: " << elem << " Address: " << &elem << "\n"; // Output each element of the array
    }


    // 2. Pointer to a pointer (double pointer)
    std::cout << "\n\nDouble Pointer Example:\n";
    int **doublePtr = &ptr; // A pointer to a pointer, initialized to the address of ptr
    std::cout << std::setw(45) << std::left << "\tWhy use a double pointer? It allows us to indirectly access and modify the value of a pointer (ptr)\n"
                                            << "\tthat points to another variable (a). This can be useful in scenarios like dynamic memory allocation,\n"
                                            << "\tpassing pointers to functions, or when we want to change the pointer itself (ptr) rather than just the value it points to (a).\n\n"; // Explanation of why we might use a double pointer
    std::cout << std::setw(45) << std::left << "\tValue of doublePtr (address of ptr): " << doublePtr << "\n"; // Output the value of doublePtr (address of ptr)
    std::cout << std::setw(45) << std::left << "\tSize of doublePtr: " << sizeof(doublePtr) << " bytes" << "\n"; // Output the size of doublePtr
    std::cout << std::setw(45) << std::left << "\tValue pointed to by doublePtr (value of ptr): " << *doublePtr << "\n"; // Output the value at the address stored in doublePtr (value of ptr, which is the address of a)
    std::cout << std::setw(45) << std::left << "\tValue pointed to by the pointer pointed to by doublePtr (value of a): " << **doublePtr << "\n"; // Output the value at the address stored in the pointer pointed to by doublePtr (value of a)     


    // 3. Void pointer (generic pointer)
    std::cout << "\n\nVoid Pointer Example:\n";
    void *voidPtr; // A void pointer, which can point to any data type but cannot be dereferenced directly
    voidPtr = &a; // Pointing to an integer variable
    std::cout << std::setw(45) << std::left << "\tValue of voidPtr (address of a): " << voidPtr << "\n"; // Output the value of voidPtr (address of a)
    std::cout << std::setw(45) << std::left << "\tSize of voidPtr: " << sizeof(voidPtr) << " bytes" << "\n"; // Output the size of voidPtr
    std::cout << std::setw(45) << std::left << "\tCannot directly dereference voidPtr because it is a generic pointer without a specific type. We need to cast it to the appropriate type first.\n"; // Explanation of why we cannot directly dereference a void pointer
    std::cout << std::setw(45) << std::left << "\tValue pointed to by voidPtr after casting to int*: " << *(static_cast<int*>(voidPtr)) << "\n"; // Output the value at the address stored in voidPtr after casting it to an int pointer (value of a)

    std::cout << std::setw(45) << std::left << "\tWhy use a void pointer? It allows us to create functions that can accept pointers to any data type, making them more flexible\n"
                                            << "\tand reusable. For example, we can write a function that takes a void pointer and a size parameter to perform operations on\n"
                                            << "\tdifferent types of data without needing to overload the function for each specific type\n" << std::endl; 

    int b = 42;
    double c = 3.14;
    char d = 'A';
    float e = 2.718f;
    void *f = nullptr; // Example of an unknown type (void is not a valid variable type, but we can use it to demonstrate the printValue function's handling of unknown types)


    std::cout << "\nUsing printValue function:\n";

    std::cout << std::setw(45) << std::left << "\tPassing different types of data to the printValue function using void pointers:\n"; // Explanation of what we are doing with the printValue function  
    printValue(&b, typeid(b).name()); // Pass the address of b and its type name to the printValue function
    printValue(&c, typeid(c).name());
    printValue(&d, typeid(d).name());
    printValue(&e, typeid(e).name()); 
    printValue(&f, typeid(f).name()); // Unknown type example
    
    pauseConsole();

    return 0;
}
