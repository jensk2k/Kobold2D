#pragma once
#include <vector>

#include "GameState.h"
#include "JointAnimation.h"
#include "VectorGraphicsObject.h"

struct Leg
{
	//const
	bool isRight;
	Vec2f rootOffset;
	float animationTargetOffset;
	float animationTargetRadius;
	//float animationTargetContactRange;
	float animationClockOffset;

	// runtime
	JointAnimation::Limb limb;
	bool contact = false;

	Vec2f GetFootPosition() const { return limb.bones[1].v2; }
};

struct Creature
{
	//const
	int vgoIndex;
	float maxSpeed;

	// runtime
	Transform transform;
	std::vector<Leg> legs;
	float animationClockScale;
	float animationClock;
	Vec2f steeringTarget;
	Vec2f velocity;
	Vec2f acceleration;
};

class CreatureGeneration : public GameState
{
public:
	CreatureGeneration(Core& core);
	void Update() override;
	void Render() override;
	void HandleKeyDown(Keys key) override;
	void HandleKeyUp(Keys key) override;
private:
	ViewPortControl vpc;

	std::vector<VectorGraphicsObject> vgos;
	std::vector<Creature> creatures;
};

