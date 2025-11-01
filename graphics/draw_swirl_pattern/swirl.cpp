//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | swirl.cpp                                                   |
//| EntryPoint   | wWinMain                                                    |
//| Purpose      | create triangles                                            |
//| Inputs       | hard coded                                                  |
//| Outputs      | graphic window with filled triangles                        |
//| Dependencies | standard only                                               |
//| By Name,Date | T.Sciple, 8/30/2025                                         |

// This program starts by drawing an square and then given an angle in radians triangle i solved for squares
// inside the other squares that progressively get smaller the program loops to recalculate the triangle for each of
// the various number of iterations.  For odd iterations the triangles are drawn filled in with two different RGB colors

#include <Windows.h>
#include <sstream>
#include <cmath>
#include <chrono>

struct point {
    double x;
    double y;
};

// Forward declaration of the window procedure
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
        L"Swirl Patter in Window",      // Window text
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


void line(HDC& hdc, double x1, double y1, double x2, double y2) {
    // Cast double values to long to avoid warnings
    long x1_lg = static_cast<long>(x1); 
    long y1_lg = static_cast<long>(y1); 
    long x2_lg = static_cast<long>(x2); 
    long y2_lg = static_cast<long>(y2); 

    MoveToEx(hdc, x1_lg, y1_lg, NULL);
    LineTo(hdc, x2_lg, y2_lg);
}


void drawFilledTriangle(HDC hdc, double x1, double y1, double x2, double y2, double x3, double y3, COLORREF color) {
    // Convert points to POINT structure with static_cast
    POINT pts[3];
    pts[0] = { static_cast<LONG>(x1), static_cast<LONG>(y1) };
    pts[1] = { static_cast<LONG>(x2), static_cast<LONG>(y2) };
    pts[2] = { static_cast<LONG>(x3), static_cast<LONG>(y3) };

    HBRUSH hBrush = CreateSolidBrush(color); // Create a brush with the specified color
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    Polygon(hdc, pts, 3);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
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
            //HBRUSH hBrush, hOldBrush;

            HBRUSH hBrush = CreateSolidBrush(RGB(18, 18, 18));
            
            // Fill the entire client area with a dark background
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdc, &rect, hBrush);
            // Clean up
            DeleteObject(hBrush);
            //EndPaint(hWnd, &ps);            

            //using screen size of 1920 x 1080
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

            // Declare Variables
            point p[8];
            p[0] = { 450, 10 };
            p[1] = { 1430, 10 };
            p[2] = { 1430, 990 };
            p[3] = { 450, 990 };

            double len_p, len, a1, b, c, cx, cy, k, xd, yd;
            double ang = 0.05, ang_delta = 0.05;      // radians
            len_p = p[1].x - p[0].x;    // starting length since its at zero degree angle at start
            k = tan(ang_delta);

            // Start Loop Here Later
            for (int i = 1; i < 100; i++) {

                // Solve the new length of the triangle with multiple formulas solution
                a1 = len_p / (1 + pow(k, 2));
                b = k * a1;                         // opposite side
                c = sqrt( pow(a1, 2) + pow(b, 2));  // New length of triangle hypotenuse
                
                // now compute the x/y delta coordinates
                cx = c * cos(ang);                  // Longer Side offset
                cy = c * sin(ang);                  // Shorter Side Offset

                p[4].x = p[0].x + cx;
                p[4].y = p[0].y + cy;
                line(hdc, p[0].x, p[0].y, p[4].x, p[4].y);

                p[5].x = p[1].x - cy;
                p[5].y = p[1].y + cx;
                line(hdc, p[1].x, p[1].y, p[5].x, p[5].y);

                p[6].x = p[2].x - cx;
                p[6].y = p[2].y - cy;
                line(hdc, p[2].x, p[2].y, p[6].x, p[6].y);

                p[7].x = p[3].x + cy;
                p[7].y = p[3].y - cx;
                line(hdc, p[3].x, p[3].y, p[7].x, p[7].y);

                // draw filled in triangle for odd iterations
                if (i % 2 == 1) drawFilledTriangle(hdc, p[0].x, p[0].y, p[1].x, p[1].y, p[4].x, p[4].y, RGB(255, 255, 0));
                if (i % 2 == 1) drawFilledTriangle(hdc, p[1].x, p[1].y, p[5].x, p[5].y, p[2].x, p[2].y, RGB(0, 204, 255));
                if (i % 2 == 1) drawFilledTriangle(hdc, p[2].x, p[2].y, p[3].x, p[3].y, p[6].x, p[6].y, RGB(187, 134, 252));
                if (i % 2 == 1) drawFilledTriangle(hdc, p[3].x, p[3].y, p[7].x, p[7].y, p[0].x, p[0].y, RGB(153, 255, 153));
                
                // Compute starting length for next iteration
                xd = p[4].x - p[7].x;
                yd = p[4].y - p[7].y;
                len_p = sqrt( pow(xd, 2) + pow(yd, 2));  

                // Set previous values for next iteration                
                ang += ang_delta;
                p[0].x = p[7].x;   p[0].y = p[7].y;
                p[1].x = p[4].x;   p[1].y = p[4].y;
                p[2].x = p[5].x;   p[2].y = p[5].y;
                p[3].x = p[6].x;   p[3].y = p[6].y;
            }
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