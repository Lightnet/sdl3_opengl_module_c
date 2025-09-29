// module_font.c
#include "module_font.h"
#include <stdio.h>
#include <stdlib.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// Define FontData locally
struct FontData {
    GLuint texture;         // Font texture ID
    stbtt_bakedchar* cdata; // Character data (ASCII 32..127)
    int bitmap_w, bitmap_h; // Bitmap dimensions
};

// Static FontData for alternative functions
static struct FontData* global_font_data = NULL;

// Initialize font: Load TTF and bake bitmap
int init_font(const char* font_path, float font_size, float scale, FontData** font_data) {
    *font_data = (struct FontData*)malloc(sizeof(struct FontData));
    if (!*font_data) {
        printf("Error: Failed to allocate FontData\n");
        return 0;
    }

    unsigned char* ttf_buffer = (unsigned char*)malloc(1 << 20);
    if (!ttf_buffer) {
        printf("Error: Failed to allocate TTF buffer\n");
        free(*font_data);
        *font_data = NULL;
        return 0;
    }

    FILE* ff = fopen(font_path, "rb");
    if (!ff) {
        printf("Error: Failed to open font file '%s'\n", font_path);
        free(ttf_buffer);
        free(*font_data);
        *font_data = NULL;
        return 0;
    }

    fread(ttf_buffer, 1, 1 << 20, ff);
    fclose(ff);

    (*font_data)->bitmap_w = 512;
    (*font_data)->bitmap_h = 512;
    unsigned char* bitmap = (unsigned char*)malloc((*font_data)->bitmap_w * (*font_data)->bitmap_h);
    if (!bitmap) {
        printf("Error: Failed to allocate bitmap\n");
        free(ttf_buffer);
        free(*font_data);
        *font_data = NULL;
        return 0;
    }

    (*font_data)->cdata = (stbtt_bakedchar*)malloc(96 * sizeof(stbtt_bakedchar));
    if (!(*font_data)->cdata) {
        printf("Error: Failed to allocate cdata\n");
        free(bitmap);
        free(ttf_buffer);
        free(*font_data);
        *font_data = NULL;
        return 0;
    }

    stbtt_BakeFontBitmap(ttf_buffer, 0, font_size * scale, bitmap, (*font_data)->bitmap_w, (*font_data)->bitmap_h, 32, 96, (*font_data)->cdata);
    free(ttf_buffer);

    // Create OpenGL texture
    glGenTextures(1, &(*font_data)->texture);
    glBindTexture(GL_TEXTURE_2D, (*font_data)->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (*font_data)->bitmap_w, (*font_data)->bitmap_h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(bitmap);

    return 1;
}

// Render text
void render_text(FontData* font_data, GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a) {
    if (!font_data) return;

    float vertices[1024 * 4]; // Enough for simple text
    int vert_count = 0;

    for (const char* p = text; *p; p++) {
        if (*p >= 32 && *p < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font_data->cdata, font_data->bitmap_w, font_data->bitmap_h, *p - 32, &x, &y, &q, 1);

            float nx0 = 2.0f * q.x0 / ww - 1.0f;
            float ny0 = 1.0f - 2.0f * q.y0 / hh;
            float nx1 = 2.0f * q.x1 / ww - 1.0f;
            float ny1 = 1.0f - 2.0f * q.y1 / hh;

            // Triangle 1
            vertices[vert_count++] = nx0; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s0; vertices[vert_count++] = q.t0;
            vertices[vert_count++] = nx1; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t0;
            vertices[vert_count++] = nx1; vertices[vert_count++] = ny1; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t1;

            // Triangle 2
            vertices[vert_count++] = nx0; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s0; vertices[vert_count++] = q.t0;
            vertices[vert_count++] = nx1; vertices[vert_count++] = ny1; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t1;
            vertices[vert_count++] = nx0; vertices[vert_count++] = ny1; vertices[vert_count++] = q.s0; vertices[vert_count++] = q.t1;
        }
    }

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "textTexture"), 0);
    glUniform4f(glGetUniformLocation(program, "textColor"), r, g, b, a);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_data->texture);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, vert_count / 4);
    glBindVertexArray(0);
}

// Clean up font resources
void cleanup_font(FontData* font_data) {
    if (!font_data) return;
    glDeleteTextures(1, &font_data->texture);
    free(font_data->cdata);
    free(font_data);
}

// Alternative functions using a static FontData
int init_font_alt(const char* font_path, float font_size, float scale) {
    if (global_font_data) {
        cleanup_font(global_font_data);
    }
    return init_font(font_path, font_size, scale, &global_font_data);
}

void render_text_alt(GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a) {
    if (global_font_data) {
        render_text(global_font_data, program, vao, vbo, text, x, y, ww, hh, r, g, b, a);
    }
}

void cleanup_font_alt(void) {
    if (global_font_data) {
        cleanup_font(global_font_data);
        global_font_data = NULL;
    }
}

// Initialize shaders and VAO/VBO
int init_font_shaders_and_buffers(GLuint* program, GLuint* vao, GLuint* vbo) {
    const char* vs_src =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "    TexCoord = texCoord;\n"
        "}\n";

    const char* fs_src =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D textTexture;\n"
        "uniform vec4 textColor;\n"
        "void main() {\n"
        "    float alpha = texture(textTexture, TexCoord).r;\n"
        "    FragColor = vec4(textColor.rgb, alpha * textColor.a);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src, NULL);
    glCompileShader(vs);
    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(vs, 512, NULL, info_log);
        printf("Vertex shader compilation failed: %s\n", info_log);
        return 0;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(fs, 512, NULL, info_log);
        printf("Fragment shader compilation failed: %s\n", info_log);
        glDeleteShader(vs);
        return 0;
    }

    *program = glCreateProgram();
    glAttachShader(*program, vs);
    glAttachShader(*program, fs);
    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(*program, 512, NULL, info_log);
        printf("Program linking failed: %s\n", info_log);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    return 1;
}















