#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

// *******************************************************************************************************************************
// **** The following portion of the class definition is typically placed in a header file 'ClassName.h' or 'ClassName.hpp' ****
// *******************************************************************************************************************************
// Provides the following:
//      - public, private, or protected access specifiers
//      - defines member variables
//      - provides constructor and destructor prototypes
//      - provides function prototypes of member functions
class SteelShapes
{
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

    struct field_def {
        std::string field_name;
        std::string var_name;
        std::string uom;
    };

    // Vector to store field_def structs
    std::vector<field_def> fields;

    int read_binary_file();
    void readString(std::ifstream &file, std::string &str);
    void PrintShapeProperties();
    void readCSV();
    field_def parseCSVLine(const std::string& line);
    std::string get_str_from_user(std::string user_msg);
    std::string toUpperCase(const std::string str);

public:
    // This is called a constructor protype which is basically similar to a function with the exact same name as the class that automatically runs
    // anytime that an instance of the Object 'SteelShapes' is created (aka instantiated).  
    SteelShapes();
    
    // This is called a destructor which is basically a function with the exact same name as the class that automatically runs anytime that an instance
    // of the Object 'SteelShapes' is created.  This is called 'instantiated'
    ~SteelShapes();
};



// ***********************************************************************************************************
// **** Class implentation File ****
// ***********************************************************************************************************
// This class implimentation is typically placed in a separate file named 'ClassName.cpp'

// This is called a constructor protype which is basically similar to a function with the exact same name as the class that automatically runs
// anytime that an instance of the Object 'SteelShapes' is created (aka instantiated).  

SteelShapes::SteelShapes() {
        input_name = get_str_from_user("1. Enter Shape Desc and Size to get Properties (i.e. W14X109)");

    int read_status = read_binary_file();
    if (read_status == 0) {
        readCSV();
        PrintShapeProperties();
    } else {
        std::cout << "Matching Shape Not Found\n";
    }
}

// Destructor to destroy the object at end of scope
SteelShapes::~SteelShapes() {
}


int SteelShapes::read_binary_file() {
       std::ifstream file("C:/dev/cpp/misc/Class/shapes.bin", std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file";
        return 1;
    }

    short row_cnt;

    while (file.peek() != EOF) {        
        // Read the number of rows of data that were written in the Binary File
        file.read(reinterpret_cast<char*>(&row_cnt), sizeof(row_cnt));

        //Read each row from the Binary file
        for (int i=0; i<row_cnt; i++) {
            readString(file, Type);
            readString(file, EDI_Name);
            file.read(reinterpret_cast<char*>(&t_f), sizeof(t_f));
            file.read(reinterpret_cast<char*>(&W), sizeof(W));
            file.read(reinterpret_cast<char*>(&A), sizeof(A));
            file.read(reinterpret_cast<char*>(&d), sizeof(d));
            file.read(reinterpret_cast<char*>(&ddet), sizeof(ddet));
            file.read(reinterpret_cast<char*>(&Ht), sizeof(Ht));
            file.read(reinterpret_cast<char*>(&h), sizeof(h));
            file.read(reinterpret_cast<char*>(&OD), sizeof(OD));
            file.read(reinterpret_cast<char*>(&bf), sizeof(bf));
            file.read(reinterpret_cast<char*>(&bfdet), sizeof(bfdet));
            file.read(reinterpret_cast<char*>(&B1), sizeof(B1));
            file.read(reinterpret_cast<char*>(&b), sizeof(b));
            file.read(reinterpret_cast<char*>(&ID), sizeof(ID));
            file.read(reinterpret_cast<char*>(&tw), sizeof(tw));
            file.read(reinterpret_cast<char*>(&twdet), sizeof(twdet));
            file.read(reinterpret_cast<char*>(&twdet_2), sizeof(twdet_2));
            file.read(reinterpret_cast<char*>(&tf), sizeof(tf));
            file.read(reinterpret_cast<char*>(&tfdet), sizeof(tfdet));
            file.read(reinterpret_cast<char*>(&t), sizeof(t));
            file.read(reinterpret_cast<char*>(&tnom), sizeof(tnom));
            file.read(reinterpret_cast<char*>(&tdes), sizeof(tdes));
            file.read(reinterpret_cast<char*>(&kdes), sizeof(kdes));
            file.read(reinterpret_cast<char*>(&kdet), sizeof(kdet));
            file.read(reinterpret_cast<char*>(&k1), sizeof(k1));
            file.read(reinterpret_cast<char*>(&x), sizeof(x));
            file.read(reinterpret_cast<char*>(&y), sizeof(y));
            file.read(reinterpret_cast<char*>(&eo), sizeof(eo));
            file.read(reinterpret_cast<char*>(&xp), sizeof(xp));
            file.read(reinterpret_cast<char*>(&yp), sizeof(yp));
            file.read(reinterpret_cast<char*>(&bf_2tf), sizeof(bf_2tf));
            file.read(reinterpret_cast<char*>(&b_t), sizeof(b_t));
            file.read(reinterpret_cast<char*>(&b_tdes), sizeof(b_tdes));
            file.read(reinterpret_cast<char*>(&h_tw), sizeof(h_tw));
            file.read(reinterpret_cast<char*>(&h_tdes), sizeof(h_tdes));
            file.read(reinterpret_cast<char*>(&D_t), sizeof(D_t));
            file.read(reinterpret_cast<char*>(&Ix), sizeof(Ix));
            file.read(reinterpret_cast<char*>(&Zx), sizeof(Zx));
            file.read(reinterpret_cast<char*>(&Sx), sizeof(Sx));
            file.read(reinterpret_cast<char*>(&rx), sizeof(rx));
            file.read(reinterpret_cast<char*>(&Iy), sizeof(Iy));
            file.read(reinterpret_cast<char*>(&Zy), sizeof(Zy));
            file.read(reinterpret_cast<char*>(&Sy), sizeof(Sy));
            file.read(reinterpret_cast<char*>(&ry), sizeof(ry));
            file.read(reinterpret_cast<char*>(&Iz), sizeof(Iz));
            file.read(reinterpret_cast<char*>(&rz), sizeof(rz));
            file.read(reinterpret_cast<char*>(&Sz), sizeof(Sz));
            file.read(reinterpret_cast<char*>(&J), sizeof(J));
            file.read(reinterpret_cast<char*>(&Cw), sizeof(Cw));
            file.read(reinterpret_cast<char*>(&C), sizeof(C));
            file.read(reinterpret_cast<char*>(&Wno), sizeof(Wno));
            file.read(reinterpret_cast<char*>(&Sw1), sizeof(Sw1));
            file.read(reinterpret_cast<char*>(&Sw2), sizeof(Sw2));
            file.read(reinterpret_cast<char*>(&Sw3), sizeof(Sw3));
            file.read(reinterpret_cast<char*>(&Qf), sizeof(Qf));
            file.read(reinterpret_cast<char*>(&Qw), sizeof(Qw));
            file.read(reinterpret_cast<char*>(&ro), sizeof(ro));
            file.read(reinterpret_cast<char*>(&H1), sizeof(H1));
            file.read(reinterpret_cast<char*>(&tan_alpha), sizeof(tan_alpha));
            file.read(reinterpret_cast<char*>(&Iw), sizeof(Iw));
            file.read(reinterpret_cast<char*>(&zA), sizeof(zA));
            file.read(reinterpret_cast<char*>(&zB), sizeof(zB));
            file.read(reinterpret_cast<char*>(&zC), sizeof(zC));
            file.read(reinterpret_cast<char*>(&wA), sizeof(wA));
            file.read(reinterpret_cast<char*>(&wB), sizeof(wB));
            file.read(reinterpret_cast<char*>(&wC), sizeof(wC));
            file.read(reinterpret_cast<char*>(&SwA), sizeof(SwA));
            file.read(reinterpret_cast<char*>(&SwB), sizeof(SwB));
            file.read(reinterpret_cast<char*>(&SwC), sizeof(SwC));
            file.read(reinterpret_cast<char*>(&SzA), sizeof(SzA));
            file.read(reinterpret_cast<char*>(&SzB), sizeof(SzB));
            file.read(reinterpret_cast<char*>(&SzC), sizeof(SzC));
            file.read(reinterpret_cast<char*>(&rts), sizeof(rts));
            file.read(reinterpret_cast<char*>(&ho), sizeof(ho));
            file.read(reinterpret_cast<char*>(&PA), sizeof(PA));
            file.read(reinterpret_cast<char*>(&PA2), sizeof(PA2));
            file.read(reinterpret_cast<char*>(&PB), sizeof(PB));
            file.read(reinterpret_cast<char*>(&PC), sizeof(PC));
            file.read(reinterpret_cast<char*>(&PD), sizeof(PD));
            file.read(reinterpret_cast<char*>(&T1), sizeof(T1));
            file.read(reinterpret_cast<char*>(&WGi), sizeof(WGi));
            file.read(reinterpret_cast<char*>(&WGo), sizeof(WGo));

            if ( EDI_Name == input_name) {
                // exit the loop once a match is found for the input size
                file.close();
                return 0;
            }
        }

    }
    file.close();
    // return -1 if not match is found
    return -1;
}


void SteelShapes::readCSV() {
    // This function reads a csv file that contains all the field names, unit of measures, etc.
    std::string filename = "C:/dev/cpp/misc/Class/SteelShapes/field_names.csv";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
 
    // Read the file line by line
    while (std::getline(file, line)) {

        // Parse the line and store the struct in the vector
        field_def field = parseCSVLine(line);
        fields.push_back(field);
    }

    file.close();
    std::cout << "csv fields read and closed\n";
}


// Helper function to split the CSV line into two fields
SteelShapes::field_def SteelShapes::parseCSVLine(const std::string& line) {
    field_def field;
    std::stringstream ss(line);
    std::getline(ss, field.field_name, ','); // Get the field name
    std::getline(ss, field.uom, ',');   // Get the variable name
    std::getline(ss, field.var_name, ',');        // Get the unit of measure (uom)

    return field;
}


void SteelShapes::PrintShapeProperties() {
    // Prints the non zero properties of the shape input
    auto itt = fields.begin();
    std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name + "," << itt->var_name << " = " << Type << " " << itt->uom << "\n"; itt++;
    std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name + "," << itt->var_name << " = " << EDI_Name << " " << itt->uom << "\n"; itt++;
    std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name + "," << itt->var_name << " = " << t_f << " " << itt->uom << "\n"; itt++;
    if(W>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << W << " " << itt->uom << "\n"; itt++;
    if(A>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << A << " " << itt->uom << "\n"; itt++;
    if(d>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << d << " " << itt->uom << "\n"; itt++;
    if(ddet>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << ddet << " " << itt->uom << "\n"; itt++;
    if(Ht>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Ht << " " << itt->uom << "\n"; itt++;
    if(h>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << h << " " << itt->uom << "\n"; itt++;
    if(OD>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << OD << " " << itt->uom << "\n"; itt++;
    if(bf>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << bf << " " << itt->uom << "\n"; itt++;
    if(bfdet>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << bfdet << " " << itt->uom << "\n"; itt++;
    if(B1>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << B1 << " " << itt->uom << "\n"; itt++;
    if(b>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << b << " " << itt->uom << "\n"; itt++;
    if(ID>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << ID << " " << itt->uom << "\n"; itt++;
    if(tw>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << tw << " " << itt->uom << "\n"; itt++;
    if(twdet>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << twdet << " " << itt->uom << "\n"; itt++;
    if(twdet_2>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << twdet_2 << " " << itt->uom << "\n"; itt++;
    if(tf>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << tf << " " << itt->uom << "\n"; itt++;
    if(tfdet>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << tfdet << " " << itt->uom << "\n"; itt++;
    if(t>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << t << " " << itt->uom << "\n"; itt++;
    if(tnom>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << tnom << " " << itt->uom << "\n"; itt++;
    if(tdes>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << tdes << " " << itt->uom << "\n"; itt++;
    if(kdes>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << kdes << " " << itt->uom << "\n"; itt++;
    if(kdet>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << kdet << " " << itt->uom << "\n"; itt++;
    if(k1>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << k1 << " " << itt->uom << "\n"; itt++;
    if(x>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << x << " " << itt->uom << "\n"; itt++;
    if(y>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << y << " " << itt->uom << "\n"; itt++;
    if(eo>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << eo << " " << itt->uom << "\n"; itt++;
    if(xp>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << xp << " " << itt->uom << "\n"; itt++;
    if(yp>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << yp << " " << itt->uom << "\n"; itt++;
    if(bf_2tf>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << bf_2tf << " " << itt->uom << "\n"; itt++;
    if(b_t>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << b_t << " " << itt->uom << "\n"; itt++;
    if(b_tdes>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << b_tdes << " " << itt->uom << "\n"; itt++;
    if(h_tw>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << h_tw << " " << itt->uom << "\n"; itt++;
    if(h_tdes>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << h_tdes << " " << itt->uom << "\n"; itt++;
    if(D_t>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << D_t << " " << itt->uom << "\n"; itt++;
    if(Ix>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Ix << " " << itt->uom << "\n"; itt++;
    if(Zx>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Zx << " " << itt->uom << "\n"; itt++;
    if(Sx>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Sx << " " << itt->uom << "\n"; itt++;
    if(rx>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << rx << " " << itt->uom << "\n"; itt++;
    if(Iy>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Iy << " " << itt->uom << "\n"; itt++;
    if(Zy>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Zy << " " << itt->uom << "\n"; itt++;
    if(Sy>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Sy << " " << itt->uom << "\n"; itt++;
    if(ry>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << ry << " " << itt->uom << "\n"; itt++;
    if(Iz>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Iz << " " << itt->uom << "\n"; itt++;
    if(rz>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << rz << " " << itt->uom << "\n"; itt++;
    if(Sz>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Sz << " " << itt->uom << "\n"; itt++;
    if(J>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << J << " " << itt->uom << "\n"; itt++;
    if(Cw>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Cw << " " << itt->uom << "\n"; itt++;
    if(C>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << C << " " << itt->uom << "\n"; itt++;
    if(Wno>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Wno << " " << itt->uom << "\n"; itt++;
    if(Sw1>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Sw1 << " " << itt->uom << "\n"; itt++;
    if(Sw2>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Sw2 << " " << itt->uom << "\n"; itt++;
    if(Sw3>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Sw3 << " " << itt->uom << "\n"; itt++;
    if(Qf>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Qf << " " << itt->uom << "\n"; itt++;
    if(Qw>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Qw << " " << itt->uom << "\n"; itt++;
    if(ro>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << ro << " " << itt->uom << "\n"; itt++;
    if(H1>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << H1 << " " << itt->uom << "\n"; itt++;
    if(tan_alpha>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << tan_alpha << " " << itt->uom << "\n"; itt++;
    if(Iw>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << Iw << " " << itt->uom << "\n"; itt++;
    if(zA>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << zA << " " << itt->uom << "\n"; itt++;
    if(zB>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << zB << " " << itt->uom << "\n"; itt++;
    if(zC>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << zC << " " << itt->uom << "\n"; itt++;
    if(wA>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << wA << " " << itt->uom << "\n"; itt++;
    if(wB>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << wB << " " << itt->uom << "\n"; itt++;
    if(wC>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << wC << " " << itt->uom << "\n"; itt++;
    if(SwA>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << SwA << " " << itt->uom << "\n"; itt++;
    if(SwB>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << SwB << " " << itt->uom << "\n"; itt++;
    if(SwC>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << SwC << " " << itt->uom << "\n"; itt++;
    if(SzA>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << SzA << " " << itt->uom << "\n"; itt++;
    if(SzB>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << SzB << " " << itt->uom << "\n"; itt++;
    if(SzC>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << SzC << " " << itt->uom << "\n"; itt++;
    if(rts>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << rts << " " << itt->uom << "\n"; itt++;
    if(ho>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << ho << " " << itt->uom << "\n"; itt++;
    if(PA>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << PA << " " << itt->uom << "\n"; itt++;
    if(PA2>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << PA2 << " " << itt->uom << "\n"; itt++;
    if(PB>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << PB << " " << itt->uom << "\n"; itt++;
    if(PC>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << PC << " " << itt->uom << "\n"; itt++;
    if(PD>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << PD << " " << itt->uom << "\n"; itt++;
    if(T1>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << T1 << " " << itt->uom << "\n"; itt++;
    if(WGi>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << WGi << " " << itt->uom << "\n"; itt++;
    if(WGo>0) std::cout << std::left << std::setw(58) << std::setfill('.') << itt->field_name << ", " << itt->var_name << " = " << WGo << " " << itt->uom << "\n"; itt++;
}


void SteelShapes::readString(std::ifstream &file, std::string &str) {
      // Read length as a short, since written as an integer in vba
    short length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    str.resize(length);
    file.read(&str[0], length);
}


std::string SteelShapes::get_str_from_user(std::string user_msg) {
    std::string tmp;
    std::cout << user_msg;
    std::getline(std::cin, tmp);
    return SteelShapes::toUpperCase(tmp);
}


//function to convert string to uppercase
std::string SteelShapes::toUpperCase(const std::string str) {  
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(c);
    }
    return result;
}


int main(int argc, char const *argv[])
{

    std::string loopAgain;
    do {

        SteelShapes stl;

        //Keep looping or 'q' to quit
        std::cout << "Enter to Continue, q to quit ";
        std::getline(std::cin, loopAgain);

    } while (loopAgain != "q");

    return 0;
}