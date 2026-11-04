# ft_newton

[TOC]

Simple physics engine

## Build & Development

### Requirements

- CMake 3.20+
- C++17 compatible compiler (GCC / Clang)
- GNU Make
- Python 3
- Doxygen
- Graphviz
- clang-format

### Configuration

Configure the project using CMake:

```bash
    cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
```

### Build Commands

| Command | Description |
| --- | --- |
| `cmake --build build` | Build the project |
| `cmake --build build --target run` | Build and run the application |
| `cmake --build build --target clean` | Remove compiled build artifacts |
| `./scripts/build.sh` | Configure and build the project |
| `./scripts/rebuild.sh` | Clean, configure and rebuild the project |

### Development Tools

| Command | Description |
| --- | --- |
| `cmake --build build --target docs` | Generate Doxygen documentation |
| `cmake --build build --target format` | Format source files using clang-format |
| `cmake --build build --target check-format` | Check source formatting |

### Documentation

Generated documentation is available at:

```bash
    build/docs/html/index.html
```

To generate and open documentation automatically:

```bash
    ./scripts/docs.sh
```

### Dependencies

External dependencies are managed automatically through CMake FetchContent.

| Library | Purpose |
| --- | --- |
| GLFW 3 | Window management and input |
| GLAD 2 | OpenGL function loading |
| GLM | Mathematics |
| Assimp | 3D model importing |
| stb_image | Image loading |

Dependencies are downloaded and built locally inside the `build/` directory.
