#include "RayMarching2D.h"
#include "Random.h"
#include <array>
#include <sstream>

RayMarching2D::RayMarching2D(Core& core)
	: GameState(core)
	, viewport(*this, GetWindowHeight() / 36.f)
{
	RandomizeShapes();
}

void RayMarching2D::Update()
{
}

void RayMarching2D::Render()
{
	Color grey(127, 127, 127, 255);

	for (Box r : rectangles)
	{
		viewport.DrawRectangleSolid(r.position, r.width, r.height, grey);
	}

	for (Circle c : circles)
	{
		viewport.DrawCircleSolid(c.position, c.radius, grey);
	}

	Vec2f rayOrigin = Vec2f(-30, 0);
	Vec2f rayTarget = viewport.ScreenToWorldPosition(GetMousePosition());
	Vec2f rayDirection = (rayTarget - rayOrigin).Normalized();

	Vec2f rayHit = rayOrigin; 
	float smallestDistance = CalculateSmallestDistanceToShape(rayHit);
	int count = 0;
	float minDistance = 1.f / viewport.scale;
	float maxDistance = GetWindowWidth() / viewport.scale;
	while (smallestDistance > minDistance && smallestDistance < maxDistance)
	{
		viewport.DrawCircle(rayHit, std::abs(smallestDistance), Colors::WHITE);

		rayHit += rayDirection * smallestDistance;
		viewport.DrawRectangleSolid(rayHit + Vec2f(-.25f, .25f), .5f, .5f, Colors::YELLOW);

		smallestDistance = CalculateSmallestDistanceToShape(rayHit);

		count++;
	}

	std::ostringstream os;
	os << "\nCount: " << count << std::endl;

	DrawText(os.str(), 0, 0);

	viewport.DrawRectangleSolid(rayOrigin + Vec2f(-.25f, .25f), .5f, .5f, Colors::WHITE);
	viewport.DrawRectangleSolid(rayTarget + Vec2f(-.25f, .25f), .5f, .5f, Colors::WHITE);
	viewport.DrawLine(rayOrigin, rayTarget, Colors::WHITE);
	viewport.DrawRectangleSolid(rayHit + Vec2f(-.25f, .25f), .5f, .5f, Colors::RED);

	//float smallestDistance = CalculateSmallestDistanceToShape(GetMousePosition());
	//DrawCircle(GetMousePosition(), std::abs(smallestDistance), Colors::WHITE);
}

void RayMarching2D::HandleKeyDown(Keys key)
{
	if (key == Keys::SPACE)
	{
		RandomizeShapes();
	}
}

void RayMarching2D::RandomizeShapes()
{
	int rectangleCount = 1;
	
	rectangles.resize(rectangleCount);
	for (int i = 0; i < rectangleCount; ++i)
	{
		float min = 100;
		float max = 300;

		float x = Random::Range(GetWindowWidth()/3, GetWindowWidth() - max);
		int y = Random::Range(0, GetWindowHeight() - max);
		float height = Random::Range(min, max);
		float width = Random::Range(min, max);

		rectangles[i] = Box(viewport.ScreenToWorldPosition(Vec2i(x, y)), height / viewport.scale, width / viewport.scale);
	}

	int circleCount = 0;
	circles.resize(circleCount);
	for (int i = 0; i < circleCount; ++i)
	{
		float min = 100;
		float max = 200;
		float x = Random::Range(GetWindowWidth() / 3, GetWindowWidth() - max);
		float y = Random::Range(GetWindowWidth() / 3, GetWindowHeight() - max);
		float radius = Random::Range(min, max);

		circles[i] = Circle(viewport.ScreenToWorldPosition(Vec2i(x, y)), radius / viewport.scale);
	}
}

float RayMarching2D::CalculateSmallestDistanceToShape(Vec2f target)
{
	float smallestDistance = FLT_MAX;

	for (Circle c : circles)
	{
		float distance = c.CalculateSignedDistance(target);
		if (distance < smallestDistance)
			smallestDistance = distance;
	}

	for (Box r : rectangles)
	{
		float distance = r.CalculateSignedDistance(target);
		if (distance < smallestDistance)
			smallestDistance = distance;
	}

	return smallestDistance;
}
