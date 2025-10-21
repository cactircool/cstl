#include "arena.h"

#include <stdlib.h>
#include <string.h>

void Arena_create_stack(Arena *arena, char *stack, size_t size) {
	arena->stack = stack;
	arena->size = size;
	arena->sp = 0;
}

void Arena_create(Arena *arena, size_t size) {
	arena->stack = (char *) malloc(sizeof(char) * size);
	arena->size = size;
	arena->sp = 0;
}

Arena Arena_init_stack(char *stack, size_t size) {
	return (Arena) {
		.stack = stack,
		.size = size,
		.sp = 0,
	};
}

Arena Arena_init(size_t size) {
	return (Arena) {
		.stack = (char *) malloc(sizeof(char) * size),
		.size = size,
		.sp = 0,
	};
}

size_t Arena_space(Arena *arena) {
	return arena->size - arena->sp;
}

void *Arena_alloc(Arena *arena, size_t bytes) {
	if (arena->size - arena->sp < bytes)
		return NULL;
	void *ptr = arena->stack + arena->sp;
	arena->sp += bytes;
	return ptr;
}

void *Arena_calloc(Arena *arena, size_t bytes) {
	if (arena->size - arena->sp < bytes)
		return NULL;
	void *ptr = arena->stack + arena->sp;
	memset(ptr, 0, bytes);
	arena->sp += bytes;
	return ptr;
}

void Arena_invalidate(Arena *arena) {
	free(arena->stack);
}
