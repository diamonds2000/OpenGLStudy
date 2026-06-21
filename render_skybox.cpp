#include "render_skybox.h"

RenderSkybox::RenderSkybox()
{
    const char* vshader_skybox_src = R"(
        #version 330 core
        uniform mat4 u_projection;
        uniform mat4 u_view;
        layout(location=0) in vec3 pos;
        out vec3 vTexCoord;
        void main()
        {
            vTexCoord = pos;
            gl_Position = u_projection * u_view * vec4(pos, 1.0);
            //gl_Position.z = gl_Position.w; // Set z to far plane
        }
    )"; 

    const char* fshader_skybox_src = R"(
            #version 330 core
            in vec3 vTexCoord;
            out vec4 fragColor;
            uniform samplerCube u_skybox;
            void main()
            {
                fragColor = texture(u_skybox, vTexCoord);
            }
    )";

    m_fshader_src = fshader_skybox_src;
    m_vshader_src = vshader_skybox_src;
}

RenderSkybox::~RenderSkybox()
{
}