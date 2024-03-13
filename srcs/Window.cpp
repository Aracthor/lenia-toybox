#include "Window.hpp"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <iostream>

namespace
{
bool hasGlExtension(const char* extensionName)
{
    const std::string extensions(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    return extensions.find(extensionName) != std::string::npos;
}
} // namespace

Window::Window(const char* name, int width, int height)
    : m_width(width)
    , m_height(height)
{
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "canvas");
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "could not initialize sdl2: " << SDL_GetError() << std::endl;
        std::terminate();
    }
    m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context)
    {
        std::cerr << "Failed to create GL context: " << SDL_GetError() << std::endl;
        std::terminate();
    }
    if (SDL_GL_MakeCurrent(m_window, m_context) < 0)
    {
        std::cerr << "Failed to make GL context current: " << SDL_GetError() << std::endl;
        std::terminate();
    }

    if (!hasGlExtension("GL_EXT_color_buffer_float"))
    {
        std::cerr << "Your system doesn't support needed extension GL_EXT_color_buffer_float." << std::endl;
        std::terminate();
    }

    if (glewInit() != GLEW_OK)
        std::cerr << "GLEW not initialized correctly" << std::endl;
}

Window::~Window()
{
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::Clear()
{
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::Refresh()
{
    SDL_GL_SwapWindow(m_window);
}

void Window::PollEvents(bool& running, bool& processing)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            running = false;
        else if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_P)
            processing = !processing;
    }
}
