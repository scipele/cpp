#include "../include/engineering_calc_app.hpp"
#include "../include/calc_types.hpp"
#include "../include/console_menu.hpp"

#include <iostream>

void EngineeringCalcApp::run() {
    while (true) {
        const int category = ConsoleMenu::promptMenu(
            "Engineering Miscellaneous Calculations",
            {
                "1. Flow / Density Related",
                "2. Heat Exchange Related",
                "3. Civil",
                "4. Chemical",
                "5. Environmental",
                "0. Exit"
            },
            0,
            5
        );

        if (category == ExitProgram) {
            ConsoleMenu::clearScreen();
            std::cout << "Goodbye.\n";
            return;
        }

        handleMainCategory(category);
    }
}

void EngineeringCalcApp::handleMainCategory(int category) {
    switch (category) {
        case FlowRelated:
            runFlowRelatedMenu();
            break;
        case HeatExchangeRelated:
            showNotImplemented("Heat Exchange Related");
            break;
        case Civil:
            showNotImplemented("Civil");
            break;
        case Chemical:
            showNotImplemented("Chemical");
            break;
        case Environmental:
            showNotImplemented("Environmental");
            break;
        default:
            break;
    }
}

void EngineeringCalcApp::runFlowRelatedMenu() {
    while (true) {
        const int selection = ConsoleMenu::promptMenu(
            "Flow / Density Related",
            {
                "1. Density Calculations",
                "2. Vapor Density / Vapor Flow Conversions",
                "3. Pressure Drop Calculations",
                "4. Velocity Limit Calculations",
                "5. Flow Rate Conversions",
                "0. Back"
            },
            0,
            5
        );

        switch (selection) {
            case 0:
                return;
            case 1:
                density_calcs_.runDensityCalculation();
                break;
            case 2:
                density_calcs_.runVaporDensityVaporFlowConversions();
                break;
            case 3:
            case 4:
            case 5:
                showNotImplemented("This flow-related option");
                break;
            default:
                break;
        }
    }
}

void EngineeringCalcApp::showNotImplemented(const std::string& area_name) {
    ConsoleMenu::clearScreen();
    ConsoleMenu::printHeader(area_name);
    std::cout << "This section is not implemented yet.\n";
    ConsoleMenu::pause();
}
