
# make emscripten tools available
source ~/emsdk/emsdk_env.sh

# dir to the llama.cpp release source files
cd ../llama.cpp-b3578

# build only the llama.cpp lib files and enable SIMD and multithreading
mkdir -p embuild && cd embuild
emcmake cmake -DCMAKE_CXX_FLAGS="-O3 -msimd128 -s USE_PTHREADS=1" -DLLAMA_BUILD_EXAMPLES=NO -DLLAMA_BUILD_SERVER=NO -DLLAMA_BUILD_TESTS=NO ..
emmake make -j4

# copy the lib files to the wasm-llama-bitcoin directory
cd ../../wasm-llama-bitcoin/
mkdir -p ./lib/em-llama.cpp-b3578 && cd ./lib/em-llama.cpp-b3578
cp ../../../llama.cpp-b3578/embuild/src/libllama.a .
cp ../../../llama.cpp-b3578/embuild/common/libcommon.a .
cp ../../../llama.cpp-b3578/embuild/ggml/src/libggml.a .
