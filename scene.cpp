#include "scene.h"
#include "render.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <cmath>


RenderData create_geometries_instances()
{
    RenderData baseData = create_cube(0.4f);

    std::vector<glm::mat4> matrices;
    for (int i = 0; i < 10; ++i)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 0.5f, 0.0f, 0.0f));
        matrices.push_back(model);
    }
    return create_geometry_instances(baseData, matrices);
}

SceneContext setup_scene()
{
    SceneContext context;
    context.prog = create_program();
    context.u_mvp = glGetUniformLocation(context.prog, "u_mvp");
    context.u_view = glGetUniformLocation(context.prog, "u_view");

    GLuint u_color = glGetUniformLocation(context.prog, "u_color");
    GLuint u_lightDir = glGetUniformLocation(context.prog, "u_light_dir");
    GLuint u_lightColor = glGetUniformLocation(context.prog, "u_light_color");

    glUseProgram(context.prog);
    glUniform3f(u_color, 1.0f, 0.5f, 0.2f);
    glUniform3f(u_lightDir, 0.5f, 1.0f, 0.3f);
    glUniform3f(u_lightColor, 1.0f, 1.0f, 1.0f);

    context.renderData = create_geometries_instances();

    glEnable(GL_DEPTH_TEST);

    //context.view = glm::rotate(viewMatrix, float(glm::radians(30.0f)), glm::vec3(1.0f, 1.0f, 0.0f));
    context.projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    glm::vec3 eye(0.0f, 0.0f, 5.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    context.view = glm::lookAt(eye, center, up);

    return context;
}

void draw_scene(SceneContext& context)
{
    //glUseProgram(context.prog);
    
    context.view = glm::rotate(context.view, float(glm::radians(1.0f)), glm::vec3(1.0f, 1.0f, 0.0f)); // Rotate over time

    glUniformMatrix4fv(context.u_mvp, 1, GL_FALSE, glm::value_ptr(context.projection * context.view));
    glUniformMatrix4fv(context.u_view, 1, GL_FALSE, glm::value_ptr(context.view));

    glBindVertexArray(context.renderData.VAO);
    //glDrawArrays(GL_TRIANGLES, 0, context.renderData.vertexCount);
    glDrawArraysInstanced(GL_TRIANGLES, 0, context.renderData.vertexCount, context.renderData.instanceCount);
    glBindVertexArray(0);
}

