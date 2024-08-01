#include <Windows.h>

/*compile on command line with batch file like the following
    kill simple_window.exe
    g++ -o simple_window.exe simple_window.cpp -mwindows
    pause
    simple_window.exe
*/

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
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            TextOutW(hdc, 10, 10, L"This is my first Window", 24); // Note the use of TextOutW
            TextOutW(hdc, 10, 30, L"Not super impressive", 21);
            TextOutW(hdc, 10, 50, L"but is kinda cool", 18);
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