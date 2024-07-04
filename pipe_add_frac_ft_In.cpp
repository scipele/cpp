#include <iostream>
#include <string>
#include <vector>

struct measurement {
    int ft_whole=0;
    int in_whole=0;
    int in_numerator=0;
    int in_denominator=0;
    int delim_loc_ft_begin=0;
    int delim_loc_ft_end=0;
    int delim_loc_space_between_inch_whole_and_numerator=0;
    int delim_loc_division_sign=0;
    int delim_loc_in=0;
    double dec_ft=0;
    double dec_in=0;
    double dec_frac_in=0;
};

//Declare Functions
    std::string get_user_input();
    void parse_measurement_data(std::string& str, measurement& m);
    int get_delim_loc(std::string& str, std::vector<std::string>& delims, int start_pos, int end_pos, int& delim_found_len);
    void replace_double_single_quotes(std::string& str);
    int extract_and_convert_to_int(const std::string& str, int startPos, int num_chars);
    std::string toLowerCase(const std::string& str);
    double calc_dec_ft(measurement& m);

int main() {

    std::cout << "Enter Fractional Feet Inches and Fractions (ie 43'-7 7/16\") :\n"
              << "'d' for done :";
    std::string str;
    int multiplier;

    while(str != std::string("d")) {
        multiplier=1;  // set as default
        double dec_ft_sum;
        //get user entry

        str = get_user_input();

        if(str.empty()) {
            std::cout << "nothing entered";
            continue;
        }

        if ( str[0] == '-') {
            multiplier = -1;
        } 

        // Define an instance of the structure to store measurement items
        measurement m;
        parse_measurement_data(str, m);

        //caculate decimal ft using parsed data
        m.dec_ft = calc_dec_ft(m) * multiplier;

        dec_ft_sum += m.dec_ft;

        std::cout << "Running Sum: " << dec_ft_sum << "\n";

    };
   
    system("pause");
}

std::string get_user_input() {
    std::cout << "entry-> ";
    std::string tmp_str;
    std::getline(std::cin, tmp_str);
    replace_double_single_quotes(tmp_str);
    return tmp_str;
}

void parse_measurement_data(std::string& str, measurement& m) {
    // Variables used as reference to stating and end posistions of string
    int start_pos=0;
    int end_pos=str.length();
    int delim_found_len=0;

    // Get Feet Whole number
    std::vector<std::string> chk_delims;
    chk_delims = {"ft", "f", "'"};
    m.delim_loc_ft_begin = get_delim_loc(str, chk_delims, start_pos, end_pos, delim_found_len);
    m.delim_loc_ft_end = m.delim_loc_ft_begin + delim_found_len;
    m.ft_whole = extract_and_convert_to_int(str, 0, m.delim_loc_ft_begin);

    // Get Fractional Delimeter Location if applicable otherwise -1 is returned
    chk_delims = { "/" };
    start_pos = m.delim_loc_ft_end + 1;
    m.delim_loc_division_sign = get_delim_loc(str, chk_delims, start_pos, end_pos, delim_found_len); 

    // Get  Delimeter Location of any space or dash between the division sign and the ft delimeter+1
    start_pos = m.delim_loc_ft_end +1;
    end_pos = m.delim_loc_division_sign;
    if (m.delim_loc_division_sign != -1) {
        chk_delims = { " ", "-" };
        m.delim_loc_space_between_inch_whole_and_numerator = get_delim_loc(str, chk_delims, start_pos, end_pos, delim_found_len);
    } else {
        m.delim_loc_space_between_inch_whole_and_numerator = -1;
    }

    // Get Inch Delimeter Location
    chk_delims = {"in", "\"", "inch"};
    start_pos =  std::max(1, m.delim_loc_ft_end);
    end_pos = str.length();
    m.delim_loc_in = get_delim_loc(str, chk_delims, start_pos, end_pos, delim_found_len);

    // Get Fractional parts if applicable
    if(m.delim_loc_division_sign != -1) {
        // Get Numerator
        start_pos = std::max(m.delim_loc_ft_end, m.delim_loc_space_between_inch_whole_and_numerator);
        end_pos = m.delim_loc_division_sign;
        m.in_numerator = extract_and_convert_to_int(str, start_pos, end_pos);

        // Get Denominator
        start_pos = m.delim_loc_division_sign;
        end_pos =  m.delim_loc_in;
        m.in_denominator = extract_and_convert_to_int(str, start_pos, end_pos);
    }

    //Get Inch Whole Number
    start_pos = std::max(0, m.delim_loc_ft_end);
    if(m.delim_loc_space_between_inch_whole_and_numerator==-1) {
        end_pos = str.length();
    } else {
        end_pos = m.delim_loc_space_between_inch_whole_and_numerator;
    }
    m.in_whole = extract_and_convert_to_int(str, start_pos, end_pos);
}

int get_delim_loc(std::string& str, std::vector<std::string>& delims, int start_pos, int end_pos, int& delim_found_len) {
    // Define a list of delimiters that will be checked through until a match is found
    int num_chars = end_pos - start_pos;
        
    // Extract the substring starting from startPos with length num_chars
    std::string sub_str = str.substr(start_pos, num_chars);

    for (const auto& d : delims) {
        size_t pos = sub_str.find(d);
        if (pos != std::string::npos) {
            delim_found_len = d.length();
            return start_pos + static_cast<int>(pos);
        }
    }
    return -1;  // return '-1' if no match is found
}

double calc_dec_ft(measurement& m) {
    // Calculated Dec Inches
    if(m.in_denominator > 0) {
        m.dec_frac_in = static_cast<double>(m.in_numerator)/static_cast<double>(m.in_denominator);
        m.dec_in = 12 * m.ft_whole + m.in_whole + m.dec_frac_in;
    } else {
        m.dec_in = static_cast<double>(12 * m.ft_whole + m.in_whole);
    }
    m.dec_ft =  m.dec_in / 12;
    return m.dec_ft;
}

void replace_double_single_quotes(std::string& str) { 
    // Function to replace any two separate single quotes with a double quote
    size_t pos = 0;
    while ((pos = str.find("''", pos)) != std::string::npos) {
        str.replace(pos, 2, "\"");
        pos += 1;  // Move past the replacement to avoid infinite loop
    }
}

int extract_and_convert_to_int(const std::string& str, int start_pos, int end_pos) {
   int num_chars = end_pos - start_pos;
   if (num_chars > 0) {
        // Extract the substring starting from startPos with length num_chars
        std::string sub_str = str.substr(start_pos, num_chars);

        // Filter out non-digit characters from the extracted substring
        std::string filtered_str;
        for (char ch : sub_str) {
            if (std::isdigit(ch)) {
                filtered_str += ch; // Append digit to numeric string
            }
        }
        // If the filtered string is not empty, convert it to an integer
        if (!filtered_str.empty()) {
            return std::stoi(filtered_str); // Convert numeric string to integer
        }
   }
    // Return 0 if no numeric values were found
    return 0;
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);  // replaces each character in the ByRef Operator '&'str
    }
    return result;
}