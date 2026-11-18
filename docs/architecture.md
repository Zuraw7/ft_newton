# Architecture {#architecture}

[TOC]

How the modules fit together and in what order things are created and destroyed.
Details of each class are in its own documentation, this page covers what is not
visible from a single class.

## Modules

The code is split into the engine and the game built on it. Every module has its own namespace
`newton::<module>` and its headers are included as `"newton/<module>/File.hpp"`.

```bash
engine/<module>/include/newton/<module>/   public headers of an engine module
engine/<module>/src/                       its sources
game/include/newton/<module>/              headers of the game modules
game/src/<module>/, game/src/main.cpp      their sources and the entry point
external/                                  third-party code kept in the repository
```

Each engine module is a static library `newton_<module>` with its own include
directory. A module sees only the headers of the libraries it links to, so a
forbidden dependency is a compile error. The game is the `ft_newton` executable
linking all engine libraries and Dear ImGui.

| Module | Library | Contents |
| --- | --- | --- |
| `common` | `newton_common` | @ref newton::common::Logger "Logger" shared by all modules |
| `geometry` | `newton_geometry` | CPU-side mesh data (@ref newton::geometry::Vertex "Vertex", @ref newton::geometry::MeshData "MeshData") and primitive generators (@ref newton::geometry::makeCube "cube", @ref newton::geometry::makePlane "plane", @ref newton::geometry::makeSphere "sphere") |
| `graphics` | `newton_graphics` | Window and OpenGL context (@ref newton::graphics::Context "Context"), frame timing, shaders (@ref newton::graphics::Shader "Shader"), GPU buffers, meshes (@ref newton::graphics::Mesh "Mesh"), camera (@ref newton::graphics::Camera "Camera"), renderers (@ref newton::graphics::MeshRenderer "MeshRenderer", @ref newton::graphics::GridRenderer "GridRenderer", @ref newton::graphics::ColliderRenderer "ColliderRenderer") |
| `input` | `newton_input` | Keyboard and mouse state collected from GLFW callbacks (@ref newton::input::InputManager "InputManager") |
| `math` | `newton_math` | Own math for physics, no dependencies: @ref newton::math::Vec3 "Vec3" |
| `physics` | `newton_physics` | Simulation built on `math`: @ref newton::physics::Particle "Particle" |
| `scene` | `newton_scene` | Objects in the world: @ref newton::scene::Scene "Scene", @ref newton::scene::SceneObject "SceneObject", @ref newton::scene::Transform "Transform" |
| `controllers` | game | Turn input into actions, e.g. @ref newton::controllers::CameraController "CameraController" moves the camera |
| `ui` | game | Dear ImGui lifetime (@ref newton::ui::ImGuiLayer "ImGuiLayer") and the windows drawn with it |

`main.cpp` creates the objects and connects them, the modules do not create each
other.

## Dependencies

Modules are kept as independent of each other as possible: a module knows only
the modules it really needs, and data crosses module boundaries as plain values
or references passed in from outside rather than through modules reaching into
each other. This keeps changes local and lets modules be reused or replaced
without touching the rest.

- Engine libraries never use the game: nothing in `engine/` includes `controllers` or `ui`.
- `geometry` has no OpenGL dependency. It produces plain data, uploading it to
  the GPU is the job of @ref newton::graphics::Mesh "graphics::Mesh".
- `ui` panels receive the object whose settings they show or plain values. One
  panel per module. A panel is a free function, unless it keeps state between
  frames.
- `controllers` know nothing about the UI. Whether the mouse belongs to an ImGui
  window is passed in as a plain `bool`.
- `scene` only stores objects, they must be passed to the renderer from outside.
  Objects point to meshes they do not own.

## Startup and shutdown order

| Step | Why |
| --- | --- |
| @ref newton::graphics::Context::create "graphics::Context::create()" | Creates the window and the OpenGL context, every OpenGL call needs it |
| @ref newton::input::InputManager::init "input::InputManager::init()" | Installs GLFW input callbacks |
| @ref newton::ui::ImGuiLayer "ui::ImGuiLayer" | Must come after `InputManager::init()`: the ImGui GLFW backend chains the callbacks already set, callbacks set later would replace its own |
| @ref newton::graphics::MeshRenderer::create "graphics::MeshRenderer::create()" | Compiles the mesh shader, needs the context |
| @ref newton::graphics::GridRenderer::create "graphics::GridRenderer::create()" | Compiles the grid shader, needs the context |
| Camera, controller, meshes | Meshes upload data to the GPU, need the context |
| @ref newton::scene::Scene "scene::Scene" | Its objects point to meshes, which must outlive it |

Everything that owns OpenGL objects (meshes, buffers, shaders, the renderer, the
ImGui layer) needs a current OpenGL context for its whole lifetime: it creates
GPU objects when constructed and deletes them when destroyed. Such objects must
therefore be created after the context and destroyed before it. Destroying the
context first leaves their destructors calling OpenGL without a context.

## Frame pacing

- VSync is off by default. Frame rate is capped by the limiter in @ref
  newton::graphics::Context::swapBuffers "Context::swapBuffers()", set by
  default to the primary monitor refresh rate (@ref
  newton::graphics::Context::monitorRefreshRate "Context::monitorRefreshRate()",
  fallback 60 FPS).
- The limiter schedules each frame one budget after the previous scheduled time,
  so oversleeping is compensated and the average matches the limit.
- ImGui is not allowed to change the OS cursor
  (`ImGuiConfigFlags_NoMouseCursorChange`).
