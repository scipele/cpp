# Example Parameters for VS Code tasks.json

## Args array 
                "-fdiagnostics-color=always",                      // Enables colored diagnostic messages (e.g., errors, warnings) in the terminal for better readability.
                "-std=c++17",                                      // Specifies the C++ standard to use (C++17)
                "-g",                                              // Includes debugging information in the compiled output, w/gdb tools
                "${file}",                                         // Refers to the currently active source file
                "-o",                                              // indicates that the next argument will be the exe file name
                "${fileDirname}\\${fileBasenameNoExtension}",      // Sets the output executable file name
                "-mwindows"                                        // Compile with windows app