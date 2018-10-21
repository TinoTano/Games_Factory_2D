#include "Timer.h"

Timer::Timer()
{

}

Timer::~Timer()
{
}

void Timer::StartTimer()
{
	start_time = std::chrono::high_resolution_clock::now();
}

float Timer::ReadAsMS() const
{
	std::chrono::time_point<std::chrono::high_resolution_clock> read_time = std::chrono::high_resolution_clock::now();

	return std::chrono::duration<float, std::chrono::milliseconds::period>(read_time - start_time).count();
}

float Timer::ReadAsSec() const
{
	std::chrono::time_point<std::chrono::high_resolution_clock> read_time = std::chrono::high_resolution_clock::now();

	return std::chrono::duration<float, std::chrono::seconds::period>(read_time - start_time).count();
}

long long Timer::GetCurrentTime() const
{
	return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}
