#include "JointAnimation.h"
#include "MathUtils.h"

void JointAnimation::SolveIKJoint(Bone& bone, Vec2f target)
{
	bone.v2 = target;

	Vec2f direction = bone.v2 - bone.v1;

	direction.Normalize();

	direction *= bone.length;

	bone.v1.x = bone.v2.x - direction.x;
	bone.v1.y = bone.v2.y - direction.y;
}

void JointAnimation::SolveIKLimb(Limb& limb, Vec2f root, Vec2f target)
{
	SolveIKJoint(limb.bones.back(), target);

	for (size_t i = limb.bones.size() - 1; i > 0; --i)
	{
		Bone& prev = limb.bones[i];
		Bone& cur = limb.bones[i - 1];

		SolveIKJoint(cur, prev.v1);
	}

	// Translate back
	Vec2f delta = limb.bones.front().v1 - root;

	for (int i = 0; i < limb.bones.size(); ++i)
	{
		limb.bones[i].v1 -= delta;
		limb.bones[i].v2 -= delta;
	}
}

void JointAnimation::Solve2BoneLimb(Limb& limb, Vec2f root, Vec2f target, bool flip)
{
	Bone& firstBone = limb.bones[0];
	Bone& secondBone = limb.bones[1];
	firstBone.v1 = root;

	Vec2f direction = target - root;
	float distBetween = (direction).Length();
	float opposite = distBetween / 2.f;
	float hypotenuse = firstBone.length;

	float angle;
	if (opposite < hypotenuse)
		angle = asin(opposite / hypotenuse);
	else
		angle = PI / 2.f;

	if (flip)
		angle = PI - angle;

	float a1 = PI / 2.f - angle;
	Vec2f firstDir = Vec2f::Rotate(direction, a1).Normalized();
	firstBone.v2 = firstBone.v1 + firstDir * firstBone.length;
	secondBone.v1 = firstBone.v2;
	float a2 = -2.f * a1;
	Vec2f secondDir = Vec2f::Rotate(firstDir, a2);
	secondBone.v2 = secondBone.v1 + secondDir * secondBone.length;
}
