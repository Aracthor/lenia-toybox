#include "ShaderProgram.hpp"

#include <fstream>
#include <iostream>

namespace
{

GLuint CompileShader(const char* parShaderFileName, GLenum parShaderType)
{
    std::ifstream ifs;

    ifs.open(parShaderFileName, std::ifstream::in | std::ifstream::binary);
    if (ifs.rdstate() & std::ios::failbit)
    {
        std::cerr << "Couldn't open file '" << parShaderFileName << "'." << std::endl;
        std::terminate();
    }

    ifs.seekg(0, ifs.end);
    uint fileSize = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    char* fileContent = new char[fileSize + 1];
    ifs.read(fileContent, fileSize);
    fileContent[fileSize] = '\0';

    GLint compiled;
    GLuint shaderID = glCreateShader(parShaderType);
    glShaderSource(shaderID, 1, &fileContent, NULL);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(shaderID, infoLen, NULL, infoLog);
            std::cerr << "Error compiling shader '" << parShaderFileName << "':" << std::endl
                      << infoLog << std::endl
                      << fileContent << std::endl;
            delete[] infoLog;
        }
        else
        {
            std::cerr << "Unkown error compiling shader '" << parShaderFileName << "':" << std::endl;
        }
        std::terminate();
    }
    delete[] fileContent;

    return shaderID;
}

} // namespace

ShaderProgram::ShaderProgram(const char* parVertexShaderFileName, const char* parFragmentShaderFileName)
{
    m_vertexShaderID = CompileShader(parVertexShaderFileName, GL_VERTEX_SHADER);
    m_fragmentShaderID = CompileShader(parFragmentShaderFileName, GL_FRAGMENT_SHADER);

    m_programID = glCreateProgram();
    glAttachShader(m_programID, m_vertexShaderID);
    glAttachShader(m_programID, m_fragmentShaderID);
    glLinkProgram(m_programID);

    GLint linked;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetProgramInfoLog(m_programID, infoLen, NULL, infoLog);
            std::cerr << "Error linking program:" << std::endl << infoLog << std::endl;
            delete[] infoLog;
        }
        else
        {
            std::cerr << "Unkown error linking program." << std::endl;
        }
        std::terminate();
    }
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_programID);
    glDeleteShader(m_vertexShaderID);
    glDeleteShader(m_fragmentShaderID);
}

void ShaderProgram::Use() const
{
    glUseProgram(m_programID);
}

void ShaderProgram::SetUniformInt(const char* name, int n) const
{
    const GLint uniformLocation = glGetUniformLocation(m_programID, name);
    glUniform1i(uniformLocation, n);
}

void ShaderProgram::SetUniformInt2(const char* name, int x, int y) const
{
    const GLint uniformLocation = glGetUniformLocation(m_programID, name);
    glUniform2i(uniformLocation, x, y);
}

void ShaderProgram::SetUniformVec2(const char* name, float x, float y) const
{
    const GLint uniformLocation = glGetUniformLocation(m_programID, name);
    glUniform2f(uniformLocation, x, y);
}
