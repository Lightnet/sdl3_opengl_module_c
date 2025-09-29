#pragma once

// #ifndef MODULE_FONT_H
// #define MODULE_FONT_H

#include <glad/gl.h>

// Opaque pointer to FontData
typedef struct FontData FontData;

// Functions using FontData
int init_font(const char* font_path, float font_size, float scale, FontData** font_data);
void render_text(FontData* font_data, GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a);
void cleanup_font(FontData* font_data);

// Alternative functions (no FontData, for internal management)
int init_font_alt(const char* font_path, float font_size, float scale);
void render_text_alt(GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a);
void cleanup_font_alt(void);

// #endif // MODULE_FONT_H