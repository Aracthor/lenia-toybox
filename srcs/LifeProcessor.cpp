#include "LifeProcessor.hpp"

namespace
{
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

LifeProcessor::LifeProcessor(const Config& config)
    : m_config(config)
    , m_elapsedTimeSinceLastUpdate(0.f)
    , m_computeShader("shaders/display_quad.vert", ComputeShaderProgram(m_config.algorithm))
    , m_textures{
        Texture(m_config.width, m_config.height),
        Texture(m_config.width, m_config.height),
    }
    , m_inputTexture(&m_textures[0])
    , m_outputTexture(&m_textures[1])
    , m_processing(!m_config.pause)
{
    if (m_config.startupFileName.empty())
    {
        m_textures[1].FillWithRandom();
    }
    else
    {
        m_textures[1].FillWithTextureFile(m_config.startupFileName.c_str());
    }
}

void LifeProcessor::Update()
{
    m_clock.Update();
    if (m_processing)
    {
        m_elapsedTimeSinceLastUpdate += m_clock.GetElapsedTimeInUs();
        const float frameTimeInUs = 1000000.f / m_config.framerate;
        if (m_elapsedTimeSinceLastUpdate > frameTimeInUs)
        {
            m_profiler.StartProfile();
            swap(m_inputTexture, m_outputTexture);
            m_outputTexture->AttachToFrameBuffer(m_frameBuffer);
            m_inputTexture->Bind();
            ConfigureComputeProgram();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            m_frameBuffer.Unbind();
            m_profiler.StopProfile();

            m_elapsedTimeSinceLastUpdate -= frameTimeInUs;
        }
    }
}

void LifeProcessor::ConfigureComputeProgram() const
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
        m_computeShader.SetUniformFloat("uniDelaTime", 1.f / m_config.timestamp);
        m_computeShader.SetUniformVec2("uniSurvival", m_config.survivalRangeMin, m_config.survivalRangeMax);
        m_computeShader.SetUniformVec2("uniBirth", m_config.birthRangeMin, m_config.birthRangeMax);
        break;
    case Algorithm::Lenia:
        m_computeShader.SetUniformFloat("uniDelaTime", 1.f / m_config.timestamp);
        m_computeShader.SetUniformFloat("uniKernelGaussCenter", m_config.kernelGaussCenter);
        m_computeShader.SetUniformFloat("uniKernelGaussWidth", m_config.kernelGaussWidth);
        m_computeShader.SetUniformFloat("uniGrowthGaussCenter", m_config.growthGaussCenter);
        m_computeShader.SetUniformFloat("uniGrowthGaussWidth", m_config.growthGaussWidth);
        break;
    }
}