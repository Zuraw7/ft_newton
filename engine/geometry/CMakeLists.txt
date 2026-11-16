add_library(newton_geometry STATIC
    src/Primitives.cpp
)

set_project_warnings(newton_geometry)

target_include_directories(newton_geometry PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(newton_geometry
    PUBLIC
        glm::glm
    PRIVATE
        newton_common
)
