# Build for Msys2 ucrt64 terminal
cd /c/dev/cpp/llm/matchQA/src

g++ -std=c++20 -O3 -march=native -Wall -Wextra \
-I ../../llama.cpp/include \
-I ../../llama.cpp/ggml/include \
./match_qa.cpp \
../../llama.cpp/build/src/libllama.a \
../../llama.cpp/build/ggml/src/ggml-base.a \
../../llama.cpp/build/ggml/src/ggml.a \
../../llama.cpp/build/ggml/src/ggml-cpu.a \
-static -static-libgcc -static-libstdc++ \
-Wl,--whole-archive -lwinpthread -Wl,--no-whole-archive \
-lgomp -lm \
-o ../build/match_qa.exe


# Args in tasks.json
"args": [
    "-fdiagnostics-color=always",
    "-std=c++17",
    "-O3",
    "-march=native",
    "-Wall",
    "-Wextra",

    // Include paths – two levels up then into llama.cpp
    "-I${workspaceFolder}/cpp/llm/llama.cpp/include",
    "-I${workspaceFolder}/cpp/llm/llama.cpp/ggml/include",

    // ---------- source file ----------
    "${file}",

    // Static libraries – two levels up then into the build folders
    "${workspaceFolder}/cpp/llm/llama.cpp/build/src/libllama.a",
    "${workspaceFolder}/cpp/llm/llama.cpp/build/ggml/src/ggml-base.a",
    "${workspaceFolder}/cpp/llm/llama.cpp/build/ggml/src/ggml.a",
    "${workspaceFolder}/cpp/llm/llama.cpp/build/ggml/src/ggml-cpu.a",

    // Static linking flags (same as your working command)
    "-static",
    "-static-libgcc",
    "-static-libstdc++",
    "-lgomp",
    "-lm",

    // ---------- output ----------
    "-o",
    "${fileDirname}/${fileBasenameNoExtension}.exe"
],