#pragma once

enum MainCategory {
    ExitProgram = 0,
    FlowRelated = 1,
    HeatExchangeRelated,
    Civil,
    Chemical,
    Environmental
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
