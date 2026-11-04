
#!/usr/bin/env bash

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$PROJECT_ROOT"

cmake -S . -B build -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build --target docs -j"$(nproc)"

echo "Documentation generated successfully!"

xdg-open build/docs/html/index.html
