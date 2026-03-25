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
            struct DATA
            {
                vec2 uv0;
            };
            #include "/include/Core.glsl"
            #ifdef VERTEX
            out DATA TransferData;
            void main()
            {
                gl_Position = vec4(aPosition.x,aPosition.y,0.0,1.0);
                TransferData.uv0 = vec2(aPosition.x,aPosition.y);
                TransferData.uv0 = (TransferData.uv0+1)*0.5;
            }
            #endif

            #ifdef FRAGMENT
            uniform sampler2D screenTexture;
            uniform sampler2D unilityTexture;
            in DATA TransferData;
            out vec4 FragColor;
            void main()
            {
                float gamma = 2.2;
                vec3 colorPass = texture(screenTexture,TransferData.uv0).rgb;
                colorPass = colorPass / (colorPass + vec3(1.0));
                colorPass = pow(colorPass,vec3(1/gamma));
                colorPass += texture(unilityTexture,TransferData.uv0).rgb;
                FragColor = vec4(vec3(colorPass),1.0);
            } 
            #endif
            ENDGLSL
        }
    }
}