add_library(newton_input STATIC
    src/InputManager.cpp
    src/utils.cpp
)

set_project_warnings(newton_input)

target_include_directories(newton_input PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(newton_input
    PUBLIC
        newton_common
        glfw
)
