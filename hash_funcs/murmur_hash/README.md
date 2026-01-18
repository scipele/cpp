# Example Parameters for VS Code tasks.json

## Args array 
    "-std=c++20",
    "-fdiagnostics-color=always",
    "-g",
    "${file}",
    "../std_lib/MurmurHash2.cpp",
    "../std_lib/MurmurHash3.cpp",
    "-o",
    "${fileDirname}/${fileBasenameNoExtension}"
