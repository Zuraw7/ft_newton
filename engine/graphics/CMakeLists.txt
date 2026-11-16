add_library(newton_graphics STATIC
    src/buffers/IndexBuffer.cpp
    src/buffers/VertexArray.cpp
    src/buffers/VertexBuffer.cpp
    src/Camera.cpp
    src/ColliderRenderer.cpp
    src/Context.cpp
    src/GLError.cpp
    src/GridRenderer.cpp
    src/Mesh.cpp
    src/MeshRenderer.cpp
    src/Shader.cpp
)

set_project_warnings(newton_graphics)

target_include_directories(newton_graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Libraries used in the public headers are PUBLIC, glad is used only in the sources
target_link_libraries(newton_graphics
    PUBLIC
        newton_common
        newton_geometry
        glfw
        glm::glm
    PRIVATE
        glad
)
