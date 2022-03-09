#include "Physics.h"

float Physics::Box::CalculateSignedDistance(Vec2f target)
{
	Vec2f size = Vec2f(width, height);
	Vec2f center = position + size / 2.f;
	
	Vec2f offset = target - center;
	offset = Vec2f(std::abs(offset.x), std::abs(offset.y));
	offset -= size/2;
	
	Vec2f outside = Vec2f(std::max(offset.x, 0.f), std::max(offset.y, 0.f));
	float outsideDistance = outside.Length();
	Vec2f inside = Vec2f(std::min(offset.x, 0.f), std::min(offset.y, 0.f));
	float insideDistance = std::max(inside.x, inside.y);
	
	return insideDistance + outsideDistance;
}

float Physics::Circle::CalculateSignedDistance(Vec2f target)
{
	return (target - position).Length() - radius;
}

bool Physics::World::Raycast(Vec2f origin, Vec2f direction, Vec2f& hit, unsigned& hitObjectID) const
{
	hit = origin;
	float smallestDistance = CalculateSignedDistanceToWorld(hit, hitObjectID);

	while (smallestDistance > 0.f && smallestDistance < raycastMax)
	{
		hit += direction * smallestDistance;

		smallestDistance = CalculateSignedDistanceToWorld(hit, hitObjectID);
		if (smallestDistance < raycastMin)
			return true;
	}
	return false;
}

float Physics::World::CalculateSignedDistanceToWorld(Vec2f target, unsigned& objectID) const
{
	float smallestDistance = FLT_MAX;

	for (Physics::Circle c : circles)
	{
		float distance = c.CalculateSignedDistance(target);
		if (distance < smallestDistance)
		{
			smallestDistance = distance;
			objectID = c.objectID;
		}
	}

	for (Physics::Box b : boxes)
	{
		float distance = b.CalculateSignedDistance(target);
		if (distance < smallestDistance)
		{
			smallestDistance = distance;
			objectID = b.objectID;
}
	}

	return smallestDistance;
}

Physics::Box* Physics::World::GetBoxByID(unsigned id)
{
	for (Box& box : boxes)
		if (box.objectID == id)
			return &box;
	return nullptr;
}

Physics::Circle* Physics::World::GetCircleByID(unsigned id)
{
	for (Circle& circle : circles)
		if (circle.objectID == id)
			return &circle;
	return nullptr;
}

unsigned Physics::World::AddBox(Layer layer, Vec2f position, float width, float height)
{
	boxes.push_back(Physics::Box(position, width, height));
	unsigned id = boxes[boxes.size() - 1].objectID;
	infoMap[id] = Info(layer, Shape::Box);
	return id;
}

unsigned Physics::World::AddCircle(Layer layer, Vec2f position, float radius)
{
	circles.push_back(Physics::Circle(position, radius));
	unsigned id = circles[circles.size() - 1].objectID;
	infoMap[id] = Info(layer, Shape::Circle);
	return id;
}

static bool CheckCollision(Physics::Box box1, Physics::Box box2, Physics::CollisionInfo& colInfo)
{
	if (box1.position.x < box2.position.x + box2.width &&
		box1.position.x + box1.width > box2.position.x &&
		box1.position.y < box2.position.y + box2.height &&
		box1.position.y + box1.height > box2.position.y)
	{
		Vec2f normal; // fix

		colInfo.contactNormal = normal;
		return true;
	}
	return false;
}

static bool CheckCollision(Physics::Circle circle1, Physics::Circle circle2, Physics::CollisionInfo& colInfo)
{
	if ((circle1.position - circle2.position).Length2() < std::pow(circle1.radius + circle2.radius, 2.f))
	{
		colInfo.contactNormal = (circle2.position - circle1.position).Normalized();
		colInfo.collisionsDepth = (circle1.radius + circle2.radius) - (circle1.position - circle2.position).Length();
		return true;
	}

	return false;
}

static bool CheckCollision(Physics::Box box, Physics::Circle circle, Physics::CollisionInfo& colInfo)
{
	if (circle.position.x > box.position.x &&
		circle.position.x < box.position.x + box.width &&
		circle.position.y + circle.radius > box.position.y&&
		circle.position.y - circle.radius < box.position.y + box.height)
	{
		if (circle.position.y > box.position.y + box.height / 2.f)
		{
			colInfo.contactNormal = Vec2f(0.f, -1.f); 
			colInfo.collisionsDepth = (box.position.y + box.height) - (circle.position.y - circle.radius);
		}
		else
		{
			colInfo.contactNormal = Vec2f(0.f, 1.f);
			colInfo.collisionsDepth = (circle.position.y + circle.radius) - box.position.y;
		}
		return true;
	}
	if (circle.position.y > box.position.y &&
		circle.position.y < box.position.y + box.height &&
		circle.position.x + circle.radius > box.position.x &&
		circle.position.x - circle.radius < box.position.x + box.width)
	{
		colInfo.contactNormal = circle.position.x < box.position.x ? Vec2f(-1.f, 0.f) : Vec2f(1.f, 0.f);

		if (circle.position.x < box.position.x + box.width / 2.f)
		{
			colInfo.contactNormal = Vec2f(-1.f, 0.f);
			colInfo.collisionsDepth = box.position.x - (circle.position.x + circle.radius);
		}
		else
		{
			colInfo.contactNormal = Vec2f(1.f, 0.f);
			colInfo.collisionsDepth = (circle.position.x -circle.radius) - (box.position.x + box.width);
		}
		return true;
	}
	if ((box.position - circle.position).Length2() < std::pow(circle.radius, 2.f))
	{
		colInfo.contactNormal = (box.position - circle.position).Normalized();
		colInfo.collisionsDepth = circle.radius - (box.position - circle.position).Length();
		return true;
	}
	if (((box.position + Vec2f(box.width, 0.f)) - circle.position).Length2() < std::pow(circle.radius, 2.f))
	{
		colInfo.contactNormal = ((box.position + Vec2f(box.width, 0.f)) - circle.position).Normalized();
		colInfo.collisionsDepth = circle.radius - (circle.position - box.position - Vec2f(box.width, 0.f)).Length();
		return true;
	}
	if (((box.position + Vec2f(0.f, box.height)) - circle.position).Length2() < std::pow(circle.radius, 2.f))
	{
		colInfo.contactNormal = ((box.position + Vec2f(0.f, box.height)) - circle.position).Normalized();
		colInfo.collisionsDepth = circle.radius - ((box.position + Vec2f(0.f, box.height)) - circle.position).Length();
		return true;
	}
	if (((box.position + Vec2f(box.width, box.height)) - circle.position).Length2() < std::pow(circle.radius, 2.f))
	{
		colInfo.contactNormal = ((box.position + Vec2f(box.width, box.height)) - circle.position).Normalized();
		colInfo.collisionsDepth = circle.radius - ((box.position + Vec2f(box.width, box.height)) - circle.position).Length();
		return true;
	}	

	return false;
}

void Physics::World::CheckCollisions()
{
	// Box x Box
	for (const Box& box1 : boxes)
	{
		for (const Box& box2 : boxes)
		{
			if (box1.objectID == box2.objectID)
				continue;

			Physics::CollisionInfo colInfo;
			if (CheckCollision(box1, box2, colInfo))
			{
				colInfo.ObjectID1 = box1.objectID;
				colInfo.ObjectID2 = box2.objectID;
				col(colInfo);
			}
		}
	}

	// Circle x Circle
	for (const Circle& circle1 : circles)
	{
		for (const Circle& circle2 : circles)
		{
			if (circle1.objectID == circle2.objectID)
				continue;

			Physics::CollisionInfo colInfo;
			if (CheckCollision(circle1, circle2, colInfo))
			{
				colInfo.ObjectID1 = circle1.objectID;
				colInfo.ObjectID2 = circle2.objectID;
				col(colInfo);
			}
		}
	}

	// Box x Circle
	for (const Circle& circle : circles)
	{
		for (const Box& box : boxes)
		{
			Physics::CollisionInfo colInfo1;
			Physics::CollisionInfo colInfo2;
			if (CheckCollision(box, circle, colInfo1))
			{
				colInfo1.ObjectID1 = circle.objectID;
				colInfo1.ObjectID2 = box.objectID;

				colInfo2.ObjectID1 = box.objectID;
				colInfo2.ObjectID2 = circle.objectID;
				colInfo2.contactNormal = colInfo1.contactNormal * -1.f;
				colInfo2.collisionsDepth = colInfo1.collisionsDepth;

				col(colInfo1);
				col(colInfo2);
			}
		}
	}
}
