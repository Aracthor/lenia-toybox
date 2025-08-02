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
[[maybe_unused]] const int windowFramerate = 60;

Application* g_app;
Config g_config;
} // namespace

Application::Application(const Config& config)
    : m_window("Lenia Toybox", windowWidth, windowHeight)
    , m_displayShader("shaders/display_quad.vert", "shaders/display_texture.frag")
{
    g_app = this;
    g_config = config;
    g_config.profile = m_window.CanProfile();
    m_lifeProcessor.reset(new LifeProcessor(g_config));
}

Application::~Application() = default;

int Application::Run()
{
    auto mainLoop = [](void* data)
    {
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
#endif
        Application* application = reinterpret_cast<Application*>(data);
        application->Update();
        application->m_window.PollEvents(application->m_running, application->m_lifeProcessor->Processing());
    };

    m_running = true;
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(mainLoop, this, 0, true);
#else
    while (m_running)
    {
        m_clock.Update();
        const int elapsedTimeInUs = m_clock.GetElapsedTimeInUs();
        const int frameTimeInUs = 1000000 / windowFramerate;
        const int timeToWaitInUs = frameTimeInUs - elapsedTimeInUs;
        if (timeToWaitInUs > 0)
            usleep(timeToWaitInUs);
        m_clock.Update();
        mainLoop(this);
    }
#endif

    return 0;
}

void Application::Restart(int framerate)
{
    m_lifeProcessor.reset(new LifeProcessor(g_config));
}

void Application::Update()
{
    m_lifeProcessor->Update();

    m_displayShader.Use();
    m_displayShader.SetUniformVec2("uniResolution", windowWidth, windowHeight);
    m_lifeProcessor->OutputTexture()->Bind();
    glViewport(0, 0, windowWidth, windowHeight);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_window.Refresh();
}

// clang-format off
#ifdef __EMSCRIPTEN__
extern "C"
{
    void EMSCRIPTEN_KEEPALIVE app_restart() { g_app->Restart(g_config.framerate); }

    void EMSCRIPTEN_KEEPALIVE app_config_width(int width) { g_config.width = width; }
    void EMSCRIPTEN_KEEPALIVE app_config_height(int height) { g_config.height = height; }
    void EMSCRIPTEN_KEEPALIVE app_config_framerate(int framerate) { g_config.framerate = framerate; }
    void EMSCRIPTEN_KEEPALIVE app_config_startup(char* startup)
    {
        g_config.startupFileName = startup ? startup : "";
        free(static_cast<void*>(startup));
    }
    void EMSCRIPTEN_KEEPALIVE app_config_pause(bool pause) { g_config.pause = pause; }

    void EMSCRIPTEN_KEEPALIVE app_config_algorithm(char* algorithm)
    {
        g_config.algorithm = Algorithm::FromName(algorithm);
        free(static_cast<void*>(algorithm));
    }
    void EMSCRIPTEN_KEEPALIVE app_config_range(int range) { g_config.range = range; }
    void EMSCRIPTEN_KEEPALIVE app_config_timestamp(int timestamp) { g_config.timestamp = timestamp; }
    void EMSCRIPTEN_KEEPALIVE app_config_survival_range_min(float min) { g_config.survivalRangeMin = min; }
    void EMSCRIPTEN_KEEPALIVE app_config_survival_range_max(float max) { g_config.survivalRangeMax = max; }
    void EMSCRIPTEN_KEEPALIVE app_config_birth_range_min(float min) { g_config.birthRangeMin = min; }
    void EMSCRIPTEN_KEEPALIVE app_config_birth_range_max(float max) { g_config.birthRangeMax = max; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_center(float center) { g_config.kernelGaussCenter = center; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_width(float width) { g_config.kernelGaussWidth = width; }
    void EMSCRIPTEN_KEEPALIVE app_config_add_kernel(int index) { g_config.kernels[index - 1] = Config::Kernel(); }
    void EMSCRIPTEN_KEEPALIVE app_config_remove_kernel(int index) { g_config.kernels[index - 1] = {}; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_1_growth_center(float center) { g_config.kernels[0]->growthGaussCenter = center; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_1_growth_width(float width) { g_config.kernels[0]->growthGaussWidth = width; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_1_ring_1_weight(float weight) { g_config.kernels[0]->ringWeights[0] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_1_ring_2_weight(float weight) { g_config.kernels[0]->ringWeights[1] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_1_ring_3_weight(float weight) { g_config.kernels[0]->ringWeights[2] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_1_remove_ring(int index) { g_config.kernels[0]->ringWeights[index - 1] = {}; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_2_growth_center(float center) { g_config.kernels[1]->growthGaussCenter = center; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_2_growth_width(float width) { g_config.kernels[1]->growthGaussWidth = width; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_2_ring_1_weight(float weight) { g_config.kernels[1]->ringWeights[0] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_2_ring_2_weight(float weight) { g_config.kernels[1]->ringWeights[1] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_2_ring_3_weight(float weight) { g_config.kernels[1]->ringWeights[2] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_2_remove_ring(int index) { g_config.kernels[1]->ringWeights[index - 1] = {}; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_3_growth_center(float center) { g_config.kernels[2]->growthGaussCenter = center; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_3_growth_width(float width) { g_config.kernels[2]->growthGaussWidth = width; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_3_ring_1_weight(float weight) { g_config.kernels[2]->ringWeights[0] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_3_ring_2_weight(float weight) { g_config.kernels[2]->ringWeights[1] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_3_ring_3_weight(float weight) { g_config.kernels[2]->ringWeights[2] = weight; }
    void EMSCRIPTEN_KEEPALIVE app_config_kernel_3_remove_ring(int index) { g_config.kernels[2]->ringWeights[index - 1] = {}; }
}
#endif // __EMSCRIPTEN__
// clang-format on
