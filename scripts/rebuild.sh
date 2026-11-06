#!/usr/bin/env bash

set -e

rm -rf build/debug

cmake -S . -B build/debug -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build/debug -j"$(nproc)"