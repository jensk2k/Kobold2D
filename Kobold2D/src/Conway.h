#include "GameState.h"

#pragma once
class Conway : public GameState
{
public:
	Conway(Core& core);
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;
	void HandleMouseDown(MouseButtons button, Vec2i mousePosition) override;
	void HandleMouseUp(MouseButtons button, Vec2i mousePosition) override;

private:
	void Reset();
	BMap map1;
	BMap map2;
	BMap mapPrev;
	bool useMap1 = true;
	Texture texture;

	float updateTimer = 0.f;
	float simTimer = 0.f;
	bool isPaused = false;	

	bool leftMouseButtonDown = false;
	bool rightMouseButtonDown = false;
};

