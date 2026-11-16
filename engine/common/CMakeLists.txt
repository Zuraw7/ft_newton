add_library(newton_common STATIC
    src/Logger.cpp
)

set_project_warnings(newton_common)

target_include_directories(newton_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
