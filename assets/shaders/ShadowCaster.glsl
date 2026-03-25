GLSLShader
{
    Properties
    {
        
    }
    SubShader
    {
        Pass
        { 
            Zwrite on
            Zfunc always
            Cull back
            GLSLPROGRAM
            #include "/include/Core.glsl"
            #ifdef VERTEX
            void main()
            {
                gl_Position = ObjectToClipPos(aPosition);
            }
            #endif
            #ifdef FRAGMENT
            out vec4 FragColor;
            void main()
            {
                FragColor = vec4(1.0);
            }
            #endif
            ENDGLSL
        }
    }
}