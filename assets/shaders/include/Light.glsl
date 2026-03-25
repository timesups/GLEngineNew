#ifndef LIGHT_INCLUDE
#define LIGHT_INCLUDE
#define MAXLIGHTS 8



uniform sampler2D _MainLightDepthTexture;
uniform sampler2D _AdditionLightDepthTexture[MAXLIGHTS];

struct AdditionalLight
{
    vec3 position;  //0-12
    float inCutOff; //12-16
    vec3 direction; //16-28
    float outCutOff;//28-32
    vec3 color;     //32-44
    float bias;     //44-48
    mat4 _LightMatrix;

};
layout(std140) uniform Lights
{

    vec3 _MainLihgtColor;
    vec3 _MainLightDirection;
    float _MainLightBias;
    mat4 _MainLightMatrix;
    vec3 _AmbientLightColor;
    int _AdditionalLightCount;
    AdditionalLight _AdditionalLight[MAXLIGHTS];
};

struct Light{
    vec3 color;
    vec3 direction;
    float attenuation;
    float bias;
};


Light GetMainLight()
{
    Light mainLight;
    mainLight.color = _MainLihgtColor;
    mainLight.direction = _MainLightDirection;
    mainLight.attenuation = 1.0;
    mainLight.bias = _MainLightBias;
    return mainLight;
}

vec4 CalcDirectionalPositionLS(vec3 positionWS)
{
    return _MainLightMatrix * vec4(positionWS,1.0);
} 

vec4[MAXLIGHTS] CalcAdditionalPositionLS(vec3 positionWS)
{
    vec4 positionLS[MAXLIGHTS];
    for(int i=0;i<_AdditionalLightCount;i++)
    {
        positionLS[i] = _AdditionalLight[i]._LightMatrix * vec4(positionWS,1.0);
    }

    return positionLS;
} 


#endif