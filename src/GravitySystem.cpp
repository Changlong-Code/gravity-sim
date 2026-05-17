#include "GravitySystem.h"
#include "Body.h"
#include <cmath>

GravitySystem::GravitySystem(double epsilon) : epsilon_(epsilon) {}

void GravitySystem::applyForces(std::vector<std::shared_ptr<Body>>& bodies) {
    // O(n²) pair loop — Newton's 3rd law applied per pair to avoid double-counting.
    // Plummer softening: r_soft = sqrt(r² + ε²) prevents singularity when r → 0.
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            const Vector3 delta = bodies[j]->getPosition() - bodies[i]->getPosition();
            const double distSq   = delta.magnitudeSquared() + epsilon_ * epsilon_;
            const double dist     = std::sqrt(distSq);
            // F = G * m1 * m2 / r_soft²
            const double forceMag = G * bodies[i]->getMass() * bodies[j]->getMass() / distSq;
            const Vector3 force   = delta * (forceMag / dist);

            bodies[i]->accumulateForce(force);
            bodies[j]->accumulateForce(-force);
        }
    }
}

double GravitySystem::getEpsilon() const         { return epsilon_; }
void   GravitySystem::setEpsilon(double epsilon) { epsilon_ = epsilon; }
