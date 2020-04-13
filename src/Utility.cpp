#include "Utility.h"

#include "Math.h"

#include <cstdlib>

float Random(float min, float max)
{
    return min + ((float)rand() / ((float)RAND_MAX / (float)(max - min)));
}

Vector RandomUnitSphere(void)
{
    float pitch = Random(-180.0f, 180.0f);
    float yaw = Random(0.0f, 360.0f);

    return Matrix4::RotateY(yaw) * Matrix4::RotateX(pitch) * Vector(0.0f, 0.0f, -1.0f);
}