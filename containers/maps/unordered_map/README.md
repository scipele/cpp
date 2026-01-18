# Example Parameters for VS Code tasks.json

## Args array 
                "-std=c++20",
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "../../../hash_funcs/std_lib/MurmurHash2.cpp",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
