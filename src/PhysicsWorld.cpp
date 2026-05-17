#include "PhysicsWorld.h"
#include "Body.h"
#include "Integrator.h"
#include "GravitySystem.h"

PhysicsWorld::PhysicsWorld(std::shared_ptr<Integrator> integrator, double dt)
    : integrator_(integrator), dt_(dt), timestamp_(0.0),
      frameCount_(0), paused_(false) {}

void PhysicsWorld::addBody(std::shared_ptr<Body> body) {
    bodies_.push_back(body);
    initialBodies_.push_back(std::make_shared<Body>(*body));
}

const std::vector<std::shared_ptr<Body>>& PhysicsWorld::getBodies() const {
    return bodies_;
}

void PhysicsWorld::reset() {
    bodies_.clear();
    for (auto& ib : initialBodies_) {
        bodies_.push_back(std::make_shared<Body>(*ib));
    }
    timestamp_  = 0.0;
    frameCount_ = 0;
}

void PhysicsWorld::step() {
    if (paused_.load()) return;
    const double dt = dt_.load();
    for (auto& fg : forceGenerators_) {
        fg->applyForces(bodies_);
    }
    integrator_->integrate(bodies_, dt);
    timestamp_  += dt;
    frameCount_ += 1;
}

void PhysicsWorld::stepN(int n) {
    for (int i = 0; i < n; ++i) step();
}

double PhysicsWorld::getTimestamp()  const { return timestamp_; }
int    PhysicsWorld::getFrameCount() const { return frameCount_; }

void   PhysicsWorld::setDt(double dt)  { dt_.store(dt); }
double PhysicsWorld::getDt()     const { return dt_.load(); }

void PhysicsWorld::addForceGenerator(std::shared_ptr<ForceGenerator> fg) {
    forceGenerators_.push_back(fg);
}

void PhysicsWorld::setPaused(bool paused) { paused_.store(paused); }
bool PhysicsWorld::isPaused()       const { return paused_.load(); }
