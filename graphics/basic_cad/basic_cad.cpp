#include <Windows.h>
#include <sstream>
#include <cmath>
#include <vector>

struct point {
    double x;
    double y;
};


struct lines {
    point pt1;
    point pt2;
};

// Forward declaration of the window procedure, and Function Prototypes
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void line(HDC& hdc, double x1, double y1, double x2, double y2);
void drawFilledTriangle(HDC hdc, double x1, double y1, double x2, double y2, double x3, double y3, COLORREF color);
void drawRectangle(HDC hdc, int left, int top, int right, int bottom);
void show_variable_value(HDC hdc, std::string str, double dbl_val, double loc_x, double loc_y);

// Entry point for Windows GUI applications
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    
    // Register the window class
    const wchar_t CLASS_NAME[] = L"MyWindowClass";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;        // lpfnWndProc is a pointer to an application-defined function called the window procedure or window proc.
    wc.hInstance = hInstance;           // hInstance is the handle to the application instance. Get this value from the hInstance parameter of wWinMain
    wc.lpszClassName = CLASS_NAME;      // lpszClassName is a string that identifies the window class.
    RegisterClassW(&wc);

    // Create the window
    HWND hWnd = CreateWindowExW(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        L"Basic Cad",      // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        // orig code  -->         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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

// Window procedure
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            HPEN hPen, hOldPen;
            HBRUSH hBrush, hOldBrush;

            // using screen size of 1920 x 1080
            // monitor width = 24"

            // define screen coordinates            
            const double scrn_wid = 1920;
            const double scrn_ht = 1080;

            // define border coordinates           
            point scrn_min = { 10, 10 };         // measured botm right
            point scrn_max = { 1890, 990 };      // measured botm right

            const double scrn_xmin = 10;
            const double scrn_ymin = 10;
            const double scrn_xmax = 1890;
            const double scrn_ymax = 990;

            const double scrn_xd = scrn_xmax - scrn_xmin;
            const double scrn_yd = scrn_ymax - scrn_ymin;

            // Declare a vector to store various lines
            std::vector<lines> line_vec;
                        
            // Draw Border
            drawRectangle(hdc, scrn_xmin, scrn_ymin, scrn_xmax, scrn_ymax);

            double max_y_dim = 35 * 12;
            double max_x_dim = 15 * 12;
            double paper_size_width = 36;
            double paper_size_height = 24;
            double paper_bord_width = 35.5;
            double paper_bord_height = 23.5;
            double scale_fact = 32;
            double reqd_vert = scrn_yd / (paper_bord_height * scale_fact);
            double reqd_horz = scrn_xd / (paper_bord_width * scale_fact);
            double zoom_fact = (reqd_vert < reqd_horz) ? reqd_vert : reqd_horz;


            point origin_pan = {-48.0, -24.0 };

            //print values on screen
            show_variable_value(hdc, "reqd_vert =", reqd_vert, 10, 10);
            show_variable_value(hdc, "reqd_horz =", reqd_horz, 10, 30);
            show_variable_value(hdc, "zoom_fact =", zoom_fact, 10, 50);
            

            EndPaint(hWnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Dummy WinMain function to call wWinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
}

point sc( double screen_x, double scale_fact, point pt) {
    // This function takes a point and returns the screen coordinate

// TODO *******************************

return {0, 0};
}

void line_start_end(HDC& hdc, double x1, double y1, double x2, double y2) {
    // Cast double values to long to avoid warnings
    long x1_lg = static_cast<long>(x1); 
    long y1_lg = static_cast<long>(y1); 
    long x2_lg = static_cast<long>(x2); 
    long y2_lg = static_cast<long>(y2); 

    MoveToEx(hdc, x1_lg, y1_lg, NULL);
    LineTo(hdc, x2_lg, y2_lg);
}

void line_at_ang(HDC& hdc, double x1, double y1, double len, double ang) {
    // Cast double values to long to avoid warnings
   
    double x2 = x1 + len*sin(ang);
    double y2 = x2 + len*cos(ang);

    long x1_lg = static_cast<long>(x1); 
    long y1_lg = static_cast<long>(y1); 

    long x2_lg = static_cast<long>(len); 
    long y2_lg = static_cast<long>(y2); 

    MoveToEx(hdc, x1_lg, y1_lg, NULL);
    LineTo(hdc, x2_lg, y2_lg);
}


// Function to draw a rectangle using the Rectangle function
void drawRectangle(HDC hdc, int left, int top, int right, int bottom) {
    Rectangle(hdc, left, top, right, bottom);
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