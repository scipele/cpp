// filename:    SteelShapes.hpp
// 
// Purpose:     SteelShapes Class Header File
//              - defines public, private, or protected access specifiers
//              - defines member variables
//              - provides constructor and destructor prototypes
//              - provides function prototypes of member functions
// Dependencies:
//              - See SteelShapes.cpp for class implementation file
//
// By:  T. Sciple, 2024-10-14

#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

class SteelShapes
{

public:
    // Define an enum to describe error codes
    enum ErrorCode {
        ERROR_NONE = 0,    // No error
        ERROR_BINARY_READ = 1,     // Specific error A
        ERROR_CSV_READ = 2,     // Specific error B
        ERROR_UNKNOWN = 99  // Unknown error
    };


    // This is called a constructor protype which is basically similar to a function with the exact same name as the class that automatically runs
    // anytime that an instance of the Object 'SteelShapes' is created (aka instantiated).  
    SteelShapes();
    
    // This is called a destructor which is basically a function with the exact same name as the class that automatically runs anytime that an instance
    // of the Object 'SteelShapes' is created.  This is called 'instantiated'
    ~SteelShapes();

    void get_str_from_user();
    int PrintShapeProperties();

    // Getter for errorCode
    ErrorCode getErrorCode() const;

    // Setter for errorCode
    void setError(ErrorCode code);

    // Function to check if there is an error
    bool hasError() const;

private:
    std::string input_name;  //	Shape_name 
    std::string Type; 	 	 //	Shape_type 
    std::string EDI_Name; 	 // Shape_designation_AISC 
    bool t_f; 	 	 	 	 // Boolean_variable_indicating_special_notes_for_the_shape 
    double W; 	 	 	 	 // Nominal_weight 
    double A; 	 	 	 	 // Cross-sectional_area 
    double d; 	 	 	 	 // Overall_depth_of_member_or_width 
    double ddet; 	 	 	 // Detailing_value_of_member_depth 
    double Ht; 	 	 	 	 // Overall_depth_of_square/rectangular_HSS 
    double h; 	 	 	 	 // Depth_of_flat_wall_HSS 
    double OD; 	 	 	 	 // Outside_diameter_HSS_or_pipe 
    double bf; 	 	 	 	 // Width_of_flange 
    double bfdet; 	 	 	 // Detailing_value_of_flange_width 
    double B1; 	 	 	 	 // Overall_width_of_square/rectangular_HSS 
    double b; 	 	 	 	 // Width_of_flat_wall_or_legs 
    double ID; 	 	 	 	 // Inside_diameter_of_pipe 
    double tw; 	 	 	 	 // Thickness_of_web 
    double twdet; 	 	 	 // Detailing_value_of_web_thickness 
    double twdet_2; 	 	 // Detailing_value_of_tw/2 
    double tf; 	 	 	 	 // Thickness_of_flange 
    double tfdet; 	 	 	 // Detailing_value_of_flange_thickness 
    double t; 	 	 	 	 // Thickness_of_angle_leg 
    double tnom; 	 	 	 // Nominal_thickness_of_HSS_and_pipe_wall 
    double tdes; 	 	 	 // Design_thickness_of_HSS_and_pipe_wall 
    double kdes; 	 	 	 // Distance_from_flange_face_to_web_toe_of_fillet_design 
    double kdet; 	 	 	 // Distance_from_flange_face_to_web_toe_of_fillet_detailing 
    double k1; 	 	 	 	 // Distance_from_web_center_line_to_flange_toe_of_fillet 
    double x; 	 	 	 	 // Horizontal_distance_to_center_of_gravity 
    double y; 	 	 	 	 // Vertical_distance_to_center_of_gravity 
    double eo; 	 	 	 	 // Horizontal_distance_to_shear_center 
    double xp; 	 	 	 	 // Horizontal_distance_to_plastic_neutral_axis 
    double yp; 	 	 	 	 // Vertical_distance_to_plastic_neutral_axis 
    double bf_2tf; 	 	 	 // Slenderness_ratio_flange 
    double b_t; 	 	 	 // Slenderness_ratio_angle/channel_flange 
    double b_tdes; 	 	 	 // Slenderness_ratio_square/rectangular_HSS 
    double h_tw; 	 	 	 // Slenderness_ratio_web 
    double h_tdes; 	 	 	 // Slenderness_ratio_long_wall_HSS 
    double D_t; 	 	 	 // Slenderness_ratio_pipe/tee 
    double Ix; 	 	 	 	 // Moment_of_inertia_x-axis 
    double Zx; 	 	 	 	 // Plastic_section_modulus_x-axis 
    double Sx; 	 	 	 	 // Elastic_section_modulus_x-axis 
    double rx; 	 	 	 	 // Radius_of_gyration_x-axis 
    double Iy; 	 	 	 	 // Moment_of_inertia_y-axis 
    double Zy; 	 	 	 	 // Plastic_section_modulus_y-axis 
    double Sy; 	 	 	 	 // Elastic_section_modulus_y-axis 
    double ry; 	 	 	 	 // Radius_of_gyration_y-axis 
    double Iz; 	 	 	 	 // Moment_of_inertia_z-axis 
    double rz; 	 	 	 	 // Radius_of_gyration_z-axis 
    double Sz; 	 	 	 	 // Elastic_section_modulus_z-axis 
    double J; 	 	 	 	 // Torsional_constant 
    double Cw; 	 	 	 	 // Warping_constant 
    double C; 	 	 	 	 // HSS_torsional_constant 
    double Wno; 	 	 	 // Normalized_warping_function 
    double Sw1; 	 	 	 // Warping_statical_moment_at_point_1 
    double Sw2; 	 	 	 // Warping_statical_moment_at_point_2 
    double Sw3; 	 	 	 // Warping_statical_moment_at_point_3 
    double Qf; 	 	 	 	 // Statical_moment_at_flange-web_point 
    double Qw; 	 	 	 	 // Statical_moment_at_mid-depth 
    double ro; 	 	 	 	 // Polar_radius_of_gyration_shear_center 
    double H1; 	 	 	 	 // Flexural_constant 
    double tan_alpha; 	 	 // Angle_between_y-y_and_z-z_axes_single_angles"
    double Iw; 	 	 	 	 // Moment_of_inertia_w-axis 
    double zA; 	 	 	 	 // Distance_from_point_A_to_center_of_gravity_z-axis 
    double zB; 	 	 	 	 // Distance_from_point_B_to_center_of_gravity_z-axis 
    double zC; 	 	 	 	 // Distance_from_point_C_to_center_of_gravity_z-axis 
    double wA; 	 	 	 	 // Distance_from_point_A_to_center_of_gravity_w-axis 
    double wB; 	 	 	 	 // Distance_from_point_B_to_center_of_gravity_w-axis 
    double wC; 	 	 	 	 // Distance_from_point_C_to_center_of_gravity_w-axis 
    double SwA; 	 	 	 // Elastic_section_modulus_w-axis,_point_A 
    double SwB; 	 	 	 // Elastic_section_modulus_w-axis,_point_B 
    double SwC; 	 	 	 // Elastic_section_modulus_w-axis,_point_C 
    double SzA; 	 	 	 // Elastic_section_modulus_z-axis,_point_A 
    double SzB; 	 	 	 // Elastic_section_modulus_z-axis,_point_B 
    double SzC; 	 	 	 // Elastic_section_modulus_z-axis,_point_C 
    double rts; 	 	 	 // Effective_radius_of_gyration 
    double ho; 	 	 	 	 // Distance_between_flange_centroids 
    double PA; 	 	 	 	 // Shape_perimeter_minus_one_flange_surface 
    double PA2; 	 	 	 // Single_angle_shape_perimeter_minus_long_leg_surface 
    double PB; 	 	 	 	 // Shape_perimeter_Design_Guide_19 
    double PC; 	 	 	 	 // Box_perimeter_minus_one_flange_surface 
    double PD; 	 	 	 	 // Box_perimeter_Design_Guide_19 
    double T1; 	 	 	 	 // Distance_between_web_toes_of_fillets 
    double WGi; 	 	 	 // Workable_gage_for_fastener_holes 
    double WGo; 	 	 	 // Bolt_spacing_across_the_flange
    ErrorCode errorCode;     // Use enum type to track errors

    struct field_def {
        std::string field_name;
        std::string var_name;
        std::string uom;
    };

    // Vector to store field_def structs
    std::vector<field_def> fields;

    int read_binary_file();
    void readString(std::ifstream &file, std::string &str);
    void readCSV();
    field_def parseCSVLine(const std::string& line);
    std::string toUpperCase(const std::string str);
    int TempWriteCSV(std::vector<std::string> temp[13]);

};