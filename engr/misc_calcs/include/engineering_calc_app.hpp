#pragma once

#include "density_calculations.hpp"
#include "heat_exchange_calculations.hpp"
#include "pipe_data_lookup.hpp"
#include "pressure_drop_calculations.hpp"

#include <string>

class EngineeringCalcApp {
public:
    void run();

private:
    DensityCalculations density_calcs_;
    HeatExchangeCalculations heat_exchange_calcs_;
    PipeDataLookup pipe_data_lookup_;
    PressureDropCalculations pressure_drop_calcs_;

    void handleMainCategory(int category);
    void runFlowRelatedMenu();
    void showNotImplemented(const std::string& area_name);
};
