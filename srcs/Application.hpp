#pragma once

#include "Clock.hpp"
#include "FrameBuffer.hpp"
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
    ShaderProgram m_computeShader;
    ShaderProgram m_displayShader;
    Texture m_textures[2];
    const Texture* m_inputTexture;
    const Texture* m_outputTexture;
    FrameBuffer m_frameBuffer;

    bool m_running = false;
};
