#include "render.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLuint create_program(const char* vshader_src, const char* fshader_src)
{
    GLint ok;
    GLuint prog;

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

    prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok)    {
        GLint len;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        char* log = new char[len];
        glGetProgramInfoLog(prog, len, nullptr, log);
        std::cerr << "Shader program linking failed:\n" << log << std::endl;
        delete[] log;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}

RenderData create_render_data(float* vertices, float* normals, int count)
{
    float* data = new float[count * 6];
    for (int i = 0; i < count; ++i)
    {
        data[i * 6 + 0] = vertices[i * 3 + 0];
        data[i * 6 + 1] = vertices[i * 3 + 1];
        data[i * 6 + 2] = vertices[i * 3 + 2];
        data[i * 6 + 3] = normals[i * 3 + 0];
        data[i * 6 + 4] = normals[i * 3 + 1];
        data[i * 6 + 5] = normals[i * 3 + 2];
    }
    
    RenderData renderData;
    glGenVertexArrays(1, &renderData.VAO);
    glGenBuffers(1, &renderData.VBO);

    glBindVertexArray(renderData.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderData.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 6, data, GL_STATIC_DRAW);
    renderData.vertexCount = count;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  // slot 0: position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  // slot 1: normal

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete[] data;
    data = nullptr;

    return renderData;
}

RenderData create_geometry_instances(
    const RenderData& baseData, 
    const std::vector<glm::mat4>& instanceMatrices,
    const std::vector<glm::vec4>& instanceColors)
{
    RenderData renderData;

    glBindVertexArray(baseData.VAO);
    {
        GLuint instancePosVBO;
        glGenBuffers(1, &instancePosVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instancePosVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size()*sizeof(glm::mat4), instanceMatrices.data(), GL_STATIC_DRAW);

        GLuint attrLoc = 2; // shader layout start (location=2) mat4 model;
        for(int i = 0; i < 4; i++)
        {
            glVertexAttribPointer(attrLoc+i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i*sizeof(glm::vec4)));
            glEnableVertexAttribArray(attrLoc+i);
            glVertexAttribDivisor(attrLoc+i,1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLuint instanceColorVBO;
        glGenBuffers(1, &instanceColorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceColors.size()*sizeof(glm::vec4), instanceColors.data(), GL_STATIC_DRAW);

        attrLoc = 6; // shader layout start (location=6) vec4 instanceColor;
        glVertexAttribPointer(attrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(attrLoc);
        glVertexAttribDivisor(attrLoc,1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
    
    renderData.VBO = 0;
    renderData.VAO = baseData.VAO;
    renderData.vertexCount = baseData.vertexCount;
    renderData.instanceCount = (GLuint)instanceMatrices.size();

    return renderData;
}