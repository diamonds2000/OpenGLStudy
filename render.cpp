#include "render.h"
#include <glew.h>

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
    float* vertices = new float[slices * stacks * 3];
    int index = 0;

    // create vertices for the sphere
    for (int i = 0; i <= stacks; ++i)
    {
        float V = (float)i / stacks;
        float phi = V * M_PI;

        for (int j = 0; j <= slices; ++j)
        {
            float U = (float)j / slices;
            float theta = U * (M_PI * 2);

            float x = radius * cos(theta) * sin(phi);
            float y = radius * cos(phi);
            float z = radius * sin(theta) * sin(phi);

            vertices[index++] = x;
            vertices[index++] = y;
            vertices[index++] = z;
        }
    }

    RenderData renderData = create_render_data(vertices, slices * stacks);

    delete[] vertices;
    return renderData;
}

RenderData create_cube(float size)
{
    float* vertices = new float[36 * 3]; // 6 faces * 2 triangles * 3 vertices
    int index = 0;

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

    RenderData renderData = create_render_data(vertices, 36);

    delete[] vertices;
    return renderData;
}

SceneContext setup_scene()
{
    SceneContext context;
    context.prog = create_program();
    context.renderData = create_triangle();

    std::cout << "cube = 0.4" << std::endl;
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