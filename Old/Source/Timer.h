#pragma once

#include <chrono>

class Timer
{
public:
	Timer();
	~Timer();

	void StartTimer();

	float ReadAsMS() const;
	float ReadAsSec() const;

	long long GetCurrentTime() const;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

