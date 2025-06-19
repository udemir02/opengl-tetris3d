#if !defined(SOURCE_GAME_CPP)
#define SOURCE_GAME_CPP

#include "game.h"

#include <stdio.h>
#include <string.h>

static void set_inactive_index(State *state) {
    state->inactive_index_size = 0;

    for (S32 i = 0; i < ENTITY_SIZE; i++) {
        if (state->entity_array[i].is_alive && (i != state->active_index)) {
            state->inactive_index[state->inactive_index_size++] = i;
        }
    }
}

static Entity *get_inactive_entity(State *state, U8 index) {
    Entity *result = &state->entity_array[state->inactive_index[index]];
    return result;
}

static Entity *get_active_entity(State *state) {
    Entity *result = &state->entity_array[state->active_index];
    return result;
}

static Entity *get_plane_entity(State *state) {
    Entity *result = &state->entity_array[state->plane_index];
    return result;
}

static void create_entity(State *state, Entity_Type type) {
    U8 index = 255;

    for (S32 i = 0; i < ENTITY_SIZE; i++) {
        if (!state->entity_array[i].is_alive) {
            index = i;
            break;
        }
    }

    Entity *entity = &state->entity_array[index];

    *entity = {};
    entity->type      = type;
    entity->is_alive  = true;
    entity->is_active = true;

    if (type == CUBE3) {
        entity->size   = CUBE3_SIZE;
        entity->color  = state->cube_color;
        entity->center = state->cube_pos;
        entity->scale  = state->cube_scale;

        state->active_index = index;

        V3 point = {};
        V3 bound = 3 * entity->scale;

        for (U32 i = 0; i < entity->size; i++) {
            point.x = (S32)(point.x + entity->scale.x) % (S32)bound.x;
            point.y = (i % 3) ? (S32)(point.y + entity->scale.y) % (S32)bound.y: point.y;
            point.z = (i % 9) ? (S32)(point.z + entity->scale.z) % (S32)bound.z: point.z;

            V3 offset = point - entity->scale;

            entity->vertex[i]    = entity->center + offset;
            entity->boundary[i]  = entity->vertex[i] + (entity->scale / 2);
            entity->transform[i] = scale(entity->vertex[i], entity->scale);
            entity->transform[i] *= translate(entity->vertex[i]);
        }
    } else if (type == PLANE9x9) {
        entity->size   = PLANE9x9_SIZE;
        entity->color  = state->plane_color;
        entity->center = state->plane_pos;
        entity->scale  = state->plane_scale;

        state->plane_index = index;

        V3 point = {};
        V3 bound = 9 * entity->scale;

        for (U32 i = 0; i < entity->size; i++) {
            point.x = (S32)(point.x + entity->scale.x) % (S32)bound.x;
            point.z = (i % 9) ? (S32)(point.z + entity->scale.z) % (S32)bound.z : point.z;

            V3 offset = point - (entity->scale * 4);
            offset.y  = 0;

            entity->vertex[i]     = entity->center + offset;
            entity->boundary[i]   = entity->vertex[i] + (entity->scale / 2);
            entity->transform[i]  = scale(entity->vertex[i], entity->scale);
            entity->transform[i] *= translate(entity->vertex[i]);
        }
    }

    set_inactive_index(state);
}

static void translate_entity(Entity *entity, V3 position) {
    entity->center += position;

    for (U32 i = 0; i < entity->size; i++) {
        entity->boundary[i]  += position;
        entity->vertex[i]    += position;
        entity->transform[i] *= translate(position);
    }
}

static void scale_entity(Entity *entity, V3 s) {
    for (U32 i = 0; i < entity->size; i++) {
        entity->transform[i] *= scale(entity->vertex[i], s);
    }
}

static void rotate_entity(Entity *entity, V3 axis, F32 angle) {
    for (U32 i = 0; i < entity->size; i++) {
        entity->transform[i] *= rotate(entity->vertex[i], axis, angle);
    }
}

static S32 get_view_orientation(F32 angle) {
    S32 result = (S32)(angle / 90) % 4;
    while (result < 0) result += 4;
    return result;
}

static void load_string(State *state, char *str, RGB color, V2 position, F32 scale) {
    S32 length = strlen(str);

    S32 i = 0;
    while (i < length) {
        state->text[state->text_size].e[i] = str[i];
        i++;
    }

    state->text[state->text_size].size = length;
    state->text[state->text_size].color = color;
    state->text[state->text_size].position = position;
    state->text[state->text_size].scale = scale;
    state->text[state->text_size].e[i] = '\0';
    state->text_size++;
}

static void process_input(Input *input, State *state, Window *window) {
    F32 flimit = (1 / window->tlimit);

    if (input->H.first) {
        state->target_angle += 90.0f;
        state->move_char = (char *)"H";
        state->target_frame = flimit / 3;
    }
    if (input->K.first) {
        state->target_angle -= 90.0f;
        state->move_char = (char *)"H";
        state->target_frame = flimit / 3;
    }

    state->angle = 0;

    if (state->target_angle > state->current_angle) {
        state->angle          = 5;
        state->current_angle += 5;
    } else if (state->target_angle < state->current_angle) {
        state->angle          = -5;
        state->current_angle -= 5;
    }

    S32 side = get_view_orientation(state->target_angle);

    V3 lateral = {};
    if (side == FRONT) {
        lateral.x = 1.0f;
        load_string(state, (char *)"Front", {1, 1, 0}, {0, (F32)window->height}, 1.0f);
    }
    if (side == BACK) {
        lateral.x = -1.0f;
        load_string(state, (char *)"Back", {1, 1, 0}, {0, (F32)window->height}, 1.0f);
    }
    if (side == LEFT) {
        lateral.z = 1.0f;
        load_string(state, (char *)"Left", {1, 1, 0}, {0, (F32)window->height}, 1.0f);
    }
    if (side == RIGHT) {
        lateral.z = -1.0f;
        load_string(state, (char *)"Right", {1, 1, 0}, {0, (F32)window->height}, 1.0f);
    }


    state->movement = {};

    if (input->S.first) {
        if (!state->is_game_active && (state->game_over_text[0] != '\0')) {
            state->is_init = false;
        }
        F32 value         = (!state->game_speed) ? 0.5f : state->game_speed;
        F32 multiplier    = (state->game_speed < 4) ? 2 : 1;
        state->game_speed = value * multiplier;
        state->is_game_active = true;
        state->target_frame = flimit / 3;
        state->move_char = (char *)"S";
    }

    if (state->is_game_active) {
        if (input->W.first) {
            F32 multiplier    = (state->game_speed > 1) ? 0.5f : 1;
            state->game_speed *= multiplier;
            state->target_frame = flimit / 3;
            state->move_char = (char *)"W";
        }
        if (input->A.first) {
            state->movement = -lateral;
            state->target_frame = flimit / 3;
            state->move_char = (char *)"A";
        }
        if (input->D.first) {
            state->movement = lateral;
            state->target_frame = flimit / 3;
            state->move_char = (char *)"D";
        }

        if (!((state->frame_counter++) % (S32)(flimit / state->game_speed))) {
            state->movement.y = -1.0f;
        }
    }

    if (state->target_frame) {
        if (state->current_frame++ != state->target_frame) {
            load_string(state, state->move_char, {1, 0, 0}, {0, (F32)window->height - 100}, 0.75f);
        } else {
            state->current_frame = 0;
            state->target_frame = 0;
            state->move_char = (char *)"\0";
        }
    }
}

static Bounding_Box get_bounding_box(V3 upper_bound, V3 scale) {
    Bounding_Box box = {};

    box.right  = upper_bound.x;
    box.top    = upper_bound.y;
    box.front  = upper_bound.z;
    box.left   = upper_bound.x - scale.x;
    box.bottom = upper_bound.y - scale.y;
    box.back   = upper_bound.z - scale.z;

    return box;
}

static void check_collision(State *state) {
    Entity *active = get_active_entity(state);

    for (S32 i = 0; i < state->inactive_index_size; i++) {
        Entity *entity = get_inactive_entity(state, i);

        for (U32 j = 0; j < entity->size; j++) {
            for (U32 k = 0; k < active->size; k++) {
                Bounding_Box active_box = get_bounding_box(active->boundary[k] + state->movement, active->scale);
                Bounding_Box entity_box = get_bounding_box(entity->boundary[j], entity->scale);

                B32 xcollision = (active->center.x < entity->center.x) ?
                    (active_box.right  > entity_box.left) :
                    (active_box.left   < entity_box.right);
                B32 ycollision = (active->center.y < entity->center.y) ?
                    (active_box.top    > entity_box.bottom) :
                    (active_box.bottom < entity_box.top);
                B32 zcollision = (active->center.z < entity->center.z) ?
                    (active_box.front  > entity_box.back) :
                    (active_box.back   < entity_box.front);

                B32 yalign = (active->center.y < entity->center.y) ?
                    (active_box.top    == entity_box.bottom) :
                    (active_box.bottom == entity_box.top);

                V3 active_next = active->vertex[k] + state->movement;

                if (active->center.y == state->cube_pos.y) {
                    if (yalign && xcollision && zcollision) {
                        state->is_game_active = false;
                        state->game_over_text = (char *)"Game Over";
                    }
                }

                if (xcollision && ycollision && zcollision) {
                    if (state->movement.y && (!state->movement.x && !state->movement.z)) {
                        active->is_active = false;
                        state->movement.y = 0;

                    }
                    state->movement.x = 0;
                    state->movement.z = 0;
                }

                if (entity->type == PLANE9x9) {
                    F32 xbound = entity->center.x + 4;
                    F32 zbound = entity->center.z + 4;
                    F32 xcheck = fabs(active_next.x) > xbound;
                    F32 zcheck = fabs(active_next.z) > zbound;
                    if (xcheck || zcheck) {
                        state->movement.x = 0;
                        state->movement.z = 0;
                    }
                }
            }
        }
    }
}

static void load_buffer(State *state, Buffer *buffer) {
    Entity *array = state->entity_array;

    S32 index = 0;
    assert(!index);

    for (S32 i = 0; i < ENTITY_SIZE; i++) {
        if (array[i].is_alive) {
            for (U32 j = 0; j < array[i].size; j++) {
                buffer->color[index] = array[i].color;
                buffer->anim[index]  = array[i].transform[j];
                index++;
            }
        }
    }

    for (U32 i = 0; i < state->text_size; i++) {
        buffer->text[i] = state->text[i];
    }

    buffer->eye        = state->eye;
    buffer->light      = state->light;
    buffer->view       = state->view;
    buffer->projection = state->projection;
    buffer->size       = index;
    buffer->text_size  = state->text_size;

    state->text_size = 0;
}

static void game_init_state(State *state, Memory *memory, Buffer *buffer) {
    if (!state->is_init) {
        *state = {};
        *buffer = {};

        arena_alloc(&state->arena,
                    memory->main_size - sizeof(State),
                    (U8 *)memory->main + sizeof(State));

        buffer->anim  = push_array(&state->arena, ENTITY_SIZE * VERTEX_SIZE, M4x4);
        buffer->color = push_array(&state->arena, ENTITY_SIZE * VERTEX_SIZE, RGB);
        buffer->text  = push_array(&state->arena, 10, STR);

        state->cube_color  = {0.86f, 0.11f, 0.31f};
        state->plane_color = {0.334f, 0.288f, 0.635f};

        state->cube_pos    = {0, 13.5f, 0};
        state->plane_pos   = {0, -0.25f, 0};
        state->cube_scale  = {1, 1, 1};
        state->plane_scale = {1, 0.5f, 1};

        state->rotation_axis = {0, 1, 0};
        state->eye           = {0, 7, 24};
        state->light         = {0, 7, 7};
        state->game_over_text = (char *)"\0";

        state->view = look_at(state->eye, {0, 7, 0}, {0, 1, 0});

        create_entity(state, PLANE9x9);
        create_entity(state, CUBE3);

        state->is_init = true;
    }
}

void game_update_render(Input *input, Buffer *buffer, Memory *memory, Window window) {
    State *state = (State *)memory->main;

    game_init_state(state, memory, buffer);

    state->projection = perspective(45.0f, window.width, window.height, 0.1f, 100.0f);

    char str[256] = {'S', 'c', 'o', 'r', 'e', ':', ' ', '\0'};
    char num[256];
    sprintf(num, "%d", state->score);
    strcat(str, num);

    load_string(state, str, {1, 1, 0}, {(F32)window.width, (F32)window.height}, 1.0f);
    load_string(state, state->game_over_text, {1, 1, 0}, {(F32)window.width / 2 - 150, (F32)window.height / 2}, 1.5f);


    process_input(input, state, &window);
    check_collision(state);

    Entity *active = get_active_entity(state);

    if (!active->is_active && state->is_game_active) {
        create_entity(state, CUBE3);
        active = get_active_entity(state);
    }

    for (F32 h = state->plane_pos.y + 1.75f;
         h <= state->cube_pos.y;
         h += 3)
    {
        S32 counter = 0;

        for (S32 i = 0; i < state->inactive_index_size; i++) {
            Entity *entity = get_inactive_entity(state, i);
            if (h == entity->center.y) {
                counter++;
            }
        }

        if (counter == 9) {
            for (S32 i = 0; i < state->inactive_index_size; i++) {
                Entity *entity = get_inactive_entity(state, i);
                if (entity->center.y == h)
                    entity->is_alive = false;
                if (entity->center.y > h)
                    translate_entity(entity, {0, -3, 0});
            }

            state->score += 9 * CUBE3_SIZE;

            break;
        }
    }

    set_inactive_index(state);

    translate_entity(active, state->movement);

    state->view  *= rotate(state->rotation_axis, state->angle);
    state->light *= rotate(state->rotation_axis, -state->angle);

    load_buffer(state, buffer);
}

#endif
