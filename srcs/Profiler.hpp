#pragma once

#include <GL/glew.h>
#include <vector>

class Profiler
{
public:
    Profiler();
    ~Profiler();

    void StartProfile();
    void StopProfile();

private:
    GLuint m_query;
    std::vector<double> m_times;
};
