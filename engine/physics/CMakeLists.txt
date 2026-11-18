add_library(newton_physics STATIC
    src/Particle.cpp
)

set_project_warnings(newton_physics)

target_include_directories(newton_physics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(newton_physics
    PUBLIC
        newton_math
)
