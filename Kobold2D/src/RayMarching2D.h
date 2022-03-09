#pragma once
#include "GameState.h"
#include <vector>

struct Box
{
	Box() :
		height(0.f),
		width(0.f)
	{}
	Box(Vec2f position, float width, float height) :
		position(position),
		height(height),
		width(width)
	{}
	float CalculateSignedDistance(Vec2f target)
	{
		Vec2f size = Vec2f(width, height);
		Vec2f center = position + size / 2.f;

		Vec2f offset = target - center;
		offset = Vec2f(std::abs(offset.x), std::abs(offset.y));
		offset -= size / 2;

		Vec2f outside = Vec2f(std::max(offset.x, 0.f), std::max(offset.y, 0.f));
		float outsideDistance = outside.Length();
		Vec2f inside = Vec2f(std::min(offset.x, 0.f), std::min(offset.y, 0.f));
		float insideDistance = std::max(inside.x, inside.y);

		return insideDistance + outsideDistance;
	}

	Vec2f position;
	float height;
	float width;
};

struct Circle
{
	Circle() :
		radius(0.f)
	{}
	Circle(Vec2f position, float radius) :
		position(position),
		radius(radius)
	{}

	float CalculateSignedDistance(Vec2f target) 
	{
		return (target - position).Length() - radius;
	}

	Vec2f position;
	float radius;
};

class RayMarching2D : public GameState
{
public:
	RayMarching2D(Core& core);
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;

private:
	void RandomizeShapes();
	float CalculateSmallestDistanceToShape(Vec2f target);
	Viewport viewport;
	std::vector<Box> rectangles;
	std::vector<Circle> circles;
};

