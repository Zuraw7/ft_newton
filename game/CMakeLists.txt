add_executable(ft_newton
    src/controllers/CameraController.cpp
    src/main.cpp
    src/ui/CameraPanel.cpp
    src/ui/DisplayPanel.cpp
    src/ui/ImGuiLayer.cpp
    src/ui/ObjectCreatorPanel.cpp
    src/ui/StatsOverlay.cpp
)

set_project_warnings(ft_newton)

set_target_properties(ft_newton PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)

target_include_directories(ft_newton PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ft_newton SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/external
)

target_link_libraries(ft_newton PRIVATE
    newton_common
    newton_geometry
    newton_graphics
    newton_input
    newton_scene
    imgui
)
