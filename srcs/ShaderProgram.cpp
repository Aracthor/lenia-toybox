#include "ShaderProgram.hpp"

#include <fstream>
#include <sstream>

namespace
{

GLuint CompileShader(const char* parShaderFileName, GLenum parShaderType)
{
    std::ifstream ifs;

    ifs.open(parShaderFileName, std::ifstream::in | std::ifstream::binary);
    if (ifs.rdstate() & std::ios::failbit)
        throw std::runtime_error(std::string("Couldn't open file '") + parShaderFileName + "'.");

    ifs.seekg(0, ifs.end);
    int fileSize = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    char* fileContent = new char[fileSize + 1];
    ifs.read(fileContent, fileSize);
    fileContent[fileSize] = '\0';

    GLint compiled;
    GLuint shaderID = glCreateShader(parShaderType);
    glShaderSource(shaderID, 1, &fileContent, NULL);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
    delete[] fileContent;
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(shaderID, infoLen, NULL, infoLog);
            std::ostringstream oss;
            oss << "Error compiling shader '" << parShaderFileName << "':" << std::endl
                << infoLog << std::endl
                << fileContent;
            delete[] infoLog;
            throw std::runtime_error(oss.str());
        }
        throw std::runtime_error(std::string("Unkown error compiling shader '") + parShaderFileName + "':");
    }

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
            std::ostringstream oss;
            oss << "Error linking program:" << std::endl << infoLog;
            delete[] infoLog;
            throw std::runtime_error(oss.str());
        }
        throw std::runtime_error("Unkown error linking program.");
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

void ShaderProgram::SetUniformFloat(const char* name, float n) const
{
    const GLint uniformLocation = glGetUniformLocation(m_programID, name);
    glUniform1f(uniformLocation, n);
}

void ShaderProgram::SetUniformVec2(const char* name, float x, float y) const
{
    const GLint uniformLocation = glGetUniformLocation(m_programID, name);
    glUniform2f(uniformLocation, x, y);
}
