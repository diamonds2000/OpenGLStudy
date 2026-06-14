#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render.h"

struct CameraController {
    float yaw = -45.0f;
    float pitch = -20.0f;
    float distance = 50.0f;
    glm::vec3 target = glm::vec3(0.0f);

    bool isDragging = false;
    bool isPanning = false;
    double lastX = 0.0;
    double lastY = 0.0;
};

struct SceneContext {
    int width = 0;
    int height = 0;
    GLuint prog_main = 0;
    GLuint prog_edge = 0;
    GLuint prog_skybox = 0;
    GLuint fbo = 0;
    GLint u_mvp = 0;
    GLint u_view = 0;
    GLint u_light_dir = 0;
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    RenderData skyboxData;
    std::vector<RenderData> renderDatas;
    CameraController camera;
};

glm::mat4 get_isometric_view_matrix();

SceneContext setup_scene(int width, int height);
void draw_skybox(SceneContext& context);
void draw_scene(SceneContext& context);
void draw_edge(SceneContext& context);
void update_view_from_camera(SceneContext& context);

#endif // SCENE_H