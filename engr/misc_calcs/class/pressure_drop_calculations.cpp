#include "../include/pressure_drop_calculations.hpp"
#include "../include/console_menu.hpp"
#include "../include/input_reader.hpp"
#include "../include/number_formatter.hpp"

#include <iostream>
#include <cmath>
#include <iomanip>

void PressureDropCalculations::runPressureDropCalculation() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Pressure Drop Calculation");

    const PressureDropRelated results = calculatePressureDrop();

    std::cout << "\n========== FLOW PROPERTIES ==========\n";
    std::cout << "Pipe Inner Diameter: " 
              << NumberFormatter::formatDoubleWithCommas(results.pipe_inner_diameter, 2, " in") << "\n";
    std::cout << "Mass Flow Rate: " 
              << NumberFormatter::formatDoubleWithCommas(results.mass_flow_rate, 1, " lb/hr") << "\n";
    std::cout << "Fluid Density: " 
              << NumberFormatter::formatDoubleWithCommas(results.fluid_density, 4, " lb/cf") << "\n";
    std::cout << "Viscosity: " 
              << NumberFormatter::formatDoubleWithCommas(results.viscosity, 3, " cp") << "\n";
    std::cout << "Equivalent Length: " 
              << NumberFormatter::formatDoubleWithCommas(results.equivalent_length, 1, " ft") << "\n";

    std::cout << "\n========== CALCULATED RESULTS ==========\n";
    std::cout << "Reynolds Number: " 
              << NumberFormatter::formatDoubleWithCommas(results.reynolds_number, 0) << "\n";
    std::cout << "Friction Factor: " 
              << NumberFormatter::formatDoubleWithCommas(results.friction_factor, 6) << "\n";
    std::cout << "Velocity: " 
              << NumberFormatter::formatDoubleWithCommas(results.velocity, 2, " ft/s") << "\n";
    std::cout << "Economic Size: " 
              << NumberFormatter::formatDoubleWithCommas(results.economic_size, 2, " in") << "\n";

    std::cout << "\n========== PRESSURE DROP ==========\n";
    std::cout << "PD per 100 ft: " 
              << NumberFormatter::formatDoubleWithCommas(results.pressure_drop_per_100ft, 5, " psig/100ft") << "\n";
    std::cout << "Total Pressure Drop: " 
              << NumberFormatter::formatDoubleWithCommas(results.pressure_drop_psig, 5, " psig") << "\n";
    std::cout << "Inlet Pressure: " 
              << NumberFormatter::formatDoubleWithCommas(results.inlet_pressure, 3, " psig") << "\n";
    std::cout << "Outlet Pressure: " 
              << NumberFormatter::formatDoubleWithCommas(results.outlet_pressure, 3, " psig") << "\n";

    std::cout << "\n========== ADDITIONAL PARAMETERS ==========\n";
    std::cout << "Max Noise Velocity Limit: " 
              << NumberFormatter::formatDoubleWithCommas(results.max_noise_velocity, 1, " ft/s") << "\n";
    std::cout << "Erosion Velocity Limit: " 
              << NumberFormatter::formatDoubleWithCommas(results.erosion_velocity, 1, " ft/s") << "\n";
    std::cout << "Mach Number: " 
              << NumberFormatter::formatDoubleWithCommas(results.mach_number, 4) << "\n";

    if (results.hazen_williams_c > 0) {
        std::cout << "\n========== HAZEN-WILLIAMS (WATER ONLY) ==========\n";
        std::cout << "HW C Coefficient: " 
                  << NumberFormatter::formatDoubleWithCommas(results.hazen_williams_c, 1) << "\n";
        std::cout << "HW PD per 100 ft: " 
                  << NumberFormatter::formatDoubleWithCommas(results.hw_pressure_drop_per_100ft, 5, " psig/100ft") << "\n";
    }

    ConsoleMenu::pause();
}

PressureDropRelated PressureDropCalculations::calculatePressureDrop() {
    PressureDropRelated results{};

    std::cout << "Enter pipe and flow parameters:\n\n";
    
    results.pipe_inner_diameter = InputReader::getDouble("Pipe Inner Diameter (in): ", 0.1, false);
    results.mass_flow_rate = InputReader::getDouble("Mass Flow Rate (lb/hr): ", 0.1, false);
    results.temperature = InputReader::getDouble("Temperature (F): ", 32.0, false);
    results.inlet_pressure = InputReader::getDouble("Inlet Pressure (psig): ", 0.0, true);
    results.fluid_density = InputReader::getDouble("Fluid Density (lb/cf): ", 0.01, false);
    results.viscosity = InputReader::getDouble("Viscosity (cp): ", 0.01, false);
    results.equivalent_length = InputReader::getDouble("Equivalent Length (ft): ", 1.0, false);
    results.roughness = InputReader::getDouble("Absolute Roughness (ft), or Enter for commercial steel (0.00015): ", 0.00015, true);
    results.cp_cv_ratio = InputReader::getDouble("Cp/Cv Ratio, or Enter for 1.33: ", 1.33, true);
    results.hazen_williams_c = InputReader::getDouble("Hazen Williams C (0 if not applicable): ", 0.0, true);

    // Calculate Reynolds Number
    results.reynolds_number = calculateReynoldsNumber(results.mass_flow_rate, 
                                                      results.pipe_inner_diameter, 
                                                      results.viscosity);

    // Calculate relative roughness
    double relative_roughness = (results.pipe_inner_diameter > 0) ? 
                                results.roughness / (results.pipe_inner_diameter / 12.0) : 0;

    // Calculate Friction Factor
    results.friction_factor = calculateFrictionFactor(results.reynolds_number, relative_roughness);

    // Calculate Velocity
    results.velocity = calculateVelocity(results.mass_flow_rate, 
                                         results.fluid_density, 
                                         results.pipe_inner_diameter);

    // Calculate Pressure Drop per 100 ft
    results.pressure_drop_per_100ft = calculatePressureDropPer100ft(results.friction_factor,
                                                                     results.mass_flow_rate,
                                                                     results.pipe_inner_diameter,
                                                                     results.fluid_density);

    // Calculate Total Pressure Drop
    results.pressure_drop_psig = results.pressure_drop_per_100ft * results.equivalent_length / 100.0;

    // Calculate Outlet Pressure
    results.outlet_pressure = results.inlet_pressure - results.pressure_drop_psig;

    // Calculate Economic Size: 1.75 * D^0.15 * sqrt((W/D/60)/rho)
    if (results.pipe_inner_diameter > 0 && results.fluid_density > 0) {
        results.economic_size = 1.75 * std::pow(results.pipe_inner_diameter, 0.15) * 
                                std::sqrt((results.mass_flow_rate / results.pipe_inner_diameter / 60.0) / 
                                         results.fluid_density);
    }

    // Calculate Max Noise Velocity: 200 / sqrt(density)
    if (results.fluid_density > 0) {
        results.max_noise_velocity = 200.0 / std::sqrt(results.fluid_density);
        results.erosion_velocity = 200.0 / std::sqrt(results.fluid_density);
    }

    // Calculate Mach Number
    if (results.velocity > 0 && results.cp_cv_ratio > 0) {
        const double GRAVITY = 32.2;  // ft/s^2
        const double PSI_TO_PSF = 144.0;
        const double PSI_ABS_STANDARD = 14.696;
        
        double abs_pressure = results.inlet_pressure + PSI_ABS_STANDARD;
        double speed_of_sound = std::sqrt(results.cp_cv_ratio * GRAVITY * PSI_TO_PSF * abs_pressure * 
                                         144.0 / results.fluid_density);
        if (speed_of_sound > 0) {
            results.mach_number = results.velocity / speed_of_sound;
        }
    }

    // Calculate Hazen-Williams pressure drop if applicable
    if (results.hazen_williams_c > 0) {
        // Convert density and flow to gpm (assuming water-like liquid)
        // For water: density ≈ 62.4 lb/cf, so gpm = lb_hr / 62.4 / 60 * 7.48
        double flow_gpm = results.mass_flow_rate / results.fluid_density / 60.0 * 7.48;
        results.hw_pressure_drop_per_100ft = calculateHazenWilliamsPressureDrop(
            results.pipe_inner_diameter, flow_gpm, 100.0);
    }

    return results;
}

double PressureDropCalculations::calculateReynoldsNumber(double mass_flow_lb_hr, 
                                                         double diameter_in, 
                                                         double viscosity_cp) {
    // Nre = 6.31 * W / (mu * D)
    // W = mass flow in lb/hr, mu = viscosity in cp, D = diameter in inches
    if (diameter_in > 0 && viscosity_cp > 0) {
        return 6.31 * mass_flow_lb_hr / (viscosity_cp * diameter_in);
    }
    return 0.0;
}

double PressureDropCalculations::calculateFrictionFactor(double reynolds_number, 
                                                         double relative_roughness) {
    // For laminar flow (Re < 2000)
    if (reynolds_number < 2000.0) {
        if (reynolds_number > 0) {
            return std::max(64.0 / reynolds_number, 0.04);
        }
        return 0.04;
    }

    // For turbulent flow, solve Colebrook-White for Darcy friction factor:
    // 1/sqrt(f) = -2*log10((e/D)/3.7 + 2.51/(Re*sqrt(f)))
    const double rr = std::max(relative_roughness, 0.0);
    double f = 0.02;  // reasonable starting guess for commercial steel

    for (int i = 0; i < 50; ++i) {
        const double sqrt_f = std::sqrt(f);
        const double arg = (rr / 3.7) + (2.51 / (reynolds_number * sqrt_f));

        if (arg <= 0.0) {
            break;
        }

        const double inv_sqrt_f = -2.0 * std::log10(arg);
        if (inv_sqrt_f <= 0.0) {
            break;
        }

        const double f_new = 1.0 / (inv_sqrt_f * inv_sqrt_f);
        if (std::abs(f_new - f) < 1e-10) {
            f = f_new;
            break;
        }

        f = f_new;
    }

    return std::max(f, 0.008);  // retain historical floor guard
}

double PressureDropCalculations::calculateVelocity(double mass_flow_lb_hr, 
                                                   double density_lb_cf, 
                                                   double diameter_in) {
    // VEL FPS = (W / rho / 3600) / ((D/2)^2 * PI / 144)
    // Simplifies to: VEL = (W / rho / 3600) * 144 / ((D/2)^2 * PI)
    // = W * 144 / (rho * 3600 * (D/2)^2 * PI)
    
    const double PI = 3.14159265359;
    
    if (diameter_in > 0 && density_lb_cf > 0) {
        double diameter_ft = diameter_in / 12.0;
        double cross_section_area_sq_ft = ((diameter_ft / 2.0) * (diameter_ft / 2.0)) * PI;
        double velocity_cubic_ft_per_sec = mass_flow_lb_hr / density_lb_cf / 3600.0;
        
        if (cross_section_area_sq_ft > 0) {
            return velocity_cubic_ft_per_sec / cross_section_area_sq_ft;
        }
    }
    return 0.0;
}

double PressureDropCalculations::calculatePressureDropPer100ft(double friction_factor, 
                                                               double mass_flow_lb_hr, 
                                                               double diameter_in, 
                                                               double density_lb_cf) {
    // PD/100ft = 0.000336 * f * W^2 / D^5 / rho
    // where f = friction factor, W = lb/hr, D = diameter in inches, rho = density in lb/cf
    
    if (diameter_in > 0 && density_lb_cf > 0) {
        double diameter_5th = std::pow(diameter_in, 5.0);
        return 0.000336 * friction_factor * (mass_flow_lb_hr * mass_flow_lb_hr) / 
               (diameter_5th * density_lb_cf);
    }
    return 0.0;
}

double PressureDropCalculations::calculateHazenWilliamsPressureDrop(double diameter_in, 
                                                                    double flow_gpm, 
                                                                    double length_ft) {
    // Hazen-Williams equation for water flow
    // PD = 0.2083 * (100/C)^1.85 * (Q/8.33/60)^1.85 / D^4.8655 / 2.31
    // Simplified for pressure drop per 100 ft:
    // PD/100ft = 0.2083 * (100/C)^1.85 * (Q)^1.85 / (8.33*60)^1.85 / D^4.8655 / 2.31
    
    if (diameter_in > 0 && flow_gpm > 0) {
        double c_factor = 120.0;  // typical commercial steel for water
        double diameter_ft = diameter_in / 12.0;
        double gpm_normalized = flow_gpm / (8.33 * 60.0);
        
        double c_factor_term = std::pow(100.0 / c_factor, 1.85);
        double flow_term = std::pow(flow_gpm, 1.85);
        double diameter_term = std::pow(diameter_ft, 4.8655);
        
        return 0.2083 * c_factor_term * flow_term / diameter_term / 2.31 / 100.0;
    }
    return 0.0;
}
