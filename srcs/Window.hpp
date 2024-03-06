#pragma once

#include <SDL2/SDL_video.h>

struct SDL_Window;

class Window
{
public:
    Window(const char* name, int width, int height);
    ~Window();

    void Clear();
    void Refresh();
    void PollEvents(bool& running, bool& processing);

private:
    int m_width;
    int m_height;
    SDL_Window* m_window;
    SDL_GLContext m_context;
};
