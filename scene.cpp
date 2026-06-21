#include "scene.h"
#include "render.h"
#include "shaders.h"
#include "geometry.h"
#include "render_base.h"
#include "render_mirror.h"
#include "render_edge.h"
#include "render_skybox.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>


enum color
{
    RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA
};

const glm::vec4 colorValues[] = {
    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // RED
    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // GREEN
    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // BLUE
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // YELLOW
    glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), // CYAN
    glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)  // MAGENTA
};

std::vector<RenderData> create_geometries()
{
    std::vector<RenderData> renderDatas;
    renderDatas.reserve(10000);
    int gridDim = 22; // 22^3 = 10648 ≈ 10000
    float spacing = 0.8f;
    float offset = (gridDim - 1) * spacing * 0.5f;

    RenderData baseRenderData = create_cube(0.4f);

    for (int i = 0; i < gridDim; ++i)
    {
        for (int j = 0; j < gridDim; ++j)
        {
            for (int k = 0; k < gridDim; ++k)
            {
                glm::vec3 pos(i * spacing - offset, j * spacing - offset, k * spacing - offset);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);

                RenderData renderData = baseRenderData;
                renderData.modelMatrix = model;
                renderDatas.push_back(renderData);
            }
        }
    }
    
    return renderDatas;
}

RenderData create_geometries_instances()
{
    RenderData baseData = create_cube(0.4f);
    //RenderData baseData = create_bunny("../models/bunny.obj");
    //glm::mat4 model_scale = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f));

    std::vector<glm::mat4> matrices;
    matrices.reserve(10000);

    std::vector<glm::vec4> colors;
    colors.reserve(10000);

    int gridDim = 22; // 22^3 = 10648 ≈ 10000
    float spacing = 0.8f;
    float offset = (gridDim - 1) * spacing * 0.5f;

    for (int i = 0; i < gridDim; ++i)
    {
        for (int j = 0; j < gridDim; ++j)
        {
            for (int k = 0; k < gridDim; ++k)
            {
                glm::vec3 pos(i * spacing - offset, j * spacing - offset, k * spacing - offset);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
                matrices.push_back(model);
                colors.push_back(colorValues[k % 6]); // Cycle through the color values
            }
        }
    }
    RenderData renderData = create_geometry_instances(baseData, matrices, colors);
    renderData.textureID = create_texture("../textures/uv_grid.png");
    return renderData;
}

RenderData create_sky_box()
{
    std::vector<std::string> skybox_faces = {
        "../skybox/right.jpg",
        "../skybox/left.jpg",
        "../skybox/top.jpg",
        "../skybox/bottom.jpg",
        "../skybox/front.jpg",
        "../skybox/back.jpg"
    };

    GLuint sky_texture;
    glGenTextures(1, &sky_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky_texture);

    for (int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(skybox_faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Failed to load skybox texture: " << skybox_faces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    RenderData skyboxData = create_skybox();
    skyboxData.textureID = sky_texture;
    return skyboxData;
}

glm::mat4 get_isometric_view_matrix()
{
    glm::vec3 eye(12.0f, 10.0f, 50.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    return glm::lookAt(eye, center, up);
}

SceneContext setup_scene(int width, int height)
{
    SceneContext context;
    context.width = width;
    context.height = height;
    context.scene_render = std::make_unique<RenderMirror>();
    context.edge_render = std::make_unique<RenderEdge>();
    context.skybox_render = std::make_unique<RenderSkybox>();
    context.fbo = create_fbo(width, height);

    context.scene_render->init();
    context.edge_render->init();
    context.skybox_render->init();

    //context.view = glm::rotate(viewMatrix, float(glm::radians(30.0f)), glm::vec3(1.0f, 1.0f, 0.0f));
    context.projection = glm::perspective(glm::radians(45.0f), float(width)/float(height), 0.1f, 100.0f);

    //glm::vec3 eye(12.0f, 10.0f, 50.0f);
    glm::vec3 eye(100.0f, 0.0f, 0.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    context.view = glm::lookAt(eye, center, up);

    context.skyboxData = create_sky_box();
    context.renderDatas.push_back(create_geometries_instances());
    //context.renderDatas = create_geometries();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    return context;
}

void update_view_from_camera(SceneContext& context)
{
    auto& cam = context.camera;
    float yawRad = glm::radians(cam.yaw);
    float pitchRad = glm::radians(cam.pitch);

    glm::vec3 eye;
    eye.x = cam.target.x + cam.distance * cos(pitchRad) * sin(yawRad);
    eye.y = cam.target.y + cam.distance * sin(pitchRad);
    eye.z = cam.target.z + cam.distance * cos(pitchRad) * cos(yawRad);

    context.view = glm::lookAt(eye, cam.target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void draw_scene(SceneContext& context)
{
    context.scene_render->use();
    glBindFramebuffer(GL_FRAMEBUFFER, context.fbo);

    // Only clear depth — color buffer already contains the skybox
    glClear(GL_DEPTH_BUFFER_BIT);

    //context.view = glm::rotate(context.view, float(glm::radians(1.0f)), glm::vec3(1.0f, 1.0f, 0.0f)); // Rotate over time
    context.scene_render->setUniform("u_mvp", context.projection * context.view);
    context.scene_render->setUniform("u_projection", context.projection);
    context.scene_render->setUniform("u_view", context.view);
    context.scene_render->setUniform("u_color", glm::vec3(1.0f, 0.5f, 0.2f));
    context.scene_render->setUniform("u_light_dir", glm::vec3(0.5f, 1.0f, 0.3f));
    context.scene_render->setUniform("u_light_color", glm::vec3(1.0f, 1.0f, 1.0f));
    context.scene_render->setUniform("u_texture", 0);  // sampler -> texture unit 0

    for (const auto& renderData : context.renderDatas)
    {
        // GLuint u_color = glGetUniformLocation(context.prog, "u_color");
        // if (u_color == -1)        {
        //     std::cerr << "Uniform 'color' not found in shader!" << std::endl;
        //     continue;
        // }
        // glUniform3f(u_color, 1.0f, 0.5f, 0.2f);
        
        // GLuint u_model = glGetUniformLocation(context.prog, "u_model");
        // if (u_model == -1)
        // {
        //     std::cerr << "Uniform 'model' not found in shader!" << std::endl;
        //     continue;
        // }
        // glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(renderData.modelMatrix));

        glBindTexture(GL_TEXTURE_2D, renderData.textureID);
        glBindVertexArray(renderData.VAO);
        //glDrawArrays(GL_TRIANGLES, 0, renderData.vertexCount);
        glDrawArraysInstanced(GL_TRIANGLES, 0, renderData.vertexCount, renderData.instanceCount);
        glBindVertexArray(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_scene_mirror(SceneContext& context)
{
    context.scene_render->use();
    glBindFramebuffer(GL_FRAMEBUFFER, context.fbo);

    // Only clear depth — color buffer already contains the skybox
    glClear(GL_DEPTH_BUFFER_BIT);

    context.scene_render->setUniform("u_projection", context.projection);
    context.scene_render->setUniform("u_view", context.view);

    // Bind cubemap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, context.skyboxData.textureID);
    context.skybox_render->setUniform("u_texture", 0);

    for (const auto& renderData : context.renderDatas)
    {
        glBindTexture(GL_TEXTURE_2D, renderData.textureID);
        glBindVertexArray(renderData.VAO);
        //glDrawArrays(GL_TRIANGLES, 0, renderData.vertexCount);
        glDrawArraysInstanced(GL_TRIANGLES, 0, renderData.vertexCount, renderData.instanceCount);
        glBindVertexArray(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_edge(SceneContext& context)
{
    // Create dummy VAO once and reuse it
    static GLuint dummyVAO = 0;
    if (dummyVAO == 0)
        glGenVertexArrays(1, &dummyVAO);

    //glClear(GL_COLOR_BUFFER_BIT);

    context.edge_render->use();
    context.edge_render->setUniform("u_ColorTex", 0);
    context.edge_render->setUniform("u_DepthTex", 1);
    context.edge_render->setUniform("u_TexelSize", glm::vec2(1.0f / context.width, 1.0f / context.height));
    context.edge_render->setUniform("u_EdgeThreshold", 0.01f);
    context.edge_render->setUniform("u_OutlineColor", colorValues[GREEN]);

    // Disable depth test for full-screen post-processing
    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, context.fbo);

    GLint colorTexture;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &colorTexture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);

    GLint depthTexture;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glBindVertexArray(dummyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Re-enable depth test for next frame's scene rendering
    glEnable(GL_DEPTH_TEST);
}

void copy_fbo_to_screen(SceneContext& context)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, context.fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer
    glBlitFramebuffer(0, 0, context.width, context.height, 0, 0, context.width, context.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_skybox(SceneContext& context)
{
    // Clear the FBO color and depth before rendering skybox (first pass into FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, context.fbo);

    glViewport(0, 0, context.width, context.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE); // Disable depth writing for skybox

    context.skybox_render->use();
    context.skybox_render->setUniform("u_projection", context.projection);

    // Remove translation from view matrix for skybox (skybox should not move with camera position)
    glm::mat4 skyboxView = glm::mat4(glm::mat3(context.view));
    context.skybox_render->setUniform("u_view", skyboxView);

    // Bind cubemap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, context.skyboxData.textureID);
    context.skybox_render->setUniform("u_skybox", 0);

    glBindVertexArray(context.skyboxData.VAO);
    glDrawArrays(GL_TRIANGLES, 0, context.skyboxData.vertexCount);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE); // Re-enable depth writing for next frame
    glEnable(GL_CULL_FACE); // Re-enable culling for scene objects
}