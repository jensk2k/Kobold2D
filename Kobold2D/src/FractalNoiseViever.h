#pragma once
#include "GameState.h"
#include "FractalNoiseGenerator.h"

template<typename T>
class Map2D;

class FractalNoiseViever : public GameState
{
public:
	FractalNoiseViever(Core& core);
	void Init() override;
	void HandleKeyDown(Keys key) override;
	void Update() override;
	void Render() override;
	
private:
	FractalNoiseGenerator generator;
	Map2D<float> noiseMap;
	Map2D<Color> colorMap;
	Texture texture;

	int n = 5;
	float amplitudeMult = 0.5f;
	float frequency = 0.008f;
	float frequencyMult = 2.f;
	bool randomSeed = false;

	int selector = 0;

	long long compDuration = 0; // microseconds
};

