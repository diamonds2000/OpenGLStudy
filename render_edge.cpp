#include "render_edge.h"

RenderEdge::RenderEdge()
{
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

    m_fshader_src = fshader_edge_src;
    m_vshader_src = vshader_edge_src;
}

RenderEdge::~RenderEdge()
{
}