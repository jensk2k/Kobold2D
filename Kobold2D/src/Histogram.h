#pragma once
#include <string>
#include <vector>
#include "Vector2.h"

class GameState;

class Histogram
{
public:
	void RecordSample(float sample);
	void Draw(GameState& gameState, Vec2i position, int histogramSize, const std::string* title, bool writeHighestLatest = true, float highestOverride = -1.f) const;
	void Reset();
private:
	std::vector<float> histogram;
};