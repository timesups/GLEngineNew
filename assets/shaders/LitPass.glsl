#ifndef GL_LIT_PASS_INCLUDE
#define GL_LIT_PASS_INCLUDE
#include "/ShaderLibrary/Common.glsl"
#include "/ShaderLibrary/Surface.glsl"
#include "/ShaderLibrary/Light.glsl"
#include "/ShaderLibrary/BRDF.glsl"
#include "/ShaderLibrary/Lighting.glsl"


struct Varyings
{
    vec3 positionWS;
    vec3 normalWS;
    vec2 baseUV;
};

#ifdef VERTEX

out Varyings v2f;

void main()
{
    gl_Position = ObjectToClipPos(aPosition);
    v2f.positionWS = ObjectToWorldPos(aPosition);
    v2f.normalWS = aNormal.xyz;
    v2f.baseUV = aTexcoord0;
}
#endif


#ifdef FRAGMENT
uniform vec4 _baseColor;
uniform float _metallic;
uniform float _roughness;
in Varyings v2f;
out vec4 FragColor;
void main()
{   
    vec4 base = vec4(_baseColor.rgb,1.0);


    Surface surface;
    surface.normal = normalize(v2f.normalWS);
    surface.color = base.rgb;
    surface.alpha = base.a;
    surface.metallic = _metallic;
    surface.roughness = _roughness;
    surface.viewDirection = normalize(_CameraPosition.xyz - v2f.positionWS);

    BRDF brdf = GetBRDF(surface);

    vec3 color = GetLighting(surface,brdf);

    FragColor = vec4(color,surface.alpha);
}
#endif


#endif