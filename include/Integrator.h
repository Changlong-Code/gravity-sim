#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <vector>
#include <memory>

class Body;

class Integrator {
public:
    virtual ~Integrator() = default;
    // Advances positions and velocities of all bodies by dt seconds.
    // Forces must already be accumulated on each body before this call.
    virtual void integrate(std::vector<std::shared_ptr<Body>>& bodies, double dt) = 0;
};

class SemiImplicitEuler : public Integrator {
public:
    void integrate(std::vector<std::shared_ptr<Body>>& bodies, double dt) override;
};

#endif
