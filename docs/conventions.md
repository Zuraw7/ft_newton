# Conventions {#conventions}

[TOC]

Agreements that hold across the whole project. Code that breaks them usually still compiles, but renders
or behaves wrong.

## Space and units

- Right-handed coordinate system, **Y up**. With no rotation, the camera looks along −Z.
- Positions and sizes are in world units. Physics will treat 1 unit as 1 meter.
- Time is in seconds, stored as `double` (delta time, frame budget, GLFW time).
- Angles exposed by the project are in **degrees** (camera FOV, rotation sensitivity per pixel).

## Geometry

- Front faces are **counter-clockwise** seen from outside. Back faces are culled (`GL_BACK`, `GL_CCW`),
  so a triangle with the wrong winding is invisible.
- Primitives from `geometry/Primitives.hpp` are unit-sized and centered at the origin:
  - @ref newton::geometry::makeCube "makeCube()": cube with side 1,
  - @ref newton::geometry::makeSphere "makeSphere()": sphere with radius 0.5, poles on the Y axis,
  - @ref newton::geometry::makePlane "makePlane()": plane 1 × 1 in the XZ plane, facing +Y, visible only from above.
- Final size, rotation and position come from the model matrix, never from the generator.

## Model matrix

Build it in the order translate → rotate → scale in code:

```cpp
glm::mat4 model(1.0f);
model = glm::translate(model, position);
model = glm::rotate(model, glm::radians(angle), axis);
model = glm::scale(model, size);
```

Applied to a vertex, the order is reversed: scale first, then rotation, then translation. Another order
moves objects around the origin instead of in place.

## Shaders and vertices

| Attribute location | Data | Shader input |
| --- | --- | --- |
| 0 | position (`vec3`) | `aPos` |
| 1 | normal (`vec3`) | `aNormal` |
| 2 | texture coordinates (`vec2`) | `aTexCoords` |

Uniforms used by `Renderer`: `uModel`, `uView`, `uProjection`, `uColor`. A shader passed to
`Renderer::create()` must provide all four.

Asset paths (`assets/shaders/...`) are relative to the working directory, run the program from the repository root.

## Colors

Colors are `glm::vec3` with components in [0, 1]. `graphics::rgb()` converts 0–255 components or a `0xRRGGBB`
value, e.g. `graphics::rgb(77, 189, 255)` or `graphics::rgb(0x4dbdff)`.

## Code

- Formatting is enforced by clang-format (`check-format` target), warnings are errors.
- Class documentation is in the header, function documentation is in the source file, private members are
  documented too.
- Logging goes through `common::Logger` with the class name prefix, e.g. `[Renderer] ...`.
