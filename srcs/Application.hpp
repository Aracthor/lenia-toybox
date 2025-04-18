#pragma once

#include <memory>

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

    int Run();

    void Restart(int framerate);

private:
    void Update();

    Clock m_clock;
    Window m_window;
    std::unique_ptr<LifeProcessor> m_lifeProcessor;
    ShaderProgram m_displayShader;

    bool m_running = false;
};
