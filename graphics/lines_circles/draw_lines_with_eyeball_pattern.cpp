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


// Function to draw a non filled-in circle (ellipse) with the specified color
void circle(HDC hdc, double centerX, double centerY, double dia, int R, int G, int B) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(R, G, B));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Draw the ellipse
    double radius = dia / 2;
    double left = centerX - radius;
    double top = centerY - radius;
    double right = centerX + radius;
    double bottom = centerY + radius;
    Ellipse(hdc, left, top, right, bottom);

    // Restore the old pen and delete the created pen
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

// Function to draw a filled-in circle (ellipse) with the specified color
void circleFilled(HDC hdc, double centerX, double centerY, double dia, int R, int G, int B) {
    // line color
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(R, G, B));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    // fill color
    HBRUSH hBrush = CreateSolidBrush(RGB(R, G, B));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // Draw the ellipse
    double radius = dia / 2;
    double left = centerX - radius;
    double top = centerY - radius;
    double right = centerX + radius;
    double bottom = centerY + radius;
    Ellipse(hdc, left, top, right, bottom);

    // Restore the old pen and delete the created pen
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Restore the old brush and delete the created brush
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
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
            const double xmin = 10;
            const double ymin = 10;
            const double xmax = 1890;
            const double ymax = 990;

            // Draw Border
            line(hdc, xmin, ymin, xmax, ymin);
            line(hdc, xmax, ymin, xmax, ymax);
            line(hdc, xmax, ymax, xmin, ymax);
            line(hdc, xmin, ymax, xmin, ymin);

            double x;
            double y;
            int n = 100;

            const double x_incr = (xmax - xmin) / n;
            const double y_incr = (ymax - ymin) / n;

            for (int i = 0; i < n; i++) {
                y = 10 + y_incr * i;           // make it vary from 10 to 990
                x = 10 + x_incr * i;           // make it vary from 10 to 1890

                chg_color(hdc, hOldPen, i*255/n, 0, 0);
                line(hdc, xmin, y, x, ymax);

                chg_color(hdc, hOldPen, 0, i*255/n, 0);
                line(hdc, x, ymax, xmax, (ymax-y));

                chg_color(hdc, hOldPen, 0, 0, i*255/n);
                line(hdc, xmin, (ymax-y), x, ymin);

                chg_color(hdc, hOldPen, i*255/n, 255-i*255/n, 255-i*255/n);
                line(hdc, x, ymin, xmax, y);
            } 
            
            double xcen = (xmax - xmin) /2 ;
            double ycen = (ymax - ymin) /2 ;
            double max_dia = 750;

            // Draw non-filled in circles             
            for (int i = max_dia; i > 225; i-=1) {
                circle(hdc, xcen, ycen, i, 27, 50+i*100/max_dia, 155+i*100/max_dia);
            }

            // filled in black circle
            circleFilled(hdc, xcen, ycen, 225, 0, 0, 0);
  
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