#pragma once

#include <GL/glew.h>

class FrameBuffer
{
public:
    FrameBuffer();
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

private:
    GLuint m_buffer = 0;
};
