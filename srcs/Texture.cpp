#include "Texture.hpp"

#include "FrameBuffer.hpp"

#include <cstdlib> // random
#include <vector>

Texture::Texture(int width, int height)
    : m_width(width)
    , m_height(height)
{
    glGenTextures(1, &m_textureID);
    Bind();
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_width, m_height, 0, GL_RED, GL_FLOAT, nullptr);
    }

    // Set the filtering mode.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    Unbind();
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::AttachToFrameBuffer(const FrameBuffer& frameBuffer) const
{
    frameBuffer.Bind();
    Bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_textureID, 0);
    GLenum buffer = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &buffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::abort();
}

void Texture::FillWithRandom()
{
    std::vector<float> values;
    {
        int pixel_count = m_width * m_height;
        values.reserve(pixel_count);
        while (pixel_count-- > 0)
            values.push_back(static_cast<float>(random() % 100000 + 1) / 100000.f);
    }

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_width, m_height, 0, GL_RED, GL_FLOAT, values.data());
    Unbind();
}
