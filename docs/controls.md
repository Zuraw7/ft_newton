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
| Scroll | Zoom (changes the field of view, 20° to 90°) |

Over a UI window the mouse controls the window instead: pressing the right button there does not start camera
movement, and scrolling does not zoom. Movement started over the scene keeps going even if the cursor passes
over a window.

## General

| Input | Action |
| --- | --- |
| `P` | Toggle wireframe |
| `Esc` | Quit |

## UI

| Window | Contents |
| --- | --- |
| Stats (top left, no background) | Frames per second and average frame time |
| Display (top right) | VSync toggle, FPS limit (detected monitor refresh rate, presets or unlimited) |

Windows can be moved by dragging their title bar. Their layout is not saved between runs.
