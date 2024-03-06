#pragma once

#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram(const char* parVertexShaderFileName, const char* parFragmentShaderFileName);
    ~ShaderProgram();

    void Use() const;

    void SetUniformInt(const char* name, int n) const;
    void SetUniformInt2(const char* name, int x, int y) const;
    void SetUniformFloat(const char* name, float n) const;
    void SetUniformVec2(const char* name, float x, float y) const;

private:
    GLuint m_vertexShaderID;
    GLuint m_fragmentShaderID;
    GLuint m_programID;
};
