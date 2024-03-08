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
    , m_computeShader("shaders/display_texture.vert", ComputeShaderProgram(m_config.algorithm))
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

void LifeProcessor::Update()
{
    if (m_processing)
    {
        swap(m_inputTexture, m_outputTexture);
        m_outputTexture->AttachToFrameBuffer(m_frameBuffer);
        m_inputTexture->Bind();
        ConfigureComputeProgram();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_frameBuffer.Unbind();
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
        break;
    }
}