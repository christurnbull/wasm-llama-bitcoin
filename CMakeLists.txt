cmake_minimum_required(VERSION 3.16)
project(wasmLlamaCpp)

if(EMSCRIPTEN)
    add_executable(wasmLlamaCpp src/cpp/emscripten/em-main.cpp)
    target_include_directories(wasmLlamaCpp PRIVATE "src/cpp/native")
else()
    add_executable(wasmLlamaCpp src/cpp/native/main.cpp)
endif()

target_include_directories(wasmLlamaCpp PRIVATE "../llama.cpp-b3578/include")
target_include_directories(wasmLlamaCpp PRIVATE "../llama.cpp-b3578/common")
target_include_directories(wasmLlamaCpp PRIVATE "../llama.cpp-b3578/ggml/include")

target_compile_features(wasmLlamaCpp PRIVATE cxx_std_17)

# target_link_libraries(wasmLlamaCpp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/llama.cpp/libcommon.a")
# target_link_libraries(wasmLlamaCpp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/llama.cpp/libggml_static.a")
# target_link_libraries(wasmLlamaCpp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/llama.cpp/libllama.a")

# find_package(OpenMP)
# if (OPENMP_FOUND)
#     set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()

if(NOT EMSCRIPTEN)

    target_link_directories(wasmLlamaCpp PRIVATE "lib/llama.cpp-b3578")
    target_link_libraries(wasmLlamaCpp PRIVATE llama common ggml)

    set(CMAKE_BUILD_TYPE RELEASE)
    set_target_properties(wasmLlamaCpp PROPERTIES COMPILE_FLAGS "-O3 -march=native")

endif(NOT EMSCRIPTEN)


if(EMSCRIPTEN)

    set(CMAKE_BUILD_TYPE Release)

    target_link_directories(wasmLlamaCpp PRIVATE "lib/em-llama.cpp-b3578")
    target_link_libraries(wasmLlamaCpp PRIVATE llama common ggml embind)
    
    set_target_properties(wasmLlamaCpp PROPERTIES COMPILE_FLAGS "-O3 -msimd128")
    set_target_properties(wasmLlamaCpp PROPERTIES LINK_FLAGS " \
    -s EXPORT_NAME=wasmLlamaCpp \
    -s MODULARIZE=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s EXPORTED_RUNTIME_METHODS=['PThread','FS','ccall'] \
    -s USE_PTHREADS=1 \
    -s PROXY_TO_PTHREAD=1 \
    -s ASYNCIFY=1 \
    ")

endif(EMSCRIPTEN)