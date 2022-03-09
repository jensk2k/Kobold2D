#pragma once
#include "GameState.h"
#include <array>
#include "Viewport.h"
#include "VectorGraphicsObject.h"

struct Tank
{
	Transform transform;
	float turretRotation = 0;
	float width = 1;
	float length = 1;
	int vgoIndex = -1;
	float speed = 0.f;
};

class Tanks : public GameState
{
public:
	Tanks(Core& core);
	void Init() override;
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;
	void HandleKeyUp(Keys key) override;

private:
	ViewPortControl vpc;
	std::vector<VectorGraphicsObject> vgos;
	Viewport viewport;
	std::array<Tank, 1> tanks;

	Texture tankBodyTex;
	Texture tankTurretTex;

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
};

