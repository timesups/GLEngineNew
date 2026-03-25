
#ifndef COMMON_HEADER_INCLUDE
#define COMMON_HEADER_INCLUDE


layout(std140) uniform Materices
{
    mat4 mView;
    mat4 mProjection;
};
layout(std140) uniform Camera
{
    vec3 cameraPos;                     //16   0
    float zNear;                        //4    16
    float zFar;                         //4    20
};
uniform mat4 lightSpaceMatrix;
uniform mat4 mModel;


#endif