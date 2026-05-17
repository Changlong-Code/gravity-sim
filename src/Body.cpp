#include "Body.h"
#include <sstream>
#include <iomanip>

Body::Body(int id, const std::string& name, double mass, double radius,
           const Vector3& pos, const Vector3& vel, const Color& color)
    : id_(id), name_(name), mass_(mass), radius_(radius),
      position_(pos), velocity_(vel), accumulatedForce_(Vector3::zero()),
      color_(color) {}

int                Body::getId()       const { return id_; }
const std::string& Body::getName()     const { return name_; }
double             Body::getMass()     const { return mass_; }
double             Body::getRadius()   const { return radius_; }
const Vector3&     Body::getPosition() const { return position_; }
const Vector3&     Body::getVelocity() const { return velocity_; }
const Vector3&     Body::getForce()    const { return accumulatedForce_; }
const Color&       Body::getColor()    const { return color_; }

void Body::accumulateForce(const Vector3& force) { accumulatedForce_ += force; }
void Body::clearForces()                         { accumulatedForce_ = Vector3::zero(); }
void Body::setPosition(const Vector3& pos)       { position_ = pos; }
void Body::setVelocity(const Vector3& vel)       { velocity_ = vel; }

std::string Body::toCSV(int /*frameNumber*/, double /*timestamp*/) const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6);
    ss << "BODY," << id_ << "," << name_ << ","
       << position_.x << "," << position_.y << "," << position_.z << ","
       << velocity_.x << "," << velocity_.y << "," << velocity_.z << ","
       << radius_ << ","
       << color_.r << "," << color_.g << "," << color_.b;
    return ss.str();
}
