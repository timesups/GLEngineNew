#ifndef TRANSFORM_INCLUDE
#define TRANSFORM_INCLUDE


vec4 ObjectToClipPos(vec3 pos)
{
    return(GL_MATRIX_P * GL_MATRIX_V * GL_MATRIX_M * vec4(pos,1.0));
}


vec3 ObjectToWorldNormal(vec3 normal)
{
    return(normalize(mat3(transpose(inverse(GL_MATRIX_M))) * aNormal));
}

vec3 ObjectToWorldDir(vec3 dir)
{
    return normalize(GL_MATRIX_M * vec4(dir,0.0)).xyz;
}

vec3 ObjectToWorldPos(vec3 pos)
{
    return (GL_MATRIX_M * vec4(pos,1.0)).xyz;
}


#endif




