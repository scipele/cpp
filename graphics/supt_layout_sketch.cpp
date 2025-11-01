#include <Windows.h>
#include <sstream>
#include <cmath>
#include <stdlib.h>

// Forward declaration of the window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Entry point for Windows GUI applications
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const wchar_t CLASS_NAME[] = L"MyWindowClass";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClassW(&wc);

    // Create the window
    HWND hWnd = CreateWindowExW(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        L"My Window",                   // Window title
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        // orig code  -->         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        // CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
    if (hWnd == NULL) {
        return 0;
    }

    // Show the window
    ShowWindow(hWnd, nCmdShow);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


void show_variable_value(HDC hdc, std::string str, double dbl_val, double loc_x, double loc_y) {
    // Draw the calculated value as text
    std::stringstream ss;
    ss << str << dbl_val;
    std::string narrowText = ss.str();
    int wstr_size = MultiByteToWideChar(CP_UTF8, 0, narrowText.c_str(), -1, NULL, 0);
    std::wstring wideText(wstr_size, 0);
    MultiByteToWideChar(CP_UTF8, 0, narrowText.c_str(), -1, &wideText[0], wstr_size);
    TextOutW(hdc, loc_x, loc_y, wideText.c_str(), wideText.length());
}

void DrawCircle(HDC hdc, double centerX, double centerY, double dia, double scale_fact) {
    double radius = dia / 2;
    double left = centerX - radius * scale_fact;
    double top = centerY - radius* scale_fact;
    double right = centerX + radius* scale_fact;
    double bottom = centerY + radius* scale_fact;
    Ellipse(hdc, static_cast<int>(left), static_cast<int>(top), static_cast<int>(right), static_cast<int>(bottom));
}

void DrawCenterLines(HDC hdc, double centerX, double centerY, double dia, double scale_fact, double overlap) {

    // Get the DPI of the system
    UINT dpiX = 96;
    int pixel_overlap = static_cast<int>(overlap * dpiX);
    double cline_gap = .0625 * dpiX;
    double cline_len_half = .125 / 2 * dpiX;
    double cline_half_plus_gap = cline_len_half + cline_gap;

    double radius = dia / 2;
    double horiz_line_left = centerX - radius * scale_fact - pixel_overlap;
    double horiz_line_right = centerX + radius * scale_fact + pixel_overlap;

    double vert_line_top = centerY - radius * scale_fact - pixel_overlap;
    double vert_line_bottom = centerY + radius * scale_fact + pixel_overlap;

    // draw center portion of horizontal line using length as overlap
    MoveToEx(hdc, centerX - cline_len_half, centerY, nullptr); // Move to the starting point
    LineTo(hdc, centerX + cline_len_half, centerY); // Draw a line to the ending point

    //draw remaining horizontal lines
    MoveToEx(hdc, centerX + cline_half_plus_gap, centerY, nullptr); // Move to the starting point
    LineTo(hdc, horiz_line_right, centerY); // Draw a line to the ending point

    MoveToEx(hdc, centerX - cline_half_plus_gap, centerY, nullptr); // Move to the starting point
    LineTo(hdc, horiz_line_left, centerY); // Draw a line to the ending point

    // draw center portion of vertical line using length as overlap
    MoveToEx(hdc, centerX, centerY - cline_len_half, nullptr); // Move to the starting point
    LineTo(hdc, centerX, centerY + cline_len_half); // Draw a line to the ending point

    //draw remaining vertical lines
    MoveToEx(hdc, centerX, centerY + cline_half_plus_gap, nullptr); // Move to the starting point
    LineTo(hdc, centerX, vert_line_bottom); // Draw a line to the ending point

    MoveToEx(hdc, centerX, centerY - cline_half_plus_gap, nullptr); // Move to the starting point
    LineTo(hdc, centerX, vert_line_top); // Draw a line to the ending point
}

double radians(double ang_deg) {
    return ang_deg * M_PI / 180;
}

void Draw_line_with_arrow_at_angle(HDC hdc, double start_screen_x, double start_screen_y, double angle_deg, double length, double scale_fact) {
    
    // Get the DPI of the system
    UINT dpiX = 96;
    double arrow_len = 0.0625 * dpiX;

    MoveToEx(hdc, start_screen_x, start_screen_y, nullptr); // Move to the starting point
    double pt2_x = start_screen_x + length * scale_fact * std::cos(radians(angle_deg));
    double pt2_y = start_screen_y - length * scale_fact * std::sin(radians(angle_deg)); //subtract since screen coordinates are top to btm
    LineTo(hdc, pt2_x, pt2_y); // Draw a line to the ending point

    // draw arrow head
    MoveToEx(hdc, pt2_x, pt2_y, nullptr); // Move to the starting point
    double pt3_x = pt2_x + arrow_len * scale_fact * std::cos(radians(angle_deg + 165));
    double pt3_y = pt2_y - arrow_len * scale_fact * std::sin(radians(angle_deg + 165)); //subtract since screen coordinates are top to btm
    LineTo(hdc, pt3_x, pt3_y); // Draw a line to the ending point

    // draw other arrow head
    MoveToEx(hdc, pt2_x, pt2_y, nullptr); // Move to the starting point
    double pt4_x = pt2_x + arrow_len * scale_fact * std::cos(radians(angle_deg + 195));
    double pt4_y = pt2_y - arrow_len * scale_fact * std::sin(radians(angle_deg + 195)); //subtract since screen coordinates are top to btm
    LineTo(hdc, pt4_x, pt4_y); // Draw a line to the ending point
    LineTo(hdc, pt3_x, pt3_y); // Draw a line to the ending point
}



double conv_angle_ref_north_up(double true_angle) {
          // Convert angle into reference from north up
            double tmpAngle;
            tmpAngle = (90 - true_angle);
            if(tmpAngle < 0) {
                tmpAngle += 360;
            }
    return tmpAngle;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            //scale factor
            //using screen size of 1920 x 1080
            // determine close scale factor
            // Need to represent about 34' (408 inches) square box on screen 
            // 1080 / 408 = 2.647 therefore use scale factor of 2

            // hard code input
            const double scale_fact = 2;

            const double ves_is_dia_in = 22 * 12;  // Feet to Inches
            const double ves_thk_in = 1;

            const double ves_cl_x_coord = -275 * 12;      // East Coordiate - note that west is negative
            const double ves_cl_y_coord = -1762 * 12;     // North Coordiate - note that south is negative

            const double pipe_od = 18; 
            const double pipe_cl_x_coord = -(280 * 12 + 8.5);        // East Coordiate - note that west is negative
            const double pipe_cl_y_coord = -(1774 * 12 + 2.8125);       // North Coordiate - note that south is negative

//            const double pipe_cl_x_coord = -(360 * 12 + 9.75);        // East Coordiate - note that west is negative
//            const double pipe_cl_y_coord = -(1277 * 12 + 8.4375);       // North Coordiate - note that south is negative
            const double staged_rotation_angle_deg = 45; // measured clockwise;

            // Calculations
            const double ves_os_dia_in = ves_is_dia_in + 2 * ves_thk_in;
            const double window_x = 1920;
            const double window_y = 1080;
            const double wind_cen_x = window_x/2;
            const double wind_cen_y = window_y/2;
            double ves_to_pipe_x_delta_in = pipe_cl_x_coord - ves_cl_x_coord;
            double ves_to_pipe_y_delta_in = pipe_cl_y_coord - ves_cl_y_coord;
            double ves_to_pipe_hypotenuse_delta_in = sqrt(pow(ves_to_pipe_x_delta_in, 2) + pow(ves_to_pipe_y_delta_in, 2));

            double orient_angle_true_deg = 180 / M_PI * std::atan2(ves_to_pipe_y_delta_in, ves_to_pipe_x_delta_in);  // calc angle in true orientation
            double orient_angle_north_up_deg = conv_angle_ref_north_up(orient_angle_true_deg);
            double orient_angle_true_rotated_deg = orient_angle_true_deg - staged_rotation_angle_deg;
            double orient_angle_rotated_north_up_deg = conv_angle_ref_north_up(orient_angle_true_rotated_deg);

            //calculate the screen coordinates of pipe at original location
            double pipe_orig_screen_cl_x = wind_cen_x + ves_to_pipe_x_delta_in * scale_fact;    // Add since screen coordinates are positive left to right
            double pipe_orig_screen_cl_y = wind_cen_y - ves_to_pipe_y_delta_in * scale_fact;    // Subtract since screen coordinates are positive going downward

            //calculate the screen coordinates of pipe at rotated location
            double ves_to_pipe_x_delta_rotated_in = ves_to_pipe_hypotenuse_delta_in * std::cos(radians(orient_angle_true_rotated_deg));  // calc angle in true orientation
            double ves_to_pipe_y_delta_rotated_in = ves_to_pipe_hypotenuse_delta_in * std::sin(radians(orient_angle_true_rotated_deg));  // calc angle in true orientation

            double pipe_rotated_screen_cl_x = wind_cen_x + ves_to_pipe_x_delta_rotated_in * scale_fact;    // Add since screen coordinates are positive left to right
            double pipe_rotated_screen_cl_y = wind_cen_y - ves_to_pipe_y_delta_rotated_in * scale_fact;    // Subtract since screen coordinates are positive going downward

            double fact_cosine_radial = std::cos(radians(90 - orient_angle_true_rotated_deg));
            double fact_sine_tangential = std::sin(radians(90 -orient_angle_true_rotated_deg));            
            
            //double pt2_x = start_screen_x + length * scale_fact * std::cos(radians(angle_deg));


            //print values on screen
            show_variable_value(hdc, "ves_is_dia_in =", ves_is_dia_in, 10, 10);
            show_variable_value(hdc, "ves_os_dia_in =", ves_os_dia_in, 10, 30);
            show_variable_value(hdc, "orient_angle_true_deg =", orient_angle_true_deg, 10, 50);
            show_variable_value(hdc, "orient_angle_north_up_deg =", orient_angle_north_up_deg, 10, 70);
            show_variable_value(hdc, "orient_angle_true_rotated_deg =", orient_angle_true_rotated_deg, 10, 90);

            show_variable_value(hdc, "orient_angle_rotated_north_up_deg =", orient_angle_rotated_north_up_deg, 10, 130);
            show_variable_value(hdc, "ves_to_pipe_x_delta_rotated_in =", ves_to_pipe_x_delta_rotated_in, 10, 150);
            show_variable_value(hdc, "ves_to_pipe_y_delta_rotated_in =", ves_to_pipe_y_delta_rotated_in, 10, 170);
            show_variable_value(hdc, "fact_cosine_radial =", fact_cosine_radial, 10, 190);
            show_variable_value(hdc, "fact_sine_tangential =", fact_sine_tangential, 10, 210);



            // draw vessel od, id, pipe od, rotated pipe od
            DrawCircle(hdc, wind_cen_x, wind_cen_y, ves_os_dia_in, scale_fact);
            DrawCircle(hdc, wind_cen_x, wind_cen_y, ves_is_dia_in, scale_fact);
            DrawCircle(hdc, pipe_orig_screen_cl_x, pipe_orig_screen_cl_y, pipe_od, scale_fact);
            DrawCircle(hdc, pipe_rotated_screen_cl_x, pipe_rotated_screen_cl_y, pipe_od, scale_fact);

            // Draw Center lines
            DrawCenterLines(hdc, wind_cen_x, wind_cen_y, ves_os_dia_in, scale_fact, 0.125);
            DrawCenterLines(hdc, pipe_orig_screen_cl_x, pipe_orig_screen_cl_y, pipe_od, scale_fact, 0.125);
            DrawCenterLines(hdc, pipe_rotated_screen_cl_x, pipe_rotated_screen_cl_y, pipe_od, scale_fact, 0.125);

            // Draw Lines representing triangle, Weight and Component Loads
            double wt_line_len = 48;
            Draw_line_with_arrow_at_angle(hdc, pipe_rotated_screen_cl_x, pipe_rotated_screen_cl_y, -90, wt_line_len, scale_fact);  //  draw straight down
            Draw_line_with_arrow_at_angle(hdc, pipe_rotated_screen_cl_x, pipe_rotated_screen_cl_y, orient_angle_true_rotated_deg + 180, wt_line_len * fact_cosine_radial, scale_fact);  //radial inward


            // calculate start point of back component
            double pt2_x = pipe_rotated_screen_cl_x + wt_line_len * fact_cosine_radial * scale_fact * std::cos(radians(orient_angle_true_rotated_deg + 180));
            double pt2_y = pipe_rotated_screen_cl_y - wt_line_len * fact_cosine_radial * scale_fact * std::sin(radians(orient_angle_true_rotated_deg + 180)); //subtract since screen coordinates are top to btm
            Draw_line_with_arrow_at_angle(hdc, pt2_x, pt2_y, orient_angle_true_rotated_deg + 270, wt_line_len * fact_sine_tangential, scale_fact);  //tangential
  
            EndPaint(hWnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Dummy WinMain function to call wWinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}
