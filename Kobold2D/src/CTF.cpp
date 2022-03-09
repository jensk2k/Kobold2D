#include "CTF.h"
#include <sstream>

CTF::CTF(Core& core)
	: GameState(core)
	, viewport(*this, GetWindowHeight() / 36.f)
	, physicsWorld(1.f / viewport.scale, 200.f, 
		[this](Physics::CollisionInfo info) { HandleCollision(info); }
	)
{
	agents.push_back(Agent());
	agents.push_back(Agent());

	{
		Agent& agent = agents[0];
		agent.transform.position = Vec2f(-16.f, 8.f);
		agent.transform.SetHeading(0.f);
		agent.team = Team::Blue;
		agent.physicsObjectId = physicsWorld.AddCircle(Physics::Layer::Agent, agent.transform.position, 1.f);
	}
	{
		Agent& agent = agents[1];
		agent.transform.position = Vec2f(16.f, -8.f);
		agent.transform.SetHeading(PI);
		agent.team = Team::Red;
		agent.physicsObjectId = physicsWorld.AddCircle(Physics::Layer::Agent, agent.transform.position, 1.f);
	}

	physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(-14.f, -14.f), 12.f, 12.f);
	physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(-14.f, 2.f), 12.f, 12.f);
	physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(2.f, -14.f), 12.f, 12.f);
	physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(2.f, 2.f), 12.f, 12.f);
	physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(-20.f, -18.f), 2.f, 36.f);
	physicsWorld.AddBox(Physics::Layer::Environment, Vec2f(18.f, -18.f), 2.f, 36.f);
}

void CTF::Update()
{
	viewport.scale = GetWindowHeight() / 36.f; // map is 64x36 at 16:9

	agents[0].inputs.mousePosition = GetMousePosition();

	for (Agent& agent : agents)
	{
		HandleLocomotion(agent);
		HandleShooting(agent);
	}

	for (Agent& agent : agents)
	{
		Physics::Circle* circle = physicsWorld.GetCircleByID(agent.physicsObjectId);
		if (circle)
		{
			circle->position = agent.transform.position;
		}
	}

	physicsWorld.CheckCollisions();
}

void CTF::HandleKeyDown(Keys key)
{
	AgentInputs& playerAgentInputs = agents[0].inputs;
	switch (key)
	{
	case Keys::W:
		playerAgentInputs.up = true;
		break;
	case Keys::S:
		playerAgentInputs.down = true;
		break;
	case Keys::A:
		playerAgentInputs.left = true;
		break;
	case Keys::D:
		playerAgentInputs.right = true;
		break;
	default:
		break;
	}
}

void CTF::HandleKeyUp(Keys key)
{
	AgentInputs& playerAgentInputs = agents[0].inputs;
	switch (key)
	{
	case Keys::W:
		playerAgentInputs.up = false;
		break;
	case Keys::S:
		playerAgentInputs.down = false;
		break;
	case Keys::A:
		playerAgentInputs.left = false;
		break;
	case Keys::D:
		playerAgentInputs.right = false;
		break;
	default:
		break;
	}
}

void CTF::HandleMouseDown(MouseButtons button, Vec2i mousePosition)
{
	if (button == MouseButtons::LEFT)
	{
		AgentInputs& playerAgentInputs = agents[0].inputs;
		playerAgentInputs.mouseButton = true;
	}
}

void CTF::HandleMouseUp(MouseButtons button, Vec2i mousePosition)
{
	if (button == MouseButtons::LEFT)
	{
		AgentInputs& playerAgentInputs = agents[0].inputs;
		playerAgentInputs.mouseButton = false;
	}
}

void CTF::Render()
{
	//viewport.DrawGrid(*this, 1.f);

	for (const Agent& agent : agents)
	{
		if (!agent.IsAlive())
		{
			Vec2f agentPos = agent.transform.position;
			viewport.DrawLine(agentPos + Vec2f(-1.f, 1.f), agentPos + Vec2f(1.f, -1.f), Colors::RED);
			viewport.DrawLine(agentPos + Vec2f(1.f, 1.f), agentPos + Vec2f(-1.f, -1.f), Colors::RED);
		}
		else
		{
			Vec2f facing = viewport.ScreenToWorldPosition(GetMousePosition()) - agent.transform.position;
			facing.Normalize();

			static Color lightBlue(80, 80, 255);
			static Color lightRed(255, 80, 80);
			Color color = agent.team == Team::Blue ? lightBlue : lightRed;

			viewport.DrawCircleSolid(agent.transform.position, 1.f, color);

			Vec2f direction(agent.transform.GetHeadingCosine(), agent.transform.GetHeadingSine());
			viewport.DrawLine(agent.transform.position, agent.transform.position + direction * 1.f, Colors::WHITE);

			Vec2f hit;
			unsigned hitObjectID;
			if (physicsWorld.Raycast(agent.transform.position + direction * 1.05f, direction, hit, hitObjectID))
			{
				viewport.DrawCircleSolid(hit, 0.125f, Colors::RED);
				viewport.DrawLine(agent.transform.position + direction * 1.6f, hit, Colors::RED);
			}
		}
	}

	{
		const AgentInputs& inputs = agents[0].inputs;
		std::ostringstream os;
		os << " "
			<< (inputs.up ? "#" : "_") << "\n"
			<< (inputs.left ? "#" : "_")
			<< (inputs.down ? "#" : "_")
			<< (inputs.right ? "#" : "_");

		Vec2f playerPos = agents[0].transform.position;
		Vec2i playerPosSS = viewport.WorldToScreenSpace(playerPos);
		DrawText(os.str(), playerPosSS.x, playerPosSS.y);
	}

	for (const Physics::Box& box : physicsWorld.GetBoxes())
	{
		Color color(140, 140, 140);
		viewport.DrawRectangleSolid(box.position, box.width, box.height, color);
		//viewport.DrawRectangle(box.position, box.width, box.height, Colors::YELLOW);
	}

	for (const Physics::Circle& circle : physicsWorld.GetCircles())
	{
		Color color(140, 140, 140);
		//viewport.DrawCircle(circle.position, circle.radius, Colors::YELLOW);
	}

	DrawRectangleSolid(GetMousePosition() - Vec2i(4, 4), 8, 8, Colors::WHITE);
}

void CTF::CalcSteering(Vec2f steeringDirection, float maxSpeed, float acceleration, float breakFriction, Vec2f& position, Vec2f& velocity)
{
	Vec2f accelerationVec;
	float deltaTime = GetDeltaTime();

	Vec2f lateral = Vec2f::Rotate(steeringDirection, PI / 2.f);

	float lateralVelocityScalarProj = Vec2f::Dot(lateral, velocity);
	float correction = lateralVelocityScalarProj / maxSpeed;

	if (!steeringDirection.IsZero())
	{
		accelerationVec = (steeringDirection * (1.f - correction) - lateral * correction) * acceleration;
		//std::cout << "moving" << std::endl;
	}
	else if (!velocity.IsZero()) // break
	{
		accelerationVec = velocity * -breakFriction;
		//std::cout << "breaking" << std::endl;
	}

	velocity += accelerationVec * deltaTime;

	if (velocity.Length2() > maxSpeed * maxSpeed)
	{
		velocity = velocity.Normalized() * maxSpeed;
	}

	// s = s0 + v0*t + 0.5*a*t^2
	position += velocity * deltaTime;

	Vec2f screenMin = viewport.ScreenToWorldPosition(Vec2i(0, 0));
	Vec2f screenMax = viewport.ScreenToWorldPosition(Vec2i(GetWindowWidth(), GetWindowHeight()));
	position.x = MathUtils::Clamp(position.x, screenMin.x, screenMax.x);
	position.y = MathUtils::Clamp(position.y, -screenMin.y, -screenMax.y);
}

void CTF::HandleLocomotion(Agent& agent)
{
	const AgentInputs& inputs = agent.inputs;

	Vec2f steeringDirection;
	if (agent.IsAlive())
	{
		if (inputs.up)
			steeringDirection += Vec2f(0.f, 1.f);
		if (inputs.down)
			steeringDirection += Vec2f(0.f, -1.f);
		if (inputs.left)
			steeringDirection += Vec2f(-1.f, 0.f);
		if (inputs.right)
			steeringDirection += Vec2f(1.f, 0.f);
		steeringDirection.Normalize();
	}

	CalcSteering(steeringDirection, 10.f, 100.f, 60.f, agent.transform.position, agent.velocity);

	float turnAmount = 0.f;
	if (agent.IsAlive())
	{
		Vec2f facing = viewport.ScreenToWorldPosition(inputs.mousePosition) - agent.transform.position;
		facing.Normalize();

		float targetTurnAngle = Vec2f::AngleSigned(agent.transform.Forward(), facing);

		float maxTurnSpeed = MathUtils::Deg2Rad(720.f);

		float turnSpeed = targetTurnAngle > 0.f ? 1.f : -1.f;
		turnSpeed *= maxTurnSpeed;

		turnAmount = turnSpeed * GetDeltaTime();

		if (MathUtils::Abs(turnAmount) > MathUtils::Abs(targetTurnAngle))
		{
			turnAmount = targetTurnAngle;
		}
	}

	agent.transform.ChangeHeading(turnAmount);
}

void CTF::HandleShooting(Agent& agent)
{
	AgentInputs& inputs = agent.inputs;
	if (agent.isGunLoaded && inputs.mouseButton)
	{
		Shoot(agent);
		agent.isGunLoaded = false;
	}
	if (!agent.isGunLoaded && !inputs.mouseButton)
	{
		agent.isGunLoaded = true;
	}
}

void CTF::HandleCollision(Physics::CollisionInfo colInfo)
{
	//std::cout << "HandleCollision() " << info.ObjectID1 << " // " << info.ObjectID2 << std::endl;
	Physics::Circle* c = physicsWorld.GetCircleByID(colInfo.ObjectID1);

	const Physics::Info& info1 = physicsWorld.GetInfoByID(colInfo.ObjectID1);

	if (info1.layer == Physics::Layer::Agent)
	{
		if (Agent* agent = GetAgentByObjectID(colInfo.ObjectID1))
		{
			agent->transform.position -= colInfo.contactNormal * colInfo.collisionsDepth;
		}
	}
}

void CTF::Shoot(const Agent& shooter)
{
	Vec2f rayDirection(shooter.transform.GetHeadingCosine(), shooter.transform.GetHeadingSine());

	Vec2f rayOrigin = shooter.transform.position + rayDirection * 1.05f;
	Vec2f rayHit;
	unsigned hitObjectID;

	if (physicsWorld.Raycast(rayOrigin, rayDirection, rayHit, hitObjectID))
	{
		const Physics::Info& hitInfo = physicsWorld.GetInfoByID(hitObjectID);

		if (hitInfo.layer == Physics::Layer::Agent)
		{
			std::cout << "Hit agent with ID: " << hitObjectID << std::endl;

			if (Agent* hitAgent = GetAgentByObjectID(hitObjectID))
			{
				hitAgent->health -= 55;
			}
		}
	}
}

Agent* CTF::GetAgentByObjectID(unsigned id)
{
	for (Agent& agent : agents)
	{
		if (agent.physicsObjectId == id)
			return &agent;
	}
	return nullptr;
}
