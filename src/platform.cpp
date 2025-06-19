#include "game.cpp"
#include "platform_defines.h"

#if ENABLE_BINARY
#include "cube_glsl.h"
#include "text_glsl.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define BUFFER_OFFSET(x) ((U8 *)0 + (x))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(0[x]))

struct Shader_Str {
    S32 vsize;
    S32 fsize;
    char *vstr;
    char *fstr;
};

struct Character {
    U32 TextureID;
    V2 Size;
    V2 Bearing;
    U32 Advance;
};

struct GL_Context {
    GLFWwindow  *window;
    GLFWmonitor *monitor;
};

global GL_Context context;
global std::map<GLchar, Character> Characters;


static void platform_process_input(Input *input, Window *window) {
    input->W.glfw   = GLFW_KEY_W;
    input->A.glfw   = GLFW_KEY_A;
    input->S.glfw   = GLFW_KEY_S;
    input->D.glfw   = GLFW_KEY_D;
    input->H.glfw   = GLFW_KEY_H;
    input->K.glfw   = GLFW_KEY_K;
    input->F11.glfw = GLFW_KEY_F11;

    for (U32 i = 0; i < ARRAY_SIZE(input->button); i++) {
        Button *button = &input->button[i];
        S32 state = glfwGetKey(context.window, button->glfw);

        if (state == GLFW_RELEASE) {
            button->held    = 0;
            button->first   = 0;
            button->press   = 0;
            button->release = 1;
        }
        if (state == GLFW_PRESS) {
            button->held    = (button->press) ? 1 : 0;
            button->first   = (!button->press) ? 1 : 0;
            button->press   = 1;
            button->release = 0;
        }
    }

    const GLFWvidmode *mode = glfwGetVideoMode(context.monitor);
    window->tlimit = (1.0f / mode->refreshRate);

    if (input->F11.first) {
        if (!window->is_fullscreen) {
            glfwGetWindowPos(context.window, &window->wx, &window->wy);
            glfwGetWindowSize(context.window, &window->ww, &window->wh);
            glfwSetWindowMonitor(context.window, context.monitor,
                                 0, 0, mode->width, mode->height,
                                 mode->refreshRate);

            window->is_fullscreen = true;
        } else {
            glfwSetWindowMonitor(context.window, 0,
                                 window->wx, window->wy - 37,
                                 window->ww, window->wh + 37,
                                 mode->refreshRate);

            window->is_fullscreen = false;
        }
    }
}

static void platform_read_glsl(const char *name, Shader_Str *shader) {
    FILE *fptr = fopen(name, "r");

    if (!fptr) {
        fprintf(stderr, "ERROR: Could not open file %s.\n", name);
        exit(1);
    }

    B32 flag = 2;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), fptr)) {
        if (!strcmp("// VERTEX_SHADER\n", buffer)) {
            shader->vsize += strlen(buffer);

            if (flag == 1) {
                while (fgets(buffer, sizeof(buffer), fptr))
                    shader->vsize += strlen(buffer);
            } else if (flag == 2) {
                while (fgets(buffer, sizeof(buffer), fptr)) {
                    if (!strcmp("// FRAGMENT_SHADER\n", buffer)) break;
                    shader->vsize += strlen(buffer);
                }
            }

            flag = 0;
        }
        if (!strcmp("// FRAGMENT_SHADER\n", buffer)) {
            shader->fsize += strlen(buffer);

            if (flag == 0) {
                while (fgets(buffer, sizeof(buffer), fptr))
                    shader->fsize += strlen(buffer);
            } else if (flag == 2) {
                while (fgets(buffer, sizeof(buffer), fptr)) {
                    if (!strcmp("// VERTEX_SHADER\n", buffer)) break;
                    shader->fsize += strlen(buffer);
                }
            }

            flag = 1;
        }
    }

    rewind(fptr);

    if (!(shader->vsize && shader->fsize)) {
        fprintf(stderr, "ERROR: Shader is not complete [VS: %d, FS: %d]\n", shader->vsize, shader->fsize);
        exit(1);
    }

    shader->vstr = (char *)malloc(shader->vsize + 1);
    shader->fstr = (char *)malloc(shader->fsize + 1);

    fread(shader->vstr, shader->vsize, 1, fptr);
    fread(shader->fstr, shader->fsize, 1, fptr);

    shader->vstr[-1] = '\0';
    shader->fstr[-1] = '\0';

    fclose(fptr);
}

static GLuint platform_create_shader(const char *filename) {
    Shader_Str cube = {};
    platform_read_glsl(filename, &cube);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vs, 1, &cube.vstr, &cube.vsize);
    glShaderSource(fs, 1, &cube.fstr, &cube.fsize);

    glCompileShader(vs);
    glCompileShader(fs);

    char output[1024] = {};

    glGetShaderInfoLog(vs, 1024, &cube.vsize, output);
    printf("VS INFO LOG: %s\n", output);

    glGetShaderInfoLog(fs, 1024, &cube.fsize, output);
    printf("FS INFO LOG: %s\n", output);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);
    glValidateProgram(shader);

    GLint status;
    glGetProgramiv(shader, GL_LINK_STATUS, &status);

    if (!status) {
        fprintf(stderr, "SHADER LINKING FAILED!!!\n");
        exit(1);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return shader;
}

static GLuint platform_create_shader(const char *vsource, const char* fsource) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    GLint vsize = strlen(vsource);
    GLint fsize = strlen(fsource);
    glShaderSource(vs, 1, &vsource, &vsize);
    glShaderSource(fs, 1, &fsource, &fsize);

    glCompileShader(vs);
    glCompileShader(fs);

    char output[1024] = {};

    glGetShaderInfoLog(vs, 1024, 0, output);
    printf("VS INFO LOG: %s\n", output);

    glGetShaderInfoLog(fs, 1024, 0, output);
    printf("FS INFO LOG: %s\n", output);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);
    glValidateProgram(shader);

    GLint status;
    glGetProgramiv(shader, GL_LINK_STATUS, &status);

    if (!status) {
        fprintf(stderr, "SHADER LINKING FAILED!!!\n");
        exit(1);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return shader;
}
static void set_shader_uniform(GLuint shader, const char *name, M4x4 matrix) {
    GLfloat gl_matrix[16];
    int index = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gl_matrix[index++] = matrix.e[j][i];
        }
    }

    GLuint location = glGetUniformLocation(shader, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, gl_matrix);
}

static void set_shader_uniform(GLuint shader, const char *name, V3 vector) {
    GLfloat gl_vector[3] = {vector.x, vector.y, vector.z};
    GLuint location = glGetUniformLocation(shader, name);
    glUniform3fv(location, 1, gl_vector);
}

static void render_text(Window *window, STR *text) {
    V2 size_sum = {};

    for (char *c = text->e; *c; c++) {
        Character ch = Characters[*c];
        GLfloat h    = ch.Size.uy * text->scale;

        size_sum.x += ((ch.Advance >> 6) * text->scale);
        size_sum.y = fmaxf(size_sum.y, h);
    }

    F32 x = text->position.x;
    F32 y = text->position.y;

    if ((x + size_sum.x) > window->width)
        x = window->width - size_sum.x;
    if ((y + size_sum.y) > window->height)
        y = window->height - size_sum.y;


    for (char *c = text->e; *c; c++) {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.ix * text->scale;
        GLfloat ypos = y - (ch.Size.uy - ch.Bearing.iy) * text->scale;

        GLfloat w = ch.Size.ux * text->scale;
        GLfloat h = ch.Size.uy * text->scale;

        size_sum.x += ((ch.Advance >> 6) * text->scale);
        size_sum.y = fmaxf(size_sum.y, h);

        char_data[0] = xpos;
        char_data[1] = ypos + h;
        char_data[4] = xpos;
        char_data[5] = ypos;
        char_data[8] = xpos + w;
        char_data[9] = ypos;
        char_data[12] = xpos;
        char_data[13] = ypos + h;
        char_data[16] = xpos + w;
        char_data[17] = ypos;
        char_data[20] = xpos + w;
        char_data[21] = ypos + h;

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBufferSubData(GL_ARRAY_BUFFER, cube_vertex_size + cube_normal_size, sizeof(char_data), char_data);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(cube_index_size + line_index_size));

        x += (ch.Advance >> 6) * text->scale;
    }
}

int main() {
    // Create Window
    //
    glfwInit();

    context.monitor         = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(context.monitor);

    Window window = {};
    window.width  = mode->width / 1.5;
    window.height = mode->height / 1.5;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    context.window = glfwCreateWindow(window.width, window.height, title, 0, 0);

    S32 x = (mode->width - window.width) / 2;
    S32 y = (mode->height - window.height) / 2;
    glfwSetWindowPos(context.window, x, y);

    glfwMakeContextCurrent(context.window);

    // OpenGL Settings
    //
    glewInit();
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glLineWidth(3);
    glPolygonOffset(0.5, 0.5);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Font Initialization
    //
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "ERROR: Could not init FreeType Library\n");
    }

    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 0, &face)) {
        fprintf(stderr, "ERROR: Failed to load font\n");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("ERROR: Failed to load Glyph\n");
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character  = {};
        character.TextureID  = texture;
        character.Size.ux    = face->glyph->bitmap.width;
        character.Size.uy    = face->glyph->bitmap.rows;
        character.Bearing.ix = face->glyph->bitmap_left;
        character.Bearing.iy = face->glyph->bitmap_top;
        character.Advance    = face->glyph->advance.x;

        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Create Shaders
    //
#if ENABLE_BINARY
    GLuint shader  = platform_create_shader(cube_vs, cube_fs);
    GLuint fshader = platform_create_shader(text_vs, text_fs);
#else
    GLuint shader  = platform_create_shader("shaders/cube.glsl");
    GLuint fshader = platform_create_shader("shaders/text.glsl");
#endif

    // Set OpenGL Buffers
    //
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, vbo_size, 0, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_size, 0, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, cube_vertex_size, cube_vertex);
    glBufferSubData(GL_ARRAY_BUFFER, cube_vertex_size, cube_normal_size, cube_normal);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, cube_index_size, cube_index);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cube_index_size, line_index_size, line_index);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cube_index_size + line_index_size, char_index_size, char_index);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube_vertex_size));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(cube_vertex_size + cube_normal_size));

    Memory memory = {};
    memory.main_size = MEGABYTES(50);
    memory.main = (U64 *)malloc(memory.main_size);

    Buffer buffer = {};
    Input input = {};

    while (!glfwWindowShouldClose(context.window)) {
        F32 frame_start = glfwGetTime();

        platform_process_input(&input, &window);
        glfwGetFramebufferSize(context.window, &window.width, &window.height);
        glViewport(0, 0, window.width, window.height);

        game_update_render(&input, &buffer, &memory, window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        set_shader_uniform(shader, "view", buffer.view);
        set_shader_uniform(shader, "projection", buffer.projection);
        set_shader_uniform(shader, "eye_position", buffer.eye);
        set_shader_uniform(shader, "light_position", buffer.light);

        for (U32 i = 0; i < buffer.size; i++) {
            set_shader_uniform(shader, "kd", buffer.color[i]);
            set_shader_uniform(shader, "model", buffer.anim[i]);

            glDrawElements(GL_TRIANGLES, ARRAY_SIZE(cube_index), GL_UNSIGNED_INT, 0);
        }

        glUseProgram(shader);
        set_shader_uniform(shader, "view", buffer.view);
        set_shader_uniform(shader, "projection", buffer.projection);
        set_shader_uniform(shader, "eye_position", buffer.eye);
        set_shader_uniform(shader, "light_position", buffer.light);

        for (U32 i = 0; i < buffer.size; i++) {
            set_shader_uniform(shader, "kd", (V3){1, 1, 1});
            set_shader_uniform(shader, "model", buffer.anim[i]);

            for (int j = 0; j < 6; j++) {
                glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(cube_index_size + (j * 4 * sizeof(U32))));
            }
        }


        M4x4 projection = ortho(window.width, window.height, 0, 1);

        glUseProgram(fshader);
        set_shader_uniform(fshader, "projection", projection);

        for (U32 i = 0; i < buffer.text_size; i++) {
            set_shader_uniform(fshader, "text_color", buffer.text[i].color);
            render_text(&window, &buffer.text[i]);
        }

        glfwSwapBuffers(context.window);
        glfwPollEvents();

        F32 frame_end = glfwGetTime();
        F32 tframe = frame_start - frame_end;

        while (tframe < window.tlimit) {
            sleep(window.tlimit - tframe);
            tframe = glfwGetTime() - frame_start;
        }

        printf("MS per frame %.2fms\n", tframe * 1000);
    }

    return 0;
}
