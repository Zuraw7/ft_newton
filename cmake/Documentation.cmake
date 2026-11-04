# ==========================================
# Documentation
# ==========================================

find_package(Doxygen REQUIRED)

set(DOXYGEN_IN
    "${PROJECT_SOURCE_DIR}/Doxyfile"
)

add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${PROJECT_BINARY_DIR}/docs"

    COMMAND ${DOXYGEN_EXECUTABLE}
        "${DOXYGEN_IN}"

    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"

    COMMENT "Generating API documentation with Doxygen"

    VERBATIM
)
