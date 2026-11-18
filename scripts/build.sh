#!/usr/bin/env bash

set -e

cmake -S . -B build/debug -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DNEWTON_BUILD_TESTS=ON

cmake --build build/debug -j"$(nproc)"