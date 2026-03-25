0GLSLShader
{
    Pass
    {
        #include "/include/VertexHelper.glsl"

        #ifdef VERTEX
        out vec3 localPos;
        void main()
        {
            localPos = aPosition;
            vec4 pos = mProjection * mat4(mat3(mView)) *vec4(aPosition,1.0);
            gl_Position = pos.xyww;

        }
        #endif



        #ifdef FRAGMENT

        uniform samplerCube _HDRIMap;
        in vec3 localPos;
        out vec4 FragColor;
        void main()
        {
            vec3 color = texture(_HDRIMap,normalize(localPos)).rgb;
            color = pow(color,vec3(2.2));
            FragColor = vec4(color,1.0);
        }
        #endif
    }
}