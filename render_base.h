#ifndef RENDER_BASE_H_
#define RENDER_BASE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class RenderBase
{
public:
    RenderBase();
    virtual ~RenderBase();
    
    virtual void init();
    //virtual void render();

    virtual void use();

    GLuint create_program();

    void setUniform(const std::string& name, const glm::int32& value);
    void setUniform(const std::string& name, const glm::float32& value);
    void setUniform(const std::string& name, const glm::vec2& value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::vec4& value);
    void setUniform(const std::string& name, const glm::mat4& value);
    void setUniform(const std::string& name, const glm::mat3& value);
    void setUniform(const std::string& name, const glm::mat2& value);
    void setUniform(const std::string& name, const glm::ivec2& value);
    void setUniform(const std::string& name, const glm::ivec3& value);
    void setUniform(const std::string& name, const glm::ivec4& value);
    void setUniform(const std::string& name, const glm::uvec2& value);
    void setUniform(const std::string& name, const glm::uvec3& value);
    void setUniform(const std::string& name, const glm::uvec4& value);
    void setUniform(const std::string& name, const glm::bvec2& value);
    void setUniform(const std::string& name, const glm::bvec3& value);
    void setUniform(const std::string& name, const glm::bvec4& value);
    void setUniform(const std::string& name, const glm::dvec2& value);
    void setUniform(const std::string& name, const glm::dvec3& value);
    void setUniform(const std::string& name, const glm::dvec4& value);

protected:
    GLuint m_program = 0;

    std::string m_vshader_src;
    std::string m_fshader_src;
};

#endif