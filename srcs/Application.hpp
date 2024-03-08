#pragma once

#include "Clock.hpp"
#include "LifeProcessor.hpp"
#include "ShaderProgram.hpp"
#include "Window.hpp"

struct Config;

class Application
{
public:
    Application(const Config& config);

    int Run(int framerate);

private:
    void Update();

    Clock m_clock;
    Window m_window;
    LifeProcessor m_lifeProcessor;
    ShaderProgram m_displayShader;

    bool m_running = false;
};
