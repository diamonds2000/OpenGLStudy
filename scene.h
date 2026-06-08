#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include "render.h"

struct SceneContext {
    GLuint prog = 0;
    GLint u_mvp = 0;
    GLint u_view = 0;
    GLint u_light_dir = 0;
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    std::vector<RenderData> renderDatas;
};

glm::mat4 get_isometric_view_matrix();

SceneContext setup_scene();
void draw_scene(SceneContext& context);

#endif // SCENE_H