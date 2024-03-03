#include "Application.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <unistd.h> // usleep

namespace
{
const int framerateLimit = 10;
const int textureWidth = 80;
const int textureHeight = 60;
const int windowWidth = 800;
const int windowHeight = 600;
} // namespace

Application::Application()
    : m_window("Lenia Toybox", windowWidth, windowHeight)
    , m_computeShader("shaders/display_texture.vert", "shaders/conway_life.frag")
    , m_displayShader("shaders/display_texture.vert", "shaders/display_texture.frag")
    , m_textures{
        Texture(textureWidth, textureHeight),
        Texture(textureWidth, textureHeight),
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
    m_computeShader.SetUniformVec2("uniResolution", textureWidth, textureHeight);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_frameBuffer.Unbind();

    m_displayShader.Use();
    m_displayShader.SetUniformVec2("uniResolution", windowWidth, windowHeight);
    m_outputTexture->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_window.Refresh();

    std::swap(m_inputTexture, m_outputTexture);
}
