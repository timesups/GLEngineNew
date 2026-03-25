#ifndef LIGHT_INCLUDE
#define LIGHT_INCLUDE
#include "Shadow.glsl"

#define MAXLIGHTS 8
#define PI 3.14159265359

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


struct Light
{
    vec3 direction;
    vec3 color;
    float attenuation;
};


struct SurfaceData
{
    vec3 albedo;
    vec3 specular;
    float roughness;
    float metallic;
    float ao;



    vec3 normalWS;
    vec3 pixelNormalWS;
    vec3 tangentWS;
    vec3 binormalWS;
    vec3 positionWS;
    vec3 ViewDirWS;
};

//input

uniform sampler2D albedoMap;
uniform sampler2D roughnessMap;
uniform sampler2D metallicMap;
uniform sampler2D normalMap;
uniform sampler2D AOMap;

SurfaceData InitSurfaceData(vec2 uv0,vec3 normalWS,vec3 tangentWS,vec3 binormalWS,vec3 positionWS)
{
    SurfaceData data;
    vec3 normalColor = texture(normalMap,uv0).rgb;


    normalColor = normalColor * 2 - 1; 
    normalColor.xy = -0.8*normalColor.xy;
    normalColor.z = sqrt(1-max(dot(normalColor.xy,normalColor.xy),0.0));
    data.albedo = texture(albedoMap,uv0).rgb;
    data.roughness = texture(roughnessMap,uv0).r;
    data.metallic = texture(metallicMap,uv0).r;
    data.ao = texture(AOMap,uv0).r;

    data.normalWS = normalize(normalWS);
    data.tangentWS = normalize(tangentWS);
    data.binormalWS = normalize(binormalWS);
    data.positionWS = normalize(positionWS);
    data.ViewDirWS = normalize(cameraPos-positionWS);


    mat3 t2w = mat3(data.tangentWS,data.binormalWS,data.normalWS);
    data.normalWS  = normalize(t2w * normalColor) ;
    return data;
}

Light GetMainLight(vec4 positionLS)
{
    Light mainLight;
    mainLight.direction = -_MainLightDirection;
    mainLight.color = _MainLihgtColor;
    mainLight.attenuation =  ShadowCalcMainLight(positionLS,_MainLightBias,_MainLightDepthTexture);
    return mainLight;
}

Light GetAdditionalLight(int index,vec3 PositionWS,vec4 positionLS[MAXLIGHTS])
{
    Light alight;
    alight.direction = normalize(_AdditionalLight[index].position - PositionWS);
    alight.color = _AdditionalLight[index].color;


    float distanceFade = 1/pow(length(_AdditionalLight[index].position - PositionWS),2);

    float inCone = _AdditionalLight[index].inCutOff;
    float outCone = _AdditionalLight[index].outCutOff;
    outCone = outCone > inCone ? inCone:outCone;


    float t = dot(alight.direction,normalize(-_AdditionalLight[index].direction));
    float e = inCone-outCone;
    float ins = clamp((t - outCone)/e,0,1);

    alight.attenuation = ShadowCalcAdditionalLight(positionLS[index],_AdditionalLight[index].bias,_AdditionLightDepthTexture[index]) * ins * distanceFade;
    return alight;
}

vec3 fresnelSchlick(float cosTheta,vec3 f0)
{
    return f0 + (1-f0) * pow(clamp(1-cosTheta,0.0,1.0),5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 PBRShading(Light light,SurfaceData data)
{
    float NdotL = max( dot(data.normalWS,light.direction),0);
    float VdotN = max( dot(data.ViewDirWS,data.normalWS),0);
    vec3 hDir = normalize(data.ViewDirWS + light.direction);
    float HdotV = max(dot(hDir,data.ViewDirWS),0);



    vec3 radiance = light.color * light.attenuation * NdotL;

    vec3 f0 = vec3(0.04);
    f0 = mix(f0,data.albedo,data.metallic);
    vec3 F = fresnelSchlick(HdotV,f0);

    float NDF = DistributionGGX(data.normalWS,hDir,data.roughness);
    float G = GeometrySmith(data.normalWS,data.ViewDirWS,light.direction,data.roughness);

    vec3 nominator = NDF*G*F;
    float denominator = 4.0 * VdotN * NdotL + 0.001;
    vec3 specular = nominator/denominator;

    vec3 KS = F;
    vec3 KD = vec3(1.0) - KS;
    KD *= 1 - data.metallic;


    vec3 l0 = (KD*data.albedo/PI + specular) * radiance * NdotL;

    return vec3(l0);
}

#endif