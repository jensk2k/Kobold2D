#pragma once
#include "Vector2.h"
#include <vector>
#include <optional>
#include <functional>
#include <map>

namespace Physics
{
	static unsigned idCounter = 0;
	enum class Layer
	{
		Default,
		Agent,
		Environment
	};
	enum class Shape
	{
		None,
		Box,
		Circle
	};

	struct Info
	{
		Info() :
			layer(Layer::Default),
			shape(Shape::None)
		{}
		Info(Layer layer, Shape shape) :
			layer(layer),
			shape(shape)
		{}
		Layer layer;
		Shape shape;
	};

	struct CollisionInfo
	{
		unsigned ObjectID1;
		unsigned ObjectID2;
		Vec2f contactNormal;
		float collisionsDepth = 0.f;
	};

	struct Box
	{
		Box(Vec2f position, float width, float height) :
			objectID(idCounter++),
			position(position),
			height(height),
			width(width)
		{
			std::cout << "new box with id " << objectID << std::endl;
		}
		float CalculateSignedDistance(Vec2f target);
		unsigned objectID;
		Vec2f position;
		float height;
		float width;
	};

	struct Circle
	{
		Circle(Vec2f position, float radius) :
			objectID(idCounter++),
			position(position),
			radius(radius)
		{
			std::cout << "new circle with id " << objectID << std::endl;
		}

		float CalculateSignedDistance(Vec2f target);
		unsigned objectID;
		Vec2f position;
		float radius;
	};

	struct World
	{
		World(float raycastMin, float raycastMax, std::function<void(CollisionInfo colInfo)> collisionHandler)
			: raycastMin(raycastMin)
			, raycastMax(raycastMax)
			, col(collisionHandler)
		{}

		bool Raycast(Vec2f origin, Vec2f direction, Vec2f& hit, unsigned& hitObjectID) const;
		float CalculateSignedDistanceToWorld(Vec2f target, unsigned& objectID) const;
		std::vector<Box>& GetBoxes() { return boxes; }
		std::vector<Circle>& GetCircles() { return circles; }
		Box* GetBoxByID(unsigned id);
		Circle* GetCircleByID(unsigned id);
		unsigned AddBox(Layer layer, Vec2f position, float width, float height);
		unsigned AddCircle(Layer layer, Vec2f position, float radius);
		const Info& GetInfoByID(unsigned id) { return infoMap[id]; }
		void CheckCollisions();

	private:
		std::vector<Box> boxes;
		std::vector<Circle> circles;
		float raycastMin;
		float raycastMax;
		std::function<void(CollisionInfo colInfo)> col;
		std::map<unsigned, Info> infoMap;
	};
}

