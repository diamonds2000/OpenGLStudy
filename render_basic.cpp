#include "render_basic.h"

RenderBasic::RenderBasic()
{
    const char* vshader_src1 = R"(
        #version 330 core
        uniform mat4 u_mvp;
        uniform mat4 u_view;
        uniform vec3 u_color;
        uniform vec3 u_light_dir;
        out vec3 mv_pos;
        out vec3 mv_normal;
        out vec3 mv_light_dir;
        out vec3 v_color;
        layout(location=0) in vec3 pos;
        layout(location=1) in vec3 normal;
        layout(location=2) in mat4 instance_model;
        void main()
        {
            gl_Position = u_mvp * instance_model * vec4(pos,1.0);
            mv_pos = (u_view * instance_model * vec4(pos, 1.0)).xyz;
            mv_normal = (u_view * instance_model * vec4(normal, 0.0)).xyz;
            mv_light_dir = normalize(u_view * vec4(u_light_dir, 0.0)).xyz;
            v_color = u_color;
        }
    )";

    const char* fshader_src1 = R"(
        #version 330 core
        uniform vec3 u_light_color;
        in vec3 v_color;
        in vec3 mv_pos;
        in vec3 mv_normal;
        in vec3 mv_light_dir;
        out vec4 color;
        const vec3 ka = vec3(0.3, 0.3, 0.3);
        const vec3 kd = vec3(0.8, 0.8, 0.8);
        const vec3 ks = vec3(0.5, 0.5, 0.5);
        const float shine = 32.0;
        void main()
        {
                vec3 N = normalize(mv_normal);
                vec3 L = normalize(mv_light_dir);
                vec3 V = normalize(-mv_pos);
                vec3 R = reflect(-L, N);
                vec3 amb = ka * u_light_color * v_color;
                float diff = max(dot(N,L),0.0);
                vec3 dif = diff * kd * u_light_color * v_color;
                float spec = diff > 0.0 ? pow(max(dot(R, V), 0.0), shine) : 0.0;
                vec3 spe = spec * ks * u_light_color;
                color = vec4(amb + dif + spe, 1.0);
        }
    )";

    m_fshader_src = fshader_src1;
    m_vshader_src = vshader_src1;
}

RenderBasic::~RenderBasic()
{
}