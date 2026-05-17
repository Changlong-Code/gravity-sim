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
constexpr double M_EARTH = 5.972e24;
constexpr double M_MOON  = 7.342e22;
constexpr double R_MOON  = 3.844e8;   // Earth-Moon separation (m)
constexpr double DT      = 60.0;      // 1 minute per physics step

// Emit one CSV frame every N physics steps.
// 48 steps x 60 s = 2,880 s ≈ 48 simulated minutes per visual frame.
constexpr int STEPS_PER_FRAME = 48;

// Shared state for stdin thread
static PhysicsWorld* g_world      = nullptr;
static volatile bool g_shouldReset = false;

static void buildWorld(PhysicsWorld& world) {
    const double M_TOTAL = M_EARTH + M_MOON;
    const double r_earth = (M_MOON  / M_TOTAL) * R_MOON;
    const double r_moon  = (M_EARTH / M_TOTAL) * R_MOON;
    // ω = sqrt(G * M_total / R³) — circular orbit angular velocity
    const double omega   = std::sqrt(G * M_TOTAL / (R_MOON * R_MOON * R_MOON));

    world.addBody(std::make_shared<Body>(
        0, "Earth", M_EARTH, 6.371e6,
        Vector3(-r_earth, 0, 0), Vector3(0, -omega * r_earth, 0),
        Color(0.2f, 0.5f, 1.0f)));

    world.addBody(std::make_shared<Body>(
        1, "Moon", M_MOON, 1.737e6,
        Vector3(r_moon, 0, 0), Vector3(0, omega * r_moon, 0),
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
