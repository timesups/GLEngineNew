0GLSLShader
{
    Pass
    {
        #include "/include/VertexHelper.glsl"
        uniform mat4 projection;
        uniform mat4 view;

        #ifdef VERTEX
        out vec3 localPosition;
        void main()
        {

            localPosition = aPosition;
            gl_Position = projection * view * mModel * vec4(aPosition,1.0);
        }
        #endif



        #ifdef FRAGMENT

        uniform sampler2D _equirectangularMap;
        in vec3 localPosition;
        out vec4 FragColor;



        const vec2 invAtan = vec2(0.1591, 0.3183);
        vec2 SampleSphericalMap(vec3 v)
        {
            vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
            uv *= invAtan;
            uv += 0.5;
            return uv * vec2(1,-1);
        }
        void main()
        {
            vec2 uv = SampleSphericalMap(normalize(localPosition)); // make sure to normalize localPos
            vec3 color = texture(_equirectangularMap, uv).rgb;
            FragColor = vec4(color,1.0);
        }
        #endif
    }
}