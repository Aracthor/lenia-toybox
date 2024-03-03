#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &m_buffer);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_buffer);
}

void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
}

void FrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
