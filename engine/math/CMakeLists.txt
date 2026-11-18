add_library(newton_math STATIC
    src/Vec3.cpp
)

set_project_warnings(newton_math)

target_include_directories(newton_math PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
