#include "Application.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <unistd.h> // usleep

namespace
{
const int windowWidth = 800;
const int windowHeight = 600;

void swap(const Texture*& textureA, const Texture*& textureB)
{
    const Texture* tmp = textureA;
    textureA = textureB;
    textureB = tmp;
}

const char* ComputeShaderProgram(Algorithm::Type type)
{
    switch (type)
    {
    case Algorithm::LargerThanLife:
        return "shaders/larger-than-life.frag";
    case Algorithm::Primordia:
        return "shaders/primordia.frag";
    case Algorithm::Lenia:
        return "shaders/lenia.frag";
    }
    return nullptr;
}

} // namespace

Application::Application(const Config& config)
    : m_config(config)
    , m_window("Lenia Toybox", windowWidth, windowHeight)
    , m_computeShader("shaders/display_texture.vert", ComputeShaderProgram(m_config.algorithm))
    , m_displayShader("shaders/display_texture.vert", "shaders/display_texture.frag")
    , m_textures{
        Texture(m_config.width, m_config.height),
        Texture(m_config.width, m_config.height),
    }
    , m_inputTexture(&m_textures[0])
    , m_outputTexture(&m_textures[1])
    , m_processing(!m_config.pause)
{
    if (m_config.startupFileName)
    {
        m_textures[1].FillWithTextureFile(m_config.startupFileName);
    }
    else
    {
        m_textures[1].FillWithRandom();
    }
}

int Application::Run()
{
    auto mainLoop = [](void* data) {
        Application* application = reinterpret_cast<Application*>(data);
        application->Update();
        application->m_window.PollEvents(application->m_running, application->m_processing);
    };

    m_running = true;
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, m_config.framerate);
    emscripten_set_main_loop_arg(mainLoop, this, 0, true);
#else
    while (m_running)
    {
        m_clock.Update();
        const int elapsedTimeInUs = m_clock.GetElapsedTimeInUs();
        const int frameTimeInUs = 1000000 / m_config.framerate;
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

    if (m_processing)
    {
        swap(m_inputTexture, m_outputTexture);
        m_outputTexture->AttachToFrameBuffer(m_frameBuffer);
        m_inputTexture->Bind();
        ConfigureComputeProgram();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_frameBuffer.Unbind();
    }

    m_displayShader.Use();
    m_displayShader.SetUniformVec2("uniResolution", windowWidth, windowHeight);
    m_outputTexture->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_window.Refresh();
}

void Application::ConfigureComputeProgram() const
{
    m_computeShader.Use();
    m_computeShader.SetUniformVec2("uniResolution", m_config.width, m_config.height);
    m_computeShader.SetUniformInt("uniRange", m_config.range);
    switch (m_config.algorithm)
    {
    case Algorithm::LargerThanLife:
        m_computeShader.SetUniformInt2("uniSurvival", m_config.survivalRangeMin, m_config.survivalRangeMax);
        m_computeShader.SetUniformInt2("uniBirth", m_config.birthRangeMin, m_config.birthRangeMax);
        break;
    case Algorithm::Primordia:
        m_computeShader.SetUniformVec2("uniSurvival", m_config.survivalRangeMin, m_config.survivalRangeMax);
        m_computeShader.SetUniformVec2("uniBirth", m_config.birthRangeMin, m_config.birthRangeMax);
        break;
    case Algorithm::Lenia:
        break;
    }
}