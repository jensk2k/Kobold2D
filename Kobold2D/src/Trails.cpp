#include "Trails.h"
#include "MathUtils.h"

void Trails::Update()
{
}

void Trails::Render()
{
	trailMap.Update(10.f);
	trailMap.Trail(GetMousePosition(), 20, Colors::WHITE);
	trailMap.Render();
}
