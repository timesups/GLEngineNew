GLSLShader
{
    Properties
    {
        
    }
    SubShader
    {
        Pass
        { 
            cull back
            GLSLPROGRAM
            #include "/include/Core.glsl"
            #include "/include/Light.glsl"

            struct V2F
            {
                vec3 normal;
                vec2 uv;
            };

            #ifdef VERTEX
            out V2F v2f;
            void main()
            {
                gl_Position = ObjectToClipPos(aPosition);
                v2f.normal = ObjectToWorldN(aNormal);
                v2f.uv = aTexcoord0;
            }
            #endif


            #ifdef FRAGMENT
            in V2F v2f;
            uniform sampler2D tex;
            out vec4 FragColor;
            void main() 
            {
                vec4 tex_color = texture(tex,v2f.uv);
                FragColor = vec4(tex_color.xyz ,1.0);
            }
            #endif
            ENDGLSL
        }
    }
}