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
            #include "LitPass.glsl"
            ENDGLSL
        }
    }
}