#pragma once
#include "Vector2.h"
#include <vector>

namespace JointAnimation
{
	struct Bone
	{
		Vec2f v1;
		Vec2f v2;
		float length = 1.f;
	};

	struct Limb
	{
		std::vector<Bone> bones;
	};

	void SolveIKJoint(Bone& bone, Vec2f target);
	void SolveIKLimb(Limb& limb, Vec2f root, Vec2f target);
	void Solve2BoneLimb(Limb& limb, Vec2f root, Vec2f target, bool flip);
};

