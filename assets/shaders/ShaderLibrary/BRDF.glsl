#ifndef GL_BRDF_INCOUDE
#define GL_BRDF_INCOUDE


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

struct BRDF
{
    vec3 diffuse;
    vec3 specular;
    float roughness;
};


vec3 SpecularStrength(Surface surface,BRDF brdf,Light light)
{
    vec3 hDir = normalize(surface.viewDirection + light.direction);
    float NdotL = max( dot(surface.normal,light.direction),0);
    float VdotN = max( dot(surface.viewDirection,surface.normal),0);
    float HdotV = max(dot(hDir,surface.viewDirection),0);
    float HdotN = max( dot(hDir,surface.normal),0);


    vec3 f0 = vec3(0.04);
    f0 = mix(f0,surface.color,surface.metallic);
    vec3 F = fresnelSchlick(VdotN,f0);


    float NDF = DistributionGGX(surface.normal,hDir,surface.roughness);
    float G = GeometrySmith(surface.normal,surface.viewDirection,light.direction,surface.roughness);

    vec3 nominator = NDF*G*F;
    float denominator = 4.0 * VdotN * NdotL + 0.001;
    vec3 specular = nominator/denominator;

    vec3 KS = F;
    vec3 KD = vec3(1.0) - KS;
    KD *= 1 - surface.metallic;


    vec3 l0 = (KD*surface.color/PI + specular) * NdotL;
    return vec3(l0);
}

vec3 DirectBRDF(Surface surface,BRDF brdf,Light light)
{
    return SpecularStrength(surface,brdf,light) * brdf.specular + brdf.diffuse;
}


BRDF GetBRDF(Surface surface)
{
    BRDF brdf;
    brdf.diffuse = vec3(0.0);
    brdf.specular = vec3(1.0);
    brdf.roughness = surface.roughness;
    return brdf;
}


#endif