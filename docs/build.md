# Build & Development {#build}

[TOC]

## Quick Start

```bash
make
./ft_newton
```

Run the executable from the repository root: shaders and other assets are loaded with paths relative to the working
directory (`assets/...`).

## Make

| Command | Description |
| --- | --- |
| `make` | Configure and build the project (Release), copy the executable to the root |
| `make BUILD_TYPE=Debug` | Same, but in Debug mode |
| `make docs` | Generate Doxygen documentation |
| `make test` | Configure and build in Debug with tests, run them |
| `make clean` | Remove the whole `build/` directory (both modes, dependencies and docs) |
| `make fclean` | `clean` and remove the executable |
| `make re` | `fclean` and build again |

Each mode has its own build directory: `build/release` for `make`, `build/debug` for `make BUILD_TYPE=Debug`,
the scripts in `scripts/` and the commands below. Switching between modes does not trigger a rebuild.

## Requirements

- CMake 3.20+
- C++17 compatible compiler (GCC / Clang)
- GNU Make
- Python 3 (GLAD code generation)
- Doxygen
- Graphviz
- clang-format

## Configuration

Configure the project using CMake:

```bash
cmake -S . -B build/debug -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
```

## Build Commands

| Command | Description |
| --- | --- |
| `cmake --build build/debug` | Build the project |
| `cmake --build build/debug --target run` | Build and run the application from the repository root |
| `cmake --build build/debug --target clean` | Remove compiled build artifacts |
| `./scripts/build.sh` | Configure and build the project (Debug) |
| `./scripts/rebuild.sh` | Clean, configure and rebuild the project |

For compiler options see `cmake/CompilerOptions.cmake`.

## Development Tools

| Command | Description |
| --- | --- |
| `cmake --build build/debug --target docs` | Generate Doxygen documentation |
| `cmake --build build/debug --target format` | Format source files using clang-format |
| `cmake --build build/debug --target check-format` | Check source formatting |

## Tests

Unit tests use GoogleTest and live in `tests/`, mirroring the engine modules (`tests/math/` tests `newton_math`).
They are built into the `newton_tests` executable only with `-DNEWTON_BUILD_TESTS=ON`, which `make test` and the
scripts in `scripts/` pass. `make` builds the game without tests.

| Command | Description |
| --- | --- |
| `ctest --test-dir build/debug --output-on-failure` | Run all tests, print the output of failed ones |
| `./build/debug/tests/newton_tests` | Run the test executable directly |
| `./build/debug/tests/newton_tests --gtest_filter='Vec3.*'` | Run only the tests of one suite |

## Documentation

Generated documentation is available at:

```bash
build/docs/html/index.html
```

To generate and open documentation automatically:

```bash
./scripts/docs.sh
```

## Dependencies

External dependencies are managed automatically through CMake FetchContent.

| Library | Purpose |
| --- | --- |
| GLFW 3.4 | Window management and input |
| GLAD 2 | OpenGL 3.3 core function loading |
| GLM 1.0 | Mathematics for graphics |
| Dear ImGui 1.91 | Debug and settings UI |
| GoogleTest 1.17 | Unit tests, downloaded only with `NEWTON_BUILD_TESTS=ON` |
| stb_image | Image loading |

Dependencies are downloaded and built locally inside each build directory (`build/debug/dependencies`,
`build/release/dependencies`). Their headers are included as system headers, so project warnings do not apply to them.
