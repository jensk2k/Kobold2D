#pragma once
#include "GameState.h"
#include "Physics.h"
#include <map>

class PhysicsDemo : public GameState
{
public:
	PhysicsDemo(Core& core);
	void Update() override;
	void Render() override;
	void HandleCollision(Physics::CollisionInfo info);

private:
	Physics::World world;
	Viewport viewport;
	std::map<unsigned, Color> colorMap;
	std::map<unsigned, Vec2f> velocityMap;
	std::vector<Physics::CollisionInfo> collisions;
};

