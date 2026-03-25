#ifndef SURFACE_INCLUDE
#define SURFACE_INCLUDE


struct SurfaceData{
    vec3 albedo;
    float metallic;
    float roughness;
    vec3 viewDirWS;
    vec3 normalWS;
    vec3 tangentWS;
    vec3 binormalWS;
};

SurfaceData GetSurfaceData(vec3 albedo,float metallic,float roughness,vec3 viewDirWS,vec3 normalWS,vec3 tangentWS,vec3 binormalWS,vec3 positionWS)
{
    SurfaceData surface;
    surface.albedo = albedo;
    surface.metallic = metallic;
    surface.roughness = roughness;
    surface.viewDirWS = normalize(viewDirWS);
    surface.normalWS = normalize(normalWS);
    surface.tangentWS = normalize(tangentWS);
    surface.binormalWS = normalize(binormalWS);
    return surface;
}


#endif