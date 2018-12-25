#include "Timer.hpp"

Timer::Timer()
{
	mLastTime = Time::now();
}

void Timer::start()
{
	mLastTime = Time::now();
}

float Timer::getTime()
{
	return std::chrono::duration_cast<std::chrono::duration<float>>(Time::now() - mLastTime).count();
}
