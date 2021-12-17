#pragma once
#include "GameState.h"
#include "Vector2.h"
#include "JointAnimation.h"
#include <vector>
#include <math.h>

class ProcIK : public GameState
{
	JointAnimation::Limb ikLimb;
	JointAnimation::Limb twoBonedLimb;
	JointAnimation::Limb twoBonedLimb2;
	Vec2i pingPong;
	Vec2f animationCycle;


public:
	ProcIK(Core& core);
	void Update() override;
	void Render() override;

private:
	void DrawSegment(const JointAnimation::Bone& segment);
};

