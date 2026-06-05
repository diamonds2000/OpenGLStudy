#include "render.h"
#include <GL/glew.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>

GLuint create_program()
{
    GLuint prog;

    const char* vshader_src =
        "#version 330 core\n"
        "layout(location=0) in vec3 pos;\n"
        "void main()"
        "{"
        "    gl_Position=vec4(pos,1.0);"
        "}";

    const char* fshader_src =
        "#version 330 core\n"
        "out vec4 color;\n"
        "void main()"
        "{"
        "    color=vec4(1, 0.5, 0.2, 1);"
        "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vshader_src, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fshader_src, nullptr);
    glCompileShader(fs);

    prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}

RenderData create_render_data(float* vertices, int count)
{
    RenderData renderData;
    glGenVertexArrays(1, &renderData.VAO);
    glGenBuffers(1, &renderData.VBO);

    glBindVertexArray(renderData.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderData.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 3, vertices, GL_STATIC_DRAW);
    renderData.vertexCount = count;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return renderData;
}

RenderData create_triangle()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    return create_render_data(vertices, 3);
}

RenderData create_sphere(float radius, int slices, int stacks)
{
    // Each quad on the sphere surface produces 2 triangles = 6 vertices
    float* vertices = new float[slices * stacks * 6 * 3];
    int index = 0;

    for (int i = 0; i < stacks; ++i)
    {
        float V0 = (float)i / stacks;
        float V1 = (float)(i + 1) / stacks;
        float phi0 = V0 * M_PI;
        float phi1 = V1 * M_PI;

        for (int j = 0; j < slices; ++j)
        {
            float U0 = (float)j / slices;
            float U1 = (float)(j + 1) / slices;
            float theta0 = U0 * (M_PI * 2);
            float theta1 = U1 * (M_PI * 2);

            // Four corners of the current quad
            // v0 -- v1   (top row: i)
            //  |     |
            // v2 -- v3   (bottom row: i+1)

            // v0: top-left  (i, j)
            float x0 = radius * cos(theta0) * sin(phi0);
            float y0 = radius * cos(phi0);
            float z0 = radius * sin(theta0) * sin(phi0);

            // v1: top-right (i, j+1)
            float x1 = radius * cos(theta1) * sin(phi0);
            float y1 = radius * cos(phi0);
            float z1 = radius * sin(theta1) * sin(phi0);

            // v2: bottom-left  (i+1, j)
            float x2 = radius * cos(theta0) * sin(phi1);
            float y2 = radius * cos(phi1);
            float z2 = radius * sin(theta0) * sin(phi1);

            // v3: bottom-right (i+1, j+1)
            float x3 = radius * cos(theta1) * sin(phi1);
            float y3 = radius * cos(phi1);
            float z3 = radius * sin(theta1) * sin(phi1);

            // Triangle 1: v0, v1, v3
            vertices[index++] = x0; vertices[index++] = y0; vertices[index++] = z0;
            vertices[index++] = x1; vertices[index++] = y1; vertices[index++] = z1;
            vertices[index++] = x3; vertices[index++] = y3; vertices[index++] = z3;

            // Triangle 2: v0, v3, v2
            vertices[index++] = x0; vertices[index++] = y0; vertices[index++] = z0;
            vertices[index++] = x3; vertices[index++] = y3; vertices[index++] = z3;
            vertices[index++] = x2; vertices[index++] = y2; vertices[index++] = z2;
        }
    }

    RenderData renderData = create_render_data(vertices, slices * stacks * 6);

    delete[] vertices;
    return renderData;
}

RenderData create_cube(float size)
{
    // create vertices for the cube
    float half = size / 2.0f;
    float cubeVertices[36][3] =
    {
        {-half, -half, -half}, {half, -half, -half}, {half, half, -half},
        {half, half, -half}, {-half, half, -half}, {-half, -half, -half},

        {-half, -half, half}, {half, -half, half}, {half, half, half},
        {half, half, half}, {-half, half, half}, {-half, -half, half},

        {-half, -half, -half}, {-half, half, -half}, {-half, half, half},
        {-half, half, half}, {-half, -half, half}, {-half, -half, -half},

        {half, -half, -half}, {half, half, -half}, {half, half, half},
        {half, half, half}, {half, -half, half}, {half, -half, -half},

        {-half, -half, -half}, {0.0f * size, -half, half}, {0.0f * size, half, half},
        {0.0f * size, half, half}, {-half, half, -half}, {-half, -half, -half},

        {half, -half, -half}, {0.0f * size, -half, half}, {0.0f * size, half, half},
        {0.0f * size, half, half}, {half, half, -half}, {half, -half, -half}
    };

    RenderData renderData = create_render_data((float*)&cubeVertices, 36);

    return renderData;
}

SceneContext setup_scene()
{
    SceneContext context;
    context.prog = create_program();
    context.renderData = create_sphere(0.2f, 20, 20);

    std::cout << "sphere = 0.2" << std::endl;
    return context;
}

void draw_scene(const SceneContext& context)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(context.prog);

    glBindVertexArray(context.renderData.VAO);
    glDrawArrays(GL_TRIANGLES, 0, context.renderData.vertexCount);
}