#pragma once
#include "Vector3.h"
class Sphere {
public:
    Vector3 pos;
    float radius;
    Sphere(float x, float y, float z, float radius) : pos{ x, y, z }, radius(radius)  {}
};