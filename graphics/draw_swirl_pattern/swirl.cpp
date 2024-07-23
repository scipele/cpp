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


void line(HDC& hdc, double x1, double y1, double x2, double y2) {
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
}

void chg_color(HDC& hdc, HPEN& hOldPen, int r, int g, int b) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(r, g, b));
    hOldPen = (HPEN)SelectObject(hdc, hPen);
}

void drawFilledTriangle(HDC hdc, std::initializer_list<POINT> vertices, COLORREF color) {
    if (vertices.size() != 3) {
        // Handle error: there must be exactly 3 points
        return;
    }
    POINT pts[3];
    std::copy(vertices.begin(), vertices.end(), pts);

    HBRUSH hBrush = CreateSolidBrush(color); // Create a brush with the specified color
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Polygon(hdc, pts, 3);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
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


// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            HPEN hPen, hOldPen;
            HBRUSH hBrush, hOldBrush;

            //using screen size of 1920 x 1080
            // Calculations
            // define border coordinates            
            const double xmin = 450;
            const double ymin = 10;
            const double xmax = 1430;
            const double ymax = 990;

            // Draw Border
            line(hdc, xmin, ymin, xmax, ymin);
            line(hdc, xmax, ymin, xmax, ymax);
            line(hdc, xmax, ymax, xmin, ymax);
            line(hdc, xmin, ymax, xmin, ymin);

            double xcen = (xmax - xmin) /2 ;
            double ycen = (ymax - ymin) /2 ;

            double x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x1_prev, y1_prev, x2_prev, y2_prev, x3_prev, y3_prev, x4_prev, y4_prev, a, b, c, a1, a2, b2, k;
            double ang = 0.02;      // radians

            // second layer (starting near 0,0 - corner)
            //start x1,y1, x2,y2
            x1_prev = 450;
            y1_prev = 10;
            x2_prev = 1430;
            y2_prev = 10;
            x3_prev = 1430;
            y3_prev = 990;
            x4_prev = 450;
            y4_prev = 990;


            // Start Loop Here Later
            for (int i = 1; i < 50; i++) {

                a = x2_prev - x1_prev;
                b = y2_prev - y1_prev;
                c = sqrt(pow(a, 2) + pow(b, 2));
                k = tan(ang);

                a1 = a / ( 1 + pow(k, 2) );     // long side new length
                a2 = pow(k, 2) * a1;            // long side offset (gets shorter by this distance)
                b2 = k * a1;                    // short side offset

                x2 = x1_prev + a1;
                y2 = y1_prev + b2;
                line(hdc, x1_prev, y1_prev, x2, y2);


                x3 = x2_prev - b2;
                y3 = y2_prev + a1;
                line(hdc, x2_prev, y2_prev, x3, y3);

                x4 = x3_prev - a1;
                y4 = y3_prev - b2;
                line(hdc, x3_prev, y3_prev, x4, y4);

                x5 = x4_prev + b2;
                y5 = y4_prev - a1;
                line(hdc, x4_prev, y4_prev, x5, y5);

                // draw filled in triangle for odd iterations
                if (i % 2 == 1) drawFilledTriangle(hdc, { {x1_prev, y1_prev}, {x2_prev, y2_prev}, {x2, y2} }, RGB(0, 255, 0));
        

                // Set previous values for next iteration                
                ang = ang + 0.02;

                x1_prev = x5;
                y1_prev = y5;
                x2_prev = x2;
                y2_prev = y2;
                x3_prev = x3;
                y3_prev = y3;
                x4_prev = x4;
                y4_prev = y4;

            }

            //print values on screen
            show_variable_value(hdc, "c =", c, 10, 10);
            show_variable_value(hdc, "a1 =", a1, 10, 30);
            show_variable_value(hdc, "a2 =", a2, 10, 50);
            show_variable_value(hdc, "b2 =", b2, 10, 70);

  
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
