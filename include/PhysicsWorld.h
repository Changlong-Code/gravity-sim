#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include <vector>
#include <memory>
#include <atomic>

class Body;
class Integrator;
class ForceGenerator;

class PhysicsWorld {
public:
    PhysicsWorld(std::shared_ptr<Integrator> integrator, double dt);

    void addBody(std::shared_ptr<Body> body);
    const std::vector<std::shared_ptr<Body>>& getBodies() const;
    void reset();

    void   step();
    void   stepN(int n);
    double getTimestamp()  const;
    int    getFrameCount() const;

    void   setDt(double dt);
    double getDt() const;

    void addForceGenerator(std::shared_ptr<ForceGenerator> fg);

    void setPaused(bool paused);
    bool isPaused() const;

private:
    std::vector<std::shared_ptr<Body>>           bodies_;
    std::vector<std::shared_ptr<Body>>           initialBodies_;
    std::vector<std::shared_ptr<ForceGenerator>> forceGenerators_;
    std::shared_ptr<Integrator>                  integrator_;

    std::atomic<double> dt_;
    double              timestamp_;
    int                 frameCount_;
    std::atomic<bool>   paused_;
};

#endif
