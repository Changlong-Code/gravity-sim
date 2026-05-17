"""
Gravity Simulation Visualiser
Launches gravity_sim.exe as a subprocess and renders its CSV output in 3D using VPython.
"""

import subprocess
import sys
import os
import threading
from vpython import (canvas, sphere, vector, color, label, rate,
                     button, slider, wtext, scene)

# ── Scale ──────────────────────────────────────────────────────────────────────
# Orbital radius ~384 Mm; scale so the canvas (~10 units wide) fits comfortably.
SCALE    = 1.0 / 4e7      # 1 m in sim  →  2.5e-8 display units
TRAIL_KEEP = 300           # number of trail points to retain

# ── Find the executable ────────────────────────────────────────────────────────
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
EXE_PATH   = os.path.join(SCRIPT_DIR, '..', 'gravity_sim.exe')

if not os.path.isfile(EXE_PATH):
    print(f"ERROR: gravity_sim.exe not found at {EXE_PATH}")
    print("Run build.bat first.")
    sys.exit(1)

# ── Launch C++ physics process ─────────────────────────────────────────────────
proc = subprocess.Popen(
    [EXE_PATH],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL,
    text=True,
    bufsize=1,          # line-buffered — must match std::cout.flush() in C++
)

def send_cmd(cmd: str):
    """Send a control command to the C++ process via stdin."""
    try:
        proc.stdin.write(cmd + '\n')
        proc.stdin.flush()
    except Exception:
        pass

# ── VPython scene setup ────────────────────────────────────────────────────────
scene.title    = '3D Gravity Simulation — Earth-Moon System'
scene.width    = 900
scene.height   = 600
scene.background = color.black
scene.autoscale  = False
scene.range      = 12           # display units — fits the Moon orbit at SCALE

# ── Shared body state (updated by reader thread, read by render loop) ──────────
bodies    = {}    # id (int) → {'sphere': sphere, 'name': str}
bodies_lock = threading.Lock()

# ── Frame reader thread ────────────────────────────────────────────────────────
def read_frames():
    for raw_line in proc.stdout:
        line = raw_line.strip()
        if not line:
            continue

        if line.startswith('BODY,'):
            parts = line.split(',')
            if len(parts) < 13:
                continue
            try:
                body_id = int(parts[1])
                name    = parts[2]
                px, py, pz = float(parts[3]), float(parts[4]), float(parts[5])
                radius  = float(parts[9])
                r, g, b = float(parts[10]), float(parts[11]), float(parts[12])
            except ValueError:
                continue

            pos = vector(px * SCALE, py * SCALE, pz * SCALE)
            rad = max(radius * SCALE * 6, 0.15)   # min visible size

            with bodies_lock:
                if body_id not in bodies:
                    sph = sphere(
                        pos=pos,
                        radius=rad,
                        color=vector(r, g, b),
                        make_trail=True,
                        trail_type='points',
                        interval=5,
                        retain=TRAIL_KEEP,
                    )
                    bodies[body_id] = {'sphere': sph, 'name': name}
                else:
                    bodies[body_id]['sphere'].pos = pos

reader_thread = threading.Thread(target=read_frames, daemon=True)
reader_thread.start()

# ── Controls ───────────────────────────────────────────────────────────────────
paused = [False]   # mutable container so closure can modify it

def toggle_pause(b):
    paused[0] = not paused[0]
    b.text = 'Resume' if paused[0] else 'Pause'
    send_cmd('P')

def do_reset(b):
    send_cmd('R')
    with bodies_lock:
        for bd in bodies.values():
            bd['sphere'].clear_trail()

def set_dt(s):
    dt_val = int(s.value)
    send_cmd(f'S {dt_val}')
    dt_display.text = f'  dt = {dt_val} s'

def set_trail(s):
    global TRAIL_KEEP
    TRAIL_KEEP = int(s.value)
    with bodies_lock:
        for bd in bodies.values():
            bd['sphere'].retain = TRAIL_KEEP

scene.append_to_caption('\n')
button(text='Pause',     bind=toggle_pause)
button(text='Reset',     bind=do_reset)

scene.append_to_caption('   Time step: ')
slider(min=30, max=3600, value=60, step=30, bind=set_dt, length=160)
dt_display = wtext(text='  dt = 60 s')

scene.append_to_caption('   Trail length: ')
slider(min=50, max=2000, value=300, step=50, bind=set_trail, length=160)

scene.append_to_caption('\n\nCamera: drag to rotate  •  scroll to zoom  •  right-drag to pan\n')

# ── Render loop ────────────────────────────────────────────────────────────────
while True:
    rate(60)    # VPython rate limiter — max 60 render updates per second
