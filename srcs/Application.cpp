#include "Application.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <unistd.h> // usleep

namespace
{
const int windowWidth = 800;
const int windowHeight = 600;

// Conway's life
const int survivalRangeMin = 2;
const int survivalRangeMax = 3;
const int birthRangeMin = 3;
const int birthRangeMax = 3;

// Bosco's rule
// const int survivalRangeMin = 33;
// const int survivalRangeMax = 57;
// const int birthRangeMin = 34;
// const int birthRangeMax = 45;

// Primordia
const float survivalRangeMinf = 0.19;
const float survivalRangeMaxf = 0.33;
const float birthRangeMinf = 0.20;
const float birthRangeMaxf = 0.25;

// Proto-Lenia Primordia
// const float survivalRangeMin = 0.12;
// const float survivalRangeMax = 0.15;
// const float birthRangeMin = 0.12;
// const float birthRangeMax = 0.15;

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
{
    if (m_config.startupFileName)
    {
        m_textures[0].FillWithTextureFile(m_config.startupFileName);
    }
    else
    {
        m_textures[0].FillWithRandom();
    }
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

    m_outputTexture->AttachToFrameBuffer(m_frameBuffer);
    m_inputTexture->Bind();
    ConfigureComputeProgram();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_frameBuffer.Unbind();

    m_displayShader.Use();
    m_displayShader.SetUniformVec2("uniResolution", windowWidth, windowHeight);
    m_outputTexture->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_window.Refresh();

    swap(m_inputTexture, m_outputTexture);
}

void Application::ConfigureComputeProgram() const
{
    m_computeShader.Use();
    m_computeShader.SetUniformVec2("uniResolution", m_config.width, m_config.height);
    m_computeShader.SetUniformInt("uniRange", m_config.range);
    switch (m_config.algorithm)
    {
    case Algorithm::LargerThanLife:
        m_computeShader.SetUniformInt2("uniSurvival", survivalRangeMin, survivalRangeMax);
        m_computeShader.SetUniformInt2("uniBirth", birthRangeMin, birthRangeMax);
        break;
    case Algorithm::Primordia:
        m_computeShader.SetUniformVec2("uniSurvival", survivalRangeMinf, survivalRangeMaxf);
        m_computeShader.SetUniformVec2("uniBirth", birthRangeMinf, birthRangeMaxf);
        break;
    case Algorithm::Lenia:
        break;
    }
}