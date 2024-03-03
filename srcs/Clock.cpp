#include "Clock.hpp"

Clock::Clock()
    : m_lastTime(ClockType::now())
{
}

void Clock::Update()
{
    const TimePoint previousTime = m_lastTime;
    m_lastTime = ClockType::now();
    std::chrono::microseconds microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(m_lastTime - previousTime);
    m_elapsedTimeInUs = static_cast<float>(microseconds.count());
}
