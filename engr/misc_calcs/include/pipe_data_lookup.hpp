#pragma once

#include <string>

struct PipeLookupResult {
    bool success = false;
    std::string error_message;

    std::string nominal_size;
    std::string schedule;
    double outside_diameter_in = 0.0;
    double wall_thickness_in = 0.0;
    double inside_diameter_in = 0.0;

    double empty_weight_lb_per_ft = 0.0;
    double water_weight_lb_per_ft = 0.0;
    double full_weight_lb_per_ft = 0.0;
};

class PipeDataLookup {
public:
    PipeLookupResult lookupByNominalAndSchedule(const std::string& nominal_size,
                                                const std::string& schedule) const;
    void runInteractive() const;

private:
    static std::string normalizeKey(const std::string& value);
};
