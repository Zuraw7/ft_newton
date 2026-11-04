#!/usr/bin/env bash

set -e

cmake -S . -B build -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build -j"$(nproc)"