#pragma once
#include "GameState.h"
#include <vector>

struct Rectangle
{
	Rectangle(Vec2i position, int height, int width) :
		position(position),
		height(height),
		width(width) {}

	Vec2i position;
	int height;
	int width;
};

struct Circle
{
	Circle(Vec2f position, float radius) :
		position(position),
		radius(radius) {}

	Vec2f position;
	float radius;
};

class RayMarching2D : public GameState
{
private:
	std::vector<Rectangle> rectangles;
	std::vector<Circle> circles;
	Vec2i mousePositon;
	Vec2i rayOrigin;
	Vec2i rayDirection;

	float hitDistance = 0.f;

public:
	RayMarching2D(Core& core);
	void Update() override;
	void Render() override;
};

