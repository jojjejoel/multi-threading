#pragma once
class Vector3 {
public:
    float x, y, z;

    float squaredDistanceTo(const Vector3& other) const {
        return (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z);
    }
};