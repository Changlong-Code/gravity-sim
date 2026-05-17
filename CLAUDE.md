# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Git Workflow

Commit and push to GitHub after every meaningful unit of work — a working feature, a bug fix, a stage completed. Never accumulate multiple unrelated changes in a single commit.

```bash
git add <specific files>
git commit -m "Short imperative summary of what changed and why"
git push
```

Keep commit messages specific (e.g. `Add drag force generator`, not `Update files`). Push immediately after each commit so no work exists only locally.

## Build and Run

```bat
build.bat          # compile all C++ sources → gravity_sim.exe
run.bat            # build + launch Python visualiser (opens browser)
python viz\visualizer.py   # launch visualiser only (exe must already be built)
```

Compiler: GCC 6.3.0 (MinGW). C++14 standard. No CMake — raw `g++` in `build.bat`.  
`std::thread` / `std::mutex` are unavailable in this MinGW build; use Windows `CreateThread` instead (already done in `main.cpp`).  
`std::atomic<double>` initialises incorrectly on this toolchain — use plain `double` with mutex or avoid cross-thread `double` access.

Before rebuilding, kill any running `gravity_sim.exe` process or the linker will fail with "Permission denied" on the output file.

## Architecture

The engine is split into a **C++ physics process** and a **Python visualiser** connected by a line-buffered stdout/stdin pipe.

### C++ physics (`src/`, `include/`)

| Class | Role |
|---|---|
| `Vector3` | Value-type 3D math. All SI units (metres, m/s, N). |
| `Body` | One simulated body: mass, radius, position, velocity, accumulated force, colour. |
| `ForceGenerator` / `GravitySystem` | Abstract base + N-body gravity. O(n²) pair loop with Plummer softening (`r_soft = √(r²+ε²)`). |
| `Integrator` / `SemiImplicitEuler` | Abstract base + symplectic Euler (velocity first, then position). Swap implementation to change integration method. |
| `PhysicsWorld` | Owns bodies, force generators, and the integrator. `step()` calls generators then integrator. Stores initial-state copies for `reset()`. |

`main.cpp` sets up the scenario (`buildWorld`), spawns a `CreateThread` stdin reader for live control commands, and runs the main loop emitting CSV to stdout.

### stdout/stdin protocol

**C++ → Python** (one frame):
```
FRAME,<frameCount>,<timestampSeconds>
BODY,<id>,<name>,<x>,<y>,<z>,<vx>,<vy>,<vz>,<radius>,<r>,<g>,<b>
```
`std::cout.flush()` after every frame is mandatory — without it stdout is fully buffered on Windows and Python starves.

**Python → C++** (stdin commands):
- `P` — toggle pause
- `R` — reset
- `S <dt>` — set timestep in seconds

### Python visualiser (`viz/visualizer.py`)

Launches `gravity_sim.exe` as a subprocess (`bufsize=1` for line-buffering), parses frames in a daemon thread, and updates VPython sphere positions in the main render loop. Body spheres are created on first appearance. Display radii are heavily exaggerated (`clamp(actual * SCALE * 1000, 0.25, 1.0)`) because real body sizes are invisible at solar-system scale.

### Key constants (`main.cpp`)

- `DT` — physics timestep in seconds (currently 3600 s = 1 hour)
- `STEPS_PER_FRAME` — physics steps per CSV frame (currently 24 → 1 sim-day per frame)
- `SCALE` in `visualizer.py` — metres per display unit (currently `1/1.5e10` so 1 AU ≈ 10 units)

### Adding a new body

Call `world.addBody(...)` in `buildWorld()` in `main.cpp`. The visualiser creates a new sphere automatically on first `BODY` line with that id.

### Adding a new force type

Subclass `ForceGenerator`, implement `applyForces()`, and call `world.addForceGenerator(...)`. No other files need changing.
