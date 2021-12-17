#include "ProcIK.h"
#include "Core.h"
#include <iostream>
#include <sstream>
#include <math.h>
#include <iomanip>
#include "Instrumentor.h"


void ProcIK::DrawSegment(const JointAnimation::Bone& segment)
{
	PROFILE_FUNCTION();
	Vec2i v1i(segment.v1.x, segment.v1.y);
	Vec2i v2i(segment.v2.x, segment.v2.y);

	DrawLine(v1i, v2i, Colors::WHITE);
	DrawCircle(static_cast<Vec2i>(v1i), 8, Colors::WHITE);
	DrawCircle(static_cast<Vec2i>(v2i), 8, Colors::WHITE);
}

ProcIK::ProcIK(Core& core) 
	: GameState(core)
{
	ikLimb.bones = std::vector<JointAnimation::Bone>(3, JointAnimation::Bone());
	for (int i = 0; i < ikLimb.bones.size(); ++i)
	{
		ikLimb.bones[i].length = 150.f;
	}

	twoBonedLimb.bones = std::vector<JointAnimation::Bone>(2, JointAnimation::Bone());
	twoBonedLimb.bones[0].length = 200.f;
	twoBonedLimb.bones[1].length = 200.f;

	twoBonedLimb2.bones = std::vector<JointAnimation::Bone>(2, JointAnimation::Bone());
	twoBonedLimb2.bones[0].length = 200.f;
	twoBonedLimb2.bones[1].length = 200.f;
}

void ProcIK::Update()
{
	// Ping pong
	{
		int y = (SCREEN_H / 2) + static_cast<int>(100.f * sin(GetCurrentTime() * 0.005f));
		pingPong = Vec2i(SCREEN_W / 3, y);
	}
	
	{
		float scale = 0.006f;
		animationCycle.x = cos(GetCurrentTime() * scale);
		animationCycle.y = sin(GetCurrentTime() * scale);
	}
	
	// IK
	{
		static Vec2f root(SCREEN_W / 3, SCREEN_H * 3 / 4);
		//Vector2 target = m_core.GetMousePosition();
		Vec2f target(pingPong.x, pingPong.y);

		JointAnimation::SolveIKLimb(ikLimb, root, target);
	}

	Vec2i animationCycleSS(animationCycle.x * 120, animationCycle.y * 120);

	{
		static Vec2i root(SCREEN_W * 2 / 3, SCREEN_H * 1 / 4);
		//Vector2 target = GetMousePosition();

		Vec2i target = root + Vec2i(0, 320) + animationCycleSS;
		if (target.y > root.y + 380)
			target.y = root.y + 380;

		Vec2f rootF(root.x, root.y);
		Vec2f targetF(target.x, target.y);

		JointAnimation::Solve2BoneLimb(twoBonedLimb, rootF, targetF, false);
	}

	{
		static Vec2f root((SCREEN_W * 2 / 3), SCREEN_H * 1 / 4);
		//Vector2 target = GetMousePosition();

		Vec2f target = root + Vec2f(0.f, 320.f) - animationCycle * 120.f;
		if (target.y > root.y + 380.f)
			target.y = root.y + 380.f;

		JointAnimation::Solve2BoneLimb(twoBonedLimb2, root, target, false);
	}

	//{
	//	static Vector2 root((SCREEN_W * 2 / 3) - 100, SCREEN_H * 1 / 4);
	//	//Vector2 target = GetMousePosition();

	//	Vector2 target = root + Vector2(0.f, 280.f) - animationCycle * 50.f - Vector2(5, 5);
	//	if (target.y > root.y + 320.f)
	//		target.y = root.y + 320.f;

	//	Solve2BoneLimb(twoBonedLimb2, root, target, false);
	//}
}

void ProcIK::Render()
{	
	PROFILE_FUNCTION();
	for (int i = 0 ; i < ikLimb.bones.size(); ++i)
	{
		DrawSegment(ikLimb.bones[i]);
	}

	for (int i = 0; i < twoBonedLimb.bones.size(); ++i)
	{
		DrawSegment(twoBonedLimb.bones[i]);
	}

	for (int i = 0; i < twoBonedLimb2.bones.size(); ++i)
	{
		DrawSegment(twoBonedLimb2.bones[i]);
	}

	Vec2i target = GetMousePosition();

	DrawRectangleSolid(Vec2i(target.x - 5, target.y - 5), 10, 10, Colors::RED);

	DrawRectangleSolid(Vec2i(pingPong.x - 5, pingPong.y - 5), 10, 10, Colors::GREEN);

	{
		Vec2i center(SCREEN_W - 120, 120);
		DrawCircle(center, 100, Colors::WHITE);
		float lineLength = 110;
		DrawLine(center + Vec2i(lineLength, 0), center + Vec2i(-lineLength, 0), Colors::WHITE);
		DrawLine(center + Vec2i(0, lineLength), center + Vec2i(0, -lineLength), Colors::WHITE);

		Vec2i animOffset(static_cast<int>(animationCycle.x) * 100, static_cast<int>(animationCycle.y) * 100);
		Vec2i indicator = center + animOffset - Vec2i(5, 5);
		DrawLine(center, indicator, Colors::MAGENTA);
		DrawRectangleSolid(indicator, 10, 10, Colors::MAGENTA);

	}
	
}

