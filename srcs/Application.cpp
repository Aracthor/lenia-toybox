#include "Application.hpp"

#include "LifeProcessor.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <unistd.h> // usleep

namespace
{
const int windowWidth = 800;
const int windowHeight = 600;

Application* g_app;
Config g_config;
} // namespace

Application::Application(const Config& config)
    : m_window("Lenia Toybox", windowWidth, windowHeight)
    , m_lifeProcessor(new LifeProcessor(config))
    , m_displayShader("shaders/display_texture.vert", "shaders/display_texture.frag")
{
    g_app = this;
    g_config = config;
}

Application::~Application()
{
    delete m_lifeProcessor;
}

int Application::Run(int framerate)
{
    auto mainLoop = [](void* data) {
        Application* application = reinterpret_cast<Application*>(data);
        application->Update();
        application->m_window.PollEvents(application->m_running, application->m_lifeProcessor->Processing());
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

void Application::Restart()
{
    delete m_lifeProcessor;
    m_lifeProcessor = new LifeProcessor(g_config);
}

void Application::Update()
{
    m_window.Clear();

    m_lifeProcessor->Update();

    m_displayShader.Use();
    m_displayShader.SetUniformVec2("uniResolution", windowWidth, windowHeight);
    m_lifeProcessor->OutputTexture()->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_window.Refresh();
}

#ifdef __EMSCRIPTEN__
extern "C"
{
    void EMSCRIPTEN_KEEPALIVE app_restart() { g_app->Restart(); }

    void EMSCRIPTEN_KEEPALIVE app_config_width(int width) { g_config.width = width; }
    void EMSCRIPTEN_KEEPALIVE app_config_height(int height) { g_config.height = height; }
    void EMSCRIPTEN_KEEPALIVE app_config_startup(char* startup)
    {
        g_config.startupFileName = startup;
        free(static_cast<void*>(startup));
    }
    void EMSCRIPTEN_KEEPALIVE app_config_pause(bool pause) { g_config.pause = pause; }

    void EMSCRIPTEN_KEEPALIVE app_config_range(int range) { g_config.range = range; }
    void EMSCRIPTEN_KEEPALIVE app_config_algorithm(char* algorithm)
    {
        g_config.algorithm = Algorithm::FromName(algorithm);
        free(static_cast<void*>(algorithm));
    }
}
#endif // __EMSCRIPTEN__
