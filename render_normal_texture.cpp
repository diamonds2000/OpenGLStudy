#include "render_normal_texture.h"

RenderNormalTexture::RenderNormalTexture()
{
    const char* vshader = R"(
        #version 330 core
        layout(location=0) in vec3 pos;
        layout(location=1) in vec3 normal;
        layout(location=2) in vec2 tex_coord;
        layout(location=3) in vec3 tex_tangent;
        uniform mat4 u_projection;
        uniform mat4 u_view;
        uniform vec3 u_color;
        uniform vec3 u_light_dir;
        out vec2 v_tex_coord;
        out mat3 v_tex_TBN;
        out vec3 v_view_pos;
        out vec3 v_light_dir;
        out vec3 v_color;
        void main()
        {
            mat4 mv = u_view;
            vec4 view_pos = mv * vec4(pos,1.0);
            gl_Position = u_projection * view_pos;
            v_view_pos = view_pos.xyz;
            v_tex_coord = tex_coord;
            vec3 tex_bitangent = cross(normal, tex_tangent);
            v_tex_TBN = mat3(tex_tangent, tex_bitangent, normal);
            v_light_dir = u_light_dir;
            v_color = u_color;
        }
    )";

    const char* fshader = R"(
        #version 330 core
        in vec2 v_tex_coord;
        in mat3 v_tex_TBN;
        in vec3 v_view_pos;
        in vec3 v_light_dir;
        in vec3 v_color;
        uniform sampler2D u_normal_texture;
        uniform vec3 u_light_color;
        out vec4 color;
        const vec3 ka = vec3(0.5, 0.5, 0.5);
        const vec3 kd = vec3(0.8, 0.8, 0.8);
        const vec3 ks = vec3(0.5, 0.5, 0.5);
        const float shine = 16.0;
        void main()
        {
            vec3 normal_raw = texture(u_normal_texture, v_tex_coord).xyz;
            vec3 normal = normalize(normal_raw * 2.0 - 1.0);
            vec3 N = v_tex_TBN * normal;
            vec3 L = normalize(v_light_dir);
            vec3 V = normalize(-v_view_pos);
            vec3 R = reflect(-L, N);
            vec3 amb = ka * u_light_color;
            vec3 dif = kd * u_light_color * max(dot(N, L), 0.0);
            vec3 spe = ks * u_light_color * pow(max(dot(R, V), 0.0), shine);
            color = vec4(amb + dif + spe, 1.0) * vec4(v_color, 1.0);
        }
    )";

    m_fshader_src = fshader;
    m_vshader_src = vshader;
}

RenderNormalTexture::~RenderNormalTexture()
{
}