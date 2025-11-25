# Build for Msys2 ucrt64 terminal
cd /c/dev/cpp/llm/matchQA

g++ -std=c++17 -O3 -march=native -Wall -Wextra \
    -I /c/dev/cpp/llm/llama.cpp/include \
    -I /c/dev/cpp/llm/llama.cpp/ggml/include \
    ./src/match_qa.cpp \
    /c/dev/cpp/llm/llama.cpp/build/src/libllama.a \
    /c/dev/cpp/llm/llama.cpp/build/ggml/src/ggml-base.a \
    /c/dev/cpp/llm/llama.cpp/build/ggml/src/ggml.a \
    /c/dev/cpp/llm/llama.cpp/build/ggml/src/ggml-cpu.a \
    -static -static-libgcc -static-libstdc++ \
    -Wl,--whole-archive -lwinpthread -Wl,--no-whole-archive \
    -lgomp -lm \
    -o ./build/match_qa.exe