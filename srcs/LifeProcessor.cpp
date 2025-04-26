#include "LifeProcessor.hpp"

#include <cstdarg>
#include <cstdio>
#include <stdexcept>

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

std::string stringprintf(const char* format, ...)
{
    char buffer[0x100];
    constexpr size_t bufferSize = std::size(buffer);
    va_list ap;
    va_start(ap, format);
    const size_t size = vsnprintf(buffer, std::size(buffer), format, ap);
    if (size >= bufferSize)
        throw std::runtime_error(std::string("String doesn't fit in buffer: ") + format);
    va_end(ap);
    return buffer;
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
        m_computeShader.SetUniformInt("uniKernelCount", m_config.kernelCount);
        for (int k = 0; k < m_config.kernelCount; k++)
        {
            m_computeShader.SetUniformFloat(stringprintf("uniKernels[%d].growthGaussCenter", k).c_str(),
                                            m_config.kernels[k].growthGaussCenter);
            m_computeShader.SetUniformFloat(stringprintf("uniKernels[%d].growthGaussWidth", k).c_str(),
                                            m_config.kernels[k].growthGaussWidth);
            int ringCount = 0;
            for (int r = 0; r < (int)m_config.kernels[k].ringWeights.size(); r++)
            {
                const std::optional<float> ringWeight = m_config.kernels[k].ringWeights[r];
                if (ringWeight)
                {
                    m_computeShader.SetUniformFloat(
                        stringprintf("uniKernels[%d].ringWeights[%d]", k, ringCount).c_str(), *ringWeight);
                    ringCount++;
                }
            }
            m_computeShader.SetUniformInt(stringprintf("uniKernels[%d].ringCount", k).c_str(), ringCount);
        }
        break;
    }
}