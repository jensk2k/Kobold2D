#pragma once
#include "Vector2.h"
#include "Transform.h"
#include "Viewport.h"
#include <vector>

class GameState;

struct VectorGraphicsObject
{
	VectorGraphicsObject(Transform& transform, std::vector<Vec2f>& vertices);
	void Draw(GameState& gameState, const Viewport& viewport) const;

	Transform& transform;
private:
	std::vector<Vec2f> vertices;
};

