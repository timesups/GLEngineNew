#ifndef GL_LIGHT_INCLUDE
#define GL_LIGHT_INCLUDE

#define MAXLIGHTS 8



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

struct Light
{
    vec3 color;
    vec3 direction;
};


Light GetDirectionalLight()
{
    Light light;
    light.color = _MainLihgtColor;
    light.direction = _MainLightDirection;
    return light;
}


#endif