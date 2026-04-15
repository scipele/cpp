#include "../include/pipe_data_lookup.hpp"

#include "../include/console_menu.hpp"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

namespace {
constexpr int kPipeRows = 37;
constexpr int kPipeCols = 16;
constexpr double kPi = 3.14159265358979323846;
constexpr double kSteelDensityLbPerCuIn = 0.2836;
constexpr double kWaterDensityLbPerCuFt = 62.4;

const std::string kPipeTable[kPipeRows][kPipeCols] = {
    {"NPS", "OD", "5", "10", "20", "30", "40", "STD", "60", "80", "XS", "100", "120", "140", "160", "XXS"},
    {"0.125", "0.405", ".035", ".049", "0", "0", ".068", ".068", "0", ".095", ".095", "0", "0", "0", "0", "0"},
    {"0.25", "0.54", ".049", ".065", "0", "0", ".088", ".088", "0", ".119", ".119", "0", "0", "0", "0", "0"},
    {"0.375", "0.675", ".049", ".065", "0", "0", ".091", ".091", "0", ".126", ".126", "0", "0", "0", "0", "0"},
    {"0.5", "0.84", ".065", ".083", "0", "0", ".109", ".109", "0", ".147", ".147", "0", "0", "0", ".187", ".294"},
    {"0.75", "1.05", ".065", ".083", "0", "0", ".113", ".113", "0", ".154", ".154", "0", "0", "0", ".218", ".308"},
    {"1", "1.315", ".065", ".109", "0", "0", ".133", ".133", "0", ".179", ".179", "0", "0", "0", ".250", ".358"},
    {"1.25", "1.66", ".065", ".109", "0", "0", ".140", ".140", "0", ".191", ".191", "0", "0", "0", ".250", ".382"},
    {"1.5", "1.9", ".065", ".109", "0", "0", ".145", ".145", "0", ".200", ".200", "0", "0", "0", ".281", ".400"},
    {"2", "2.375", ".065", ".109", "0", "0", ".154", ".154", "0", ".218", ".218", "0", "0", "0", ".343", ".436"},
    {"2.5", "2.875", ".083", ".120", "0", "0", ".203", ".203", "0", ".276", ".276", "0", "0", "0", ".375", ".552"},
    {"3", "3.5", ".083", ".120", "0", "0", ".216", ".216", "0", ".300", ".300", "0", "0", "0", ".437", ".600"},
    {"3.5", "4", ".083", ".120", "0", "0", ".226", ".226", "0", ".318", ".318", "0", "0", "0", "0", ".636"},
    {"4", "4.5", ".083", ".120", "0", "0", ".237", ".237", ".281", ".337", ".337", "0", ".437", "0", ".531", ".674"},
    {"4.5", "5", "0", "0", "0", "0", "0", ".247", "0", "0", ".355", "0", "0", "0", "0", ".710"},
    {"5", "5.563", ".109", ".134", "0", "0", ".258", ".258", "0", ".375", ".375", "0", ".500", "0", ".625", ".750"},
    {"6", "6.625", ".109", ".134", "0", "0", ".280", ".280", "0", ".432", ".432", "0", ".562", "0", ".718", ".864"},
    {"7", "7.625", "0", "0", "0", "0", "0", ".301", "0", "0", ".500", "0", "0", "0", "0", ".875"},
    {"8", "8.625", ".109", ".148", ".250", ".277", ".322", ".322", ".406", ".500", ".500", ".593", ".718", ".812", ".906", ".875"},
    {"9", "9.625", "0", "0", "0", "0", "0", ".342", "0", "0", ".500", "0", "0", "0", "0", "0"},
    {"10", "10.75", ".134", ".165", ".250", ".307", ".365", ".365", ".500", ".593", ".500", ".718", ".843", "1.000", "1.125", "0"},
    {"11", "11.75", "0", "0", "0", "0", "0", ".375", "0", "0", ".500", "0", "0", "0", "0", "0"},
    {"12", "12.75", ".165", ".180", ".250", ".330", ".406", ".375", ".562", ".688", ".500", ".843", "1.000", "1.125", "1.312", "0"},
    {"14", "14", "0", ".250", ".312", ".375", ".437", ".375", ".593", ".750", ".500", ".937", "1.093", "1.250", "1.406", "0"},
    {"16", "16", "0", ".250", ".312", ".375", ".500", ".375", ".656", ".843", ".500", "1.031", "1.218", "1.437", "1.593", "0"},
    {"18", "18", "0", ".250", ".312", ".437", ".562", ".375", ".750", ".937", ".500", "1.156", "1.375", "1.562", "1.781", "0"},
    {"20", "20", "0", ".250", ".375", ".500", ".593", ".375", ".812", "1.031", ".500", "1.280", "1.500", "1.750", "1.968", "0"},
    {"22", "22", "0", ".250", ".375", ".500", "0", ".375", ".875", "1.125", ".500", "1.375", "1.625", "1.875", "2.125", "0"},
    {"24", "24", "0", ".250", ".375", ".562", ".688", ".375", ".968", "1.218", ".500", "1.531", "1.812", "2.062", "2.343", "0"},
    {"26", "26", "0", ".312", ".500", "0", "0", ".375", "0", "0", ".500", ".750", "1.000", "1.250", "1.500", "2.000"},
    {"28", "28", "0", ".312", ".500", ".625", "0", ".375", "0", "0", ".500", ".750", "1.000", "1.250", "1.500", "2.000"},
    {"30", "30", "0", ".312", ".500", ".625", "0", ".375", "0", "0", ".500", ".750", "1.000", "1.250", "1.500", "2.000"},
    {"32", "32", "0", ".312", ".500", ".625", ".688", ".375", "0", "0", ".500", ".750", "1.000", "1.250", "1.500", "2.000"},
    {"34", "34", "0", ".312", ".500", ".625", ".688", ".375", "0", "0", ".500", ".750", "1.000", "1.250", "1.500", "2.000"},
    {"36", "36", "0", ".312", ".500", ".625", ".750", ".375", "0", "0", ".500", ".875", "1.000", "1.250", "1.500", "2.000"},
    {"42", "42", "0", "0", "0", "0", "0", ".375", "0", "0", ".500", ".750", "1.000", "1.250", "1.500", "2.000"},
    {"48", "48", "0", "0", "0", "0", "0", ".375", "0", "0", ".500", ".750", "1.000", "1.250", "1.500", "2.000"}
};
}  // namespace

std::string PipeDataLookup::normalizeKey(const std::string& value) {
    std::string normalized = value;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return normalized;
}

PipeLookupResult PipeDataLookup::lookupByNominalAndSchedule(const std::string& nominal_size,
                                                            const std::string& schedule) const {
    PipeLookupResult result;
    result.nominal_size = nominal_size;
    result.schedule = normalizeKey(schedule);

    std::map<std::string, int> size_map;
    std::map<std::string, int> schedule_map;

    for (int row = 1; row < kPipeRows; ++row) {
        size_map[kPipeTable[row][0]] = row;
    }

    for (int col = 2; col < kPipeCols; ++col) {
        schedule_map[normalizeKey(kPipeTable[0][col])] = col;
    }

    const auto size_it = size_map.find(result.nominal_size);
    if (size_it == size_map.end()) {
        result.error_message = "No matching nominal size was found.";
        return result;
    }

    const auto sched_it = schedule_map.find(result.schedule);
    if (sched_it == schedule_map.end()) {
        result.error_message = "No matching schedule was found.";
        return result;
    }

    const int size_index = size_it->second;
    const int sched_index = sched_it->second;

    result.outside_diameter_in = std::stod(kPipeTable[size_index][1]);
    result.wall_thickness_in = std::stod(kPipeTable[size_index][sched_index]);

    if (result.wall_thickness_in <= 0.0) {
        result.error_message = "That nominal size and schedule combination is not available.";
        return result;
    }

    result.inside_diameter_in = result.outside_diameter_in - (2.0 * result.wall_thickness_in);

    if (result.inside_diameter_in <= 0.0) {
        result.error_message = "Calculated inside diameter is not valid.";
        return result;
    }

    result.empty_weight_lb_per_ft = (kPi / 4.0) *
                                    (std::pow(result.outside_diameter_in, 2.0) - std::pow(result.inside_diameter_in, 2.0)) *
                                    12.0 * kSteelDensityLbPerCuIn;
    result.water_weight_lb_per_ft = (kPi / 4.0) * std::pow(result.inside_diameter_in / 12.0, 2.0) *
                                    kWaterDensityLbPerCuFt;
    result.full_weight_lb_per_ft = result.empty_weight_lb_per_ft + result.water_weight_lb_per_ft;

    result.success = true;
    return result;
}

void PipeDataLookup::runInteractive() const {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Pipe Data Lookup");

    std::cout << "1. Enter Nominal Pipe size in Decimal Format (0.5,1.25,2,3,6,8,12...) --> ";
    std::string nominal_size;
    std::getline(std::cin, nominal_size);

    std::cout << "2. Enter Pipe Schedule (std,xs,xxs,5,10,20,30,40,60,80,120,140,160) --> ";
    std::string schedule;
    std::getline(std::cin, schedule);

    PipeLookupResult result = lookupByNominalAndSchedule(nominal_size, schedule);

    if (!result.success) {
        std::cout << "\nError: " << result.error_message << "\n";
        ConsoleMenu::pause();
        return;
    }

    std::cout << "\n========== PIPE DATA ==========" << "\n";
    std::cout << "\tEntered NPS =          " << std::setw(10) << std::left << result.nominal_size << "\n";
    std::cout << "\tEntered Schedule =     " << std::setw(10) << std::left << result.schedule << "\n";
    std::cout << "\tPipe OD =              " << result.outside_diameter_in << "\n";
    std::cout << "\tPipe Thickness =       " << result.wall_thickness_in << "\n";

    std::cout << "\n========== EXTENDED DATA ==========" << "\n";
    std::cout << "\tPipe ID =              " << result.inside_diameter_in << "\n";
    std::cout << "\tPipe Empty Wt/Ft =     " << result.empty_weight_lb_per_ft << "\n";
    std::cout << "\tWater #/Ft =           " << result.water_weight_lb_per_ft << "\n";
    std::cout << "\tPipe Full H2O #/Ft =   " << result.full_weight_lb_per_ft << "\n";

    ConsoleMenu::pause();
}
