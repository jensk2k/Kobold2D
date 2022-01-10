#include "MathUtils.h"

float MathUtils::Clamp(float f, float min, float max)
{
    if (f < min)
        return min;
    if (f > max)
        return max;
    return f;
}

float MathUtils::Max(float a, float b)
{
    return a > b ? a : b;
}

float MathUtils::Min(float a, float b)
{
    return a < b ? a : b;
}

float MathUtils::Abs(float f)
{
    return f < 0.f ? -f : f;
}

float MathUtils::Deg2Rad(float deg)
{
    return deg * PI / 180.f;
}

float MathUtils::Rad2Deg(float rad)
{
    return rad * 180.f / PI;
}

float MathUtils::Square(float f)
{
    return f * f;
}
