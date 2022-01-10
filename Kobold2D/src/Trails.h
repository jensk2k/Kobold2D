#pragma once
#include "GameState.h"
#include "TrailMap.h"

#define WIDTH 160
#define HEIGHT 90

class Trails : public GameState
{
public:
	Trails(Core& core) 
		: GameState(core)
		, trailMap(*this, WIDTH, HEIGHT)
	{}

	void Update() override;
	void Render() override;

private:
	TrailMap trailMap;
};

