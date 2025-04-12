#pragma once

#include <SDL2/SDL_video.h>

struct SDL_Window;

class Window
{
public:
    Window(const char* name, int width, int height);
    ~Window();

    void Refresh();
    void PollEvents(bool& running, bool& processing);

    bool CanProfile() const;

private:
    SDL_Window* m_window;
    SDL_GLContext m_context;
};
