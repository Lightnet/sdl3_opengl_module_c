#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cimgui.h>
#include <cimgui_impl.h>

#define igGetIO igGetIO_Nil

// #define STB_TRUETYPE_IMPLEMENTATION
// #include "stb_truetype.h"

// typedef struct {
//     GLuint texture;         // Font texture ID
//     stbtt_bakedchar* cdata; // Character data (ASCII 32..127)
//     int bitmap_w, bitmap_h; // Bitmap dimensions
// } FontData;

// Initialize font: Load TTF and bake bitmap
// int init_font(const char* font_path, float font_size, float scale, FontData* font_data) {
//     unsigned char* ttf_buffer = (unsigned char*)malloc(1 << 20);
//     if (!ttf_buffer) {
//         printf("Error: Failed to allocate TTF buffer\n");
//         return 0;
//     }

//     FILE* ff = fopen(font_path, "rb");
//     if (!ff) {
//         printf("Error: Failed to open font file '%s'\n", font_path);
//         free(ttf_buffer);
//         return 0;
//     }

//     fread(ttf_buffer, 1, 1 << 20, ff);
//     fclose(ff);

//     font_data->bitmap_w = 512;
//     font_data->bitmap_h = 512;
//     unsigned char* bitmap = (unsigned char*)malloc(font_data->bitmap_w * font_data->bitmap_h);
//     if (!bitmap) {
//         printf("Error: Failed to allocate bitmap\n");
//         free(ttf_buffer);
//         return 0;
//     }

//     font_data->cdata = (stbtt_bakedchar*)malloc(96 * sizeof(stbtt_bakedchar));
//     if (!font_data->cdata) {
//         printf("Error: Failed to allocate cdata\n");
//         free(bitmap);
//         free(ttf_buffer);
//         return 0;
//     }

//     stbtt_BakeFontBitmap(ttf_buffer, 0, font_size * scale, bitmap, font_data->bitmap_w, font_data->bitmap_h, 32, 96, font_data->cdata);
//     free(ttf_buffer);

//     // Create OpenGL texture
//     glGenTextures(1, &font_data->texture);
//     glBindTexture(GL_TEXTURE_2D, font_data->texture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_data->bitmap_w, font_data->bitmap_h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     free(bitmap);

//     return 1;
// }

// Initialize shaders and VAO/VBO
int init_shaders_and_buffers(GLuint* program, GLuint* vao, GLuint* vbo) {
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

// // Render text
// void render_text(FontData* font_data, GLuint program, GLuint vao, GLuint vbo, const char* text, float x, float y, int ww, int hh, float r, float g, float b, float a) {
//     float vertices[1024 * 4]; // Enough for simple text
//     int vert_count = 0;

//     for (const char* p = text; *p; p++) {
//         if (*p >= 32 && *p < 128) {
//             stbtt_aligned_quad q;
//             stbtt_GetBakedQuad(font_data->cdata, font_data->bitmap_w, font_data->bitmap_h, *p - 32, &x, &y, &q, 1);

//             float nx0 = 2.0f * q.x0 / ww - 1.0f;
//             float ny0 = 1.0f - 2.0f * q.y0 / hh;
//             float nx1 = 2.0f * q.x1 / ww - 1.0f;
//             float ny1 = 1.0f - 2.0f * q.y1 / hh;

//             // Triangle 1
//             vertices[vert_count++] = nx0; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s0; vertices[vert_count++] = q.t0;
//             vertices[vert_count++] = nx1; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t0;
//             vertices[vert_count++] = nx1; vertices[vert_count++] = ny1; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t1;

//             // Triangle 2
//             vertices[vert_count++] = nx0; vertices[vert_count++] = ny0; vertices[vert_count++] = q.s0; vertices[vert_count++] = q.t0;
//             vertices[vert_count++] = nx1; vertices[vert_count++] = ny1; vertices[vert_count++] = q.s1; vertices[vert_count++] = q.t1;
//             vertices[vert_count++] = nx0; vertices[vert_count++] = ny1; vertices[vert_count++] = q.s0; vertices[vert_count++] = q.t1;
//         }
//     }

//     glUseProgram(program);
//     glUniform1i(glGetUniformLocation(program, "textTexture"), 0);
//     glUniform4f(glGetUniformLocation(program, "textColor"), r, g, b, a);
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, font_data->texture);

//     glBindVertexArray(vao);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float), vertices, GL_DYNAMIC_DRAW);
//     glDrawArrays(GL_TRIANGLES, 0, vert_count / 4);
//     glBindVertexArray(0);
// }

// // Clean up font resources
// void cleanup_font(FontData* font_data) {
//     glDeleteTextures(1, &font_data->texture);
//     free(font_data->cdata);
// }

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        fprintf(stderr, "Failed to init video! %s\n", SDL_GetError());
        return 1;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL;
    SDL_Window* window = SDL_CreateWindow("SDL3 OpenGL Font Example", (int)(1280 * main_scale), (int)(720 * main_scale), window_flags);
    if (!window) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Create OpenGL context
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

    // Initialize font
    // FontData font_data = {0};
    // if (!init_font("resources/Kenney Mini.ttf", 32.0f, main_scale, &font_data)) {
    //     SDL_GL_DestroyContext(gl_context);
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     return -1;
    // }

    // Setup Dear ImGui context
    igCreateContext(NULL);
    ImGuiIO* io = igGetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Dear ImGui style
    igStyleColorsDark(NULL);
    ImGuiStyle* style = igGetStyle();
    ImGuiStyle_ScaleAllSizes(style, main_scale);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    

    // Initialize shaders and buffers
    GLuint program, vao, vbo;
    if (!init_shaders_and_buffers(&program, &vao, &vbo)) {
        // cleanup_font(&font_data);
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};
    bool done = false;
    const char* text = "Hello World! Glad 2.0.8";

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        igNewFrame();

        // Simple window
        {
            static float f = 0.0f;
            static int counter = 0;
            igBegin("Hello, world!", NULL, 0);
            igText("This is some useful text.");
            igCheckbox("Demo Window", &show_demo_window);
            igCheckbox("Another Window", &show_another_window);
            igSliderFloat("float", &f, 0.0f, 1.0f, "%.3f", 0);
            igColorEdit4("clear color", (float*)&clear_color, 0);
            ImVec2 buttonSize = {0, 0};
            if (igButton("Button", buttonSize))
                counter++;
            igSameLine(0.0f, -1.0f);
            igText("counter = %d", counter);
            igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
            igEnd();
        }

        // Rendering
        igRender();



        int ww, hh;
        SDL_GetWindowSize(window, &ww, &hh);
        glViewport(0, 0, ww, hh);
        glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y); //cimgui
        glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render text
        // render_text(&font_data, program, vao, vbo, text, 25.0f, 150.0f, ww, hh, 1.0f, 1.0f, 1.0f, 1.0f);

        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    igDestroyContext(NULL);
    
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}