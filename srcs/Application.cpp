#include "Application.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <unistd.h> // usleep

namespace
{
const int framerateLimit = 60;
}

Application::Application()
    : m_window("Lenia Toybox", 800, 600)
    , m_computeShader("shaders/display_texture.vert", "shaders/compute.frag")
    , m_displayShader("shaders/display_texture.vert", "shaders/display_texture.frag")
    , m_textures{
        Texture(800, 600),
        Texture(800, 600),
    }
    , m_inputTexture(&m_textures[0])
    , m_outputTexture(&m_textures[1])
{
    m_textures[0].FillWithRandom();
}

int Application::Run()
{
    auto mainLoop = [](void* data) {
        Application* application = reinterpret_cast<Application*>(data);
        application->Update();
        application->m_running = !application->m_window.GotQuitEvent();
    };

    m_running = true;
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, framerateLimit);
    emscripten_set_main_loop_arg(mainLoop, this, 0, true);
#else
    while (m_running)
    {
        m_clock.Update();
        const int elapsedTimeInUs = m_clock.GetElapsedTimeInUs();
        const int frameTimeInUs = 1000000 / framerateLimit;
        const int timeToWaitInUs = frameTimeInUs - elapsedTimeInUs;
        if (timeToWaitInUs > 0)
            usleep(timeToWaitInUs);
        m_clock.Update();
        mainLoop(this);
    }
#endif

    return 0;
}

void Application::Update()
{
    m_window.Clear();

    m_outputTexture->AttachToFrameBuffer(m_frameBuffer);
    m_inputTexture->Bind();
    m_computeShader.Use();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_frameBuffer.Unbind();

    m_displayShader.Use();
    m_outputTexture->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_window.Refresh();

    std::swap(m_inputTexture, m_outputTexture);
}
