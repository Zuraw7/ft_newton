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
    src/controllers/CameraController.cpp
    src/geometry/Primitives.cpp
    src/graphics/buffers/IndexBuffer.cpp
    src/graphics/buffers/VertexArray.cpp
    src/graphics/buffers/VertexBuffer.cpp
    src/graphics/Camera.cpp
    src/graphics/Context.cpp
    src/graphics/GLError.cpp
    src/graphics/GridRenderer.cpp
    src/graphics/Mesh.cpp
    src/graphics/Renderer.cpp
    src/graphics/Shader.cpp
    src/input/InputManager.cpp
    src/input/utils.cpp
    src/main.cpp
    src/physics/PhysicsWorld.cpp
    src/scene/Scene.cpp
    src/scene/Transform.cpp
    src/ui/CameraPanel.cpp
    src/ui/DisplayPanel.cpp
    src/ui/ImGuiLayer.cpp
    src/ui/ObjectCreatorPanel.cpp
    src/ui/StatsOverlay.cpp
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
    imgui
)
