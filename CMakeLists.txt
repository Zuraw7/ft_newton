cmake_minimum_required(VERSION 3.20)

project(ft_newton
	VERSION 1.0
	DESCRIPTION "A simple physics engine"
	LANGUAGES C CXX
)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CMake includes
include(cmake/CompilerOptions.cmake)
include(cmake/Dependencies.cmake)
include(cmake/DeveloperTools.cmake)
include(cmake/Documentation.cmake)

# Source files
add_executable(ft_newton
    src/common/Logger.cpp
    src/graphics/Camera.cpp
    src/graphics/Context.cpp
    src/graphics/resolution.cpp
    src/input/InputManager.cpp
    src/input/utils.cpp
    src/main.cpp
)

# CMake functions
set_project_warnings(ft_newton)

# Include directories
target_include_directories(ft_newton PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(ft_newton SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/external
)

# Link libraries
target_link_libraries(ft_newton PRIVATE
    glad
    glfw
    glm::glm
    assimp
)
