#include "C:/dev_tools/vcpkg/installed/x64-windows-static/include/miniz/miniz.h"
#include <iostream>

int main() {
    mz_zip_archive zip_archive = {0};
    std::cout << "Miniz version: " << MZ_VERSION << std::endl;
    return 0;
}