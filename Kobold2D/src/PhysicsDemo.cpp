#include "PhysicsDemo.h"

PhysicsDemo::PhysicsDemo(Core& core) :
	GameState(core),
	world(0.1f, 100.f, [this](Physics::CollisionInfo info) { HandleCollision(info); }),
	viewport(*this, GetWindowHeight() / 36.f)
{
	unsigned id1 = world.AddCircle(Physics::Layer::Default, Vec2f(-10.f, 10.f), 4.f);
	colorMap[id1] = Colors::CYAN;
	velocityMap[id1] = Vec2f(1.f, 1.f);

	unsigned id2 = world.AddCircle(Physics::Layer::Default, Vec2f(10.f, 10.f), 6.f);
	colorMap[id2] = Colors::YELLOW;
	velocityMap[id2] = Vec2f(-1.f, 1.f);

	unsigned id3 = world.AddBox(Physics::Layer::Default, Vec2f(-14.f, -7.f), 8.f, 6.f);
	colorMap[id3] = Colors::RED;
	velocityMap[id3] = Vec2f(1.f, -1.f);

	unsigned id4 = world.AddBox(Physics::Layer::Default, Vec2f(10.f, -10.f), 6.f, 4.f);
	colorMap[id4] = Colors::GREEN;
	velocityMap[id4] = Vec2f(-1.f, -1.f);
}

void PhysicsDemo::Update()
{
	for (Physics::Box& box : world.GetBoxes())
	{
		if (box.position.x < 0.f)
			velocityMap[box.objectID].x = -velocityMap[box.objectID].x;
		if (box.position.x > GetWindowWidth() - box.width)
			velocityMap[box.objectID].x = -velocityMap[box.objectID].x;
		if (box.position.y < 0.f)
			velocityMap[box.objectID].y = -velocityMap[box.objectID].y;
		if (box.position.y > GetWindowHeight() - box.height)
			velocityMap[box.objectID].y = -velocityMap[box.objectID].y;

		colorMap[box.objectID] = Colors::GREEN;
		//box.position += velocityMap[box.info.id];
	}
	for (Physics::Circle& circle : world.GetCircles())
	{
		if (circle.position.x < circle.radius)
			velocityMap[circle.objectID].x = -velocityMap[circle.objectID].x;
		if (circle.position.x > GetWindowWidth() - circle.radius)
			velocityMap[circle.objectID].x = -velocityMap[circle.objectID].x;
		if (circle.position.y < circle.radius)
			velocityMap[circle.objectID].y = -velocityMap[circle.objectID].y;
		if (circle.position.y > GetWindowHeight() - circle.radius)
			velocityMap[circle.objectID].y = -velocityMap[circle.objectID].y;

		colorMap[circle.objectID] = Colors::GREEN;
		//circle.position += velocityMap[circle.info.id];
	}

	world.GetCircleByID(0)->position = viewport.ScreenToWorldPosition(GetMousePosition());
	//world.GetBoxByID(2)->position = viewport.ScreenToWorldPosition(GetMousePosition());
	
	collisions.clear();
	world.CheckCollisions();
}

void PhysicsDemo::Render()
{
	viewport.DrawGrid(*this, 1.f);
	for (const Physics::Box& box : world.GetBoxes())
	{
		Color color = colorMap[box.objectID];
		viewport.DrawRectangleSolid(box.position, box.width, box.height, color);
	}

	for (const Physics::Circle& circle : world.GetCircles())
	{
		Color color = colorMap[circle.objectID];
		viewport.DrawCircleSolid(circle.position, circle.radius, color);
	}

	for (const Physics::CollisionInfo& col : collisions)
	{
		Vec2f normal = col.contactNormal * col.collisionsDepth;
		Vec2f pos;
		const Physics::Info& info1 = world.GetInfoByID(col.ObjectID1);
		const Physics::Info& info2 = world.GetInfoByID(col.ObjectID2);

		if (info1.shape == Physics::Shape::Box)
		{
			Physics::Box* box = world.GetBoxByID(col.ObjectID1);
			pos = box->position + Vec2f(box->width/2.f, box->height/2.f);
			//viewport.DrawCircleSolid(box->position, 1.f, Colors::RED);
		}
		else if (info1.shape == Physics::Shape::Circle)
		{
			pos = world.GetCircleByID(col.ObjectID1)->position;
		}

		viewport.DrawLine(pos, pos + normal, Colors::YELLOW);
	}
}

void PhysicsDemo::HandleCollision(Physics::CollisionInfo info)
{
	collisions.push_back(info);
	colorMap[info.ObjectID1] = Colors::RED;
	colorMap[info.ObjectID2] = Colors::RED;
}
