#if !defined(HEADER_GAME_H)
#define HEADER_GAME_H

#include "game_base.h"
#include "game_memory.h"
#include "game_transform.cpp"

#define CUBE3_SIZE 27
#define PLANE9x9_SIZE 81
#define ENTITY_SIZE 256
#define VERTEX_SIZE PLANE9x9_SIZE

enum View_Orientation { FRONT, LEFT, BACK, RIGHT };
enum Entity_Type      { PLANE9x9, CUBE3 };
enum Uniform_Type     { VECTOR3, VECTOR4, MATRIX4x4 };

struct Button {
    B32 held;
    B32 first;
    B32 release;
    B32 press;
    S32 glfw;
};

union Input {
    struct {
        Button W;
        Button A;
        Button S;
        Button D;
        Button H;
        Button K;
        Button F11;
    };
    Button button[7];
};

struct Window {
    S32 width, height;
    S32 wx, wy, ww, wh;
    F32 tlimit;
    B32 is_fullscreen;
};

struct STR {
    U8   size;
    char e[256];
    RGB color;
    V2  position;
    F32 scale;
};

struct Buffer {
    U32  size;
    RGB  *color;
    M4x4 *anim;

    U32  text_size;
    STR  *text;

    M4x4 view;
    M4x4 projection;
    V3   eye;
    V3   light;
};

struct Memory {
    U64  main_size;
    U64 *main;
};

struct Bounding_Box {
    F32 front;
    F32 back;
    F32 left;
    F32 right;
    F32 top;
    F32 bottom;
};

struct Entity {
    Entity_Type type;

    B32 is_alive;
    B32 is_active;

    RGB color;
    V3  scale;
    V3  center;

    U32  size;
    V3   vertex[VERTEX_SIZE];
    V3   boundary[VERTEX_SIZE];
    M4x4 transform[VERTEX_SIZE];
};

struct State {
    B32 is_init;
    B32 is_game_active;

    F32 game_speed;
    V3  movement;

    F32 angle;
    F32 target_angle;
    F32 current_angle;
    V3  rotation_axis;

    S32 frame_counter;
    S32 target_frame;
    S32 current_frame;
    char *move_char;
    char *game_over_text;

    V3  plane_pos;
    V3  cube_pos;
    V3  plane_scale;
    V3  cube_scale;
    RGB cube_color;
    RGB plane_color;

    V3   eye;
    V3   light;
    M4x4 view;
    M4x4 projection;

    U32 text_size;
    STR text[10];

    S32 score;

    U8 active_index;
    U8 plane_index;
    U8 inactive_index_size;
    U8 inactive_index[ENTITY_SIZE];

    Entity entity_array[ENTITY_SIZE];
    Arena arena;
};

#endif
