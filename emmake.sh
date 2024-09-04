#!/bin/bash

mkdir -p embuild && cd embuild
rm -rf *
emcmake cmake .. && emmake make -j4

cp wasmLlamaCpp.js ../../ng-llama-bitcoin/src/assets/
cp wasmLlamaCpp.wasm ../../ng-llama-bitcoin/src/assets/
cp wasmLlamaCpp.worker.js ../../ng-llama-bitcoin/src/assets/
