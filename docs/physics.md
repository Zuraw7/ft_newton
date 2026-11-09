# Physics {#physics}

[fiedler-timestep]: https://gafferongames.com/post/fix_your_timestep/
[fiedler-integration]: https://gafferongames.com/post/integration_basics/
[wiki-semi-implicit-euler]: https://en.wikipedia.org/wiki/Semi-implicit_Euler_method
[wiki-force]: https://en.wikipedia.org/wiki/Force
[wiki-impulse]: https://en.wikipedia.org/wiki/Impulse_(physics)

[TOC]

How the simulation works and where the methods come from. The classes themselves are described in
@ref newton::physics::PhysicsWorld "PhysicsWorld" and @ref newton::physics::RigidBody "RigidBody".

## Fixed time step

The simulation always advances in steps of the same length (1/120 s), no matter how long a frame takes.
Each frame adds its duration to an accumulator, and as many steps run as whole steps fit in it. The rest
carries over to the next frame.

- The result does not depend on FPS: a body falls the same way at 30 and at 180 FPS.
- A frame longer than 0.25 s counts as 0.25 s, so a stall (the first frame, dragging the window) does not
  run hundreds of steps at once.
- The time scale multiplies the frame time before it reaches the accumulator, so slow motion uses the same
  step and stays just as stable.

Source: Glenn Fiedler, [Fix Your Timestep!][fiedler-timestep]

## Integration

Each step uses semi-implicit (symplectic) Euler: velocity is updated first, then the position is moved
with the new velocity.

```cpp
velocity += acceleration * dt;
position += velocity * dt;
```

Explicit Euler, which moves the position with the old velocity, slowly gains energy and becomes unstable.
Semi-implicit Euler costs the same and stays stable at a fixed step. Collisions will change the velocity
with impulses, and the position then follows the corrected velocity in the same step.

Sources:

- Glenn Fiedler, [Integration Basics][fiedler-integration]
- [Semi-implicit Euler method][wiki-semi-implicit-euler]

## Mass and gravity

- Bodies store the inverse mass (1 / mass). A static body, such as the ground, has inverse mass 0, which acts
  as infinite mass: impulses multiplied by 0 do not move it, and the step skips it.
- Gravity is an acceleration, not a force. The force of gravity is `mass * g`, so the acceleration is `g` for every
  body, and all bodies fall the same way regardless of their mass.
- Units: 1 world unit is 1 meter, mass is in kilograms, time in seconds. Default gravity is `9.81 m/s^2` along −Y.

## Forces and impulses

Both change the velocity of a body, they differ in how long they act.

| | Force | Impulse |
| --- | --- | --- |
| Unit | N (kg \* m/s^2) | N \* s (kg \* m/s) |
| Acts | During a step | Immediately |
| Velocity change | `force / mass * dt` per step | `impulse / mass` once |
| Use | Continuous effects: engine, wind, drag | Sudden changes: hit, launch, collision |

- Forces applied to a body are summed and turned into an acceleration in the next step, together with gravity:
  `acceleration = gravity + force * inverseMass`. The sum is cleared after the step, so a lasting force has to be
  applied again every step.
- An impulse is a force gathered over a short time (`impulse = force * dt`). It gives the same change of velocity
  regardless of the current velocity, but the resulting velocity depends on it: stopping a body needs an impulse
  equal to its momentum (`mass * velocity`), reversing it needs twice as much.
- With the same force or impulse, a heavier body accelerates less. Static bodies ignore both.

Sources:

- [Force][wiki-force]
- [Impulse][wiki-impulse]
