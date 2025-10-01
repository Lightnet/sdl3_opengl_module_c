https://cglm.readthedocs.io/en/latest/getting_started.html


```
typedef float                   vec2[2];
typedef float                   vec3[3];
typedef int                    ivec3[3];
typedef CGLM_ALIGN_IF(16) float vec4[4];
typedef vec4                    versor;
typedef vec3                    mat3[3];

#ifdef __AVX__
typedef CGLM_ALIGN_IF(32) vec4  mat4[4];
#else
typedef CGLM_ALIGN_IF(16) vec4  mat4[4];
#endif
```