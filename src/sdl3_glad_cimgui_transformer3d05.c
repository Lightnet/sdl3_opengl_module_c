// 

#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cimgui.h>
#include <cimgui_impl.h>

#include "module_font.h"
#include <cglm/cglm.h> // Include CGLM
#include "flecs.h"

#define igGetIO igGetIO_Nil

typedef struct {
    float x;
    float y;
} Position;

typedef struct {
    float x;
    float y;
} Velocity;

typedef struct {
    vec3 position; // Vector3 for position (x, y, z)
    vec3 rotation; // Euler angles in degrees (x, y, z)
    vec3 scale;    // Scale (x, y, z)
    mat4 local;    // Local transformation matrix
    mat4 world;    // World transformation matrix
    bool isDirty;  // Flag to indicate if transform needs recalculation
} Transform3D;
ECS_COMPONENT_DECLARE(Transform3D);

typedef struct {
    GLuint vao, vbo, ebo; // OpenGL buffer objects
    GLuint shaderProgram; // Shader program for the cube
    int indexCount;       // Number of indices for rendering
} CubeContext;

// Define the Transform3DContext struct
typedef struct {
    const char* name;  // Entity name (pointer to Flecs-managed string)
    ecs_entity_t id;   // Entity ID
} Transform3DContext;



// Cube vertices: position (x, y, z)
static const float cubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f, // 0
     0.5f, -0.5f,  0.5f, // 1
     0.5f,  0.5f,  0.5f, // 2
    -0.5f,  0.5f,  0.5f, // 3
    // Back face
    -0.5f, -0.5f, -0.5f, // 4
     0.5f, -0.5f, -0.5f, // 5
     0.5f,  0.5f, -0.5f, // 6
    -0.5f,  0.5f, -0.5f  // 7
};

// Indices for the cube (6 faces, 2 triangles per face, 3 vertices per triangle)
static const unsigned int cubeIndices[] = {
    // Front
    0, 1, 2,  2, 3, 0,
    // Right
    1, 5, 6,  6, 2, 1,
    // Back
    5, 4, 7,  7, 6, 5,
    // Left
    4, 0, 3,  3, 7, 4,
    // Top
    3, 2, 6,  6, 7, 3,
    // Bottom
    4, 5, 1,  1, 0, 4
};

bool init_cube_mesh(CubeContext* cube) {
    // Vertex Shader
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "}\n";

    // Fragment Shader (simple color)
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n" // Orange color
        "}\n";

    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex Shader Compilation Failed: %s\n", infoLog);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment Shader Compilation Failed: %s\n", infoLog);
        return false;
    }

    cube->shaderProgram = glCreateProgram();
    glAttachShader(cube->shaderProgram, vertexShader);
    glAttachShader(cube->shaderProgram, fragmentShader);
    glLinkProgram(cube->shaderProgram);
    glGetProgramiv(cube->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(cube->shaderProgram, 512, NULL, infoLog);
        printf("Shader Program Linking Failed: %s\n", infoLog);
        return false;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Setup VAO, VBO, EBO
    glGenVertexArrays(1, &cube->vao);
    glGenBuffers(1, &cube->vbo);
    glGenBuffers(1, &cube->ebo);

    glBindVertexArray(cube->vao);

    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    cube->indexCount = sizeof(cubeIndices) / sizeof(cubeIndices[0]);
    return true;
}

// void update_transform_system(ecs_iter_t *it) {
//     Transform3D *transforms = ecs_field(it, Transform3D, 0);
//     for (int i = 0; i < it->count; i++) {
//         if (!transforms[i].isDirty) continue;
//         mat4 local;
//         glm_mat4_identity(local);
//         glm_scale(local, transforms[i].scale);
//         glm_rotate_x(local, glm_rad(transforms[i].rotation[0]), local);
//         glm_rotate_y(local, glm_rad(transforms[i].rotation[1]), local);
//         glm_rotate_z(local, glm_rad(transforms[i].rotation[2]), local);
//         glm_translate(local, transforms[i].position);
//         glm_mat4_copy(local, transforms[i].local);
//         glm_mat4_copy(local, transforms[i].world); // No parent transform
//         transforms[i].isDirty = false;
//         printf("Updated transform %d: Pos=(%.2f, %.2f, %.2f)\n",
//                i, transforms[i].position[0], transforms[i].position[1], transforms[i].position[2]);
//     }
// }

void update_transform_system(ecs_iter_t *it) {
    Transform3D *transforms = ecs_field(it, Transform3D, 0);
    for (int i = 0; i < it->count; i++) {
        if (!transforms[i].isDirty) continue;
        // Calculate local matrix
        mat4 local;
        glm_mat4_identity(local);
        // Apply scale
        glm_scale(local, transforms[i].scale);
        // Apply rotation (Euler angles in degrees)
        glm_rotate_x(local, glm_rad(transforms[i].rotation[0]), local);
        glm_rotate_y(local, glm_rad(transforms[i].rotation[1]), local);
        glm_rotate_z(local, glm_rad(transforms[i].rotation[2]), local);
        // Apply translation
        glm_translate(local, transforms[i].position);
        glm_mat4_copy(local, transforms[i].local);
        // Check for parent transform
        ecs_entity_t parent = ecs_get_parent(it->world, it->entities[i]);
        // if (parent && ecs_has(it->world, parent, Transform3D) != 0) {
        // if (parent && ecs_has_id(it->world, parent, ecs_id(Transform3D))) {
        if (parent && ecs_is_valid(it->world, parent)) {
            Transform3D *parent_transform = ecs_get_mut(it->world, parent, Transform3D);
            if(parent_transform){
                mat4 world;
                glm_mat4_mul(parent_transform->world, local, world);
                glm_mat4_copy(world, transforms[i].world);
            }
            
        } else {
            glm_mat4_copy(local, transforms[i].world);
        }
        transforms[i].isDirty = false;
    }
}


void render_3d_cube_system(ecs_iter_t *it) {
    Transform3D *transforms = ecs_field(it, Transform3D, 0);
    CubeContext *cube = (CubeContext *)ecs_get_ctx(it->world);
    
    if (!cube) {
        printf("CubeContext is NULL in render_3d_cube_system!\n");
        return;
    }

    // printf("Rendering %d cubes\n", it->count);
    if (it->count == 0) {
        printf("No entities with Transform3D found!\n");
        return;
    }

    glUseProgram(cube->shaderProgram);

    // Ensure depth testing is enabled
    glEnable(GL_DEPTH_TEST);

    // Setup view and projection matrices
    mat4 view, projection;
    glm_mat4_identity(view);
    glm_lookat((vec3){0.0f, 0.0f, 5.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);

    int ww, hh;
    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &ww, &hh);
    glm_perspective(glm_rad(45.0f), (float)ww / hh, 0.1f, 100.0f, projection);

    GLint modelLoc = glGetUniformLocation(cube->shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(cube->shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(cube->shaderProgram, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)projection);

    glBindVertexArray(cube->vao);
    for (int i = 0; i < it->count; i++) {
        // printf("Rendering cube %d: Pos=(%.2f, %.2f, %.2f), Rot=(%.2f, %.2f, %.2f), Scale=(%.2f, %.2f, %.2f)\n",
        //        i, transforms[i].position[0], transforms[i].position[1], transforms[i].position[2],
        //        transforms[i].rotation[0], transforms[i].rotation[1], transforms[i].rotation[2],
        //        transforms[i].scale[0], transforms[i].scale[1], transforms[i].scale[2]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)transforms[i].world);
        glDrawElements(GL_TRIANGLES, cube->indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // Check for OpenGL errors
    // GLenum err;
    // while ((err = glGetError()) != GL_NO_ERROR) {
    //     printf("OpenGL Error in render_3d_cube_system: %u\n", err);
    // }
}

// nope error on attach child
void start_up_system(ecs_iter_t *it) {
    printf("start up\n");
}



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

    ecs_world_t *world = ecs_init();
    CubeContext* cube;

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    // ECS_COMPONENT(world, CubeContext);
    // ECS_COMPONENT(world, Transform3D);
    ECS_COMPONENT_DEFINE(world, Transform3D);// need to able to access for system get component

    // EcsOnStart
    // EcsPreUpdate
    // EcsOnUpdate
    // EcsPostUpdate

    // start up system
    ECS_SYSTEM(world, start_up_system, EcsOnStart);
    ECS_SYSTEM(world, update_transform_system, EcsPreUpdate, Transform3D);
    //render 3d cube
    ECS_SYSTEM(world, render_3d_cube_system, EcsOnUpdate, Transform3D);

    // Create parent cube
    ecs_entity_t parent = ecs_entity(world, { .name = "ParentCube" });
    ecs_set(world, parent, Transform3D, {
        .position = {0.0f, 0.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f},
        .isDirty = true
    });

    // Create child cube
    ecs_entity_t child = ecs_entity(world, { .name = "ChildCube" });
    ecs_set(world, child, Transform3D, {
        // .position = {2.0f, 0.0f, 0.0f}, // Offset from parent
        .position = {1.0f, 0.0f, 0.0f}, // Offset from parent
        // .position = {0.0f, 0.0f, 0.0f}, // Offset from parent
        // .rotation = {0.0f, 0.0f, 0.0f},
        .rotation = {0.0f, 45.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f},
        .isDirty = true
    });
    ecs_add_pair(world, child, EcsChildOf, parent);

    // Initialize cube mesh
    cube = malloc(sizeof(CubeContext));
    if (!init_cube_mesh(cube)) {
        printf("Failed to initialize cube mesh\n");
        // return;
    }
    ecs_set_ctx(world, cube, NULL);

    // Do the ECS stuff
    ecs_entity_t e = ecs_entity(world, { .name = "Bob" });
    printf("Entity name: %s\n", ecs_get_name(world, e));

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
            // igBegin("Hello, world!", NULL, 0);
            // igText("This is some useful text.");
            // igCheckbox("Demo Window", &show_demo_window);
            // igCheckbox("Another Window", &show_another_window);
            // igSliderFloat("float", &f, 0.0f, 1.0f, "%.3f", 0);
            // igText("Choose a color:");
            // if(igColorEdit4("clear color E4", (float*)&clear_colorE4, 0)){
            //     // Log the new color values whenever they change
            //     printf("Color changed to: R=%.2f, G=%.2f, B=%.2f, A=%.2f\n", 
            //         clear_colorE4[0], clear_colorE4[1], clear_colorE4[2], clear_colorE4[3]);
            // }
            // igColorEdit3("clear color E3", (float *)&clearColorE3, 0);

            // ImVec2 buttonSize = {0, 0};
            // if (igButton("Button", buttonSize))
            //     counter++;
            // igSameLine(0.0f, -1.0f);
            // igText("counter = %d", counter);
            // igText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
            // igEnd();
        }

        {
            static Transform3DContext* contexts = NULL; // Persist array between frames
            static int context_count = 0;              // Number of contexts
            static ecs_entity_t selected_id = 0;       // Track selected entity

            igBegin("transform3d", NULL, 0);
            ImVec2 buttonSize = {0, 0};
            if (igButton("query Transform3Ds", buttonSize)){

                ecs_query_t *query0 = ecs_query(world, {
                    .terms = {
                        { .id = ecs_id(Transform3D) }
                    }
                });
                ecs_iter_t it = ecs_query_iter(world, query0);

                // First, count total entities to allocate memory
                int total_count = 0;
                while (ecs_query_next(&it)) {
                    total_count += it.count;
                }

                // Resize or allocate contexts array
                if (total_count != context_count) {
                    contexts = realloc(contexts, total_count * sizeof(Transform3DContext));
                    context_count = total_count;
                }

                // Reset iterator and populate contexts
                it = ecs_query_iter(world, query0);
                int index = 0;
                while (ecs_query_next(&it)) {
                    for (int i = 0; i < it.count; i++) {
                        // contexts[index].name = ecs_get_name(world, it.entities[i]);
                        const char* name = ecs_get_name(world, it.entities[i]);
                        contexts[index].name = name ? name : "Unnamed Entity"; // Fallback for NULL name
                        contexts[index].id = it.entities[i];
                        printf("Name: %s, ID: %llu\n", contexts[index].name, contexts[index].id);
                        index++;
                    }
                }
                //clean up
                ecs_query_fini(query0);
            }

            // Display buttons for each context
            for (int i = 0; i < context_count; i++) {
                char button_label[128];
                snprintf(button_label, sizeof(button_label), "%s (ID: %llu)", 
                        contexts[i].name, contexts[i].id);
                if (igButton(button_label, buttonSize)) {
                    selected_id = contexts[i].id; // Update selected entity
                    printf("Selected Entity: %s (ID: %llu)\n", contexts[i].name, selected_id);
                }
            }

            // Display Transform3D inputs for selected entity
            if (selected_id != 0) {
                // Find the name of the selected entity
                const char* selected_name = "Unknown Entity";
                for (int i = 0; i < context_count; i++) {
                    if (contexts[i].id == selected_id) {
                        selected_name = contexts[i].name;
                        break;
                    }
                }
                // Display selected entity name
                char selected_label[128];
                snprintf(selected_label, sizeof(selected_label), "Selected: %s (ID: %llu)", 
                        selected_name, selected_id);
                igText(selected_label);
                
                Transform3D* transform = ecs_get_mut(world, selected_id, Transform3D);
                if (transform) {
                    bool changed = false;

                    // Position input fields
                    igText("Position");
                    changed |= igInputFloat("X##pos", &transform->position[0], 0.1f, 1.0f, "%.3f", 0);
                    changed |= igInputFloat("Y##pos", &transform->position[1], 0.1f, 1.0f, "%.3f", 0);
                    changed |= igInputFloat("Z##pos", &transform->position[2], 0.1f, 1.0f, "%.3f", 0);

                    // Rotation sliders (-360 to 360 degrees)
                    igText("Rotation");
                    changed |= igSliderFloat("X##rot", &transform->rotation[0], -360.0f, 360.0f, "%.1f", 0);
                    changed |= igSliderFloat("Y##rot", &transform->rotation[1], -360.0f, 360.0f, "%.1f", 0);
                    changed |= igSliderFloat("Z##rot", &transform->rotation[2], -360.0f, 360.0f, "%.1f", 0);

                    // Scale input fields
                    igText("Scale");
                    changed |= igInputFloat("X##scale", &transform->scale[0], 0.1f, 1.0f, "%.3f", 0);
                    changed |= igInputFloat("Y##scale", &transform->scale[1], 0.1f, 1.0f, "%.3f", 0);
                    changed |= igInputFloat("Z##scale", &transform->scale[2], 0.1f, 1.0f, "%.3f", 0);

                    // Mark transform as dirty if changed
                    if (changed) {
                        transform->isDirty = true;
                        ecs_modified(world, selected_id, Transform3D);
                    }
                }
            }




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
        // glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Enable depth testing for 3D rendering
        glEnable(GL_DEPTH_TEST);

        // Test cube rendering
        // {
        //     printf("Test cube rendering\n");

        //     glUseProgram(cube->shaderProgram);

        //     // Setup view and projection matrices (same as render_3d_cube_system)
        //     mat4 view, projection;
        //     glm_mat4_identity(view);
        //     glm_lookat((vec3){0.0f, 0.0f, 5.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);
        //     glm_perspective(glm_rad(45.0f), (float)ww / hh, 0.1f, 100.0f, projection);

        //     // Simple model matrix (e.g., translate to (0, 0, 0) with no rotation or scale)
        //     mat4 model;
        //     glm_mat4_identity(model);
        //     // Optional: Translate to make sure it’s visible
        //     glm_translate(model, (vec3){0.0f, 0.0f, 0.0f});

        //     // Set uniforms
        //     GLint modelLoc = glGetUniformLocation(cube->shaderProgram, "model");
        //     GLint viewLoc = glGetUniformLocation(cube->shaderProgram, "view");
        //     GLint projLoc = glGetUniformLocation(cube->shaderProgram, "projection");
        //     glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);
        //     glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
        //     glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)projection);

        //     // Draw the cube
        //     glBindVertexArray(cube->vao);
        //     glDrawElements(GL_TRIANGLES, cube->indexCount, GL_UNSIGNED_INT, 0);
        //     glBindVertexArray(0);

        //     // Check for OpenGL errors
        //     GLenum err;
        //     while ((err = glGetError()) != GL_NO_ERROR) {
        //         printf("OpenGL Error during test render: %u\n", err);
        //     }
        // }
        

        ecs_progress(world, 0); // run systems in default pipeline


        // Render 2D text
        // render_text(&font_data, program, vao, vbo, text, 25.0f, 150.0f, ww, hh, 1.0f, 1.0f, 1.0f, 1.0f);// test
        render_text(font_data, program, vao, vbo, "Hello, World!", 100.0f, 100.0f, ww, hh, 1.0f, 1.0f, 1.0f, 1.0f);

        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup

    // CubeContext* cube = ecs_get_ctx(world);
    // cube = ecs_get_ctx(world);
    if (cube) {
        glDeleteVertexArrays(1, &cube->vao);
        glDeleteBuffers(1, &cube->vbo);
        glDeleteBuffers(1, &cube->ebo);
        glDeleteProgram(cube->shaderProgram);
        free(cube);
    }
    

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    igDestroyContext(NULL);
    
    cleanup_font(font_data); // conflict due cimgui modified font match data type.
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    ecs_fini(world);
    
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}