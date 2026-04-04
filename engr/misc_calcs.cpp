#include<iostream>
#include<cmath>
#include<string>
#include<limits>
#include<sstream>
#include<iomanip>


enum MainCategory
{
    FlowRelated = 1,
    HeatExchangeRelated,
    Civil,
    Chemical,
    Environmental
};


struct densityRelated
{
    double atmospheric_pressure;
    double standard_temperature;
    double molecular_weight;
    double temperature;
    double pressure;
    double compressibility_factor;
    double density;
    double std_density;
};


// Function prototypes for calculation functions can be declared here
int mainMenu();
int subMenu(int category);
int getSubSelection(int main_category, int max_option);
densityRelated densityCalcGivenMw(); 
int vaporDensityVaporFlowConversions();
double getUserInput(const std::string& prompt, double default_value = 0.0);
std::string formatDoubleWithCommas(double value, int max_significant_digits = 5, std::string label = "");


int main()
{
    int main_category = mainMenu();
    int subMenuResult = subMenu(main_category);
    return 0;
}


int mainMenu()
{
    std::cout << "Welcome to the Engineering Miscellaneous Calculations Program!" << std::endl;
    std::cout << "Please select a category of calculations:" << std::endl;
    std::cout   << "1. Flow / Density Related\n"
                << "2. Heat Exchange Related\n"
                << "3. Civil\n"
                << "4. Chemical\n"
                << "5. Environmental\n"
                << "User Entry: ";

    int category;
    std::cin >> category;
    std::cout << std::endl;
    return category;
}


int subMenu(int main_category)
{
    bool has_sub_selection = false;
    int sub_selection = 0;

    switch (main_category)
    {
        case FlowRelated:
            std::cout   << "You selected Flow Related calculations.\n"
                        << "next Enter the specific flow related calculation you want to perform\n"
                        << "1. Density Calculations\n"
                        << "2. Vapor Density / Vapor Flow Conversions\n"
                        << "3. Pressure Drop Calculations\n"
                        << "4. Velocity Limit Calculations\n"
                        << "5. Flow Rate Conversions\n"
                        << "User Entry: ";
            sub_selection = getSubSelection(main_category, 4);
            break;
        case HeatExchangeRelated:
            std::cout << "You selected Heat Exchange Related calculations.\n" << std::endl;
            // Call heat exchange related calculation functions here
            break;
        case Civil:
            std::cout << "You selected Civil calculations.\n" << std::endl;
            // Call civil related calculation functions here
            break;
        case Chemical:
            std::cout << "You selected Chemical calculations." << std::endl;
            // Call chemical related calculation functions here
            break;
        case Environmental:
            std::cout << "You selected Environmental calculations." << std::endl;
            // Call environmental related calculation functions here
            break;
        default:
            std::cout << "Invalid selection. Please restart the program and select a valid category." << std::endl;
            break;
    }

    if (main_category == FlowRelated)
    {
        switch (sub_selection)
        {
        case 1:
            {  
                densityRelated results = densityCalcGivenMw();
            }
            break;

        case 2:
            {
                int result = vaporDensityVaporFlowConversions();
            }
            break;

        default:
            break;
        }
        
    }

    return 0;
}


int getSubSelection(int main_category, int max_option) {
    int sub_selection;
        std::cout << "Please enter your selection: ";
    std::cin >> sub_selection;
    while (sub_selection < 1 || sub_selection > max_option) {
        std::cout << "Invalid selection. Please enter a number between 1 and " << max_option << ": ";
        std::cin >> sub_selection;
    }
    std::cout << std::endl;
    return sub_selection;
}


densityRelated densityCalcGivenMw() {
    densityRelated results;
    // Clear trailing newline left by operator>> before getline-based prompts.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    results.atmospheric_pressure = getUserInput("Enter atmospheric pressure (psia) or Enter for 14.696: ", 14.696);
    results.standard_temperature = getUserInput("Enter standard temperature (F) or Enter for 59: ", 59.0);
    results.molecular_weight = getUserInput("Enter molecular weight (lb/lb-mol): ");
    results.temperature = getUserInput("Enter actual temperature (F): ");
    results.pressure = getUserInput("Enter gauge pressure (psig): ");
    results.compressibility_factor = getUserInput("Enter compressibility factor Z or Enter for 1: ", 1.0);
    results.density = results.molecular_weight * (results.pressure + results.atmospheric_pressure) / (results.compressibility_factor * 10.72 * (results.temperature + 460.0));
    results.std_density = results.molecular_weight * results.atmospheric_pressure / (results.compressibility_factor * 10.72 * (results.standard_temperature + 460.0));
    
    std::cout << "Calculated density: " << formatDoubleWithCommas(results.density, 5, "lb/ft^3");
    std::cout << "Standard density: " << formatDoubleWithCommas(results.std_density, 5, "lb/ft^3");
    std::cout << "Density ratio (actual/standard): " << formatDoubleWithCommas(results.density / results.std_density, 5, "");
    return results; 
}


int vaporDensityVaporFlowConversions() {
    // Clear trailing newline left by operator>> before getline-based prompts.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    densityRelated densRel = densityCalcGivenMw();

    std::cout << "Known Flow Unit:\n"
              << "1. SCFM          5. ACFH          9.  kg/hr\n"
              << "2. ACFM          6. lb/hr         10. kg/min\n"
              << "3. SCFH          7. lb/min        11. kg/s\n"
              << "4. SCFD          8. lb/s\n"
              << "User Entry: ";

    int flow_unit;
    std::cin >> flow_unit;
    while (flow_unit < 1 || flow_unit > 12) {
        std::cout << "Invalid selection. Please enter a number between 1 and 12: ";
        std::cin >> flow_unit;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const std::string unit_names[12] = {"", "SCFM", "ACFM", "SCFH", "SCFD", "ACFH", "lb/hr", "lb/min", "lb/s", "kg/hr", "kg/min", "kg/s"};

    double input_flow = getUserInput("Enter the flow rate in " + unit_names[flow_unit] + ": ");

    // Convert any entered unit to lb/hr, then derive every other unit once.
    const double LB_PER_KG = 2.20462;
    double lb_per_hr = 0.0;

    switch (flow_unit) {
        case 1: lb_per_hr = input_flow * densRel.std_density * 60.0; break;             // SCFM
        case 2: lb_per_hr = input_flow * densRel.density * 60.0; break;                 // ACFM
        case 3: lb_per_hr = (input_flow / 60.0) * densRel.std_density * 60.0; break;    // SCFH
        case 4: lb_per_hr = input_flow * densRel.std_density; break;                    // SCFD
        case 5: lb_per_hr = (input_flow / 60.0) * densRel.density * 60.0; break;        // ACFH
        case 6: lb_per_hr = input_flow; break;                                          // lb/hr
        case 7: lb_per_hr = input_flow * 60.0; break;                                   // lb/min
        case 8: lb_per_hr = input_flow * 3600.0; break;                                 // lb/s
        case 9: lb_per_hr = (input_flow * LB_PER_KG) / 60.0 * 60.0; break;              // kg/hr
        case 10: lb_per_hr = input_flow * LB_PER_KG * 60.0; break;                      // kg/min
        case 11: lb_per_hr = input_flow * LB_PER_KG * 3600.0; break;                    // kg/s
    }

    const double scfm = lb_per_hr / densRel.std_density / 60.0;
    const double acfm = lb_per_hr / densRel.density / 60.0;
    const double scfh = scfm * 60.0;
    const double acfh = acfm * 60.0;
    const double lb_per_min = lb_per_hr / 60.0;
    const double lb_per_sec = lb_per_min / 60.0;
    const double kg_per_hr = lb_per_hr / LB_PER_KG;
    const double kg_per_min = lb_per_min / LB_PER_KG;
    const double kg_per_sec = lb_per_sec / LB_PER_KG;

    std::cout << "\nFlow rate conversions:\n";
    std::cout << "\t" << formatDoubleWithCommas(scfm, 5, "SCFM");
    std::cout << "\t" << formatDoubleWithCommas(acfm, 5, "ACFM");
    std::cout << "\t" << formatDoubleWithCommas(scfh, 5, "SCFH");
    std::cout << "\t" << formatDoubleWithCommas(acfh, 5, "ACFH");
    std::cout << "\t" << formatDoubleWithCommas(lb_per_hr, 5, "lb/hr");
    std::cout << "\t" << formatDoubleWithCommas(lb_per_min, 5, "lb/min");
    std::cout << "\t" << formatDoubleWithCommas(lb_per_sec, 5, "lb/s");
    std::cout << "\t" << formatDoubleWithCommas(kg_per_hr, 5, "kg/hr");
    std::cout << "\t" << formatDoubleWithCommas(kg_per_min, 5, "kg/min");
    std::cout << "\t" << formatDoubleWithCommas(kg_per_sec, 5, "kg/s");
    return 0;
}


double getUserInput(const std::string& prompt, double default_value) {
    while (true) {
        std::cout << prompt;

        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) {
            return default_value;
        }

        try {
            return std::stod(line);
        } catch (...) {
            std::cout << "Invalid numeric input. Press Enter for default or type a valid number.\n";
        }
    }
}


std::string formatDoubleWithCommas(double value, int max_significant_digits, std::string label) {
    if (max_significant_digits < 1) {
        max_significant_digits = 1;
    }

    double abs_value = std::fabs(value);
    int decimals = 0;

    if (abs_value > 0.0) {
        double exponent = std::floor(std::log10(abs_value));
        decimals = max_significant_digits - 1 - static_cast<int>(exponent);
        if (decimals < 0) {
            decimals = 0;
        }
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    std::string text = oss.str();

    std::size_t dot_pos = text.find('.');
    std::size_t int_end = (dot_pos == std::string::npos) ? text.size() : dot_pos;
    std::size_t comma_start = (text[0] == '-') ? 1 : 0;

    for (std::size_t i = int_end; i > comma_start + 3; i -= 3) {
        text.insert(i - 3, ",");
    }

    if (dot_pos != std::string::npos) {
        while (!text.empty() && text.back() == '0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == '.') {
            text.pop_back();
        }
    }

    return text + " " + label + "\n";
}