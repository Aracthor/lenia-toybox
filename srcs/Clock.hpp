#pragma once

#include <chrono>

class Clock
{
public:
    Clock();

    void Update();

    float GetElapsedTimeInUs() const { return m_elapsedTimeInUs; }

private:
    using ClockType = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<ClockType>;

    TimePoint m_lastTime;
    float m_elapsedTimeInUs = 0.f;
};
