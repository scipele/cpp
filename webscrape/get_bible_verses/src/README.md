# Example Parameters for VS Code tasks.json

## Args array (Windows)
                "-fdiagnostics-color=always",
                "-std=c++17",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}",
                "-lcurl",
                "-lws2_32"


## Args array (Linux)
                "-fdiagnostics-color=always",
                "-std=c++17",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}",
                "-lcurl"

command line within the folder
g++ -fdiagnostics-color=always -std=c++17 -g scrape_bible_verses.cpp -o scrape_bible_verses -lcurl