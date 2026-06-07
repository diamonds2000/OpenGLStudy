#include "performance.h"
#include "scene.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

// --- Off-screen FBO helpers ---

struct OffscreenFBO {
    GLuint fbo = 0;
    GLuint colorTex = 0;
    GLuint depthRBO = 0;
    int width = 0;
    int height = 0;
};

OffscreenFBO create_offscreen_fbo(int width, int height)
{
    OffscreenFBO off;

    off.width = width;
    off.height = height;

    // Color texture attachment
    glGenTextures(1, &off.colorTex);
    glBindTexture(GL_TEXTURE_2D, off.colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Depth renderbuffer
    glGenRenderbuffers(1, &off.depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, off.depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Framebuffer
    glGenFramebuffers(1, &off.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, off.fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, off.colorTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, off.depthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return off;
}

void destroy_offscreen_fbo(OffscreenFBO& off)
{
    if (off.fbo)       glDeleteFramebuffers(1, &off.fbo);
    if (off.colorTex)  glDeleteTextures(1, &off.colorTex);
    if (off.depthRBO)  glDeleteRenderbuffers(1, &off.depthRBO);
    off = OffscreenFBO{};
}

// --- Performance tests ---

void performance_test_onscreen(SceneContext& context)
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
    std::cout << "[On-screen] Time taken: " << duration.count() / 1000.0 << " s"
              << "  (avg " << 360.0 / (duration.count() / 1000.0) << " fps)" << std::endl;
}

void performance_test_offscreen(SceneContext& context, int width, int height, int numFrames)
{
    // Create offscreen FBO
    OffscreenFBO off = create_offscreen_fbo(width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, off.fbo);
    glViewport(0, 0, width, height);

    // Update projection to match offscreen size
    glm::mat4 originalProj = context.projection;
    context.projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    // Warm-up frames (let GPU settle)
    for (int i = 0; i < 30; ++i) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw_scene(context);
    }
    // Force finish to ensure warm-up is done
    glFinish();

    // Timed test
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numFrames; ++i)
    {
        context.view = glm::rotate(context.view, float(glm::radians(1.0f)), glm::vec3(1.0f, 1.0f, 0.0f));

        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw_scene(context);
    }
    glFinish(); // Ensure all GPU commands complete before stopping the timer
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    double seconds = duration.count() / 1000.0;
    std::cout << "[Off-screen] " << numFrames << " frames in " << seconds << " s"
              << "  (avg " << numFrames / seconds << " fps)" << std::endl;

    // Restore state
    context.projection = originalProj;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 800, 600);
    destroy_offscreen_fbo(off);
}