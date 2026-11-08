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
| GLM 1.0 | Mathematics |
| Dear ImGui 1.91 | Debug and settings UI |
| stb_image | Image loading |

Dependencies are downloaded and built locally inside each build directory (`build/debug/dependencies`,
`build/release/dependencies`). Their headers are included as system headers, so project warnings do not apply to them.
