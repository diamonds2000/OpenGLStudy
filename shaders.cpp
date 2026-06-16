

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

const char* vshader_edge_src = R"(
        #version 330 core
        out vec2 vUV;
        void main()
        {
            vec2 pos = vec2(0.0, 0.0);
            if (gl_VertexID == 0) { pos = vec2(-1.0, -1.0); }
            else if (gl_VertexID == 1) { pos = vec2(1.0, -1.0); }
            else if (gl_VertexID == 2) { pos = vec2(-1.0, 1.0); }
            else if (gl_VertexID == 3) { pos = vec2(-1.0, 1.0); }
            else if (gl_VertexID == 4) { pos = vec2(1.0, -1.0); }
            else if (gl_VertexID == 5) { pos = vec2(1.0, 1.0); }
            gl_Position = vec4(pos, 0.0, 1.0);
            vUV = pos * 0.5 + 0.5;
        }
)";

const char* fshader_edge_src = R"(
        #version 330 core
        in vec2 vUV;
        out vec4 fragColor;
        uniform sampler2D u_ColorTex;
        uniform sampler2D u_DepthTex;
        uniform vec2 u_TexelSize;
        uniform float u_EdgeThreshold; 
        uniform vec4 u_OutlineColor;
        float sampleDepth(vec2 uv)
        {
            return texture(u_DepthTex, uv).r;
        }

        void main()
        {
            vec3 baseColor = texture(u_ColorTex, vUV).rgb;
    
            float d00 = sampleDepth(vUV + vec2(-u_TexelSize.x, -u_TexelSize.y));
            float d01 = sampleDepth(vUV + vec2(0, -u_TexelSize.y));
            float d02 = sampleDepth(vUV + vec2(u_TexelSize.x, -u_TexelSize.y));
    
            float d10 = sampleDepth(vUV + vec2(-u_TexelSize.x, 0));
            float d12 = sampleDepth(vUV + vec2(u_TexelSize.x, 0));
            
            float d20 = sampleDepth(vUV + vec2(-u_TexelSize.x, u_TexelSize.y));
            float d21 = sampleDepth(vUV + vec2(0, u_TexelSize.y));
            float d22 = sampleDepth(vUV + vec2(u_TexelSize.x, u_TexelSize.y));
    
            float gx = d02 + 2.0 * d12 + d22 - (d00 + 2.0 * d10 + d20);
            float gy = d20 + 2.0 * d21 + d22 - (d00 + 2.0 * d01 + d02);
            float edgeStrength = sqrt(gx * gx + gy * gy);
            if (edgeStrength > u_EdgeThreshold)
            {
                fragColor = u_OutlineColor;
            }
            else
            {
                fragColor = vec4(baseColor, 1.0);
            }

            //float edge = smoothstep(u_EdgeThreshold, u_EdgeThreshold * 3.0, edgeStrength);
            //vec3 edgeColor = mix(baseColor, u_OutlineColor.rgb, edge * u_OutlineColor.a);
            //fragColor = vec4(edgeColor, 1.0);
        }
)";

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