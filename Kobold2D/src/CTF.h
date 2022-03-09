#pragma once
#include "GameState.h"
#include "Physics.h"
#include "Transform.h"

#include <vector>

enum class Team { Red, Blue };

struct AgentInputs
{
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool mouseButton = false;
	Vec2i mousePosition;
};

struct Agent
{
	Transform transform;
	Vec2f velocity;
	Team team;
	unsigned physicsObjectId;
	int health = 100;
	bool IsAlive() const { return health > 0; }
	AgentInputs inputs;
	bool isGunLoaded = true;
};

class CTF : public GameState
{
public:
	CTF(Core& core);
	void Update() override;
	void HandleKeyDown(Keys key);
	void HandleKeyUp(Keys key);
	void HandleMouseDown(MouseButtons button, Vec2i mousePosition) override;
	void HandleMouseUp(MouseButtons button, Vec2i mousePosition) override;
	void Render() override;
	void CalcSteering(Vec2f steeringDirection, float maxSpeed, float acceleration, float breakFriction, Vec2f& position, Vec2f& velocity);
	void HandleLocomotion(Agent& agent);
	void HandleShooting(Agent& agent);
	void HandleCollision(Physics::CollisionInfo colInfo);
	void Shoot(const Agent& shooter);
	Agent* GetAgentByObjectID(unsigned id);
private:
	Viewport viewport;
	Physics::World physicsWorld;
	std::vector<Agent> agents;
};

