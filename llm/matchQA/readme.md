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