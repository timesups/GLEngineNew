GLSLShader
{
    Properties
    {
        
    }
    SubShader
    {
        Pass
        { 
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
                FragColor = vec4(1.0,0.0,1.0 ,1.0);
            }
            #endif
            ENDGLSL
        }
    }
}