#include "Application.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <unistd.h> // usleep

namespace
{
const int windowWidth = 800;
const int windowHeight = 600;
} // namespace

Application::Application(const Config& config)
    : m_window("Lenia Toybox", windowWidth, windowHeight)
    , m_lifeProcessor(config)
    , m_displayShader("shaders/display_texture.vert", "shaders/display_texture.frag")
{
}

int Application::Run(int framerate)
{
    auto mainLoop = [](void* data) {
        Application* application = reinterpret_cast<Application*>(data);
        application->Update();
        application->m_window.PollEvents(application->m_running, application->m_lifeProcessor.Processing());
    };

    m_running = true;
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, framerate);
    emscripten_set_main_loop_arg(mainLoop, this, 0, true);
#else
    while (m_running)
    {
        m_clock.Update();
        const int elapsedTimeInUs = m_clock.GetElapsedTimeInUs();
        const int frameTimeInUs = 1000000 / framerate;
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

    m_lifeProcessor.Update();

    m_displayShader.Use();
    m_displayShader.SetUniformVec2("uniResolution", windowWidth, windowHeight);
    m_lifeProcessor.OutputTexture()->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_window.Refresh();
}
