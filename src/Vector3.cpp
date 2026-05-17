#include "Vector3.h"
#include <cmath>
#include <sstream>
#include <iomanip>

Vector3::Vector3() : x(0.0), y(0.0), z(0.0) {}
Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3 Vector3::operator+(const Vector3& o) const { return {x+o.x, y+o.y, z+o.z}; }
Vector3 Vector3::operator-(const Vector3& o) const { return {x-o.x, y-o.y, z-o.z}; }
Vector3 Vector3::operator*(double s)         const { return {x*s, y*s, z*s}; }
Vector3 Vector3::operator/(double s)         const { return {x/s, y/s, z/s}; }
Vector3 Vector3::operator-()                 const { return {-x, -y, -z}; }

Vector3& Vector3::operator+=(const Vector3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
Vector3& Vector3::operator-=(const Vector3& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }
Vector3& Vector3::operator*=(double s)         { x*=s;   y*=s;   z*=s;   return *this; }

bool Vector3::operator==(const Vector3& o) const {
    return x == o.x && y == o.y && z == o.z;
}

double  Vector3::dot(const Vector3& o)   const { return x*o.x + y*o.y + z*o.z; }
Vector3 Vector3::cross(const Vector3& o) const {
    return { y*o.z - z*o.y,
             z*o.x - x*o.z,
             x*o.y - y*o.x };
}

double Vector3::magnitudeSquared() const { return x*x + y*y + z*z; }
double Vector3::magnitude()        const { return std::sqrt(magnitudeSquared()); }

Vector3 Vector3::normalized() const {
    double m = magnitude();
    if (m < 1e-15) return Vector3::zero();
    return *this / m;
}

double Vector3::distanceTo(const Vector3& o) const {
    return (*this - o).magnitude();
}

std::string Vector3::toString() const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6)
       << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}

Vector3 Vector3::zero() { return {0.0, 0.0, 0.0}; }

Vector3 operator*(double s, const Vector3& v) { return v * s; }
