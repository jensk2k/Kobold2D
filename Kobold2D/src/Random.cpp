#include "Random.h"
#include <random>

int Random::Range(int a, float b)
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(a, b);

    return dist(e2);
}

float Random::Range(float a, float b)
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(a, b);

    return dist(e2);
}
