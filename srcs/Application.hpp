#pragma once

#include "Clock.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "Window.hpp"

class Application
{
public:
    Application();

    int Run();

private:
    void Update();

    Clock m_clock;
    Window m_window;
    ShaderProgram m_shader;
    Texture m_texture;

    bool m_running = false;
};
