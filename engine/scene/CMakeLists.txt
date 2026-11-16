add_library(newton_scene STATIC
    src/Scene.cpp
    src/Transform.cpp
)

set_project_warnings(newton_scene)

target_include_directories(newton_scene PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(newton_scene
    PUBLIC
        glm::glm
    PRIVATE
        newton_common
)
