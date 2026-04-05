#include "../include/heat_exchange_calculations.hpp"

#include "../include/console_menu.hpp"
#include "../include/input_reader.hpp"
#include "../include/number_formatter.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {
constexpr double kPi = 3.14159265358979323846;

void printResult(const std::string& label, double value, const std::string& unit = "") {
    std::cout << label << ": " << NumberFormatter::formatDoubleWithCommas(value, 8, unit) << "\n";
}
}  // namespace

void HeatExchangeCalculations::runHeatExchangeMenu() {
    while (true) {
        const int selection = ConsoleMenu::promptMenu(
            "Heat Exchange Related",
            {
                "1. Area = q / (DT * U)",
                "2. q = mdot * Cp * DT",
                "3. Comparative Exchanger Size (A2)",
                "4. Tube Surface Area and Tube Weight",
                "5. Tube Bundle Flow Area and Velocity",
                "6. Kelix Factor psi/(gpm^2)",
                "7. Baffle Cut Area Percent",
                "8. Average (Mix) Density",
                "9. Exchanger Nozzle Rho*V^2",
                "10. Tube Resistance @ OD",
                "11. Shell Pass P/R/F",
                "12. Tubesheet Thickness (approx)",
                "13. Mixing Temperature",
                "0. Back"
            },
            0,
            13
        );

        switch (selection) {
            case 0:
                return;
            case 1:
                runAreaFromQDtU();
                break;
            case 2:
                runQFromMdotCpDt();
                break;
            case 3:
                runComparativeExchangerSize();
                break;
            case 4:
                runTubeSurfaceAreaAndWeight();
                break;
            case 5:
                runTubeFlowAreaAndVelocity();
                break;
            case 6:
                runKelixFactor();
                break;
            case 7:
                runBaffleCutArea();
                break;
            case 8:
                runAverageMixDensity();
                break;
            case 9:
                runExchangerNozzleRhoV2();
                break;
            case 10:
                runTubeResistanceAtOD();
                break;
            case 11:
                runShellPassCorrections();
                break;
            case 12:
                runTubesheetThickness();
                break;
            case 13:
                runMixingTemperature();
                break;
            default:
                break;
        }
    }
}

void HeatExchangeCalculations::runAreaFromQDtU() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Area = q / (DT * U)");

    const double q_btu_hr = InputReader::getDouble("q (Btu/hr): ", 10000000.0, true);
    const double dt_f = InputReader::getDouble("DT (F): ", 15.0, true);
    const double u = InputReader::getDouble("U (Btu/hr/ft^2/F): ", 125.0, true);

    const double area = (dt_f > 0.0 && u > 0.0) ? (q_btu_hr / (dt_f * u)) : 0.0;

    std::cout << "\nResults\n";
    printResult("A", area, " ft^2");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runQFromMdotCpDt() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("q = mdot * Cp * DT");

    const double mdot = InputReader::getDouble("m dot (#/hr): ", 2497364.0, true);
    const double cp = InputReader::getDouble("Cp (Btu/#-F): ", 0.6, true);
    const double dt_f = InputReader::getDouble("DT (F): ", 0.0, true);

    const double q_btu_hr = mdot * cp * dt_f;

    std::cout << "\nResults\n";
    printResult("q", q_btu_hr, " Btu/hr");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runComparativeExchangerSize() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Comparative Exchanger Size");

    const double a1 = InputReader::getDouble("A1 (SF): ", 1000.0, true);
    const double dt1 = InputReader::getDouble("DT1 (F): ", 150.0, true);
    const double dt2 = InputReader::getDouble("DT2 (F): ", 5000.0, true);
    const double q2 = InputReader::getDouble("Q2 (MM BTU/H): ", 100000.0, true);
    const double q1 = InputReader::getDouble("Q1 (MM BTU/H): ", 200000.0, true);

    const double a2 = (dt2 > 0.0 && q1 > 0.0) ? (a1 * dt1 / dt2 * q2 / q1) : 0.0;

    std::cout << "\nResults\n";
    printResult("A2", a2, " SF");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runTubeSurfaceAreaAndWeight() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Tube Surface Area and Tube Weight");

    const double no_tubes = InputReader::getDouble("No Tubes: ", 676.0, true);
    const double od_in = InputReader::getDouble("Tube OD (in): ", 1.0, true);
    const double thk_in = InputReader::getDouble("Tube Wall/Gage (in): ", 0.165, true);
    const double avg_length_ft = InputReader::getDouble("Avg Length (ft): ", 25.7, true);
    const double steel_density_lb_cuin = InputReader::getDouble("Steel density (lb/in^3): ", 0.2836, true);

    const double area_one_tube_sf = kPi * (od_in / 12.0) * avg_length_ft;
    const double total_area_sf = no_tubes * area_one_tube_sf;

    const double id_in = od_in - 2.0 * thk_in;
    const double annulus_area_sq_in = (id_in > 0.0) ? (kPi / 4.0 * (od_in * od_in - id_in * id_in)) : 0.0;
    const double weight_per_tube = annulus_area_sq_in * avg_length_ft * 12.0 * steel_density_lb_cuin;
    const double total_weight = no_tubes * weight_per_tube;

    std::cout << "\nResults\n";
    printResult("Surface Area", total_area_sf, " SF");
    printResult("Tube Weight", total_weight, " #");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runTubeFlowAreaAndVelocity() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Tube Bundle Flow Area and Velocity");

    const double no_tubes = InputReader::getDouble("No Tubes: ", 1327.0, true);
    const double od_in = InputReader::getDouble("Tube OD (in): ", 0.75, true);
    const double thk_in = InputReader::getDouble("Tube Wall/Gage (in): ", 0.083, true);
    const double flow_lb_hr = InputReader::getDouble("Flow (#/hr): ", 75446.0, true);
    const double rho_lb_cf = InputReader::getDouble("Density (#/cf): ", 2.92, true);

    const double id_in = od_in - 2.0 * thk_in;
    const double area_per_tube_sqft = (id_in > 0.0) ? (kPi / 4.0 * std::pow(id_in / 12.0, 2.0)) : 0.0;
    const double total_area_sqft = no_tubes * area_per_tube_sqft;

    const double vol_flow_cfs = (rho_lb_cf > 0.0) ? (flow_lb_hr / rho_lb_cf / 3600.0) : 0.0;
    const double velocity_fps = (total_area_sqft > 0.0) ? (vol_flow_cfs / total_area_sqft) : 0.0;

    std::cout << "\nResults\n";
    printResult("Flow Area", total_area_sqft, " ft^2");
    printResult("Velocity", velocity_fps, " ft/s");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runKelixFactor() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Kelix Factor psi/(gpm^2)");

    const double flow_lb_hr = InputReader::getDouble("Flow (#/hr): ", 119171.0, true);
    const double dp_psi = InputReader::getDouble("DP (psi): ", 9.613, true);
    const double density_lb_cf = InputReader::getDouble("Density (#/cf): ", 62.4, true);

    const double gpm = (density_lb_cf > 0.0) ? (flow_lb_hr / density_lb_cf / 60.0 * 7.48052) : 0.0;
    const double k = (gpm > 0.0) ? (dp_psi / (gpm * gpm)) : 0.0;

    std::cout << "\nResults\n";
    printResult("GPM", gpm);
    printResult("psi / (gpm^2)", k);
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runBaffleCutArea() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Baffle Cut Area");

    const double cut_percent_dia = InputReader::getDouble("% Cut Dia: ", 21.0, true);
    const double dia_in = InputReader::getDouble("Shell/Baffle Dia (in): ", 36.0, true);

    const double r = dia_in / 2.0;
    const double chord = dia_in * cut_percent_dia / 100.0;
    const double h = chord;
    const double rad_minus_chord = r - chord;

    double segment_area = 0.0;
    if (r > 0.0 && h > 0.0 && h < 2.0 * r) {
        const double theta = 2.0 * std::acos((r - h) / r);
        segment_area = (r * r / 2.0) * (theta - std::sin(theta));
    }

    const double full_area = kPi * r * r;
    const double percent_cut_area = (full_area > 0.0) ? (segment_area / full_area * 100.0) : 0.0;

    std::cout << "\nResults\n";
    printResult("Chord dim", chord, " in");
    printResult("Radius", r, " in");
    printResult("Rad-chord", rad_minus_chord, " in");
    printResult("Chord Area", segment_area, " sq in");
    printResult("Full Circle Area", full_area, " sq in");
    printResult("% Cut Area", percent_cut_area, " %");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runAverageMixDensity() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Average (Mix) Density");

    const double vap_flow = InputReader::getDouble("Vap Flow (#/hr): ", 13477.0, true);
    const double vap_rho = InputReader::getDouble("Vap Rho (#/cf): ", 0.55, true);
    const double liq_flow = InputReader::getDouble("Liq Flow (#/hr): ", 61968.0, true);
    const double liq_rho = InputReader::getDouble("Liq Rho (#/cf): ", 45.03, true);

    const double total_flow = vap_flow + liq_flow;
    const double total_vol_cf_hr = (vap_rho > 0.0 ? vap_flow / vap_rho : 0.0) +
                                   (liq_rho > 0.0 ? liq_flow / liq_rho : 0.0);
    const double rho_mix = (total_vol_cf_hr > 0.0) ? (total_flow / total_vol_cf_hr) : 0.0;

    std::cout << "\nResults\n";
    printResult("Total", total_flow, " #/hr");
    printResult("Rho mix", rho_mix, " #/cf");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runExchangerNozzleRhoV2() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Exchanger Nozzle Rho*V^2");

    const double id_in = InputReader::getDouble("Nozzle ID (in): ", 15.25, true);
    const double rho = InputReader::getDouble("Rho (#/cf): ", 2.92, true);
    const double flow_lb_hr = InputReader::getDouble("Flow (#/hr): ", 75446.0, true);

    const double area_sqft = kPi / 4.0 * std::pow(id_in / 12.0, 2.0);
    const double vol_flow_cfs = (rho > 0.0) ? (flow_lb_hr / rho / 3600.0) : 0.0;
    const double velocity = (area_sqft > 0.0) ? (vol_flow_cfs / area_sqft) : 0.0;
    const double rho_v2 = rho * velocity * velocity;

    std::cout << "\nResults\n";
    printResult("v (calc)", velocity, " ft/s");
    printResult("rho v^2", rho_v2);
    std::cout << "Max guideline with imp plate: 4000\n";
    std::cout << "Max guideline w/o imp plate (single phase): 1500\n";
    std::cout << "Max guideline w/o imp plate (boiling fluids): 500\n";
    std::cout << "Max guideline tubeside: 4000\n";
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runTubeResistanceAtOD() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Tube Resistance @ OD");

    const double d_in = InputReader::getDouble("Tube OD d (in): ", 1.0, true);
    const double k = InputReader::getDouble("Tube k (Btu/hr/ft/F): ", 29.0, true);
    const double t_in = InputReader::getDouble("Wall thickness t (in): ", 0.109, true);

    double rw = 0.0;
    if (k > 0.0 && d_in > 0.0 && d_in > 2.0 * t_in) {
        rw = d_in / (24.0 * k) * std::log(d_in / (d_in - 2.0 * t_in));
    }

    std::cout << "\nResults\n";
    printResult("rw", rw);
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runShellPassCorrections() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Shell Pass P/R/F");

    const double t1_shell_in = InputReader::getDouble("Shell Inlet T1 (F): ", 124.6, true);
    const double t2_shell_out = InputReader::getDouble("Shell Out T2 (F): ", 100.0, true);
    const double t2_tube_out = InputReader::getDouble("Tube Out t2 (F): ", 118.4, true);
    const double t1_tube_in = InputReader::getDouble("Tube In t1 (F): ", 90.0, true);
    const double f_chart = InputReader::getDouble("F (chart): ", 0.88, true);

    const double p = (t2_tube_out - t1_tube_in) / (t1_shell_in - t1_tube_in);
    const double r = (t1_shell_in - t2_shell_out) / (t2_tube_out - t1_tube_in);

    std::cout << "\nResults\n";
    printResult("P", p);
    printResult("R", r);
    printResult("F (chart)", f_chart);
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runTubesheetThickness() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Tubesheet Thickness (approx)");

    const double f = InputReader::getDouble("F: ", 1.0, true);
    const double g = InputReader::getDouble("G: ", 64.0, true);
    const double p = InputReader::getDouble("P (psi): ", 100.0, true);
    const double s = InputReader::getDouble("S (psi): ", 20000.0, true);
    const double tube_od = InputReader::getDouble("Tube OD (in): ", 0.75, true);
    const double pitch = InputReader::getDouble("Pitch (in): ", 0.75, true);
    const double ca = InputReader::getDouble("Corrosion Allowance CA (in): ", 0.25, true);

    const double n = (pitch > 0.0) ? (0.41 - 0.177 * (tube_od / pitch)) : 0.0;
    const double t = (s > 0.0) ? (f * g * std::sqrt(p / s) + n) : 0.0;
    const double t_corr = t + ca;

    std::cout << "\nResults\n";
    printResult("n", n);
    printResult("T", t, " in");
    printResult("T corr", t_corr, " in");
    ConsoleMenu::pause();
}

void HeatExchangeCalculations::runMixingTemperature() {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader("Mixing Temperature");

    const double flow_a_inline = InputReader::getDouble("Flow A -Inline (#/hr): ", 23526.0, true);
    const double flow_b_bypass = InputReader::getDouble("Flow B -Bypass (#/hr): ", 30346.0, true);
    const double temp_a = InputReader::getDouble("Temp A (F): ", 350.0, true);
    const double cp_a = InputReader::getDouble("Cp A: ", 0.5515, true);
    const double temp_b = InputReader::getDouble("Temp B (F): ", 606.0, true);
    const double cp_b = InputReader::getDouble("Cp B: ", 0.665, true);

    std::cout << "Optional Trial Mix Temp (F) for Heat Balance check, Enter to skip: ";
    std::string trial_line;
    std::getline(std::cin, trial_line);

    bool has_trial_temp = false;
    double trial_mix_temp = 0.0;
    if (!trial_line.empty()) {
        try {
            std::size_t parsed = 0;
            trial_mix_temp = std::stod(trial_line, &parsed);
            if (parsed == trial_line.size()) {
                has_trial_temp = true;
            }
        } catch (...) {
            std::cout << "Invalid trial temperature; skipping trial heat-balance check.\n";
        }
    }

    const double flow_total  = flow_a_inline + flow_b_bypass;
    const double mcp_a       = flow_a_inline * cp_a;
    const double mcp_b       = flow_b_bypass * cp_b;
    const double q_a         = mcp_a * temp_a;
    const double q_b         = mcp_b * temp_b;
    const double denom       = mcp_a + mcp_b;
    const double cp_mix      = (flow_total > 0.0) ? (denom / flow_total) : 0.0;
    const double mix_temp    = (denom > 0.0) ? ((q_a + q_b) / denom) : 0.0;
    const double heat_balance = q_a + q_b - flow_total * cp_mix * mix_temp;

    std::cout << "\n--- Intermediate Steps ---\n";
    printResult("m_A * Cp_A", mcp_a);
    printResult("m_B * Cp_B", mcp_b);
    printResult("m_A * Cp_A * T_A", q_a, " Btu/hr");
    printResult("m_B * Cp_B * T_B", q_b, " Btu/hr");
    printResult("Numerator (sum)", q_a + q_b, " Btu/hr");
    printResult("Denominator (mCpA+mCpB)", denom);

    std::cout << "\n--- Results ---\n";
    printResult("Mix Cp (mass-weighted)", cp_mix);
    printResult("Mix Temperature (HB=0)", mix_temp, " F");
    printResult("Heat Balance @ Mix Temp", heat_balance, " Btu/hr");

    if (has_trial_temp) {
        const double trial_heat_balance = q_a + q_b - flow_total * cp_mix * trial_mix_temp;
        std::cout << "\n--- Trial Temperature Check ---\n";
        printResult("Trial Mix Temperature", trial_mix_temp, " F");
        printResult("Heat Balance @ Trial Temp", trial_heat_balance, " Btu/hr");
    }

    ConsoleMenu::pause();
}
