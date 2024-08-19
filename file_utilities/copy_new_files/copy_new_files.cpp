// filename:    copy_new_files
//
// Purpose:     copy over only files if they are new files determined as follows
//
// Structure:
//  1. Main Program - This File (copy_new_files.cpp):
//  - Gets the paths from the user.
//  - Uses the CompareFiles class to compare files and copy only the new ones.
//
//  2. GetPaths Class (GetPaths.hpp):
//  - Handles user input to get the source and destination paths.
//  - Validates the paths.
//
//  3. CompareFiles Class:
//  - Creates a map of original files and the new ones by calling the .get_hashes method from the
//    the DirectoryAnalyzer Class
//  - compares the hashes of the original/new files and only copies over the new files that 
//    have different hash codes.
//  - creates a print to the screen showning the hash values and the filenames
//  - creates a log file to record the new files that were copied and the ones that were
//    skipped if they were already existing in the orig path provided
//
//  4. DirectoryAnalyzer Class (DirectoryAnalyzer.hpp):
//  - Analyzes the directory and computes the hash values for each file.
//  - Stores these hash values in a map for comparison.
//
//  5. FileHasher Class (FileHasher.hpp):
//  - Computes the SHA-1 hash for files, with a maximum size limit of 50MB.
//  - Handles file I/O and hashing operations.
//
// Dependencies:
//       +-------+-----------------------+-------------------+----------------------+
//       |  Seq  |  Class File Name      |  Class Name       |  Public Method       |
//       +-------+-----------------------+-------------------+----------------------+
//       |  1    | GetPaths.hpp          | GetPaths          | .get_path_locs()     |
//       |  2    | CompareFiles.hpp      | CompareFiles      | .compare_and_copy()  |
//       |  2.1  | DirectoryAnalyzer.hpp | DirectoryAnalyzer | .get_hashes()         |
//       |  2.2  | FileHasher.hpp        | FileHasher        | .sha1FileHash()      |
//       +-------+-----------------------+-------------------+----------------------+
//
// Output:  The log file listing is placed in the orig path file_comparison_log.csv
// By:      Tony Sciple  08/19/2024

#include <iostream>
#include "GetPaths.hpp"
#include "CompareFiles.hpp"

int main() {

    // call a class to get path names from the user and validate that the paths are legit
    GetPaths pth;
    pth.get_path_locs();

    CompareFiles comparer(pth.copy_to_path, pth.copy_from_path);
    comparer.compare_and_copy();

    // Pause the console window before exiting
    system("pause");
    return 0;
}