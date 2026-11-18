# ==========================================
# Engine libraries
# ==========================================

# Each module is a static library with its own include directory, so a module
# sees only the headers of the modules it links to.
add_subdirectory(common)
add_subdirectory(geometry)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(math)
add_subdirectory(scene)
