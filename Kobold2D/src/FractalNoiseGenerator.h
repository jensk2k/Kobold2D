#pragma once
#include "PerlinNoise.h"

template<typename T>
class Map2D;

class FractalNoiseGenerator
{
public:
	FractalNoiseGenerator();

	void GenerateFractalNoiseMap(Map2D<float>& noiseMap, int n, float amplitudeMult, float frequency, float frequencyMult);
	void NormalizePixelMap(Map2D<float>& map);
private:
	PerlinNoise perlin;
};

