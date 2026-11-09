# Controls {#controls}

[TOC]

## Camera

The camera moves only while the right mouse button is held. The cursor is hidden for that time and comes back
on release.

| Input | Action |
| --- | --- |
| Right mouse button (hold) + mouse | Look around |
| Right mouse button (hold) + `W` / `S` | Move forward / backward |
| Right mouse button (hold) + `A` / `D` | Move left / right |
| Right mouse button (hold) + `Space` / `Left Shift` | Move up / down |
| Right mouse button (hold) + `Ctrl` | Move faster |
| Scroll | Zoom (changes the field of view, 10° to 90°) |

Over a UI window the mouse controls the window instead: pressing the right button there does not start camera
movement, and scrolling does not zoom. Movement started over the scene keeps going even if the cursor passes
over a window.

## General

| Input | Action |
| --- | --- |
| `P` | Toggle wireframe |
| `F11` | Toggle fullscreen |
| `Esc` | Quit |

## UI

| Window | Contents |
| --- | --- |
| Stats (top left, no background) | Frames per second, average frame time and number of objects in the scene |
| Display (top right) | VSync, grid and fullscreen toggles, FPS limit (detected monitor refresh rate, presets or unlimited), window resolution (presets smaller than the monitor, disabled while fullscreen) |
| Camera (right, under Display) | Current position and field of view, sliders for speed, fast multiplier, sensitivity and zoom sensitivity, buttons resetting the settings to defaults and the camera to its starting position, direction and field of view |
| Object (right, under Camera) | Form creating a new object: shape, position, rotation in degrees, scale and color |

All windows keep their places, also when the main window is resized, and cannot be moved.

Ctrl+click a field to type its value. While typing, the keys from the General section do not work: `Esc` only
cancels the edit.
