#pragma once

#include "calc_types.hpp"

class DensityCalculations {
public:
    void runDensityCalculation();
    void runVaporDensityVaporFlowConversions();

private:
    DensityRelated densityCalcGivenMw();
};
