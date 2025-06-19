#if !defined(HEADER_MEMORY_H)
#define HEADER_MEMORY_H

#include <assert.h>

struct Arena {
    U64 size;
    U64 used;
    U8 *base;
};

inline void arena_alloc(Arena *arena, U64 size, U8 *base) {
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

#define push_struct(arena, type) (type *)push_size((arena), sizeof(type))
#define push_array(arena, count, type) (type *)push_size((arena), (count) * sizeof(type))
inline void *push_size(Arena *arena, U64 size) {
    assert((arena->used + size) <= arena->size);
    void *result = arena->base + arena->used;
    arena->used += size;

    return result;
}

#endif
