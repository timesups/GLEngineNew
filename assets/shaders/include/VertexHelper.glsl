#ifndef VERTEX_HELPER_INCLUDE
#define VERTEX_HELPER_INCLUDE

#ifdef VERTEX
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec2 aTexcoord0;
layout (location = 5) in vec2 aTexcoord1;
layout (location = 6) in vec2 aTexcoord2;
layout (location = 7) in mat4 aInstanceTransform;


layout(std140) uniform Materices
{
    mat4 mView;
    mat4 mProjection;
};
uniform mat4 mModel;


vec4 ObjectToClipPos(vec3 pos)
{
    return(mProjection * mView * mModel * vec4(pos,1.0));
}


vec3 ObjectToWorldNormal(vec3 normal)
{
    return(normalize(mat3(transpose(inverse(mModel))) * aNormal));
}

vec3 ObjectToWorldDir(vec3 dir)
{
    return normalize((mProjection * mView * mModel * vec4(dir,0.0)).xyz);
}
#endif

#endif