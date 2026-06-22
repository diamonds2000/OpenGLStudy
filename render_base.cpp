#include "render_base.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

RenderBase::RenderBase()
{
}

RenderBase::~RenderBase()
{
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
    }
}
void RenderBase::init()
{
    create_program();
}

void RenderBase::use()
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    glUseProgram(m_program);
}

GLuint RenderBase::create_program()
{
    GLint ok;
    const char* vshader_src = m_vshader_src.c_str();
    const char* fshader_src = m_fshader_src.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vshader_src, nullptr);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        GLint len;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &len);
        char* log = new char[len];
        glGetShaderInfoLog(vs, len, nullptr, log);
        std::cerr << "Vertex shader compilation failed:\n" << log << std::endl;
        delete[] log;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fshader_src, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        GLint len;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &len);
        char* log = new char[len];
        glGetShaderInfoLog(fs, len, nullptr, log);
        std::cerr << "Fragment shader compilation failed:\n" << log << std::endl;
        delete[] log;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &ok);
    if (!ok)    {
        GLint len;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &len);
        char* log = new char[len];
        glGetProgramInfoLog(m_program, len, nullptr, log);
        std::cerr << "Shader program linking failed:\n" << log << std::endl;
        delete[] log;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return m_program;
}

void RenderBase::setUniform(const std::string& name, const glm::int32& value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(loc, value);

}
void RenderBase::setUniform(const std::string& name, const glm::float32& value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(loc, value);
}

void RenderBase::setUniform(const std::string& name, const glm::bvec2 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }

    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2i(loc, static_cast<int>(value.x), static_cast<int>(value.y));
}

void RenderBase::setUniform(const std::string& name, const glm::bvec3 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc == 0)
    {
        std::cerr << "Uniform '" << name.c_str() << "' not found in shader!" << std::endl;
    }

    glUniform3i(loc, static_cast<int>(value.x), static_cast<int>(value.y), static_cast<int>(value.z));
}

void RenderBase::setUniform(const std::string& name, const glm::bvec4 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());

    glUniform4i(loc, static_cast<int>(value.x), static_cast<int>(value.y), static_cast<int>(value.z), static_cast<int>(value.w));
}

void RenderBase::setUniform(const std::string& name, const glm::ivec2 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2i(loc, value.x, value.y);
}

void RenderBase::setUniform(const std::string& name, const glm::ivec3 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3i(loc, value.x, value.y, value.z);
}

void RenderBase::setUniform(const std::string& name, const glm::ivec4 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4i(loc, value.x, value.y, value.z, value.w);
}

void RenderBase::setUniform(const std::string& name, const glm::uvec2 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2ui(loc, value.x, value.y);
}

void RenderBase::setUniform(const std::string& name, const glm::uvec3 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3ui(loc, value.x, value.y, value.z);
}

void RenderBase::setUniform(const std::string& name, const glm::uvec4 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4ui(loc, value.x, value.y, value.z, value.w);
}

void RenderBase::setUniform(const std::string& name, const glm::vec2 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2f(loc, value.x, value.y);
}

void RenderBase::setUniform(const std::string& name, const glm::vec3 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3f(loc, value.x, value.y, value.z);
}

void RenderBase::setUniform(const std::string& name, const glm::vec4 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void RenderBase::setUniform(const std::string& name, const glm::mat2 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void RenderBase::setUniform(const std::string& name, const glm::mat3 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void RenderBase::setUniform(const std::string& name, const glm::mat4 &value)
{
    if (m_program == 0)
    {
        assert(false);
        return;
    }
    GLuint loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
