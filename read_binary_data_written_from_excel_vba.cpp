#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct estim_data {
    long rowNo;
    std::string filterX;
    std::string desc;
    std::string client_brkd_ref;
    double other_mh;
    std::string param1;
    std::string param2;
    std::string param3;
    std::string type;
    double qty;
    std::string uom;
    double umh;
    double mh_tot;
    double rate;
    double labor;
    double matl;
    double sub;
    double eq;
    double total;
    std::string div;
    std::string discp;
    std::string labtype;
    std::vector<double> area;
};


//Function Declarations
void readString(std::ifstream &file, std::string &str);
void readStructure(std::ifstream &file, estim_data &s);
void printRowToFile(int rowNum, const std::vector<std::string>& row, std::ofstream& outFile);
void printRow(int rowNum, const std::vector<std::string>& row);


int main() {
    std::ifstream file("C:/Users/mscip/cpp/excel/data.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file";
        return 1;
    }

   std::vector<estim_data> data;

    while (file.peek() != EOF) {
        estim_data ed;
        readStructure(file, ed);
        data.push_back(ed);
    }

    file.close();

    // Output to screen verify the read of the data
    for (const auto& s : data) {
        std::cout   << s.rowNo << "|" 
                    << s.filterX << "|" 
                    << s.desc << "|" 
                    << s.client_brkd_ref << "|" 
                    << s.other_mh << "|" 
                    << s.param1 << "|" 
                    << s.param2 << "|" 
                    << s.param3 << "|" 
                    << s.type << "|" 
                    << s.qty << "|" 
                    << s.uom << "|" 
                    << s.umh << "|" 
                    << s.mh_tot << "|" 
                    << s.rate << "|" 
                    << s.labor << "|" 
                    << s.matl << "|" 
                    << s.sub << "|" 
                    << s.eq << "|" 
                    << s.total << "|" 
                    << s.div << "|" 
                    << s.discp << "|" 
                    << s.labtype << "|" ;
                    for (const auto &val : s.area) {
                        std::cout << val << "|" ;
                    }
                    std::cout << "\n";
    }

    return 0;
}


void readString(std::ifstream &file, std::string &str) {
    int length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    str.resize(length);
    file.read(&str[0], length);
}


void readStructure(std::ifstream &file, estim_data &s) {
    int areaSize;
    file.read(reinterpret_cast<char*>(&s.rowNo), sizeof(s.rowNo));
    readString(file, s.filterX);
    readString(file, s.desc);
    readString(file, s.client_brkd_ref);
    file.read(reinterpret_cast<char*>(&s.other_mh), sizeof(s.other_mh));
    readString(file, s.param1);
    readString(file, s.param2);
    readString(file, s.param3);
    readString(file, s.type);
    file.read(reinterpret_cast<char*>(&s.qty), sizeof(s.qty));
    readString(file, s.uom);
    file.read(reinterpret_cast<char*>(&s.umh), sizeof(s.umh));
    file.read(reinterpret_cast<char*>(&s.mh_tot), sizeof(s.mh_tot));
    file.read(reinterpret_cast<char*>(&s.rate), sizeof(s.rate));
    file.read(reinterpret_cast<char*>(&s.labor), sizeof(s.labor));
    file.read(reinterpret_cast<char*>(&s.matl), sizeof(s.matl));
    file.read(reinterpret_cast<char*>(&s.sub), sizeof(s.sub));
    file.read(reinterpret_cast<char*>(&s.eq), sizeof(s.eq));
    file.read(reinterpret_cast<char*>(&s.total), sizeof(s.total));
    readString(file, s.div);
    readString(file, s.discp);
    readString(file, s.labtype);

    file.read(reinterpret_cast<char*>(&areaSize), sizeof(areaSize));
    s.area.resize(areaSize);
    file.read(reinterpret_cast<char*>(s.area.data()), areaSize * sizeof(double));
}


// Function to print a row with row number and "|" delimited columns to a file
void printRowToFile(int rowNum, const std::vector<std::string>& row, std::ofstream& outFile) {
    std::ostringstream oss;
    oss << "Row " << rowNum << ": ";
    for (size_t i = 0; i < row.size(); ++i) {
        if (i != 0) oss << "|" ;
        oss << row[i];
    }
    oss << "\n"; // New line after each row
    outFile << oss.str(); // Write formatted string to file
}

/*  include this stuff later
#include <xlnt/xlnt.hpp>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

// Function to read data from an Excel file into a 2D vector
bool isNumeric(const std::string& str);
std::map<std::string, int> createFieldMap(const std::vector<std::vector<std::string>>& data);
std::vector<int> generateAreaIndices(const std::map<std::string, int>& fieldMap);
std::vector<std::vector<std::string>> read_excel_data(const std::string& file_path, const std::string& sheet_name);
double stringToDouble(const std::string& str);
void printVector(const std::vector<std::string>& vec);
std::map<std::string, double> sumColumnByClientBrkdRef(const std::vector<std::vector<std::string>>& data, int colIndex);
std::map<std::string, double> proportionAndSumByArea(const std::vector<std::vector<std::string>>& data, const std::vector<int>& areaIndices, int valueIndex);


int main()
{
    std::string file_path = "C:/Users/mscip/source/repos/read_excel/read_excel/breakdown.xlsx";
    std::string sheet_name = "Estimate";

    // Make the file read-only
    std::filesystem::permissions(file_path, std::filesystem::perms::owner_read | std::filesystem::perms::group_read | std::filesystem::perms::others_read, std::filesystem::perm_options::replace);

    //Call Function to read the excel data into a vector of string vectors
    std::vector<std::vector<std::string>> excel_data = read_excel_data(file_path, sheet_name);


    // Print headers
    printVector(excel_data[0]);

    // Create the field map
    std::map<std::string, int> fieldMap = createFieldMap(excel_data);

    std::cout << "The first area index is: " << fieldMap["Type"] + 1 << "\n";
    std::cout << "The last area index is: " << fieldMap["Qty"] - 1;

    // Generate the area indices vector
    std::vector<int> areaIndices = generateAreaIndices(fieldMap);

        int valueIndex = 24; // Assuming 24 is the index for Mh-Tot

        std::map<std::string, double> proportioned = proportionAndSumByArea(excel_data, areaIndices, valueIndex);
        std::cout << "Proportioned sum of Mh-Tot by Area grouped by Client_Brkd_Ref:" << std::endl;
        for (const auto& pair : proportioned) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }


        // Print the data
        for (const auto& row : excel_data)
        {
            for (const auto& cell : row)
            {
                std::cout << cell << "|";
            }
            std::cout << std::endl;
        }
        

        // Remove the read-only setting
        std::filesystem::permissions(file_path, std::filesystem::perms::owner_all | std::filesystem::perms::group_all | std::filesystem::perms::others_all, std::filesystem::perm_options::replace);

        return 0;
 }


/* Incorporate this stuff later

 // Function to create a map of field names to their indices
 std::map<std::string, int> createFieldMap(const std::vector<std::vector<std::string>>&data) {
        std::map<std::string, int> fieldMap;
        const auto& header = data[0]; // First row contains the field names

        for (size_t i = 0; i < header.size(); ++i) {
            fieldMap[header[i]] = i;
        }

        return fieldMap;
}

// Function to generate area indices vector
std::vector<int> generateAreaIndices(const std::map<std::string, int>& fieldMap) {
    int firstAreaIndex = fieldMap.at("Type") + 1;
    int lastAreaIndex = fieldMap.at("Qty") - 1;

    std::vector<int> areaIndices;
    for (int i = firstAreaIndex; i <= lastAreaIndex; ++i) {
        areaIndices.push_back(i);
    }

    return areaIndices;
}

std::vector<std::vector<std::string>> read_excel_data(const std::string& file_path, const std::string& sheet_name)
{
    xlnt::workbook wb;
    try {
        wb.load(file_path);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load workbook: " << e.what() << std::endl;
        return {};
    }

    xlnt::worksheet ws;
    try {
        ws = wb.sheet_by_title(sheet_name);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to access sheet: " << e.what() << std::endl;
        return {};
    }

    std::vector<std::vector<std::string>> data;
    try {
        for (const auto& row : ws.rows())
        {
            std::vector<std::string> row_data;
            for (const auto& cell : row)
            {
                try {
                    // Check if the cell is null or empty
                    if (cell.has_value()) {
                        row_data.push_back(cell.to_string());
                    }
                    else {
                        row_data.push_back(""); // Push an empty string for null or empty cells
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Failed to read cell data: " << e.what() << std::endl;
                    row_data.push_back("Error");
                }
            }
            data.push_back(row_data);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to read data from sheet: " << e.what() << std::endl;
    }
    return data;
}

// Function to check if a string is numeric
bool isNumeric(const std::string& str) {
    if (str.empty()) return false;
    char* endptr = nullptr;
    std::strtod(str.c_str(), &endptr);
    return (*endptr == '\0');
}

// Function to convert string to double, returning 0 for non-numeric values
double stringToDouble(const std::string& str) {
    return isNumeric(str) ? std::stod(str) : 0.0;
}

// Function to print vector of strings
void printVector(const std::vector<std::string>& vec) {
    for (const auto& val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

// Function to perform sum aggregation on a specific column grouped by Client_Brkd_Ref
std::map<std::string, double> sumColumnByClientBrkdRef(const std::vector<std::vector<std::string>>& data, int colIndex) {
    std::map<std::string, double> result;
    for (size_t i = 1; i < data.size(); ++i) {
        std::string clientBrkdRef = data[i][3]; // Assuming 2 is the index for Client_Brkd_Ref
        double value = stringToDouble(data[i][colIndex]);
        result[clientBrkdRef] += value;
    }
    return result;
}

// Function to proportion values by Area and sum by Client_Brkd_Ref
std::map<std::string, double> proportionAndSumByArea(const std::vector<std::vector<std::string>>& data, const std::vector<int>& areaIndices, int valueIndex) {
    std::map<std::string, double> result;
    for (size_t i = 1; i < data.size(); ++i) {
        std::string clientBrkdRef = data[i][3]; // Assuming 3 is the index for Client_Brkd_Ref
        double totalArea = 0.0;
        for (int areaIndex : areaIndices) {
            totalArea += stringToDouble(data[i][areaIndex]);
        }
        for (int areaIndex : areaIndices) {
            double areaValue = stringToDouble(data[i][areaIndex]);
            double proportionedValue = (areaValue / totalArea) * stringToDouble(data[i][valueIndex]);
            result[clientBrkdRef] += proportionedValue;
        }
    }
    return result;
}
*/