#pragma once

#include "Clock.hpp"
#include "ShaderProgram.hpp"
#include "Window.hpp"

struct Config;
class LifeProcessor;

class Application
{
public:
    Application(const Config& config);
    ~Application();

    int Run(int framerate);

    void Restart();

private:
    void Update();

    Clock m_clock;
    Window m_window;
    LifeProcessor* m_lifeProcessor;
    ShaderProgram m_displayShader;

    bool m_running = false;
};
