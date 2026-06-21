#include "render.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>



GLuint create_fbo(GLuint width, GLuint height)
{
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint colorTex;
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    GLuint depthRBO;
    glGenTextures(1, &depthRBO);
    glBindTexture(GL_TEXTURE_2D, depthRBO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRBO, 0);

    GLuint drawBuf[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuf);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Fail to create FBO!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}

RenderData create_render_data(float* vertices, int count)
{
    RenderData renderData;
    glGenVertexArrays(1, &renderData.VAO);
    glGenBuffers(1, &renderData.VBO);

    glBindVertexArray(renderData.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderData.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * count, vertices, GL_STATIC_DRAW);
    renderData.vertexCount = count;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  // slot 0: position

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return renderData;
}

RenderData create_render_data(float* vertices, float* normals, float* textCoords, int count)
{
    float* data = new float[count * 8];
    for (int i = 0; i < count; ++i)
    {
        data[i * 8  + 0] = vertices[i * 3 + 0];
        data[i * 8 + 1] = vertices[i * 3 + 1];
        data[i * 8 + 2] = vertices[i * 3 + 2];
        data[i * 8 + 3] = normals[i * 3 + 0];
        data[i * 8 + 4] = normals[i * 3 + 1];
        data[i * 8 + 5] = normals[i * 3 + 2];
        data[i * 8 + 6] = textCoords[i * 2 + 0];
        data[i * 8 + 7] = textCoords[i * 2 + 1];
    }
    
    RenderData renderData;
    glGenVertexArrays(1, &renderData.VAO);
    glGenBuffers(1, &renderData.VBO);

    glBindVertexArray(renderData.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderData.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 8, data, GL_STATIC_DRAW);
    renderData.vertexCount = count;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  // slot 0: position

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  // slot 1: normal

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);  // slot 2: texture coordinates

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete[] data;
    data = nullptr;

    return renderData;
}

RenderData create_geometry_instances(
    const RenderData& baseData, 
    const std::vector<glm::mat4>& instanceMatrices,
    const std::vector<glm::vec4>& instanceColors)
{
    RenderData renderData;

    glBindVertexArray(baseData.VAO);
    {
        GLuint instancePosVBO;
        glGenBuffers(1, &instancePosVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instancePosVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size()*sizeof(glm::mat4), instanceMatrices.data(), GL_STATIC_DRAW);

        GLuint attrLoc = 3; // shader layout start (location=3) mat4 model;
        for(int i = 0; i < 4; i++)
        {
            glVertexAttribPointer(attrLoc+i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i*sizeof(glm::vec4)));
            glEnableVertexAttribArray(attrLoc+i);
            glVertexAttribDivisor(attrLoc+i,1);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLuint instanceColorVBO;
        glGenBuffers(1, &instanceColorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceColors.size()*sizeof(glm::vec4), instanceColors.data(), GL_STATIC_DRAW);

        attrLoc = 7; // shader layout start (location=7) vec4 instanceColor;
        glVertexAttribPointer(attrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glEnableVertexAttribArray(attrLoc);
        glVertexAttribDivisor(attrLoc,1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
    
    renderData.VBO = 0;
    renderData.VAO = baseData.VAO;
    renderData.vertexCount = baseData.vertexCount;
    renderData.instanceCount = (GLuint)instanceMatrices.size();

    return renderData;
}

GLuint create_texture(const char* imagePath)
{
    int width, height, nrChannels;
    // Force 4 channels (RGBA) for consistent OpenGL format
    unsigned char* image = stbi_load(imagePath, &width, &height, &nrChannels, 4);
    if (!image)
    {
        std::cerr << "Failed to load texture image: " << imagePath << std::endl;
        return 0;
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);

    std::cout << "[Texture] Loaded " << imagePath << ": "
              << width << "x" << height << " channels=" << nrChannels << std::endl;
    return textureID;
}