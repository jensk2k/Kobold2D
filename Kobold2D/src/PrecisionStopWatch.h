#pragma once
#include <chrono>

class PrecisionStopWatch
{
public:
	PrecisionStopWatch() 
	{
		Start();
	}

	void Start() 
	{
		start = std::chrono::high_resolution_clock::now();
	}

	long long Stop()
	{
		stop = std::chrono::high_resolution_clock::now();
		return GetMicro();
	}

	long long GetMicro() const
	{
		long long startMicro = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
		long long stopMicro = std::chrono::time_point_cast<std::chrono::microseconds>(stop).time_since_epoch().count();
		return (stopMicro - startMicro);
	}

	long long GetMilli() const
	{
		long long startMicro = std::chrono::time_point_cast<std::chrono::milliseconds>(start).time_since_epoch().count();
		long long stopMicro = std::chrono::time_point_cast<std::chrono::milliseconds>(stop).time_since_epoch().count();
		return (stopMicro - startMicro);
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> stop;
};

