// module_cube.h
#pragma once

#include <glad/gl.h>
#include <cglm/cglm.h> // Include CGLM

typedef struct {
    GLuint texture;
    GLuint vao, vbo;
} CubeData;

// Initialize cube (unchanged)
int init_cube(const char* texture_path, CubeData* cube_data);

// Initialize cube shaders
int init_cube_shaders_and_buffers(GLuint* cube_program);

// Render cube with CGLM matrices
void render_cube(CubeData* cube_data, GLuint program, vec3 rotation, int ww, int hh);
