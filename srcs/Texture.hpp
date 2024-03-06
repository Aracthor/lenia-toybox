#pragma once

#include <GL/glew.h>

class FrameBuffer;

class Texture
{
public:
    Texture(int width, int height);
    ~Texture();

    void Bind() const;
    void Unbind() const;

    void AttachToFrameBuffer(const FrameBuffer& frameBuffer) const;

    void FillWithRandom();
    void FillWithTextureFile(const char* fileName);

private:
    const int m_width;
    const int m_height;
    GLuint m_textureID = 0;
};
