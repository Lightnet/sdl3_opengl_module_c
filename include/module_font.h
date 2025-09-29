// module_font.h
#pragma once

// #ifndef MODULE_FONT_H
// #define MODULE_FONT_H

#include <glad/gl.h>

// Opaque pointer to FontData
typedef struct FontData FontData;

int init_font_shaders_and_buffers(GLuint* program, GLuint* vao, GLuint* vbo);

// Functions using FontData
int init_font(const char* font_path, float font_size, float scale, FontData** font_data);
void render_text(FontData* font_data, GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a);
void cleanup_font(FontData* font_data);

// Alternative functions (no FontData, for internal management)
int init_font_alt(const char* font_path, float font_size, float scale);
void render_text_alt(GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a);
void cleanup_font_alt(void);

// #endif // MODULE_FONT_H



/*
// refs
typedef struct {
    GLuint texture;
    stbtt_bakedchar* cdata;
    int bitmap_w, bitmap_h;
} FontData;

render_text(&font_data, text_program, text_vao, text_vbo, text, 25.0f, 150.0f, ww, hh, 1.0f, 1.0f, 1.0f, 1.0f);
*/