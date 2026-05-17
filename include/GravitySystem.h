#ifndef GRAVITYSYSTEM_H
#define GRAVITYSYSTEM_H

#include <vector>
#include <memory>

class Body;

class ForceGenerator {
public:
    virtual ~ForceGenerator() = default;
    virtual void applyForces(std::vector<std::shared_ptr<Body>>& bodies) = 0;
};

class GravitySystem : public ForceGenerator {
public:
    // Universal gravitational constant (N·m²/kg²)
    static constexpr double G = 6.674e-11;

    // epsilon: Plummer softening length (m) — prevents r→0 singularity
    explicit GravitySystem(double epsilon = 1000.0);

    void applyForces(std::vector<std::shared_ptr<Body>>& bodies) override;

    double getEpsilon() const;
    void   setEpsilon(double epsilon);

private:
    double epsilon_;
};

#endif
