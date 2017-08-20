#ifndef TIMER_H
#define TIMER_H

#include "Windows.h"

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void Stop();

	float GetTime();

private:

	LARGE_INTEGER start_time_;
	LARGE_INTEGER stop_time_;
	LARGE_INTEGER frequency_;
};

#endif