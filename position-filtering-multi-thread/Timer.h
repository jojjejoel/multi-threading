#pragma once
#include <chrono>

using namespace std::chrono;

class Timer
{
public:
    Timer();

    void Start();
    void Stop();
    double GetElapsedTime() const;
    void Reset();

private:
    time_point<high_resolution_clock> startTime;
    time_point<high_resolution_clock> endTime;
    double elapsedTime = 0.0;

    void SetElapsedTime(const time_point<high_resolution_clock>& in_startTime, const time_point<high_resolution_clock>& in_endTime);
};