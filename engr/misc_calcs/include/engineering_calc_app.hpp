#pragma once

#include "density_calculations.hpp"
#include "pressure_drop_calculations.hpp"

#include <string>

class EngineeringCalcApp {
public:
    void run();

private:
    DensityCalculations density_calcs_;
    PressureDropCalculations pressure_drop_calcs_;

    void handleMainCategory(int category);
    void runFlowRelatedMenu();
    void showNotImplemented(const std::string& area_name);
};
