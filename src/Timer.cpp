#include "Timer.h"

Timer::Timer()
{
	QueryPerformanceFrequency(&frequency_);
}

Timer::~Timer()
{

}

void Timer::Start()
{
	QueryPerformanceCounter(&start_time_);
}

void Timer::Stop()
{
	QueryPerformanceCounter(&stop_time_);
}

float Timer::GetTime()
{
	LARGE_INTEGER microseconds;
	microseconds.QuadPart = stop_time_.QuadPart - start_time_.QuadPart;

	microseconds.QuadPart *= 1000000;
	microseconds.QuadPart /= frequency_.QuadPart;

	return (float)microseconds.QuadPart / 1000000.f;
}