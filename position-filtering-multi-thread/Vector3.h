#pragma once
struct Vector3
{
	float x, y, z;

    float squaredDistanceTo(const Vector3& other) const {
        return (x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z);
    }
};