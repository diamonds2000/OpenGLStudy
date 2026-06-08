#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

struct RenderData {
    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint vertexCount = 0;
    GLuint instanceCount = 0;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

// Internal: create RenderData from flat vertex/normal arrays (count = number of vertices)
RenderData create_render_data(float* vertices, float* normals, int count);

GLuint create_program(const char* vshader_src, const char* fshader_src);
RenderData create_triangle();
RenderData create_cube(float size);
RenderData create_sphere(float radius, int slices, int stacks);
RenderData create_bunny(const std::string& objPath);

RenderData create_geometry_instances(
    const RenderData& baseData, 
    const std::vector<glm::mat4>& instanceMatrices,
    const std::vector<glm::vec4>& instanceColors);

#endif // RENDER_H