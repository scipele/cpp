# Extract Page 1 from PDFs

A C++ utility that extracts the first page from all PDF files in a folder and saves them to an output folder.

## Prerequisites

- **poppler-utils** - Provides the `pdfseparate` command-line tool
- **g++** with C++17 support

### Install Prerequisites (Ubuntu/Debian)

```bash
sudo apt install poppler-utils g++
```

## Compile

```bash
g++ -std=c++17 -o extract_pg1 src/extract_pg1.cpp
```

Or with optimizations for release:

```bash
g++ -std=c++17 -O2 -o extract_pg1 src/extract_pg1.cpp
```

## Usage

```bash
./extract_pg1 <input_folder> <output_folder>
```

### Arguments

| Argument        | Description                                      |
|-----------------|--------------------------------------------------|
| `input_folder`  | Path to folder containing PDF files to process   |
| `output_folder` | Path to folder where extracted pages are saved   |

### Example

```bash
# Extract first page from all PDFs in ./isos folder and save to ./output
./extract_pg1 ./isos ./output
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
