
# dir to the llama.cpp release source files
cd ../llama.cpp-b3578

# build only the llama.cpp lib files
mkdir -p build && cd build
cmake -DLLAMA_BUILD_EXAMPLES=NO -DLLAMA_BUILD_SERVER=NO -DLLAMA_BUILD_TESTS=NO ..
make -j4

# copy the lib files to the wasm-llama-bitcoin directory
cd ../../wasm-llama-bitcoin/
mkdir -p ./lib/llama.cpp-b3578 && cd ./lib/llama.cpp-b3578
cp ../../../llama.cpp-b3578/build/src/libllama.so .
cp ../../../llama.cpp-b3578/build/common/libcommon.a .
cp ../../../llama.cpp-b3578/build/ggml/src/libggml.so .
