#include "Timer.h"

Timer::Timer() : elapsedTime(0.0) {}

void Timer::Start() {
	startTime = high_resolution_clock::now();
}

void Timer::Stop() {
	endTime = high_resolution_clock::now();
	SetElapsedTime(startTime, endTime);
}

void Timer::SetElapsedTime(const time_point<high_resolution_clock>& in_startTime, const time_point<high_resolution_clock>& in_endTime) {
	auto duration = std::chrono::duration<double, std::milli>(in_endTime - in_startTime);
	elapsedTime = duration.count();
}

double Timer::GetElapsedTime() const {
	return elapsedTime;
}

void Timer::Reset() {
	elapsedTime = 0.0;
	startTime = high_resolution_clock::now();
}