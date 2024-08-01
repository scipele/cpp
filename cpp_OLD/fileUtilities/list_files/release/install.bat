@echo off
setlocal

REM Define variables
set "EXE_NAME=listFiles_v11.exe"
set "EXCEL_FILE=getFileList.xlsm"
set "INSTALL_DIR=%ProgramFiles%\listFiles"
set "DESKTOP_DIR=%UserProfile%\Desktop"
set "SHORTCUT_NAME=listFiles.lnk"
set "EXCEL_SHORTCUT_NAME=getFileList.lnk"

REM Create the installation directory if it does not exist
if not exist "%INSTALL_DIR%" (
    mkdir "%INSTALL_DIR%"
)

REM Copy the executable to the installation directory
copy "%~dp0%EXE_NAME%" "%INSTALL_DIR%\%EXE_NAME%" /Y

REM Copy the Excel file to the installation directory
copy "%~dp0%EXCEL_FILE%" "%INSTALL_DIR%\%EXCEL_FILE%" /Y

REM Check if the copies were successful
if errorlevel 1 (
    echo Failed to copy files to %INSTALL_DIR%.
    exit /b 1
)

REM Create a shortcut for the executable on the desktop using PowerShell
powershell -Command "$WS = New-Object -ComObject WScript.Shell; $Shortcut = $WS.CreateShortcut('%DESKTOP_DIR%\%SHORTCUT_NAME%'); $Shortcut.TargetPath = '%INSTALL_DIR%\%EXE_NAME%'; $Shortcut.Save()"

REM Check if the shortcut creation was successful
if errorlevel 1 (
    echo Failed to create shortcut for %EXE_NAME% on desktop.
    exit /b 1
)

REM Create a shortcut for the Excel file on the desktop using PowerShell
powershell -Command "$WS = New-Object -ComObject WScript.Shell; $Shortcut = $WS.CreateShortcut('%DESKTOP_DIR%\%EXCEL_SHORTCUT_NAME%'); $Shortcut.TargetPath = '%INSTALL_DIR%\%EXCEL_FILE%'; $Shortcut.Save()"

REM Check if the shortcut creation was successful
if errorlevel 1 (
    echo Failed to create shortcut for %EXCEL_FILE% on desktop.
    exit /b 1
)

echo Installation completed.
pause
endlocal
