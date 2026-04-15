#pragma once

enum MainCategory {
    ExitProgram = 0,
    FlowRelated = 1,
    HeatExchangeRelated,
    PipeDataLookupMain,
    Civil,
    Chemical,
};

struct DensityRelated {
    double atmospheric_pressure;
    double standard_temperature;
    double molecular_weight;
    double temperature;
    double pressure;
    double compressibility_factor;
    double density;
    double std_density;
};

struct PressureDropRelated {
    // Input parameters
    double pipe_inner_diameter;  // in (internal diameter)
    double mass_flow_rate;       // lb/hr
    double temperature;          // F
    double inlet_pressure;       // psig
    double fluid_density;        // lb/cf
    double viscosity;            // cp (centipoise)
    double equivalent_length;    // ft
    double roughness;            // absolute roughness (ft)
    double cp_cv_ratio;          // Cp/Cv ratio
    double hazen_williams_c;     // Hazen Williams C coefficient

    // Calculated results
    double reynolds_number;      // Nre
    double friction_factor;      // ff
    double velocity;             // VEL FPS (ft/s)
    double pressure_drop_per_100ft;  // PD/100'
    double pressure_drop_psig;   // PD psig
    double outlet_pressure;      // psig
    double max_noise_velocity;   // 200/sqrt(density)
    double mach_number;          // fluid flow mach number
    double hw_pressure_drop_per_100ft;  // Hazen Williams PD/100'
    double erosion_velocity;     // 200/sqrt(density)
    double economic_size;        // calculated economic pipe size
};
