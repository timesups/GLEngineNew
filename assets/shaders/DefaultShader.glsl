GLSLShader
{
    Pass
    {
        #include "Include/Light.glsl"
        uniform mat4 _lightSpaceMatrix;
        struct DATA
        {
            vec3 worldNormal;
            vec3 worldPosition;
            vec3 worldTangent;
            vec3 worldBinormal;
            vec2 texcoord0;
            vec4 PositionMLS;
            vec4 PositionALS[MAXLIGHTS];
        };
        #include "Include/VertexHelper.glsl"

        #ifdef VERTEX
        out DATA VsOut;
        void main()
        {
            gl_Position = ObjectToClipPos(aPosition);
            VsOut.worldNormal = ObjectToWorldNormal(aNormal);
            VsOut.worldPosition = vec3(mModel * vec4(aPosition,1.0));
            VsOut.worldTangent = ObjectToWorldDir(aTangent);
            VsOut.texcoord0 = aTexcoord0;
            VsOut.PositionMLS = _MainLightMatrix * vec4(VsOut.worldPosition,1.0);
            VsOut.worldBinormal = cross(VsOut.worldNormal,VsOut.worldTangent);
            for(int i=0;i<_AdditionalLightCount;i++)
            {
                VsOut.PositionALS[i] = _AdditionalLight[i]._LightMatrix * vec4(VsOut.worldPosition,1.0);
            }

        }
        #endif

        #ifdef FRAGMENT
        in DATA VsOut;
        out vec4 FragColor;
        void main() 
        {
            Light mainLight = GetMainLight(VsOut.PositionMLS);
            SurfaceData data = InitSurfaceData(VsOut.texcoord0,VsOut.worldNormal,VsOut.worldTangent,VsOut.worldBinormal,VsOut.worldPosition);
            vec3 finalColor = PBRShading(mainLight,data);

            for(int i = 0;i<_AdditionalLightCount;i++)
            {
                Light aLight = GetAdditionalLight(i,VsOut.worldPosition,VsOut.PositionALS);
                finalColor += PBRShading(aLight,data);
            }

            finalColor += _AmbientLightColor * data.albedo;
            FragColor = vec4(vec3(finalColor),1.0);
        }
        #endif
    }
}