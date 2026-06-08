#include "render.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model_loader.h"

#define _USE_MATH_DEFINES
#include <math.h>

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

RenderData create_triangle()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    float normals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    return create_render_data(vertices, normals, 3);
}

RenderData create_sphere(float radius, int slices, int stacks)
{
    // Each quad on the sphere surface produces 2 triangles = 6 vertices
    float* vertices = new float[slices * stacks * 6 * 3];
    float* normals = new float[slices * stacks * 6 * 3];
    int index = 0;

    for (int i = 0; i < stacks; ++i)
    {
        float V0 = (float)i / stacks;
        float V1 = (float)(i + 1) / stacks;
        float phi0 = V0 * M_PI;
        float phi1 = V1 * M_PI;

        for (int j = 0; j < slices; ++j)
        {
            float U0 = (float)j / slices;
            float U1 = (float)(j + 1) / slices;
            float theta0 = U0 * (M_PI * 2);
            float theta1 = U1 * (M_PI * 2);

            // Four corners of the current quad
            // v0 -- v1   (top row: i)
            //  |     |
            // v2 -- v3   (bottom row: i+1)

            // v0: top-left  (i, j)
            float x0 = radius * cos(theta0) * sin(phi0);
            float y0 = radius * cos(phi0);
            float z0 = radius * sin(theta0) * sin(phi0);

            // v1: top-right (i, j+1)
            float x1 = radius * cos(theta1) * sin(phi0);
            float y1 = radius * cos(phi0);
            float z1 = radius * sin(theta1) * sin(phi0);

            // v2: bottom-left  (i+1, j)
            float x2 = radius * cos(theta0) * sin(phi1);
            float y2 = radius * cos(phi1);
            float z2 = radius * sin(theta0) * sin(phi1);

            // v3: bottom-right (i+1, j+1)
            float x3 = radius * cos(theta1) * sin(phi1);
            float y3 = radius * cos(phi1);
            float z3 = radius * sin(theta1) * sin(phi1);

            // For a sphere centered at origin, the normal at each vertex
            // is just the normalized position vector (vertex / radius)
            float nx0 = x0 / radius, ny0 = y0 / radius, nz0 = z0 / radius;
            float nx1 = x1 / radius, ny1 = y1 / radius, nz1 = z1 / radius;
            float nx2 = x2 / radius, ny2 = y2 / radius, nz2 = z2 / radius;
            float nx3 = x3 / radius, ny3 = y3 / radius, nz3 = z3 / radius;

            // Triangle 1: v0, v1, v3
            vertices[index] = x0; normals[index] = nx0; index++;
            vertices[index] = y0; normals[index] = ny0; index++;
            vertices[index] = z0; normals[index] = nz0; index++;
            vertices[index] = x1; normals[index] = nx1; index++;
            vertices[index] = y1; normals[index] = ny1; index++;
            vertices[index] = z1; normals[index] = nz1; index++;
            vertices[index] = x3; normals[index] = nx3; index++;
            vertices[index] = y3; normals[index] = ny3; index++;
            vertices[index] = z3; normals[index] = nz3; index++;

            // Triangle 2: v0, v3, v2
            vertices[index] = x0; normals[index] = nx0; index++;
            vertices[index] = y0; normals[index] = ny0; index++;
            vertices[index] = z0; normals[index] = nz0; index++;
            vertices[index] = x3; normals[index] = nx3; index++;
            vertices[index] = y3; normals[index] = ny3; index++;
            vertices[index] = z3; normals[index] = nz3; index++;
            vertices[index] = x2; normals[index] = nx2; index++;
            vertices[index] = y2; normals[index] = ny2; index++;
            vertices[index] = z2; normals[index] = nz2; index++;
        }
    }

    RenderData renderData = create_render_data(vertices, normals, slices * stacks * 6);

    delete[] vertices;
    delete[] normals;
    return renderData;
}

RenderData create_cube(float size)
{
    // create vertices for the cube
    float half = size / 2.0f;
    float cubeVertices[36][3] =
    {
        {-half, -half, -half}, {half, -half, -half}, {half, half, -half},
        {half, half, -half}, {-half, half, -half}, {-half, -half, -half},

        {-half, -half, half}, {half, -half, half}, {half, half, half},
        {half, half, half}, {-half, half, half}, {-half, -half, half},

        {-half, -half, -half}, {-half, half, -half}, {-half, half, half},
        {-half, half, half}, {-half, -half, half}, {-half, -half, -half},

        {half, -half, -half}, {half, half, -half}, {half, half, half},
        {half, half, half}, {half, -half, half}, {half, -half, -half},

        {-half, -half, -half}, {half, -half, -half}, {half, -half, half},
        {half, -half, half}, {-half, -half, half}, {-half, -half, -half},

        {-half, half, -half}, {half, half, -half}, {half, half, half},
        {half, half, half}, {-half, half, half}, {-half, half, -half}
    };

    float cubeNormals[36][3] =
    {
        // front face (z = -half)
        { 0, 0, -1}, { 0, 0, -1}, { 0, 0, -1},
        { 0, 0, -1}, { 0, 0, -1}, { 0, 0, -1},
        // back face (z = half)
        { 0, 0,  1}, { 0, 0,  1}, { 0, 0,  1},
        { 0, 0,  1}, { 0, 0,  1}, { 0, 0,  1},
        // left face (x = -half)
        {-1, 0,  0}, {-1, 0,  0}, {-1, 0,  0},
        {-1, 0,  0}, {-1, 0,  0}, {-1, 0,  0},
        // right face (x = half)
        { 1, 0,  0}, { 1, 0,  0}, { 1, 0,  0},
        { 1, 0,  0}, { 1, 0,  0}, { 1, 0,  0},
        // bottom face (y = -half)
        { 0, -1, 0}, { 0, -1, 0}, { 0, -1, 0},
        { 0, -1, 0}, { 0, -1, 0}, { 0, -1, 0},
        // top face (y = half)
        { 0,  1, 0}, { 0,  1, 0}, { 0,  1, 0},
        { 0,  1, 0}, { 0,  1, 0}, { 0,  1, 0}
    };

    RenderData renderData = create_render_data((float*)&cubeVertices, (float*)&cubeNormals, 36);

    return renderData;
}

// ---------------------------------------------------------------------------
// Convenience: Stanford Bunny
// ---------------------------------------------------------------------------
RenderData create_bunny(const std::string& objPath)
{
    return create_from_obj(objPath);
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
    renderData.instanceCount = instanceMatrices.size();

    return renderData;
}