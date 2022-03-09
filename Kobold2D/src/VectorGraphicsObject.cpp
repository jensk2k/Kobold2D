#include "VectorGraphicsObject.h"
#include "Color.h"
#include "GameState.h"
#include "Viewport.h"

VectorGraphicsObject::VectorGraphicsObject(Transform& transform, std::vector<Vec2f>& vertices)
	: transform(transform)
{
	this->vertices.reserve(vertices.size());

	for (Vec2f vertex : vertices)
	{
		this->vertices.push_back(vertex);
	}
}

void VectorGraphicsObject::Draw(GameState& gameState, const Viewport& viewport) const
{
	float cosR = transform.GetHeadingCosine();
	float sinR = transform.GetHeadingSine();

	if (vertices.size() > 1)
	{
		for (int j = 0; j < vertices.size() - 1; j++)
		{
			const Vec2f& v1 = vertices[j];
			const Vec2f& v2 = vertices[j + 1];

			// translate
			Vec2f v1Trans = v1 + transform.position;
			Vec2f v2Trans = v2 + transform.position;

			// rotate
			Vec2f v1Rot = Vec2f::RotateAroundPoint(v1Trans, transform.position, cosR, sinR);
			Vec2f v2Rot = Vec2f::RotateAroundPoint(v2Trans, transform.position, cosR, sinR);

			Vec2i v1ss = viewport.WorldToScreenSpace(v1Rot);
			Vec2i v2ss = viewport.WorldToScreenSpace(v2Rot);

			gameState.DrawLine(v1ss, v2ss, Colors::WHITE);
		}
	}
}
