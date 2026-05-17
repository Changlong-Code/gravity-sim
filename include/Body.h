#ifndef BODY_H
#define BODY_H

#include "Vector3.h"
#include <string>

struct Color {
    float r, g, b;
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

class Body {
public:
    Body(int id,
         const std::string& name,
         double mass,
         double radius,
         const Vector3& initialPosition,
         const Vector3& initialVelocity,
         const Color& color);

    int                getId()       const;
    const std::string& getName()     const;
    double             getMass()     const;
    double             getRadius()   const;
    const Vector3&     getPosition() const;
    const Vector3&     getVelocity() const;
    const Vector3&     getForce()    const;
    const Color&       getColor()    const;

    // Force accumulator pattern: each ForceGenerator calls accumulateForce();
    // the integrator reads the total once, then clears it before the next step.
    void accumulateForce(const Vector3& force);
    void clearForces();
    void setPosition(const Vector3& pos);
    void setVelocity(const Vector3& vel);

    // Serialises one BODY line for the stdout CSV protocol.
    std::string toCSV(int frameNumber, double timestamp) const;

private:
    int         id_;
    std::string name_;
    double      mass_;
    double      radius_;
    Vector3     position_;
    Vector3     velocity_;
    Vector3     accumulatedForce_;
    Color       color_;
};

#endif
