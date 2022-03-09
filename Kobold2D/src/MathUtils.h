#pragma once

#define PI 3.14159265358979323846f

namespace MathUtils
{
    float Clamp(float f, float min, float max);
    //int Clamp(int f, int min, int max);
    float Max(float a, float b);
    float Min(float a, float b);
    float Abs(float f);
    float Deg2Rad(float deg);
    float Rad2Deg(float deg);
    float Square(float f);
    //static float kPI = 3.14159265358979323846f;
};

