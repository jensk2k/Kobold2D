#pragma once
#include "GameState.h"

class Squares : public GameState
{
public:
	Squares(Core& core);
	void Update() override;
	void Render() override;
};

