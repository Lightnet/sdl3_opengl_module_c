// module_cube.c

#include "module_cube.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Initialize cube shaders
int init_cube_shaders_and_buffers(GLuint* cube_program) {
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
    glShaderSource(vs, 1, &cube_vs_src, NULL);
    glCompileShader(vs);
    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(vs, 512, NULL, info_log);
        printf("Cube vertex shader compilation failed: %s\n", info_log);
        return 0;
    }
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
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


