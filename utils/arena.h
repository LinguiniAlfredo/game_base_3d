#pragma once
#define ARENA_SIZE 4092
#include <cstdint>
#include <cstdlib>

typedef struct Arena {
    char  *buffer;
    size_t size;
    size_t offset;
} Arena;

void arena_create(Arena *arena, const size_t size)
{
    arena->buffer = malloc(size);
    arena->size   = size;
    arena->offset = 0;
}

void* arena_alloc(Arena* arena, const size_t size)
{
    void *ptr = NULL;
    if (arena->offset + size > arena->size) {
        printf("ran out of space in arena...\n");
        return NULL;
    }
    ptr = arena->buffer + arena->offset;
    arena->offset += size;

    return ptr;
}

void arena_reset(Arena* arena)
{
    arena->offset = 0;
}

void arena_destroy(Arena* arena)
{
    free(arena->buffer);
}
