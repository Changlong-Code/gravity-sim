#ifndef VECTOR3_H
#define VECTOR3_H

#include <string>

// Immutable-style 3D vector in SI units (metres, m/s, m/s², N …).
// All methods return new values; fields are public for direct access in hot paths.
struct Vector3 {
    double x, y, z;

    Vector3();
    Vector3(double x, double y, double z);

    Vector3  operator+(const Vector3& o) const;
    Vector3  operator-(const Vector3& o) const;
    Vector3  operator*(double s) const;
    Vector3  operator/(double s) const;
    Vector3& operator+=(const Vector3& o);
    Vector3& operator-=(const Vector3& o);
    Vector3& operator*=(double s);
    Vector3  operator-() const;
    bool     operator==(const Vector3& o) const;

    double   dot(const Vector3& o) const;
    Vector3  cross(const Vector3& o) const;
    double   magnitudeSquared() const;
    double   magnitude() const;
    Vector3  normalized() const;
    double   distanceTo(const Vector3& o) const;
    std::string toString() const;

    static Vector3 zero();
};

Vector3 operator*(double s, const Vector3& v);

#endif
