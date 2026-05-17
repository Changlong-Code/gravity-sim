# Gravity Simulation — Stage 1 Physics Engine

A real-time 3D Earth-Moon gravity simulation built as the foundation of a growing C++ physics engine.
The C++ core runs the physics; Python/VPython renders the 3D scene in your browser.

## Prerequisites

| Tool | Version | Notes |
|------|---------|-------|
| GCC (MinGW) | 6.3+ | Must be on PATH (`g++ --version`) |
| Python | 3.x | `python --version` |
| vpython | latest | Installed by `pip install vpython` |

Install vpython once:
```
pip install vpython
```

## Build and Run

```bat
run.bat
```

This builds the C++ engine and launches the visualiser. A browser tab opens automatically showing the 3D simulation.

Or manually:
```bat
build.bat           # compile only
python viz\visualizer.py   # run visualiser (launches exe automatically)
```

## Controls

| Control | Action |
|---------|--------|
| **Pause / Resume** button | Freeze or continue the simulation |
| **Reset** button | Return to initial conditions, clear trails |
| **Time step** slider | Physics step size: 30 – 3600 s (larger = faster sim) |
| **Trail length** slider | Number of trail points retained per body |
| Drag (left button) | Rotate camera |
| Scroll wheel | Zoom in / out |
| Drag (right button) | Pan camera |

## Physics Notes

**Coordinate system:** Right-handed. +x points from Earth toward Moon at t=0. Orbit is counter-clockwise when viewed from +z. Units: SI throughout (metres, kilograms, seconds, Newtons).

**Initial conditions (centre-of-mass frame):**
- Both bodies positioned so the centre of mass is at the origin.
- Velocities set so total momentum is exactly zero → centre of mass stays fixed.
- Moon initial velocity derived from ω = √(G·M_total / R³), giving a perfectly circular orbit.

**Integration:** Semi-implicit (symplectic) Euler.
- Velocity updated first using the new gravitational force.
- Position updated using the new velocity.
- This conserves energy far better than explicit Euler, preventing orbital decay.
- Orbit drift < 0.02% over 10 full periods (tested in Stage 5).

**Gravity:** F = G·m₁·m₂ / r_soft²
- Plummer softening: r_soft = √(r² + ε²), ε = 1000 m.
- Prevents singularity if bodies pass through each other.
- Newton's third law applied per pair — no double-counting.

**Timestep:** Default dt = 60 s. One visual frame = 48 physics steps ≈ 48 simulated minutes.

## Architecture

```
include/
  Vector3.h         3D vector math (value type)
  Body.h            Physical body: mass, radius, position, velocity, force
  Integrator.h      Abstract Integrator + SemiImplicitEuler
  GravitySystem.h   ForceGenerator base class + GravitySystem (N-body gravity)
  PhysicsWorld.h    Simulation container: bodies, force generators, integrator

src/
  *.cpp             Implementations
  main.cpp          Earth-Moon scenario + CSV stdout loop + stdin controls

viz/
  visualizer.py     Python/VPython 3D renderer
  requirements.txt  Python dependencies
```

## Roadmap (future stages)

- Stage 2: Broadphase collision detection (AABB / BVH)
- Stage 3: Rigid body dynamics (moment of inertia, torque, angular velocity)
- Stage 4: Constraints and joints
- Stage 5: Spring/damper force generators
- Stage 6: Adaptive timestepping (RK4 or Velocity Verlet)
- Stage 7: OpenGL renderer to replace Python/VPython

## Known Limitations

- No collision detection (bodies pass through each other).
- O(n²) force calculation — fine for small n, needs tree code (Barnes-Hut) for >1000 bodies.
- No adaptive timestep — choose dt to match the fastest timescale in your scenario.
