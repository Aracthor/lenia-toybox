#pragma once

#include <GL/gl.h>

class Texture
{
public:
    Texture(int width, int height);
    ~Texture();

    void Bind() const;
    void Unbind() const;

    void FillWithRandom();

private:
    const int m_width;
    const int m_height;
    GLuint m_textureID = 0;
};
