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

option(NEWTON_BUILD_TESTS "Build unit tests" OFF)

# CMake includes
include(cmake/CompilerOptions.cmake)
include(cmake/Dependencies.cmake)
include(cmake/DeveloperTools.cmake)
include(cmake/Documentation.cmake)

# Engine libraries and the game executable
add_subdirectory(engine)
add_subdirectory(game)

# Unit tests, run with ctest
if(NEWTON_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
