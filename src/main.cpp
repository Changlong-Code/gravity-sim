#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <windows.h>
#include "Body.h"
#include "GravitySystem.h"
#include "Integrator.h"
#include "PhysicsWorld.h"

// Physics constants (SI units throughout)
constexpr double G       = 6.674e-11;
constexpr double M_SUN   = 1.989e30;
constexpr double M_EARTH = 5.972e24;
constexpr double M_MOON  = 7.342e22;
constexpr double R_SUN   = 6.957e8;
constexpr double AU      = 1.496e11;   // Earth-Sun distance (m)
constexpr double R_MOON  = 3.844e8;    // Earth-Moon distance (m)
constexpr double DT      = 3600.0;     // 1 hour per physics step

// 24 steps × 3600 s = 1 simulated day per visual frame (~30 days/second at 30 fps).
constexpr int STEPS_PER_FRAME = 24;

// Shared state for stdin thread
static PhysicsWorld* g_world      = nullptr;
static volatile bool g_shouldReset = false;

static void buildWorld(PhysicsWorld& world) {
    // Sun sits at the origin. Its recoil from Earth+Moon is ~0.09 m/s — negligible.
    world.addBody(std::make_shared<Body>(
        0, "Sun", M_SUN, R_SUN,
        Vector3(0, 0, 0), Vector3(0, 0, 0),
        Color(1.0f, 0.9f, 0.2f)));

    // Earth: circular orbit around Sun at 1 AU.
    // v_earth = sqrt(G * M_sun / AU)
    const double v_earth = std::sqrt(G * M_SUN / AU);

    // Moon: orbits Earth while Earth orbits Sun.
    // v_moon_total = v_earth + v_moon_around_earth
    // v_moon_around_earth = sqrt(G * M_earth / R_moon)
    const double v_moon_rel = std::sqrt(G * M_EARTH / R_MOON);

    world.addBody(std::make_shared<Body>(
        1, "Earth", M_EARTH, 6.371e6,
        Vector3(AU, 0, 0), Vector3(0, v_earth, 0),
        Color(0.2f, 0.5f, 1.0f)));

    // Moon starts slightly further from the Sun than Earth, moving faster.
    world.addBody(std::make_shared<Body>(
        2, "Moon", M_MOON, 1.737e6,
        Vector3(AU + R_MOON, 0, 0), Vector3(0, v_earth + v_moon_rel, 0),
        Color(0.8f, 0.8f, 0.8f)));
}

// Stdin reader thread: listens for control commands from the Python visualiser.
//   P       — toggle pause/resume
//   R       — reset simulation to initial conditions
//   S <dt>  — change physics timestep (seconds)
static DWORD WINAPI stdinReaderThread(LPVOID /*param*/) {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!g_world) continue;
        if (line == "P") {
            g_world->setPaused(!g_world->isPaused());
        } else if (line == "R") {
            g_shouldReset = true;
        } else if (line.size() >= 3 && line[0] == 'S' && line[1] == ' ') {
            try {
                double dt = std::stod(line.substr(2));
                if (dt > 0.0) g_world->setDt(dt);
            } catch (...) {}
        }
    }
    return 0;
}

int main() {
    auto integrator = std::make_shared<SemiImplicitEuler>();
    PhysicsWorld world(integrator, DT);
    world.addForceGenerator(std::make_shared<GravitySystem>());
    buildWorld(world);

    g_world = &world;

    // Spawn stdin reader on a Windows thread (std::thread unavailable in MinGW 6.3)
    HANDLE hThread = CreateThread(nullptr, 0, stdinReaderThread, nullptr, 0, nullptr);
    if (hThread) CloseHandle(hThread);

    // Main simulation loop — runs indefinitely until killed.
    while (true) {
        if (g_shouldReset) {
            world.reset();
            g_shouldReset = false;
        }

        for (int i = 0; i < STEPS_PER_FRAME; ++i) {
            world.step();
        }

        // std::cout.flush() is critical: without it stdout is fully buffered on
        // Windows when piped, and the Python subprocess starves waiting for data.
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "FRAME," << world.getFrameCount()
                  << "," << world.getTimestamp() << "\n";
        for (const auto& body : world.getBodies()) {
            std::cout << body->toCSV(world.getFrameCount(), world.getTimestamp()) << "\n";
        }
        std::cout.flush();
    }

    return 0;
}
