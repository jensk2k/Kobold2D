#pragma once
#include "Vector2.h"

struct Transform
{
	Vec2f position;

	float GetHeading() const { return heading; }
	float GetHeadingCosine() const { return headingCosine; }
	float GetHeadingSine() const { return headingSine; }
	void SetHeading(float newHeading);
	void ChangeHeading(float change);

	Vec2f Forward() const
	{
		return Vec2f(headingCosine, headingSine);
	}
	Vec2f Back() const
	{
		return Vec2f(-headingCosine, -headingSine);
	}

	Vec2f Right() const
	{
		return Vec2f(headingSine, -headingCosine);
	}

	Vec2f Left() const
	{
		return Vec2f(-headingSine, headingCosine);
	}

private:
	float heading;
	float headingCosine;
	float headingSine;
};

