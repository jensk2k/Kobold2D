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
	DrawLine(segment.v1, segment.v2, Colors::WHITE);
	DrawCircle(segment.v1, 8.f, Colors::WHITE);
	DrawCircle(segment.v2, 8.f, Colors::WHITE);
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
		float y = (SCREEN_H / 2) + 100.f * sin(GetCurrentTime() * 0.005f);
		pingPong = Vec2f(SCREEN_W / 3, y);
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
		Vec2f target = pingPong;

		JointAnimation::SolveIKLimb(ikLimb, root, target);
	}

	{
		static Vec2f root(SCREEN_W * 2 / 3, SCREEN_H * 1 / 4);
		//Vector2 target = GetMousePosition();

		Vec2f target = root + Vec2f(0.f, 320.f) + animationCycle * 120.f;
		if (target.y > root.y + 380.f)
			target.y = root.y + 380.f;

		JointAnimation::Solve2BoneLimb(twoBonedLimb, root, target, false);
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

	Vec2f target = GetMousePosition();

	DrawRectangleSolid(Vec2f(target.x - 5.f, target.y - 5.f), 10, 10, Colors::RED);

	DrawRectangleSolid(Vec2f(pingPong.x - 5.f, pingPong.y - 5.f), 10, 10, Colors::GREEN);


	{
		Vec2f center(SCREEN_W - 120, 120);
		DrawCircle(center, 100.f, Colors::WHITE);
		float lineLength = 110.f;
		DrawLine(center + Vec2f(lineLength, 0.f), center + Vec2f(-lineLength, 0.f), Colors::WHITE);
		DrawLine(center + Vec2f(0.f, lineLength), center + Vec2f(0.f, -lineLength), Colors::WHITE);

		Vec2f indicator = center + animationCycle * 100.f - Vec2f(5, 5);
		DrawLine(center, indicator, Colors::MAGENTA);
		DrawRectangleSolid(indicator, 10, 10, Colors::MAGENTA);

	}
	
}

