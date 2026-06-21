



/////////////////////////////////////////////////////////////////////////////
// calucate mvp in shader
const char* vshader_src2 =
        "#version 330 core\n"
        "uniform mat4 u_projection;"
        "uniform mat4 u_view;"
        "uniform vec3 u_color;"
        "uniform vec3 u_light_dir;"
        "out vec3 mv_pos;"
        "out vec3 mv_normal;"
        "out vec3 mv_light_dir;"
        "out vec3 v_color;"
        "out vec2 v_texCoord;"
        "layout(location=0) in vec3 pos;"
        "layout(location=1) in vec3 normal;"
        "layout(location=2) in vec2 textureCoord;"
        "layout(location=3) in mat4 instance_model;"
        "layout(location=7) in vec3 color;"
        "void main()"
        "{"
        "    mat4 mv = u_view * instance_model;"
        "    vec4 view_pos = mv * vec4(pos,1.0);"
        "    gl_Position = u_projection * view_pos;"
        "    mv_pos = view_pos.xyz;"
        "    mv_normal = (mv * vec4(normal, 0.0)).xyz;"
        "    mv_light_dir = normalize(mv * vec4(u_light_dir, 0.0)).xyz;"
        "    v_color = color;"
        "    v_texCoord = textureCoord;"
        "}";

const char* fshader_src2 =
        "#version 330 core\n"
        "uniform vec3 u_light_color;"
        "uniform sampler2D u_texture;"
        "in vec3 v_color;"
        "in vec3 mv_pos;"
        "in vec3 mv_normal;"
        "in vec3 mv_light_dir;"
        "in vec2 v_texCoord;"
        "out vec4 color;"
        "const vec3 ka = vec3(0.3, 0.3, 0.3);"
        "const vec3 kd = vec3(0.8, 0.8, 0.8);"
        "const vec3 ks = vec3(0.5, 0.5, 0.5);"
        "const float shine = 32.0;"
        "void main()"
        "{"
        "    vec3 N = normalize(mv_normal);"
        "    vec3 L = normalize(mv_light_dir);"
        "    vec3 V = normalize(-mv_pos);"
        "    vec3 R = reflect(-L, N);"
        "    vec3 amb = ka * u_light_color * v_color;"
        "    float diff = max(dot(N,L),0.0);"
        "    vec3 dif = diff * kd * u_light_color * v_color;"
        "    float spec = diff > 0.0 ? pow(max(dot(R, V), 0.0), shine) : 0.0;"
        "    vec3 spe = spec * ks * u_light_color;"
        "    color = vec4(amb + dif + spe, 1.0) * texture(u_texture, v_texCoord);"
        "}";


/////////////////////////////////////////////////////////////////////////////
// pass model matrix per instance
const char* vshader_src3 =
        "#version 330 core\n"
        "uniform mat4 u_projection;"
        "uniform mat4 u_model;"
        "uniform mat4 u_view;"
        "uniform vec3 u_color;"
        "uniform vec3 u_light_dir;"
        "out vec3 mv_pos;"
        "out vec3 mv_normal;"
        "out vec3 mv_light_dir;"
        "out vec3 v_color;"
        "layout(location=0) in vec3 pos;"
        "layout(location=1) in vec3 normal;"
        "void main()"
        "{"
        "    mat4 mv = u_view * u_model;"
        "    vec4 view_pos = mv * vec4(pos,1.0);"
        "    gl_Position = u_projection * view_pos;"
        "    mv_pos = view_pos.xyz;"
        "    mv_normal = (mv * vec4(normal, 0.0)).xyz;"
        "    mv_light_dir = normalize(mv * vec4(u_light_dir, 0.0)).xyz;"
        "    v_color = u_color;"
        "}";

const char* fshader_src3 =
        "#version 330 core\n"
        "uniform vec3 u_light_color;"
        "in vec3 v_color;"
        "in vec3 mv_pos;"
        "in vec3 mv_normal;"
        "in vec3 mv_light_dir;"
        "out vec4 color;"
        "const vec3 ka = vec3(0.3, 0.3, 0.3);"
        "const vec3 kd = vec3(0.8, 0.8, 0.8);"
        "const vec3 ks = vec3(0.5, 0.5, 0.5);"
        "const float shine = 32.0;"
        "void main()"
        "{"
        "    vec3 N = normalize(mv_normal);"
        "    vec3 L = normalize(mv_light_dir);"
        "    vec3 V = normalize(-mv_pos);"
        "    vec3 R = reflect(-L, N);"
        "    vec3 amb = ka * u_light_color * v_color;"
        "    float diff = max(dot(N,L),0.0);"
        "    vec3 dif = diff * kd * u_light_color * v_color;"
        "    float spec = diff > 0.0 ? pow(max(dot(R, V), 0.0), shine) : 0.0;"
        "    vec3 spe = spec * ks * u_light_color;"
        "    color = vec4(amb + dif + spe, 1.0);"
        "}";





