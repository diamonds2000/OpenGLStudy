#include "scene.h"
#include "render.h"
#include "shaders.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

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
    //RenderData baseData = create_cube(0.4f);
    RenderData baseData = create_bunny("../models/bunny.obj");

    glm::mat4 model_scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));

    std::vector<glm::mat4> matrices;
    matrices.reserve(10000);

    std::vector<glm::vec4> colors;
    colors.reserve(10000);

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

    int gridDim = 5; // 22^3 = 10648 ≈ 10000
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
                matrices.push_back(model * model_scale);
                colors.push_back(colorValues[k % 6]); // Cycle through the color values
            }
        }
    }
    return create_geometry_instances(baseData, matrices, colors);
}

glm::mat4 get_isometric_view_matrix()
{
    glm::vec3 eye(12.0f, 10.0f, 50.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    return glm::lookAt(eye, center, up);
}

SceneContext setup_scene()
{
    SceneContext context;
    context.prog = create_program(vshader_src2, fshader_src2);

    //context.view = glm::rotate(viewMatrix, float(glm::radians(30.0f)), glm::vec3(1.0f, 1.0f, 0.0f));
    context.projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    glm::vec3 eye(12.0f, 10.0f, 50.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    context.view = glm::lookAt(eye, center, up);

    //context.u_mvp = glGetUniformLocation(context.prog, "u_mvp");
    context.u_view = glGetUniformLocation(context.prog, "u_view");

    GLuint u_projection = glGetUniformLocation(context.prog, "u_projection");
    GLuint u_color = glGetUniformLocation(context.prog, "u_color");
    GLuint u_lightDir = glGetUniformLocation(context.prog, "u_light_dir");
    GLuint u_lightColor = glGetUniformLocation(context.prog, "u_light_color");

    glUseProgram(context.prog);
    glUniform3f(u_color, 1.0f, 0.5f, 0.2f);
    glUniform3f(u_lightDir, 0.5f, 1.0f, 0.3f);
    glUniform3f(u_lightColor, 1.0f, 1.0f, 1.0f);
    glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(context.projection));

    context.renderDatas.push_back(create_geometries_instances());
    //context.renderDatas = create_geometries();

    glEnable(GL_DEPTH_TEST);

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
    //glUseProgram(context.prog);
    
    //context.view = glm::rotate(context.view, float(glm::radians(1.0f)), glm::vec3(1.0f, 1.0f, 0.0f)); // Rotate over time

    //glUniformMatrix4fv(context.u_mvp, 1, GL_FALSE, glm::value_ptr(context.projection * context.view));
    glUniformMatrix4fv(context.u_view, 1, GL_FALSE, glm::value_ptr(context.view));

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

        glBindVertexArray(renderData.VAO);
        //glDrawArrays(GL_TRIANGLES, 0, renderData.vertexCount);
        glDrawArraysInstanced(GL_TRIANGLES, 0, renderData.vertexCount, renderData.instanceCount);
        glBindVertexArray(0);
    }
}

