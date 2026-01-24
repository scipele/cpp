# extr_bom.cpp

## Compile Args
### Linux
            "args": [
                "-fdiagnostics-color=always",
                "-std=c++17",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}",
                " $(pkg-config --cflags --libs poppler-cpp)"
            ],



## Windows
- g++ -std=c++17 extr_bom.cpp -o extr_bom.exe -I<vcpkg_path>/installed/x64-windows/include -L<vcpkg_path>/installed/x64-windows/lib -lpoppler-cpp