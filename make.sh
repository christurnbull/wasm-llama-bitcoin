#!/bin/bash

cd build

rm -rf *
cmake cmake .. 

make -j4
