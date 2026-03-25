#ifndef SHADOW_INCLUDE
#define SHADOW_INCLUDE
layout(std140) uniform Camera
{
    vec3 cameraPos;                     //16   0
    float zNear;                        //4    16
    float zFar;                         //4    20
};
float LinearizeDepth(float depth)
{
    float z = depth; // Back to NDC 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

float ShadowCalcMainLight(vec4 positionLS,float bias,sampler2D depthTexture)
{
    //其次除法
    positionLS = vec4(positionLS.x/positionLS.w,positionLS.y/positionLS.w,positionLS.z/positionLS.w,1.0);
    //回到NDC
    positionLS = positionLS *0.5+0.5;

    float currentDepth = positionLS.z;

    float shaodw = 0.0;
    vec2 texelSize = 1.0/textureSize(depthTexture,0) * 0.1;

    if (currentDepth >= 1)
        return 1.0;

    for(int x=-1;x<=1;x++)
    {
        for(int y=-1;y<=1;y++)
        {
            float closeDepth = texture(depthTexture,vec2(positionLS.x,positionLS.y)+vec2(x,y) * texelSize ).r;
            shaodw += currentDepth-bias > closeDepth? 0.0:1.0;
        }
    }
    shaodw /= 9;
    float closeDepth = texture(depthTexture,vec2(positionLS.x,positionLS.y)* texelSize ).r;
    return shaodw;

}

float ShadowCalcAdditionalLight(vec4 positionLS,float bias,sampler2D depthTexture)
{
    int sample = 4;
    positionLS = vec4(positionLS.x/positionLS.w,positionLS.y/positionLS.w,positionLS.z/positionLS.w,1.0);
    positionLS = positionLS *0.5+0.5;

    float currentDepth = LinearizeDepth(positionLS.z);

    float shaodw = 0.0;


    float closeDepth = LinearizeDepth(texture(depthTexture,vec2(positionLS.x,positionLS.y) ).r);
    shaodw += currentDepth-bias > closeDepth? 0.0:1.0;

    return shaodw;

}



#endif