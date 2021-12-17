#pragma once
#include "GameState.h"
#include "FractalNoiseGenerator.h"

struct GridCell
{

};

class LifeSim : public GameState
{
public:
	LifeSim(Core& core);
	void Update() override;
	void Render() override;

private:
	FractalNoiseGenerator generator;
	Map2D<GridCell> grid;
	Map2D<float> noiseMap;
};

