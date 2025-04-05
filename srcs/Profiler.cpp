#include "Profiler.hpp"

#include <iomanip>
#include <iostream>
#include <numeric>

namespace
{
const int averageProfileCount = 100;
}

Profiler::Profiler()
{
    glGenQueries(1, &m_query);
}

Profiler::~Profiler()
{
    glDeleteQueries(1, &m_query);
}

void Profiler::StartProfile()
{
    glBeginQuery(GL_TIME_ELAPSED_EXT, m_query);
}

void Profiler::StopProfile()
{
    glEndQuery(GL_TIME_ELAPSED_EXT);
    GLuint64 timeElapsed;
    glGetQueryObjectui64vEXT(m_query, GL_QUERY_RESULT, &timeElapsed);
    m_times.push_back(timeElapsed / 1000.0);
    if (m_times.size() >= averageProfileCount)
    {
        const double averageTimeInUs = std::accumulate(m_times.begin(), m_times.end(), 0.0) / averageProfileCount;
        const float timeInMs = averageTimeInUs / 1000.f;
        std::cout << "Average process time: " << std::fixed << std::setprecision(2) << timeInMs << "ms" << std::endl;
        m_times.clear();
    }
}
