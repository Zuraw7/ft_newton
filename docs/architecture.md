# Architecture {#architecture}

[TOC]

How the modules fit together and in what order things are created and destroyed.
Details of each class are in its own documentation, this page covers what is not visible from a single class.

## Modules

Each directory in `include/` and `src/` is one module with its own namespace `newton::<module>`.

| Module | Contents |
| --- | --- |
| `common` | @ref newton::common::Logger "Logger" shared by all modules |
| `geometry` | CPU-side mesh data (@ref newton::geometry::Vertex "Vertex", @ref newton::geometry::MeshData "MeshData") and primitive generators (@ref newton::geometry::makeCube "cube", @ref newton::geometry::makePlane "plane", @ref newton::geometry::makeSphere "sphere") |
| `graphics` | Window and OpenGL context (@ref newton::graphics::Context "Context"), frame timing, shaders (@ref newton::graphics::Shader "Shader"), GPU buffers, meshes (@ref newton::graphics::Mesh "Mesh"), camera (@ref newton::graphics::Camera "Camera"), renderer (@ref newton::graphics::Renderer "Renderer") |
| `input` | Keyboard and mouse state collected from GLFW callbacks (@ref newton::input::InputManager "InputManager") |
| `controllers` | Turn input into actions, e.g. @ref newton::controllers::CameraController "CameraController" moves the camera |
| `scene` | Objects in the world: @ref newton::scene::Scene "Scene", @ref newton::scene::SceneObject "SceneObject", @ref newton::scene::Transform "Transform" |
| `ui` | Dear ImGui lifetime (@ref newton::ui::ImGuiLayer "ImGuiLayer") and the windows drawn with it |

`main.cpp` creates the objects and connects them, the modules do not create each other.

## Dependencies

Modules are kept as independent of each other as possible: a module knows only the modules it really needs,
and data crosses module boundaries as plain values or references passed in from outside rather than through
modules reaching into each other. This keeps changes local and lets modules be reused or replaced
without touching the rest.

- `geometry` has no OpenGL dependency. It produces plain data, uploading it to the GPU is the job of @ref newton::graphics::Mesh "graphics::Mesh".
- `ui` panels receive the object whose settings they show (e.g. @ref newton::ui::drawDisplayPanel "drawDisplayPanel(Context&)") or plain values
  (@ref newton::ui::drawStatsOverlay "drawStatsOverlay(int fps, std::size_t objectCount)"). One panel per module.
- `controllers` know nothing about the UI. Whether the mouse belongs to an ImGui window is passed in as a plain `bool`.
- `scene` only stores objects, `main.cpp` passes them to the renderer. Objects point to meshes they do not own.

## Startup and shutdown order

| Step | Why |
| --- | --- |
| @ref newton::graphics::Context::create "graphics::Context::create()" | Creates the window and the OpenGL context, every OpenGL call needs it |
| @ref newton::input::InputManager::init "input::InputManager::init()" | Installs GLFW input callbacks |
| @ref newton::ui::ImGuiLayer "ui::ImGuiLayer" | Must come after `InputManager::init()`: the ImGui GLFW backend chains the callbacks already set, callbacks set later would replace its own |
| @ref newton::graphics::Renderer::create "graphics::Renderer::create()" | Compiles the shader, needs the context |
| @ref newton::graphics::GridRenderer::create "graphics::GridRenderer::create()" | Compiles the grid shader, needs the context |
| Camera, controller, meshes | Meshes upload data to the GPU, need the context |
| @ref newton::scene::Scene "scene::Scene" | Its objects point to meshes, which must outlive it |

Everything that owns OpenGL objects (meshes, buffers, shaders, the renderer, the ImGui layer) needs a current
OpenGL context for its whole lifetime: it creates GPU objects when constructed and deletes them when destroyed.
Such objects must therefore be created after the context and destroyed before it. Destroying the context first
leaves their destructors calling OpenGL without a context.

## Frame pacing

- VSync is off by default. Frame rate is capped by the limiter in @ref newton::graphics::Context::swapBuffers "Context::swapBuffers()", set by default
  to the primary monitor refresh rate (@ref newton::graphics::Context::monitorRefreshRate "Context::monitorRefreshRate()", fallback 60 FPS).
- The limiter schedules each frame one budget after the previous scheduled time, so oversleeping is compensated
  and the average matches the limit.
- ImGui is not allowed to change the OS cursor (`ImGuiConfigFlags_NoMouseCursorChange`).
