#include "CreatureGeneration.h"
#include "Core.h"
#include "MathUtils.h"
#include <sstream>

CreatureGeneration::CreatureGeneration(Core& core)
	: GameState(core)
{
	// Generate creatures
	int nCreatures = 1;
	creatures.resize(nCreatures);

	for (int i = 0; i < nCreatures; i++)
	{
		Creature& creature = creatures[i];

		// Gen Transform
		creature.transform.position = Vec2f(-20.f, 0.f);
		creature.transform.SetHeading(MathUtils::Deg2Rad(0.f));

		// Gen VGO
		std::vector<Vec2f> vertices;
		Vec2f v1(3.f, 0.f);
		Vec2f v2(2.f, 2.2f);
		Vec2f v3(-1.f, 2.f);
		Vec2f v4(-3.f, 0.f);

		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		vertices.push_back(v4);
		vertices.push_back(Vec2f(v3.x, -v3.y));
		vertices.push_back(Vec2f(v2.x, -v2.y));
		vertices.push_back(v1);

		vgos.push_back(VectorGraphicsObject(creature.transform, vertices));

		int index = vgos.size() - 1;
		creature.vgoIndex = index;

		// Gen anim clock
		creature.animationClockScale = 4.0f;

		// Gen limbs
		int nLegs = 4;
		creature.legs.resize(nLegs);
		for (int j = 0; j < nLegs; j++)
		{
			Leg& leg = creature.legs[j];

			leg.limb.bones = std::vector<JointAnimation::Bone>(2, JointAnimation::Bone());
			leg.limb.bones[0].length = 2.5f;
			leg.limb.bones[1].length = 2.5f;

			bool even = j % 2 == 0;

			leg.isRight = even;

			int row = j / 2.f;

			leg.rootOffset = Vec2f(even ? 2.f : -2.f, 2.f - row * 2.2f);

			leg.animationTargetOffset = 3.2f;
			leg.animationTargetRadius = 1.6f;
			//leg.animationTargetContactRange = 4.2f;

			leg.animationClockOffset = j * 0.24f;
		}

		creature.maxSpeed = 24.f;
	}
}

void CreatureGeneration::Update()
{
	for (int i = 0; i < creatures.size(); i++)
	{
		Creature& creature = creatures[i];
		Transform& transform = creature.transform;
		Vec2f right = transform.Right();
		Vec2f left = transform.Left();
		Vec2f forward = transform.Forward();

		// Update animation clock
		float animationSpeedScale = creature.velocity.Length() / creature.maxSpeed;
		creature.animationClock += GetDeltaTime() * animationSpeedScale;

		// Update legs
		for (int j = 0; j < creature.legs.size(); j++)
		{
			Leg& leg = creature.legs[j];

			bool even = j % 2 == 0;
			
			//if (even) continue;


			float animClockDirection = even ? -1.f : 1.f;
			Vec2f animationTargetDirection = even ? right : left;

			Vec2f root = transform.position + forward * leg.rootOffset.y + right * leg.rootOffset.x;

			float time = creature.animationClock * creature.animationClockScale + leg.animationClockOffset;

			Vec2f animationClockVector;
			animationClockVector.x = cos(time * animClockDirection * PI * 2.f);
			animationClockVector.y = sin(time * animClockDirection * PI * 2.f);

			leg.contact = animationSpeedScale < 0.1f || (even ? animationClockVector.y < -0.5f : animationClockVector.y > 0.5f);

			Vec2f target;

			if (leg.contact)
			{
				Vec2f footPos = leg.GetFootPosition();
				target = footPos;
			}
			else
			{
				Vec2f animationOffsetVector = Vec2f::Rotate(animationClockVector, -creature.transform.GetHeading());
				target = root + animationTargetDirection * leg.animationTargetOffset + animationOffsetVector * leg.animationTargetRadius;
			}

			//float scalarProj = Vector2::Dot(target - root, animationTargetDirection);

			//float contactRange = leg.animationTargetContactRange;

			//leg.contact = scalarProj > contactRange;

			//if (leg.contact)
			//{
			//	float offset = scalarProj - contactRange;
			//	target += Vector2(-animationTargetDirection.x, -animationTargetDirection.y) * offset;
			//	
			//	Vector2 footPos = leg.limb.bones[1].v2;
			//}

			JointAnimation::Solve2BoneLimb(leg.limb, root, target, even);
		}

		bool legContanct = false;
		for (int j = 0; j < creature.legs.size(); j++)
		{
			Leg& leg = creature.legs[j];
			if (leg.contact)
			{
				legContanct = true;
				break;
			}
		}

		Vec2f mouseInWS = viewport.ScreenToWorldPosition(GetMousePosition());
		float bound = 5.f;
		bool shouldAccelerate = legContanct && (mouseInWS - creature.transform.position).Length2() > bound * bound;

		// Update locomotion
		creature.steeringTarget = mouseInWS;

		Vec2f steeringDirection = creature.steeringTarget - creature.transform.position;
		steeringDirection.Normalize();

		float deltaTime = GetDeltaTime();

		float targetTurnAngle = Vec2f::AngleSigned(forward, steeringDirection);

		float maxTurnSpeed = MathUtils::Deg2Rad(270.f);

		float turnSpeed = targetTurnAngle > 0.f ? 1.f : -1.f;
		turnSpeed *= maxTurnSpeed;

		float turnAmount = turnSpeed * deltaTime;

		if (MathUtils::Abs(turnAmount) > MathUtils::Abs(targetTurnAngle))
		{
			turnAmount = targetTurnAngle;
		}

		creature.transform.ChangeHeading(turnAmount);

		creature.acceleration = Vec2f();

		if (shouldAccelerate)
		{
			float maxAcc = 40.f;

			Vec2f lateral = Vec2f::Rotate(steeringDirection, PI / 2.f);
			
			float lateralVelocityScalarProj = Vec2f::Dot(lateral, creature.velocity);
			float correction = lateralVelocityScalarProj / creature.maxSpeed;

			creature.acceleration = (steeringDirection * (1.f - correction) - lateral * correction) * maxAcc;

		}
		else if (creature.velocity.Length2() > 0.f)
		{
			float maxDecc = 70.f;
			//break
			creature.acceleration = creature.velocity.Normalized() * -maxDecc;
		}

		//Vector2 friction = creature.velocity * 0.9f;

		creature.velocity += creature.acceleration * deltaTime;

		if (creature.velocity.Length2() > creature.maxSpeed*creature.maxSpeed)
		{
			creature.velocity = creature.velocity.Normalized() * creature.maxSpeed;
		}

		// s = s0 + v0*t + 0.5*a*t^2

		creature.transform.position += creature.velocity * deltaTime;
	}

	vpc.Update(viewport, GetDeltaTime());
}

void CreatureGeneration::Render()
{
	viewport.DrawGrid(*this, 2);

	for (int i = 0; i < vgos.size(); i++)
	{
		VectorGraphicsObject& vgo = vgos[i];

		vgo.Draw(*this);
	}

	for (int i = 0; i < creatures.size(); ++i)
	{
		const Creature& creature = creatures[i];

		// Draw legs
		for (int j = 0; j < creature.legs.size(); ++j)
		{
			const Leg& leg = creature.legs[j];
			bool even = j % 2 == 0;

			for (int k = 0; k < leg.limb.bones.size(); ++k)
			{
				const JointAnimation::Bone& segment = leg.limb.bones[k];

				Vec2f v1 = viewport.WorldToScreenSpace(segment.v1);
				Vec2f v2 = viewport.WorldToScreenSpace(segment.v2);

				DrawLine(v1, v2, Colors::WHITE);				
			}

			// Contacts
			if (leg.contact)
			{
				Vec2f footPos = leg.GetFootPosition();

				DrawCircle(viewport.WorldToScreenSpace(footPos), viewport.scale * .5f, Colors::WHITE);
			}


		}

		std::ostringstream ss;
		ss << "h: " << MathUtils::Rad2Deg(creature.transform.GetHeading());
		Vec2f textPos = viewport.WorldToScreenSpace(creature.transform.position);

		DrawText(ss.str(), textPos.x, textPos.y);

		//// Draw steering
		//DrawArrowWS(creature.transform.position, (creature.steeringTarget - creature.transform.position).Normalized(), 4.f, Colors::YELLOW);

		//DrawArrowWS(creature.transform.position, creature.velocity, 1.f, Colors::MAGENTA);
		//DrawArrowWS(creature.transform.position, creature.acceleration, 1.f, Colors::RED);
	}

	Vec2f mousePosition = GetMousePosition();
	DrawRectangleSolid(Vec2f(mousePosition.x - 5.f, mousePosition.y - 5.f), 10, 10, Colors::RED);

}

void CreatureGeneration::HandleKeyDown(Keys key)
{
	vpc.HandleKeyDown(key);
}

void CreatureGeneration::HandleKeyUp(Keys key)
{
	vpc.HandleKeyUp(key);
}
