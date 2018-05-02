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

	return std::chrono::duration_cast<std::chrono::milliseconds>(read_time - start_time).count();
}

float Timer::ReadAsSec() const
{
	std::chrono::time_point<std::chrono::high_resolution_clock> read_time = std::chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<std::chrono::seconds>(read_time - start_time).count();
}
