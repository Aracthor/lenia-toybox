#include "Texture.hpp"

#include "FrameBuffer.hpp"

#include <SDL2/SDL_surface.h>

#include <cstdlib> // random
#include <iostream>
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);
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

void Texture::FillWithTextureFile(const char* fileName)
{
    SDL_Surface* surface = SDL_LoadBMP(fileName);
    if (surface == nullptr)
    {
        std::cerr << "Cannot load texture file '" << fileName << "': " << SDL_GetError() << std::endl;
        std::abort();
    }

    SDL_PixelFormat* pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    SDL_Surface* convertedSurface = SDL_ConvertSurface(surface, pixelFormat, 0);
    SDL_FreeSurface(surface);
    SDL_FreeFormat(pixelFormat);
    if (convertedSurface == nullptr)
    {
        std::cerr << "Error converting surface: " << SDL_GetError() << std::endl;
        std::abort();
    }
    const unsigned char* fileBytes = reinterpret_cast<const unsigned char*>(convertedSurface->pixels);

    std::vector<float> values;
    values.reserve(m_width * m_height);
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            const int yOnFile = y - m_height / 2 + convertedSurface->h / 2;
            const int xOnFile = x - m_width / 2 + convertedSurface->w / 2;
            if (yOnFile > 0 && yOnFile < convertedSurface->h && xOnFile > 0 && xOnFile < convertedSurface->w)
            {
                const int pixelIndex = (yOnFile * convertedSurface->w + xOnFile) * 4;
                values.push_back(static_cast<float>(fileBytes[pixelIndex]) / 0x100);
            }
            else
            {
                values.push_back(0.f);
            }
        }
    }

    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_width, m_height, 0, GL_RED, GL_FLOAT, values.data());
    Unbind();
}
