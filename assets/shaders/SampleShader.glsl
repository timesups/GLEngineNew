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
            #include "/include/Light.glsl"
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
                FragColor = vec4(_MainLihgtColor,1.0);
            }
            #endif
            ENDGLSL
        }
    }
}