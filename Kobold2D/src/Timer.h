#pragma once
class Timer
{
public:
	Timer(float reset) : reset(reset) {};
	Timer() : reset(0.f) {};
	bool Update(float deltaTime) 
	{
		bool ret = false;
		if (timer <= 0.f)
		{
			timer = reset;
			ret = true;
		}
		timer -= deltaTime;
		return ret;
	}
	float timer = 0.f;
	float reset;
};

