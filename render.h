#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct RenderData {
    GLuint VBO;
    GLuint VAO;
    GLuint vertexCount;
};

struct SceneContext {
    GLuint prog;
    GLint u_mvp;
    RenderData renderData;
};

SceneContext setup_scene();
void draw_scene(const SceneContext& context);

#endif // RENDER_H