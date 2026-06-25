#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct RenderData {
    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint vertexCount = 0;
    GLuint instanceCount = 0;
    GLuint textureID = 0;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

GLuint create_fbo(GLuint width, GLuint height);

RenderData create_render_data(float* vertices, int count);
// Internal: create RenderData from flat vertex/normal arrays (count = number of vertices)
RenderData create_render_data(float* vertices, float* normals, float* textCoords, int count, bool hasTangent);

GLuint create_texture(const char* imagePath);

RenderData create_geometry_instances(
    const RenderData& baseData, 
    const std::vector<glm::mat4>& instanceMatrices,
    const std::vector<glm::vec4>& instanceColors);


#endif // RENDER_H