#pragma once

#include "Config.hpp"
#include "FrameBuffer.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

class LifeProcessor
{
public:
    LifeProcessor(const Config& config);

    void Update();

    const Texture* OutputTexture() const { return m_outputTexture; }
    bool& Processing() { return m_processing; }

private:
    void ConfigureComputeProgram() const;

    const Config m_config;

    ShaderProgram m_computeShader;
    Texture m_textures[2];
    const Texture* m_inputTexture;
    const Texture* m_outputTexture;
    FrameBuffer m_frameBuffer;

    bool m_processing = true;
};