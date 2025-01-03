# main filename - copy_new_files.cpp

## Purpose - copy over only files if they are new files determined as follows

## Structure:
### Main Program - This File (copy_new_files.cpp):
- Gets the paths from the user.
- Uses the CompareFiles class to compare files and copy only the new ones.

### GetPaths Class (GetPaths.hpp):
- Handles user input to get the source and destination paths.
- Validates the paths.

### CompareFiles Class:
- Creates a map of original files and the new ones by calling the .get_hashes method from the
  the DirectoryAnalyzer Class
- compares the hashes of the original/new files and only copies over the new files that 
  have different hash codes.
- creates a print to the screen showning the ahas values and the filenames
- creates a log file to record the new files that were copied and the ones that were
  skipped if they were already existing in the orig path provided

### DirectoryAnalyzer Class (DirectoryAnalyzer.hpp):
- Analyzes the directory and computes the hash values for each file.
- Stores these hash values in a map for comparison.

### FileHasher Class (FileHasher.hpp):
- Computes the SHA-1 hash for files, with a maximum size limit of 50MB.
- Handles file I/O and hashing operations.

## Dependencies:
| Seq | Class File Name        | Class Name        | Public Method         |
|-----|------------------------|-------------------|-----------------------|
| 1   | `GetPaths.hpp`          | `GetPaths`        | `.get_path_locs()`   |
| 2   | `CompareFiles.hpp`      | `CompareFiles`    | `.compare_and_copy()`|
| 2.1 | `DirectoryAnalyzer.hpp` | `DirectoryAnalyzer`| `.get_hashes()`     |
| 2.2 | `FileHasher.hpp`        | `FileHasher`      | `.sha1FileHash()`    |
