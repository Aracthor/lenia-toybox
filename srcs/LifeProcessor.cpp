#include "LifeProcessor.hpp"

#include <sstream>

#include "Profiler.hpp"

namespace
{
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

    if (m_config.profile)
        m_profiler = std::make_unique<Profiler>();
}

LifeProcessor::~LifeProcessor() = default;

void LifeProcessor::Update()
{
    m_clock.Update();
    if (m_processing)
    {
        m_elapsedTimeSinceLastUpdate += m_clock.GetElapsedTimeInUs();
        const float frameTimeInUs = 1000000.f / m_config.framerate;
        if (m_elapsedTimeSinceLastUpdate > frameTimeInUs)
        {
            if (m_profiler)
                m_profiler->StartProfile();
            std::swap(m_inputTexture, m_outputTexture);
            m_outputTexture->AttachToFrameBuffer(m_frameBuffer);
            m_inputTexture->Bind();
            ConfigureComputeProgram();
            glViewport(0, 0, m_config.width, m_config.height);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            m_frameBuffer.Unbind();
            if (m_profiler)
                m_profiler->StopProfile();

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
        m_computeShader.SetUniformFloat("uniKernel.growthGaussCenter", m_config.kernel.growthGaussCenter);
        m_computeShader.SetUniformFloat("uniKernel.growthGaussWidth", m_config.kernel.growthGaussWidth);
        m_computeShader.SetUniformInt("uniKernel.ringCount", m_config.kernel.ringWeights.size());
        for (int i = 0; i < (int)m_config.kernel.ringWeights.size(); i++)
        {
            std::ostringstream uniformNameOss;
            uniformNameOss << "uniKernel.ringWeights[" << i << "]";
            m_computeShader.SetUniformFloat(uniformNameOss.str().c_str(), m_config.kernel.ringWeights[i]);
        }
        break;
    }
}