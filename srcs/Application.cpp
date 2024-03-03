#include "Application.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "Window.hpp"

Application::Application()
    : m_window("Lenia Toybox", 800, 600)
{
}

int Application::Run()
{
    auto mainLoop = [](void* data) {
        Application* application = reinterpret_cast<Application*>(data);
        application->m_window.Clear();
        application->m_window.Refresh();
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
