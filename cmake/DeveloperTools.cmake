
# ==========================================
# Developer tools
# ==========================================

# clang-format
find_program(CLANG_FORMAT_EXECUTABLE clang-format)

if(CLANG_FORMAT_EXECUTABLE)

    file(GLOB_RECURSE FORMAT_SOURCES
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/engine/*.cpp"
        "${PROJECT_SOURCE_DIR}/engine/*.hpp"
        "${PROJECT_SOURCE_DIR}/engine/*.h"
        "${PROJECT_SOURCE_DIR}/game/*.cpp"
        "${PROJECT_SOURCE_DIR}/game/*.hpp"
        "${PROJECT_SOURCE_DIR}/game/*.h"
        "${PROJECT_SOURCE_DIR}/tests/*.cpp"
        "${PROJECT_SOURCE_DIR}/tests/*.hpp"
    )

    # Format source files
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${FORMAT_SOURCES}
        COMMENT "Formatting source files"
        VERBATIM
    )

    # Check source formatting
    add_custom_target(check-format
        COMMAND ${CLANG_FORMAT_EXECUTABLE}
            --dry-run
            --Werror
            ${FORMAT_SOURCES}
        COMMENT "Checking source formatting"
        VERBATIM
    )

else()
    message(STATUS "clang-format not found. Formatting targets disabled.")
endif()

# ==========================================
# Run application
# ==========================================

add_custom_target(run
    COMMAND $<TARGET_FILE:ft_newton>
    DEPENDS ft_newton
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Running ft_newton"
    VERBATIM
)
