# Extract Page 1 from PDFs

A C++ utility that extracts the first page from all PDF files in a folder and saves them to an output folder.
**Cross-platform:** Works on Windows and Linux.

## Usage

```bash
./extr <input_folder> <output_folder>
```

Windows:
```cmd
extr.exe <input_folder> <output_folder>
```

### Arguments

| Argument        | Description                                      |
|-----------------|--------------------------------------------------|
| `input_folder`  | Path to folder containing PDF files to process   |
| `output_folder` | Path to folder where extracted pages are saved   |

### Example

```bash
# Linux
./extr ./input ./output

# Windows
extr.exe C:\PDFs\input C:\PDFs\output
```

### Output

- Extracted PDFs are named with `_page1` suffix
- Example: `document.pdf` → `document_page1.pdf`

```
Extracted: ./isos/2001-176-OW-1152-01.pdf -> ./output/2001-176-OW-1152-01_page1.pdf
Extracted: ./isos/2001-176-OW-1152-02.pdf -> ./output/2001-176-OW-1152-02_page1.pdf
...
Complete: 10 succeeded, 0 failed.
```

## Notes

- The output folder is created automatically if it doesn't exist
- Only files with `.pdf` extension are processed
- The program uses `pdfseparate` from poppler-utils internally
- Paths with spaces are handled correctly on both platforms



## Prerequisites

- **poppler-utils** - Provides the `pdfseparate` command-line tool
- **C++17 compiler** (g++, MSVC, or clang++)

### Install Prerequisites

#### Linux (Ubuntu/Debian)

```bash
sudo apt install poppler-utils g++
```

#### Windows

**Option 1: Using Chocolatey**
```powershell
choco install poppler
```

**Option 2: Manual Installation**
1. Download poppler for Windows from: https://github.com/oschwartz10612/poppler-windows/releases
2. Extract to a folder (e.g., `C:\poppler`)
3. Add the `bin` folder to your PATH (e.g., `C:\poppler\Library\bin`)

## Compile

### Linux

```bash
g++ -std=c++17 -o extr src/extr.cpp
```

With optimizations:
```bash
g++ -std=c++17 -O2 -o extr src/extr.cpp
```

### Windows (MSVC - Developer Command Prompt)

```cmd
cl /std:c++17 /EHsc /Fe:extr.exe src\extr.cpp
```

### Windows (MinGW/g++)

```cmd
g++ -std=c++17 -o extr.exe src/extr.cpp
```

