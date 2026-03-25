GLSLShader
{
    Pass
    {
        layout(std140) uniform Materices
        {
            mat4 mView;
            mat4 mProjection;
        };
        layout(std140) uniform Camera
        {
            vec3 cameraPos;                     //16   0
            float zNear;                        //4    16
            float zFar;                         //4    20
        };
        uniform mat4 mModel;
        struct DATA
        {
            vec3 worldNormal;
            vec3 worldPosition;
            vec3 texcoord0;
        };

        #ifdef VERTEX
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexcoord0;
        
        out DATA VSOUT;
        void main()
        {
            vec4 pos = mProjection * mat4(mat3(mView)) * vec4(aPosition,1.0);

            gl_Position =pos.xyww;

            VSOUT.worldNormal = normalize(mat3(transpose(inverse(mModel))) * aNormal);
            VSOUT.worldPosition = vec3(mModel * vec4(aPosition,1.0));
            VSOUT.texcoord0 = vec3(aPosition);
        }
        #endif

        #ifdef FRAGMENT
        uniform samplerCube texSkybox;
        in DATA VSOUT;
        out vec4 FragColor;
        void main()
        {
            vec3 finalcolor = vec3(texture(texSkybox,VSOUT.texcoord0));
            FragColor = vec4(finalcolor,1.0);
        }
        #endif
    }
}