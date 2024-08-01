#include <Windows.h>
#include <ShlObj.h> // Shell functions
#include <string>   // For std::wstring
#include <iostream> // For std::wcout


// Function to create a window and display the folder picker dialog
std::wstring OpenFolderPicker(HWND hwnd) {
    std::wstring selectedFolder;

    // Initialize COM
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Initialize structure for folder selection dialog
    BROWSEINFO browseInfo = { 0 };
    browseInfo.hwndOwner = hwnd;
    browseInfo.lpszTitle = reinterpret_cast<LPCSTR>(L"Select a folder:");
    browseInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;

    // Display folder selection dialog
    LPITEMIDLIST pidl = SHBrowseForFolder(&browseInfo);
    if (pidl != NULL) {
        // Get the selected folder path
        wchar_t folderPath[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, folderPath)) {
        selectedFolder = folderPath;
        }

        // Free the PIDL memory
        CoTaskMemFree(pidl);
    }

    // Uninitialize COM
    CoUninitialize();

    return selectedFolder;
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            // Display folder picker dialog when the window is created
            MessageBox(hwnd, "Click OK to select a folder.", "Folder Picker", MB_OK | MB_ICONINFORMATION);
            {
                std::wstring folderPath = OpenFolderPicker(hwnd);
                MessageBox(hwnd, ("Selected folder: " + folderPath).c_str(), "Selected Folder", MB_OK | MB_ICONINFORMATION);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const wchar_t CLASS_NAME[] = L"FolderPickerWindow";

    WNDCLASSW wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    // Create the window
    HWND hwnd = CreateWindowExW(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        L"Folder Picker Example",       // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Dummy WinMain function to call wWinMain
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//    return wWinMain(hInstance, hPrevInstance, GetCommandLineW(), nCmdShow);
//}