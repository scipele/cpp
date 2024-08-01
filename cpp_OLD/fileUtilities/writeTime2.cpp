#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

// GetLastWriteTime - Retrieves the last-write time and converts
//                    the time to a string
//
// Return value - TRUE if successful, FALSE otherwise
// lpszFileName - File name/path of the file to get the last write time for
// lpszString   - Pointer to buffer to receive string
// dwSize       - Size of the buffer

BOOL GetLastWriteTime(LPCTSTR lpszFileName, LPTSTR lpszString, DWORD dwSize)
{
    HANDLE hFile = CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // Error opening the file
        return FALSE;
    }

    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;

    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
    {
        CloseHandle(hFile);
        return FALSE;
    }

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    // Build a string showing the date and time.
    DWORD dwRet = StringCchPrintf(lpszString, dwSize,
        TEXT("%02d/%02d/%d  %02d:%02d"),
        stLocal.wMonth, stLocal.wDay, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute);

    CloseHandle(hFile);

    return (S_OK == dwRet);
}



// Include the definition of GetLastWriteTime function here

int main()
{
    TCHAR szBuf[MAX_PATH];

    // Replace "filename.txt" with the actual file name/path
    if (GetLastWriteTime(TEXT("C:/Users/tsciple/cpp/cpp/fileUtilities/listFiles5.cpp"), szBuf, MAX_PATH))
    {
        _tprintf(TEXT("Last write time is: %s\n"), szBuf);
    }
    else
    {
        _tprintf(TEXT("Failed to get last write time.\n"));
    }

    return 0;
}