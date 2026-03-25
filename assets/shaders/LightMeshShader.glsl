GLSLShader
{
    Properties
    {
        
    }
    SubShader
    {
        Pass
        { 
            Zwrite off
            Zfunc always
            Cull back
            GLSLPROGRAM
            #include "/include/Core.glsl"
            #include "/include/Light.glsl"
            #ifdef VERTEX
            void main()
            {
                gl_Position = ObjectToClipPos(aPosition);
            }
            #endif


            #ifdef FRAGMENT
            uniform vec3 _LightColor;
            out vec4 FragColor;
            void main()
            {
                FragColor = vec4(_LightColor,1.0);
            }
            #endif
            ENDGLSL
        }
    }
}