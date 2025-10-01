#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h> // Include CGLM

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*
Controls:
Q: Rotate cube positively around X-axis.
A: Rotate cube negatively around X-axis.
W: Rotate cube positively around Y-axis.
S: Rotate cube negatively around Y-axis.
E: Rotate cube positively around Z-axis.
D: Rotate cube negatively around Z-axis.
*/

// FontData and CubeData structures (unchanged)
typedef struct {
    GLuint texture;
    stbtt_bakedchar* cdata;
    int bitmap_w, bitmap_h;
} FontData;

typedef struct {
    GLuint texture;
    GLuint vao, vbo;
} CubeData;

// Initialize font (unchanged)
int init_font(const char* font_path, float font_size, float scale, FontData* font_data) {
    unsigned char* ttf_buffer = (unsigned char*)malloc(1 << 20);
    if (!ttf_buffer) {
        printf("Error: Failed to allocate TTF buffer\n");
        return 0;
    }
    FILE* ff = fopen(font_path, "rb");
    if (!ff) {
        printf("Error: Failed to open font file '%s'\n", font_path);
        free(ttf_buffer);
        return 0;
    }
    fread(ttf_buffer, 1, 1 << 20, ff);
    fclose(ff);
    font_data->bitmap_w = 512;
    font_data->bitmap_h = 512;
    unsigned char* bitmap = (unsigned char*)malloc(font_data->bitmap_w * font_data->bitmap_h);
    if (!bitmap) {
        printf("Error: Failed to allocate bitmap\n");
        free(ttf_buffer);
        return 0;
    }
    font_data->cdata = (stbtt_bakedchar*)malloc(96 * sizeof(stbtt_bakedchar));
    if (!font_data->cdata) {
        printf("Error: Failed to allocate cdata\n");
        free(bitmap);
        free(ttf_buffer);
        return 0;
    }
    stbtt_BakeFontBitmap(ttf_buffer, 0, font_size * scale, bitmap, font_data->bitmap_w, font_data->bitmap_h, 32, 96, font_data->cdata);
    free(ttf_buffer);
    glGenTextures(1, &font_data->texture);
    glBindTexture(GL_TEXTURE_2D, font_data->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_data->bitmap_w, font_data->bitmap_h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(bitmap);
    return 1;
}

// Initialize cube (unchanged)
int init_cube(const char* texture_path, CubeData* cube_data) {
    int width, height, channels;
    unsigned char* image = stbi_load(texture_path, &width, &height, &channels, 4);
    if (!image) {
        printf("Error: Failed to load texture '%s'\n", texture_path);
        return 0;
    }
    glGenTextures(1, &cube_data->texture);
    glBindTexture(GL_TEXTURE_2D, cube_data->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Changed to GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Changed to GL_NEAREST

    stbi_image_free(image);

    float vertices[] = {
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,
        4, 5, 6,  6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
    glGenVertexArrays(1, &cube_data->vao);
    glGenBuffers(1, &cube_data->vbo);
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindVertexArray(cube_data->vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube_data->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return 1;
}

// Initialize shaders (unchanged)
int init_shaders_and_buffers(GLuint* text_program, GLuint* cube_program, GLuint* text_vao, GLuint* text_vbo) {
    const char* text_vs_src =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "    TexCoord = texCoord;\n"
        "}\n";
    const char* text_fs_src =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D textTexture;\n"
        "uniform vec4 textColor;\n"
        "void main() {\n"
        "    float alpha = texture(textTexture, TexCoord).r;\n"
        "    FragColor = vec4(textColor.rgb, alpha * textColor.a);\n"
        "}\n";
    const char* cube_vs_src =
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec2 texCoord;\n"
        "layout(location = 2) in vec3 normal;\n"
        "out vec2 TexCoord;\n"
        "out vec3 Normal;\n"
        "out vec3 FragPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "    gl_Position = projection * view * model * vec4(position, 1.0);\n"
        "    TexCoord = texCoord;\n"
        "    Normal = mat3(transpose(inverse(model))) * normal;\n"
        "    FragPos = vec3(model * vec4(position, 1.0));\n"
        "}\n";
    const char* cube_fs_src =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "in vec3 Normal;\n"
        "in vec3 FragPos;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D cubeTexture;\n"
        "uniform vec3 lightPos;\n"
        "uniform vec3 lightColor;\n"
        "void main() {\n"
        "    vec3 norm = normalize(Normal);\n"
        "    vec3 lightDir = normalize(lightPos - FragPos);\n"
        "    float diff = max(dot(norm, lightDir), 0.0);\n"
        "    vec3 diffuse = diff * lightColor;\n"
        "    vec3 ambient = 0.1 * lightColor;\n"
        "    vec4 texColor = texture(cubeTexture, TexCoord);\n"
        "    FragColor = vec4((ambient + diffuse) * texColor.rgb, texColor.a);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &text_vs_src, NULL);
    glCompileShader(vs);
    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(vs, 512, NULL, info_log);
        printf("Text vertex shader compilation failed: %s\n", info_log);
        return 0;
    }
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &text_fs_src, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(fs, 512, NULL, info_log);
        printf("Text fragment shader compilation failed: %s\n", info_log);
        glDeleteShader(vs);
        return 0;
    }
    *text_program = glCreateProgram();
    glAttachShader(*text_program, vs);
    glAttachShader(*text_program, fs);
    glLinkProgram(*text_program);
    glGetProgramiv(*text_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(*text_program, 512, NULL, info_log);
        printf("Text program linking failed: %s\n", info_log);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &cube_vs_src, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(vs, 512, NULL, info_log);
        printf("Cube vertex shader compilation failed: %s\n", info_log);
        return 0;
    }
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &cube_fs_src, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(fs, 512, NULL, info_log);
        printf("Cube fragment shader compilation failed: %s\n", info_log);
        glDeleteShader(vs);
        return 0;
    }
    *cube_program = glCreateProgram();
    glAttachShader(*cube_program, vs);
    glAttachShader(*cube_program, fs);
    glLinkProgram(*cube_program);
    glGetProgramiv(*cube_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(*cube_program, 512, NULL, info_log);
        printf("Cube program linking failed: %s\n", info_log);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, text_vao);
    glGenBuffers(1, text_vbo);
    glBindVertexArray(*text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, *text_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    return 1;
}

// Render text (unchanged)
void render_text(FontData* font_data, GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a) {
    float vertices[1024 * 4];
    int vert_count = 0;
    for (const char* p = text; *p; p++) {
        if (*p >= 32 && *p < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font_data->cdata, font_data->bitmap_w, font_data->bitmap_h, *p - 32, &x, &y, &q, 1);
            float nx0 = 2.0f * q.x0 / ww - 1.0f;
            float ny0 = 1.0f - 2.0f * q.y0 / hh;
            float nx1 = 2.0f * q.x1 / ww - 1.0f;
            float ny1 = 1.0f - 2.0f * q.y1 / hh;
            vertices[vert_count++] = nx0; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s0; vertices[vert_count++] = q.t0;
            vertices[vert_count++] = nx1; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t0;
            vertices[vert_count++] = nx1; vertices[vert_count++] = ny1; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t1;
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

// Render cube with CGLM matrices
void render_cube(CubeData* cube_data, GLuint program, vec3 rotation, int ww, int hh) {
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "cubeTexture"), 0);
    glUniform3f(glGetUniformLocation(program, "lightPos"), 0.0f, 0.0f, 3.0f);
    glUniform3f(glGetUniformLocation(program, "lightColor"), 1.0f, 1.0f, 1.0f);

    mat4 model, view, projection;
    glm_mat4_identity(model);
    glm_rotate_x(model, glm_rad(rotation[0]), model);
    glm_rotate_y(model, glm_rad(rotation[1]), model);
    glm_rotate_z(model, glm_rad(rotation[2]), model);
    glm_lookat((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);
    glm_perspective(glm_rad(45.0f), (float)ww / hh, 0.1f, 100.0f, projection);

    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube_data->texture);
    glBindVertexArray(cube_data->vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Clean up font and cube (unchanged)
void cleanup_font(FontData* font_data) {
    glDeleteTextures(1, &font_data->texture);
    free(font_data->cdata);
}

void cleanup_cube(CubeData* cube_data) {
    glDeleteTextures(1, &cube_data->texture);
    glDeleteBuffers(1, &cube_data->vbo);
    glDeleteVertexArrays(1, &cube_data->vao);
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        fprintf(stderr, "Failed to init video! %s\n", SDL_GetError());
        return 1;
    }
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL;
    SDL_Window* window = SDL_CreateWindow("SDL3 OpenGL Font and Cube Example", (int)(1280 * main_scale), (int)(720 * main_scale), window_flags);
    if (!window) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_GL_MakeCurrent(window, gl_context);

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    if (version == 0) {
        printf("Failed to initialize GLAD\n");
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    printf("OpenGL loaded: version %s\n", glGetString(GL_VERSION));

    FontData font_data = {0};
    if (!init_font("resources/Kenney Mini.ttf", 32.0f, main_scale, &font_data)) {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    CubeData cube_data = {0};
    if (!init_cube("resources/ph16.png", &cube_data)) {
        cleanup_font(&font_data);
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    GLuint text_program, cube_program, text_vao, text_vbo;
    if (!init_shaders_and_buffers(&text_program, &cube_program, &text_vao, &text_vbo)) {
        cleanup_font(&font_data);
        cleanup_cube(&cube_data);
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};
    bool done = false;
    const char* text = "Hello World! Glad 2.0.8";
    vec3 rotation = {0.0f, 0.0f, 0.0f}; // Track rotation angles (X, Y, Z)
    bool keys[6] = {false}; // Q, W, E, A, S, D
    const float rotation_speed = 1.0f; // Degrees per frame

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_Q: keys[0] = true; break;
                    case SDLK_W: keys[1] = true; break;
                    case SDLK_E: keys[2] = true; break;
                    case SDLK_A: keys[3] = true; break;
                    case SDLK_S: keys[4] = true; break;
                    case SDLK_D: keys[5] = true; break;
                }
            }
            if (event.type == SDL_EVENT_KEY_UP) {
                switch (event.key.key) {
                    case SDLK_Q: keys[0] = false; break;
                    case SDLK_W: keys[1] = false; break;
                    case SDLK_E: keys[2] = false; break;
                    case SDLK_A: keys[3] = false; break;
                    case SDLK_S: keys[4] = false; break;
                    case SDLK_D: keys[5] = false; break;
                }
            }
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // Update rotation based on key states
        if (keys[0]) rotation[0] += rotation_speed; // Q: +X
        if (keys[3]) rotation[0] -= rotation_speed; // A: -X
        if (keys[1]) rotation[1] += rotation_speed; // W: +Y
        if (keys[4]) rotation[1] -= rotation_speed; // S: -Y
        if (keys[2]) rotation[2] += rotation_speed; // E: +Z
        if (keys[5]) rotation[2] -= rotation_speed; // D: -Z

        int ww, hh;
        SDL_GetWindowSize(window, &ww, &hh);
        glViewport(0, 0, ww, hh);
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_cube(&cube_data, cube_program, rotation, ww, hh);
        render_text(&font_data, text_program, text_vao, text_vbo, text, 25.0f, 150.0f, ww, hh, 1.0f, 1.0f, 1.0f, 1.0f);

        SDL_GL_SwapWindow(window);
    }


    //clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    igDestroyContext(NULL);

    cleanup_font(&font_data);
    cleanup_cube(&cube_data);
    glDeleteProgram(text_program);
    glDeleteProgram(cube_program);
    glDeleteBuffers(1, &text_vbo);
    glDeleteVertexArrays(1, &text_vao);
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}