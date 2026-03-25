#ifndef GL_SURFACE_INCLUDE
#define GL_SURFACE_INCLUDE


struct Surface
{
    vec3 color;
    float metallic;
    float roughness;
    vec3 normal;
    vec3 viewDirection;
    float alpha;
};




#endif