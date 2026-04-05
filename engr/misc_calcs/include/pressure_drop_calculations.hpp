#pragma once

#include "calc_types.hpp"

class PressureDropCalculations {
public:
    void runPressureDropCalculation();

private:
    PressureDropRelated calculatePressureDrop();
    double calculateReynoldsNumber(double mass_flow_lb_hr, double diameter_in, double viscosity_cp);
    double calculateFrictionFactor(double reynolds_number, double relative_roughness);
    double calculateVelocity(double mass_flow_lb_hr, double density_lb_cf, double diameter_in);
    double calculatePressureDropPer100ft(double friction_factor, double mass_flow_lb_hr, 
                                         double diameter_in, double density_lb_cf);
    double calculateHazenWilliamsPressureDrop(double diameter_in, double flow_gpm, double length_ft);
};
