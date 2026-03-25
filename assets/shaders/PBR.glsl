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
            #include "/include/Surface.glsl"
            #include "/include/Light.glsl"
            #include "/include/Lighting.glsl"
            struct V2F{
                vec3 positionWS;
                vec3 normalWS;
                vec3 tangentWS;
                vec3 binormalWS;
                vec2 uv0;
                vec4 dirPositionLS;
                vec4 addPositionLS[MAXLIGHTS];
            };

            #ifdef VERTEX
            out V2F v2f;
            void main()
            {
                gl_Position = ObjectToClipPos(aPosition);
                v2f.positionWS = ObjectToWorldPos(aPosition);
                v2f.normalWS = ObjectToWorldNormal(aNormal);
                v2f.tangentWS = ObjectToWorldDir(aTangent);
                v2f.binormalWS = normalize(cross(v2f.normalWS ,v2f.tangentWS));
                v2f.uv0 = aTexcoord0;
                v2f.dirPositionLS = CalcDirectionalPositionLS(v2f.positionWS);
                v2f.addPositionLS = CalcAdditionalPositionLS(v2f.positionWS);


            }
            #endif


            #ifdef FRAGMENT

            out vec4 FragColor;
            in V2F v2f;
            void main()
            {
                vec3 albedo = vec3(0.5);
                float metallic = 0.0;
                float roughness = 0.1;
                vec3 viewDirWS = normalize(_CameraPosition.xyz - v2f.positionWS);

                SurfaceData surface = GetSurfaceData(albedo,metallic,roughness,viewDirWS,v2f.normalWS,v2f.tangentWS,v2f.binormalWS,v2f.positionWS);
                Light mainLight = GetMainLight();
                vec3 finalColor = vec3(0.0);
                finalColor = PBRShading(mainLight,surface);
                // for (int i=0;i<_AdditionalLightCount;i++){
                //     Light additionalLight = GetMainLight
                // }

                finalColor += _AmbientLightColor;
                FragColor = vec4(finalColor,1.0);
            }
            #endif
            ENDGLSL
        }
    }
}