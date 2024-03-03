#pragma once

#include "Window.hpp"

class Application
{
public:
    Application();

    int Run();

private:
    Window m_window;
    bool m_running = false;
};
