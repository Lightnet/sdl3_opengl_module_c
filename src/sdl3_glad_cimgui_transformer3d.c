// 
#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cimgui.h>
#include <cimgui_impl.h>

#include "module_font.h"
#include <cglm/cglm.h> // Include CGLM

#define igGetIO igGetIO_Nil

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        fprintf(stderr, "Failed to init video! %s\n", SDL_GetError());
        return 1;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL;
    SDL_Window* window = SDL_CreateWindow("SDL3 OpenGL Font Tranformer 3d", (int)(1280 * main_scale), (int)(720 * main_scale), window_flags);
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

    FontData *font_data = NULL;
    if (!init_font("resources/Kenney Mini.ttf", 32.0f, main_scale, &font_data)) {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize shaders and buffers
    GLuint program, vao, vbo;
    if (!init_font_shaders_and_buffers(&program, &vao, &vbo)) {
        // cleanup_font(&font_data);
        cleanup_font(font_data);
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

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

    float clear_colorE4[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    ImVec4 clearColorE3;
    clearColorE3.x = 0.45f;
    clearColorE3.y = 0.55f;
    clearColorE3.z = 0.60f;
    clearColorE3.w = 1.00f;

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
            igText("Choose a color:");
            if(igColorEdit4("clear color E4", (float*)&clear_colorE4, 0)){
                // Log the new color values whenever they change
                printf("Color changed to: R=%.2f, G=%.2f, B=%.2f, A=%.2f\n", 
                    clear_colorE4[0], clear_colorE4[1], clear_colorE4[2], clear_colorE4[3]);
            }
            igColorEdit3("clear color E3", (float *)&clearColorE3, 0);

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
        glClearColor(clear_colorE4[0], clear_colorE4[1], clear_colorE4[2], clear_colorE4[3]);
        // glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);




        // Render 2D text
        // render_text(&font_data, program, vao, vbo, text, 25.0f, 150.0f, ww, hh, 1.0f, 1.0f, 1.0f, 1.0f);// test
        render_text(font_data, program, vao, vbo, "Hello, World!", 100.0f, 100.0f, ww, hh, 1.0f, 1.0f, 1.0f, 1.0f);

        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    igDestroyContext(NULL);
    
    cleanup_font(font_data); // conflict due cimgui modified font match data type.
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}