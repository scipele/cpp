# Compile Notes

Make sure that your tasks .json includes the following

    "args": [
        "-fdiagnostics-color=always",
        "-g",
        "${file}",      // This is for the current file
        "class\\*.cpp",  // Add this line to compile class implementation files along with the base .cpp file
        "-o",           // compiler 
        "${fileDirname}\\${fileBasenameNoExtension}.exe",
        "-static",       // Add this line to static compile
        "-lbcrypt"
    ],
