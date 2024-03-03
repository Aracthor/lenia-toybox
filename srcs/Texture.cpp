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
        const GLenum format = GL_RED;
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, nullptr);
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
    std::vector<GLubyte> bytes;
    {
        int byte_count = m_width * m_height;
        bytes.reserve(byte_count);
        while (byte_count-- > 0)
            bytes.push_back(random() % 0x100);
    }

    Bind();
    const GLenum format = GL_RED;
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, bytes.data());
    Unbind();
}
