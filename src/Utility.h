#pragma once

#include <cmath>

#define EPSILON 1e-5
#define PI 3.14159f
#define DEG_TO_RAD(x) ((x) * PI / 180.0f)
#define RAD_TO_DEG(x) ((x) * 180.0f / PI)
#define BIT(x) (1 << x)

template<typename T>
bool Equal(T lhs, T rhs)
{
    return fabs(rhs - lhs) < EPSILON;
}

template<typename T>
T Clamp(T val, T min, T max)
{
    if(val < min)
        val = min;
    else if(val > max)
        val = max;
    return val;
}

struct Vector;

float Random(float min, float max);
Vector RandomUnitSphere(void);