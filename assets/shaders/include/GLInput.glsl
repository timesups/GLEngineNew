#ifndef GLINPUT_INCLUDE
#define GLINPUT_INCLUDE

#extension GL_ARB_separate_shader_objects : enable//启动GL_ARB_separate_shader_objects
#extension GL_ARB_shading_language_420pack : enable//启动GL_ARB_shading_language_420pack

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec2 aTexcoord0;



layout(std140) uniform DataInputs
{
    mat4 GL_MATRIX_V;
    mat4 GL_MATRIX_P;
    vec3 _CameraPosition;              
    float _zNear;                     
    float _zFar;    
};

uniform mat4 GL_MATRIX_M;

#endif
