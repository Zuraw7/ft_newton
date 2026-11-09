# ft_newton

Simple physics engine with an OpenGL renderer.

## Features

- Rigid bodies with gravity, simulated with a fixed time step (semi-implicit Euler)
- Objects built from primitives, created at runtime from the UI, optionally with a rigid body
- Free-flying camera, infinite world grid
- Debug UI (Dear ImGui): stats, display and camera settings

## Quick Start

```bash
make
./ft_newton
```

Run the executable from the repository root, assets are loaded with relative paths.

## Documentation

- @subpage build - requirements, build commands, tools, dependencies
- @subpage controls - camera, keyboard shortcuts, UI windows
- @subpage architecture - modules, rules between them, creation and destruction order
- @subpage conventions - coordinate system, units, winding, model matrix, shaders
- @subpage physics - fixed time step, integration, mass and gravity, sources
