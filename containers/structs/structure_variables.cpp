#include <iostream>
#include <string>
#include <sstream>
using namespace std;

/*One of the most significant benefits of a struct is that we can pass the entire 
structure to another function without having to pass all those variables individually.
We can also add members to the struct without having to re-work all of the function calls.*/


struct {             // Structure declaration
    string filename;
    string folderName;
    long long size;
    string Type;
    string Date;
    string fileSHA1Hash;
    int Count;
} finfo;       // Structure variable

int main()
{
    // set structure variables equal to some values
    finfo.filename = "S-300 Table of Contents revA.pdf";
    finfo.folderName = "T:/Estimates/2024/1.ME/Valero/24-0070 FCC Optimization (BMcD Budget)/1. RFP";
    finfo.size = 17596;
    finfo.Type = "PDF Document";
    finfo.Date = "1/22/2024  5:25:32 PM";
    finfo.fileSHA1Hash = "15662e589d9e9e18bec49abc00b1529e0ce2349f";
    finfo.Count = 1;
    
    // output the structure variable values
    cout << "filename: " << finfo.filename << endl;
    cout << "folderName: " << finfo.folderName << endl;
    cout << "size" << finfo.size << endl;
    cout << "Type : " << finfo.Type << endl;
    cout << "Date :" << finfo.Date << endl;
    cout << "fileSHA1Hash : " <<finfo.fileSHA1Hash << endl;
    cout << "Count:" <<finfo.Count << endl;

    return 0;
}
