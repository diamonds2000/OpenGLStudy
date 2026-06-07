#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render.h"
#include "scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

void performance_test(SceneContext& context)
{
    GLFWwindow* window = glfwGetCurrentContext();
    auto start = std::chrono::high_resolution_clock::now();
    for (int i =0; i < 360 && !glfwWindowShouldClose(window); ++i)
    {
        context.view = glm::rotate(context.view, float(glm::radians(1.0f)), glm::vec3(1.0f, 1.0f, 0.0f)); 

        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render OpenGL here
        draw_scene(context);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time taken: " << duration.count() / 1000.0 << " s" << std::endl;
}

int main()
{
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
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

    SceneContext context = setup_scene();

    performance_test(context);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render OpenGL here
        draw_scene(context);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}