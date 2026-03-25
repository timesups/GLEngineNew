#ifndef GL_LIGHTING_INCLUDE
#define GL_LIGHTING_INCLUDE

//radiance
vec3 IncomingLight(Surface surface,Light light)
{
    
    return saturate(dot(surface.normal,light.direction)) * light.color;
}



vec3 GetLighting(Surface surface,BRDF brdf,Light light)
{
    return IncomingLight(surface,light) * DirectBRDF(surface,brdf,light);
}



vec3 GetLighting(Surface surface,BRDF brdf)
{
    vec3 color = GetLighting(surface,brdf,GetDirectionalLight());

    return color;
}




#endif