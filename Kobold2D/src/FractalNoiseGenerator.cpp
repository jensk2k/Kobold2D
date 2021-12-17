#include "FractalNoiseGenerator.h"
#include "Map2D.h"

FractalNoiseGenerator::FractalNoiseGenerator() : perlin(rand())
{
}

void FractalNoiseGenerator::GenerateFractalNoiseMap(Map2D<float>& noiseMap, int n, float amplitudeMult, float frequency, float frequencyMult)
{
	//int n = 5;

	//float amplitudeMult = 0.5f;
	//float frequency = 0.032f;
	//float frequencyMult = 1.8f;


	for (int y = 0; y < noiseMap.m_height; ++y)
	{
		for (int x = 0; x < noiseMap.m_width; ++x)
		{
			float noise = 0.f;

			float sampleX = x * frequency;
			float sampleY = y * frequency;
			float amplitude = 1.f;

			for (int i = 0; i < n; ++i)
			{
				float noiseN = (float)perlin.noise(sampleX, sampleY, 0.f);
				noise += noiseN * amplitude;

				sampleX *= frequencyMult;
				sampleY *= frequencyMult;


				amplitude *= amplitudeMult;
			}
			noise /= n;

			noiseMap(x, y) = noise;
		}
	}
}

void FractalNoiseGenerator::NormalizePixelMap(Map2D<float>& map)
{
	float maxNoise = 0.0f;
	float minNoise = 1.0f;

	for (int y = 0; y < map.m_height; ++y)
	{
		for (int x = 0; x < map.m_width; ++x)
		{
			float noise = map(x, y);
			if (noise > maxNoise)
				maxNoise = noise;

			if (noise < minNoise)
				minNoise = noise;
		}
	}

	for (int y = 0; y < map.m_height; ++y)
	{
		for (int x = 0; x < map.m_width; ++x)
		{
			map(x, y) = (map(x, y) - minNoise) / (maxNoise - minNoise);
		}
	}
}
