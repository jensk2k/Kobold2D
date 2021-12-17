#include "RayMarching2D.h"
#include <time.h>
#include <algorithm>

int RandomRange(int min, int max)
{
	return min + rand() % ((max + 1) - min);
}

RayMarching2D::RayMarching2D(Core& core)
	: GameState(core)
{
	int count = 4;

	srand(time(NULL));
	for (int i = 0; i < count; ++i)
	{
		int max = 200;

		int x = RandomRange(0, 1280 - max);
		int y = RandomRange(0, 720 - max);
		int height = RandomRange(10, max);
		int width = RandomRange(10, max);

		rectangles.push_back(Rectangle(Vec2i(x, y), height, width));
	}

	for (int i = 0; i < count; ++i)
	{
		int  max = 200;
		int x = RandomRange(max, 1280 - max);
		int y = RandomRange(max, 720 - max);
		int radius = RandomRange(10, max);

		circles.push_back(Circle(Vec2f(x, y), radius));
	}
}

void RayMarching2D::Update()
{
	mousePositon = GetMousePosition();

	rayOrigin = Vec2i(20, 20);
	rayDirection = (mousePositon - rayOrigin).Normalized();

	//bool hit = false;
	float smallestDistance = 2000.f;

	for (Rectangle r : rectangles)
	{
		//Vec2f p = mousePositon;

		float xMin = r.position.x;
		float xMax = r.position.x + r.width;
		float yMin = r.position.y;
		float yMax = r.position.y + r.height;

		float distance;
		float dx;
		float dy;

//		if (p.x < xMin)
		//{
		//	dx = xMin - p.x;
		//}
		//else if (p.x > xMax)
		//{
		//	dx = x
		//}

		/*float dx = std::max<float>(std::max<float>(xMin - p.x, p.x - yMax), 0.f);
		float dy = std::max<float>(std::max<float>(yMin - p.y, p.y - yMax), 0.f);
		float distance = std::sqrt(dx * dx + dy * dy);*/

		//if (distance < smallestDistance)
		//	smallestDistance = distance;
	}

	//for (Circle c : circles)
	//{
	//	float distance = (c.position - mousePositon).Length() - c.radius;
	//	if (distance < smallestDistance)
	//		smallestDistance = distance;
	//}

	hitDistance = smallestDistance;
}

void RayMarching2D::Render()
{
	Color grey(127, 127, 127, 255);

	for (Rectangle r : rectangles)
	{
		DrawRectangle(r.position, r.height, r.width, grey);
	}

	for (Circle c : circles)
	{
		//DrawCircle(c.position, c.radius, grey);
	}

	//DrawLine(rayOrigin, rayOrigin + rayDirection * hitDistance, Colors::WHITE);
	DrawRectangleSolid(mousePositon - Vec2i(10, 10), 20, 20, Colors::WHITE);
	DrawCircle(mousePositon, hitDistance, Colors::WHITE);

	//DrawCircle(Vector2(400.f, 300.f), 100.f, Colors::WHITE);
	//DrawRectangle(Vector2(700.f, 400.f), 150, 100, Colors::WHITE);
}
