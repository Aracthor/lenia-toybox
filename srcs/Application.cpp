#include "Application.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Application::Application()
    : m_window("Lenia Toybox", 800, 600)
    , m_shader("shaders/display_texture.vert", "shaders/display_texture.frag")
    , m_texture(800, 600)
{
    m_texture.FillWithRandom();
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
    emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, 60);
    emscripten_set_main_loop_arg(mainLoop, this, 0, true);
#else
    while (m_running)
    {
        mainLoop(this);
    }
#endif

    return 0;
}

void Application::Update()
{
    m_window.Clear();
    m_texture.Bind();
    m_shader.Use();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_window.Refresh();
}
