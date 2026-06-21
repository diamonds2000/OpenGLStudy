#include "render_mirror.h"

RenderMirror::RenderMirror()
{
    const char* vshader_mirror_src =
        "#version 330 core\n"
        "uniform mat4 u_projection;"
        "uniform mat4 u_view;"
        "out vec3 geom_pos;"
        "out vec3 geom_normal;"
        "out vec3 view_pos;"
        "out vec3 v_color;"
        "layout(location=0) in vec3 pos;"
        "layout(location=1) in vec3 normal;"
        "layout(location=2) in vec2 textureCoord;"
        "layout(location=3) in mat4 instance_model;"
        "layout(location=7) in vec3 color;"
        "void main()"
        "{"
        "    geom_pos = (instance_model * vec4(pos, 1.0)).xyz;"
        "    geom_normal = (instance_model * vec4(normal, 0.0)).xyz;"
        "    view_pos = -(u_view * vec4(0, 0, 0, 1.0)).xyz;"
        "    gl_Position = u_projection * u_view * vec4(geom_pos, 1.0);"
        "    v_color = color;"
        "}";

    const char* fshader_mirror_src =
        "#version 330 core\n"
        "uniform samplerCube u_texture;"
        "in vec3 geom_pos;"
        "in vec3 geom_normal;"
        "in vec3 view_pos;"
        "in vec3 v_color;"
        "out vec4 color;"
        "const float shine = 0.5;"
        "void main()"
        "{"
        "    vec3 cube_dir = reflect(geom_pos - view_pos, geom_normal);"
        "    color = vec4(texture(u_texture, cube_dir).rgb, 1.0);"
        "}";

    m_fshader_src = fshader_mirror_src;
    m_vshader_src = vshader_mirror_src;
}

RenderMirror::~RenderMirror()
{
}