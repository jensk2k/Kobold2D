#include "FractalNoiseViever.h"
#include <sstream>
#include <chrono>

static void NoiseToColorMap(const Map2D<float>& noiseMap, Map2D<Color>& colorMapOUT)
{
	int w = noiseMap.m_width;
	int h = noiseMap.m_height;

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			float noise = noiseMap(x, y);

			uint8_t noise8 = static_cast<uint8_t>(noise * 255);

			Color c(noise8, noise8, noise8, 255);

			colorMapOUT(x, y) = c;
		}
	}
}

static void NormalizePixelMap(Map2D<float>& map)
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

FractalNoiseViever::FractalNoiseViever(Core& core)
	: GameState(core)
	, noiseMap(1280, 720)
	, colorMap(1280, 720)
{
	
}

void FractalNoiseViever::Init()
{
	//generator.GenerateFractalNoiseMap(noiseMap, 0.f, 0.f);
	//NoiseToColorMap(noiseMap, colorMap);
	//RenderPixelMapToTexture(colorMap, texture);
	//LoadTexture("assets/moon-stars.jpg", texture, 626, 626);
}

void FractalNoiseViever::HandleKeyDown(Keys key)
{
	if (key == Keys::SPACE)
	{
		auto startTimepoint = std::chrono::high_resolution_clock::now();

		if (randomSeed)
			generator = FractalNoiseGenerator();
		generator.GenerateFractalNoiseMap(noiseMap, n, amplitudeMult, frequency, frequencyMult);
		NormalizePixelMap(noiseMap);
		NoiseToColorMap(noiseMap, colorMap);
		RenderPixelMapToTexture(colorMap, texture);

		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		compDuration = end - start;
	}

	if (key == Keys::UP)
		selector--;
	if (key == Keys::DOWN)
		selector++;
	if (selector < 0)
		selector = 0;
	if (selector > 4)
		selector = 4;

	if (selector == 0)
	{
		if (key == Keys::LEFT)
			n--;
		if (key == Keys::RIGHT)
			n++;
	}
	if (selector == 1)
	{
		if (key == Keys::LEFT)
			amplitudeMult -= 0.1f;
		if (key == Keys::RIGHT)
			amplitudeMult += 0.1f;
	}
	if (selector == 2)
	{
		if (key == Keys::LEFT)
			frequency -= 0.001f;
		if (key == Keys::RIGHT)
			frequency += 0.001f;
	}
	if (selector == 3)
	{
		if (key == Keys::LEFT)
			frequencyMult -= 0.1f;
		if (key == Keys::RIGHT)
			frequencyMult += 0.1f;
	}
	if (selector == 4)
	{
		if (key == Keys::LEFT || key == Keys::RIGHT)
			randomSeed = !randomSeed;
	}
}

void FractalNoiseViever::Update()
{

}

void FractalNoiseViever::Render()
{
	DrawTexture(texture, Vec2i(), texture.m_width, texture.m_height);

	{
		std::ostringstream ss;
		ss << "n: " << n;
		DrawText(ss.str(), 5, 40);
	}
	{
		std::ostringstream ss; 
		ss << "amplitudeMult: " << amplitudeMult;
		DrawText(ss.str(), 5, 70);
	}
	{
		std::ostringstream ss;
		ss << "frequency: " << frequency;
		DrawText(ss.str(), 5, 100);
	}
	{
		std::ostringstream ss;
		ss << "frequencyMult: " << frequencyMult;
		DrawText(ss.str(), 5, 130);
	}
	{
		std::ostringstream ss;
		ss << "randomSeed: " << randomSeed;
		DrawText(ss.str(), 5, 160);
	}

	{
		std::ostringstream ss;
		ss << "comp time: " << ((int)(compDuration/1000.f))/1000.f << " s";
		DrawText(ss.str(), 5, 720 - 30);
	}

	DrawRectangle(Vec2i(0, 40 + (30 * selector)), 250, 35, Colors::WHITE);
}
