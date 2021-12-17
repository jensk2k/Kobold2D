#pragma once
#include "GameState.h"
#include <array>
#include "Viewport.h"
#include "VectorGraphicsObject.h"



//struct VectorGraphicsObject
//{
//	Vector2 position;
//	float rotation = 0;
//	std::vector<Vector2> vertices;
//};

struct Tank
{
	Transform transform;
	float turretRotation = 0;
	float width = 1;
	float length = 1;
	int vgoIndex = -1;
	float speed = 0.f;
};

//struct Viewport
//{
//	Vector2 position;
//	float scale;
//	int width;
//	int height;
//
//	Vector2 WorldToScreenSpace(Vector2 worldPos) const;
//};
//
//class ViewPortControl
//{
//public:
//	void Update(Viewport vp, float deltaTime);
//	void HandleKeyDown(Keys key);
//	void HandleKeyUp(Keys key);
//private:
//	bool up = false;
//	bool down = false;
//	bool left = false;
//	bool right = false;
//};


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
	//Viewport viewport;
	std::array<Tank, 1> tanks;

	Texture tankBodyTex;
	Texture tankTurretTex;

	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
};

