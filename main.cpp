#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render.h"
#include "scene.h"
#include "performance.h"
#include <iostream>

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    SceneContext* context = static_cast<SceneContext*>(glfwGetWindowUserPointer(window));
    if (!context) return;

    auto& cam = context->camera;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        cam.isDragging = true;
        cam.isPanning = false;
        glfwGetCursorPos(window, &cam.lastX, &cam.lastY);
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        cam.isPanning = true;
        cam.isDragging = false;
        glfwGetCursorPos(window, &cam.lastX, &cam.lastY);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        cam.isPanning = true;
        cam.isDragging = false;
        glfwGetCursorPos(window, &cam.lastX, &cam.lastY);
    }
    else if (action == GLFW_RELEASE)
    {
        cam.isDragging = false;
        cam.isPanning = false;
    }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    SceneContext* context = static_cast<SceneContext*>(glfwGetWindowUserPointer(window));
    if (!context) return;

    auto& cam = context->camera;

    double dx = xpos - cam.lastX;
    double dy = ypos - cam.lastY;
    cam.lastX = xpos;
    cam.lastY = ypos;

    if (cam.isDragging)
    {
        // Orbit
        cam.yaw   += static_cast<float>(dx) * 0.3f;
        cam.pitch += static_cast<float>(dy) * 0.3f;
    }
    else if (cam.isPanning)
    {
        // Pan: move target in screen-space
        float sensitivity = cam.distance * 0.002f;
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 forward = glm::normalize(context->view[2]);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        up = glm::normalize(glm::cross(right, forward));

        cam.target += right * static_cast<float>(-dx) * sensitivity
                    + up   * static_cast<float>(dy)  * sensitivity;
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    SceneContext* context = static_cast<SceneContext*>(glfwGetWindowUserPointer(window));
    if (!context) return;

    context->camera.distance -= static_cast<float>(yoffset) * 2.0f;
    context->camera.distance = glm::max(context->camera.distance, 0.1f);
}

int main()
{
    if (!glfwInit()) {
        return -1;
    }

    int width = 800;
    int height = 600;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "glew init err:" << glewGetErrorString(err) << std::endl;
        return -1;
    }

    SceneContext context = setup_scene(width, height);

    // Off-screen performance test (pure GPU, no vsync)
    //performance_test_offscreen(context, width, height, 1000);

    // Set up camera controls
    glfwSetWindowUserPointer(window, &context);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    update_view_from_camera(context);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);

        // Update view from camera controls
        update_view_from_camera(context);

        // Render OpenGL here
        draw_skybox(context);
        draw_scene(context);
        draw_edge(context);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}