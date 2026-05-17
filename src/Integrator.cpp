#include "Integrator.h"
#include "Body.h"

// Semi-implicit (symplectic) Euler integration.
// Key difference from explicit Euler: velocity is updated FIRST using the new
// force, then position is updated using the NEW velocity. This conserves
// energy far better for oscillatory/orbital systems, preventing orbit decay.
//
// Order per body:
//   1. a = F / m
//   2. v += a * dt        (velocity updated with current force)
//   3. pos += v * dt      (position updated with NEW velocity)
//   4. clearForces()      (reset accumulator for next step)
void SemiImplicitEuler::integrate(std::vector<std::shared_ptr<Body>>& bodies,
                                   double dt) {
    for (auto& body : bodies) {
        const Vector3 acceleration = body->getForce() * (1.0 / body->getMass());
        const Vector3 newVelocity  = body->getVelocity() + acceleration * dt;
        const Vector3 newPosition  = body->getPosition() + newVelocity * dt;
        body->setVelocity(newVelocity);
        body->setPosition(newPosition);
        body->clearForces();
    }
}
