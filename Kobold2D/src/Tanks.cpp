#include "Tanks.h"
#include "Random.h"
#include "MathUtils.h"

Tanks::Tanks(Core& core)
	: GameState(core)
	, viewport(*this, 12.7f)
{
	// generate tank data
	for (int i = 0; i < tanks.size(); i++)
	{
		tanks[i].transform.position = Vec2f(Random::Range(-20.f, 20.f), Random::Range(-10.f, 10.f));
		tanks[i].transform.SetHeading(0.f);
		tanks[i].turretRotation = 0.f;
		tanks[i].width = Random::Range(2.f, 4.f);
		tanks[i].length = Random::Range(4.f, 6.f);
	}

	// generate VGOs
	for (int i = 0; i < tanks.size(); i++)
	{
		float w = tanks[i].width;
		float l = tanks[i].length;

		Vec2f frontLeft(l / 2.f, -w / 2.f); // A
		Vec2f frontRight(l / 2.f, w / 2.f); // B
		Vec2f backLeft(-l / 2.f, -w / 2.f); // D
		Vec2f backRight(-l / 2.f, w / 2.f); // C

		std::vector<Vec2f> vertices;
		vertices.push_back(frontLeft);
		vertices.push_back(frontRight);
		vertices.push_back(backRight);
		vertices.push_back(backLeft);
		vertices.push_back(frontLeft);


		Vec2f position = tanks[i].transform.position;
		float rotation = tanks[i].transform.GetHeading();

		vgos.push_back(VectorGraphicsObject(tanks[i].transform, vertices));

		int index = vgos.size() - 1;

		tanks[i].vgoIndex = index;
	}
}

void Tanks::Init()
{
	LoadTexture("assets/tankbody.png", tankBodyTex, 148, 240);
	LoadTexture("assets/tankturret.png", tankTurretTex, 120, 222);
}

void Tanks::Update()
{
	vpc.Update(viewport, GetDeltaTime());

	for (int i = 0; i < tanks.size(); i++)
	{
		Tank& tank = tanks[i];
		
		Vec2f toCenter(-tank.transform.position.x, -tank.transform.position.y);
		Vec2f tankForward(cos(tank.transform.GetHeading()), sin(tank.transform.GetHeading()));

		float angle = 0.f;
		if (left)
			angle = -1.f;
		if (right)
			angle = 1.f;
		if (up)
			tank.speed = 10.f;
		else if (down)
			tank.speed = -10.f;
		else
			tank.speed = 0.f;

		tank.transform.ChangeHeading(-angle * GetDeltaTime());
		tank.transform.position += tankForward * tank.speed * GetDeltaTime();

		if (tank.vgoIndex > -1)
		{
			vgos[tank.vgoIndex].transform.position = tank.transform.position;
			vgos[tank.vgoIndex].transform.SetHeading(tank.transform.GetHeading());
		}
	}
}

void Tanks::Render()
{
	for (int i = 0; i < vgos.size(); i++)
	{
		const VectorGraphicsObject& vgo = vgos[i];
		//const std::vector<Vector2>& vertices = vgo.vertices;

		vgo.Draw(*this, viewport);

		//float cosR = cos(vgo.rotation);
		//float sinR = sin(vgo.rotation);

		//if (vertices.size() > 1)
		//{
		//	for (int j = 0; j < vertices.size() - 1; j++)
		//	{
		//		const Vector2& v1 = vertices[j];
		//		const Vector2& v2 = vertices[j + 1];

		//		// translate
		//		Vector2 v1Trans = v1 + vgo.position;
		//		Vector2 v2Trans = v2 + vgo.position;

		//		// rotate
		//		Vector2 v1Rot = Vector2::RotateAroundPoint(v1Trans, vgo.position, cosR, sinR);
		//		Vector2 v2Rot = Vector2::RotateAroundPoint(v2Trans, vgo.position, cosR, sinR);

		//		Vector2 v1ss = viewport.WorldToScreenSpace(v1Rot);
		//		Vector2 v2ss = viewport.WorldToScreenSpace(v2Rot);

		//		DrawLine(v1ss, v2ss, Colors::WHITE);
		//	}
		//}
	}

	DrawTexture(tankBodyTex, viewport.WorldToScreenSpace(Vec2f(0, 0)), tankBodyTex.m_width, tankBodyTex.m_height);
	DrawTexture(tankTurretTex, viewport.WorldToScreenSpace(Vec2f(0, 0)), tankTurretTex.m_width, tankTurretTex.m_height);

	DrawLine(viewport.WorldToScreenSpace(Vec2f(-1.f, 0.f)), viewport.WorldToScreenSpace(Vec2f(1.f, 0.f)), Colors::WHITE);
	DrawLine(viewport.WorldToScreenSpace(Vec2f(0.f, -1.f)), viewport.WorldToScreenSpace(Vec2f(0.f, 1.f)), Colors::WHITE);
}

void Tanks::HandleKeyDown(Keys key)
{
	vpc.HandleKeyDown(key);

	if (key == Keys::W)
		up = true;
	if (key == Keys::S)
		down = true;
	if (key == Keys::A)
		left = true;
	if (key == Keys::D)
		right = true;
}

void Tanks::HandleKeyUp(Keys key)
{
	vpc.HandleKeyUp(key);

	if (key == Keys::W)
		up = false;
	if (key == Keys::S)
		down = false;
	if (key == Keys::A)
		left = false;
	if (key == Keys::D)
		right = false;
}

//void ViewPortControl::HandleKeyDown(Keys key)
//{
//	if (key == Keys::UP)
//		up = true;
//	if (key == Keys::DOWN)
//		down = true;
//	if (key == Keys::LEFT)
//		left = true;
//	if (key == Keys::RIGHT)
//		right = true;
//}
//
//void ViewPortControl::HandleKeyUp(Keys key)
//{
//	if (key == Keys::UP)
//		up = false;
//	if (key == Keys::DOWN)
//		down = false;
//	if (key == Keys::LEFT)
//		left = false;
//	if (key == Keys::RIGHT)
//		right = false;
//}

//void ViewPortControl::Update(Viewport vp, float deltaTime)
//{
//	float moveSpeed = 50.f;
//	if (up)
//		vp.position.y += moveSpeed * deltaTime;
//	if (down)
//		vp.position.y -= moveSpeed * deltaTime;
//	if (left)
//		vp.position.x -= moveSpeed * deltaTime;
//	if (right)
//		vp.position.x += moveSpeed * deltaTime;
//}
//
//Vector2 Viewport::WorldToScreenSpace(Vector2 worldPos) const
//{
//	Vector2 worldPosRelativeToVP = worldPos - position;
//
//	Vector2 screenSpacePos;
//	screenSpacePos.x = width / 2.f + worldPosRelativeToVP.x * scale;
//	screenSpacePos.y = height / 2.f - worldPosRelativeToVP.y * scale;
//
//	return screenSpacePos;
//}
