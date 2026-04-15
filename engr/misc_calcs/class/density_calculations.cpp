#include "../include/density_calculations.hpp"
#include "../include/console_menu.hpp"
#include "../include/input_reader.hpp"
#include "../include/number_formatter.hpp"

#include <iostream>
#include <vector>

void DensityCalculations::runDensityCalculation() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Density Calculation (Given Molecular Weight)");

    const DensityRelated results = densityCalcGivenMw();

    std::cout << "\nCalculated density: "
              << NumberFormatter::formatDoubleWithCommas(results.density, 5, "lb/ft^3") << "\n";
    std::cout << "Standard density: "
              << NumberFormatter::formatDoubleWithCommas(results.std_density, 5, "lb/ft^3") << "\n";
    std::cout << "Density ratio (actual/standard): "
              << NumberFormatter::formatDoubleWithCommas(results.density / results.std_density, 5) << "\n";

    ConsoleMenu::pause();
}

void DensityCalculations::runVaporDensityVaporFlowConversions() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Vapor Density / Vapor Flow Conversions");

    std::cout << "Enter conditions used to calculate actual and standard density.\n\n";
    const DensityRelated dens_rel = densityCalcGivenMw();

    const std::vector<std::string> flow_menu = {
        "Known Flow Unit:",
        "1. SCFM          5. ACFH          9.  kg/hr",
        "2. ACFM          6. lb/hr         10. kg/min",
        "3. SCFH          7. lb/min        11. kg/s",
        "4. SCFD          8. lb/s"
    };

    for (const std::string& line : flow_menu) {
        std::cout << line << "\n";
    }

    const int flow_unit = InputReader::getIntInRange("Selection: ", 1, 11);

    const std::string unit_names[12] = {
        "", "SCFM", "ACFM", "SCFH", "SCFD", "ACFH", "lb/hr", "lb/min", "lb/s", "kg/hr", "kg/min", "kg/s"
    };

    const double input_flow = InputReader::getDouble("Enter the flow rate in " + unit_names[flow_unit] + ": ", 0.0, false);

    const double LB_PER_KG = 2.20462;
    double lb_per_hr = 0.0;

    switch (flow_unit) {
        case 1: lb_per_hr = input_flow * dens_rel.std_density * 60.0; break;
        case 2: lb_per_hr = input_flow * dens_rel.density * 60.0; break;
        case 3: lb_per_hr = input_flow * dens_rel.std_density; break;
        case 4: lb_per_hr = (input_flow / 24.0) * dens_rel.std_density; break;
        case 5: lb_per_hr = input_flow * dens_rel.density; break;
        case 6: lb_per_hr = input_flow; break;
        case 7: lb_per_hr = input_flow * 60.0; break;
        case 8: lb_per_hr = input_flow * 3600.0; break;
        case 9: lb_per_hr = input_flow * LB_PER_KG; break;
        case 10: lb_per_hr = input_flow * LB_PER_KG * 60.0; break;
        case 11: lb_per_hr = input_flow * LB_PER_KG * 3600.0; break;
    }

    const double scfm = lb_per_hr / dens_rel.std_density / 60.0;
    const double acfm = lb_per_hr / dens_rel.density / 60.0;
    const double scfh = scfm * 60.0;
    const double scfd = scfh * 24.0;
    const double acfh = acfm * 60.0;
    const double lb_per_min = lb_per_hr / 60.0;
    const double lb_per_sec = lb_per_min / 60.0;
    const double kg_per_hr = lb_per_hr / LB_PER_KG;
    const double kg_per_min = lb_per_min / LB_PER_KG;
    const double kg_per_sec = lb_per_sec / LB_PER_KG;

    std::cout << "\nFlow rate conversions:\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(scfm, 5, "SCFM") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(acfm, 5, "ACFM") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(scfh, 5, "SCFH") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(scfd, 5, "SCFD") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(acfh, 5, "ACFH") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(lb_per_hr, 5, "lb/hr") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(lb_per_min, 5, "lb/min") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(lb_per_sec, 5, "lb/s") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(kg_per_hr, 5, "kg/hr") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(kg_per_min, 5, "kg/min") << "\n";
    std::cout << "  " << NumberFormatter::formatDoubleWithCommas(kg_per_sec, 5, "kg/s") << "\n";

    ConsoleMenu::pause();
}

DensityRelated DensityCalculations::densityCalcGivenMw() {
    DensityRelated results;
    results.atmospheric_pressure = InputReader::getDouble("Enter atmospheric pressure (psia) or Enter for 14.696: ", 14.696, true);
    results.standard_temperature = InputReader::getDouble("Enter standard temperature (F) or Enter for 59: ", 59.0, true);
    results.molecular_weight = InputReader::getDouble("Enter molecular weight (lb/lb-mol): ", 0.0, false);
    results.temperature = InputReader::getDouble("Enter actual temperature (F): ", 0.0, false);
    results.pressure = InputReader::getDouble("Enter gauge pressure (psig): ", 0.0, false);
    results.compressibility_factor = InputReader::getDouble("Enter compressibility factor Z or Enter for 1: ", 1.0, true);

    results.density = results.molecular_weight * (results.pressure + results.atmospheric_pressure)
        / (results.compressibility_factor * 10.72 * (results.temperature + 460.0));
    results.std_density = results.molecular_weight * results.atmospheric_pressure
        / (results.compressibility_factor * 10.72 * (results.standard_temperature + 460.0));

    return results;
}
