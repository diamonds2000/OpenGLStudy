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
};

GLuint create_program();
RenderData create_triangle();
RenderData create_cube(float size);
RenderData create_sphere(float radius, int slices, int stacks);

RenderData create_geometry_instances(const RenderData& baseData, const std::vector<glm::mat4>& instanceMatrices);

#endif // RENDER_H