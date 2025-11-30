// clipboard.cpp
// Works perfectly with ucrt64 (MSYS2) on Windows 11
// g++ -std=c++17 clipboard.cpp -o clipboard.exe -lole32 -luuid -luser32 -static

#include <windows.h>
#include <objidl.h>
#include <iostream>
#include <string>
#include <vector>

std::string wstr_to_utf8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &result[0], size, nullptr, nullptr);
    return result;
}

int main()
{
    OleInitialize(nullptr);

    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard\n";
        OleUninitialize();
        return 1;
    }

    UINT format = RegisterClipboardFormatW(L"ClipboardHistoryItemsList");
    if (!format) {
        std::cout << "Clipboard history disabled or not supported\n";
        CloseClipboard(); OleUninitialize();
        return 0;
    }

    HANDLE hData = GetClipboardData(format);
    if (!hData) {
        std::cout << "Clipboard history is empty\n";
        CloseClipboard(); OleUninitialize();
        return 0;
    }

    SIZE_T size = GlobalSize(hData);
    BYTE* data = (BYTE*)GlobalLock(hData);
    if (!data || size < 4) {
        GlobalUnlock(hData); CloseClipboard(); OleUninitialize();
        return 1;
    }

    UINT count = *(UINT*)data;
    BYTE* ptr = data + 4;

    std::cout << "Clipboard history has " << count << " item(s):\n";
    std::cout << std::string(60, '-') << "\n\n";

    for (UINT i = 0; i < count && ptr + sizeof(GUID) < data + size; ++i) {
        GUID guid;
        memcpy(&guid, ptr, sizeof(GUID));
        ptr += sizeof(GUID);

        std::wstring json = (wchar_t*)ptr;
        size_t len = json.find(L'\0');
        if (len == std::wstring::npos) break;
        json.resize(len);
        ptr += (len + 1) * sizeof(wchar_t);

        std::cout << "[" << (i + 1) << "]\n";

        // Look for plain text in JSON
        size_t pos = json.find(L"\"text\":\"");
        if (pos != std::wstring::npos) {
            pos += 8;
            size_t end = json.find(L"\"", pos);
            if (end != std::wstring::npos) {
                std::wstring text = json.substr(pos, end - pos);
                // Simple unescape
                size_t n;
                while ((n = text.find(L"\\n")) != std::wstring::npos) text.replace(n, 2, L"\n");
                while ((n = text.find(L"\\r")) != std::wstring::npos) text.replace(n, 2, L"\r");
                while ((n = text.find(L"\\\"")) != std::wstring::npos) text.replace(n, 2, L"\"");
                while ((n = text.find(L"\\\\")) != std::wstring::npos) text.replace(n, 2, L"\\");

                std::cout << wstr_to_utf8(text) << "\n\n";
                continue;
            }
        }

        // No text → probably image/file/HTML
        if (json.find(L"\"image/png\"") != std::wstring::npos ||
            json.find(L"image/") != std::wstring::npos)
            std::cout << "[Image]\n\n";
        else if (json.find(L"\"file\"") != std::wstring::npos)
            std::cout << "[File(s)]\n\n";
        else if (json.find(L"\"html\"") != std::wstring::npos)
            std::cout << "[HTML content]\n\n";
        else
            std::cout << "[Other/unknown format]\n\n";
    }

    GlobalUnlock(hData);
    CloseClipboard();
    OleUninitialize();

    std::cout << "Finished.\n";
    system("pause");
    return 0;
}