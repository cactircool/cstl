#include "arena.h"
#include "error.h"

#include <stdlib.h>
#include <string.h>

void ArenaAllocator_create_stack(ArenaAllocator *arena, char *stack, size_t size) {
	arena->stack = stack;
	arena->size = size;
	arena->sp = 0;
	arena->external_stack = true;
}

ArenaAllocatorError ArenaAllocator_create(ArenaAllocator *arena, size_t size) {
	arena->size = size;
	arena->sp = 0;
	arena->external_stack = false;
	arena->stack = (char *) malloc(sizeof(char) * size);
	if (!arena->stack)
		return ARENA_ERR_OOM;
	return ARENA_ERR_SUCCESS;
}

ArenaAllocator ArenaAllocator_init_stack(char *stack, size_t size) {
	return (ArenaAllocator) {
		.stack = stack,
		.size = size,
		.sp = 0,
		.external_stack = true,
	};
}

Errable(ArenaAllocator) ArenaAllocator_init(size_t size) {
	ArenaAllocator a;
	ArenaAllocatorError result;
	if ((result = ArenaAllocator_create(&a, size)))
		return Err(result, ArenaAllocator);
	return Ok(a, ArenaAllocator);
}

size_t ArenaAllocator_space(ArenaAllocator *arena) {
	return arena->size - arena->sp;
}

void *ArenaAllocator_alloc(ArenaAllocator *arena, size_t bytes) {
	if (arena->size - arena->sp < bytes)
		return NULL;
	void *ptr = arena->stack + arena->sp;
	arena->sp += bytes;
	return ptr;
}

void *ArenaAllocator_calloc(ArenaAllocator *arena, size_t bytes) {
	if (arena->size - arena->sp < bytes)
		return NULL;
	void *ptr = arena->stack + arena->sp;
	memset(ptr, 0, bytes);
	arena->sp += bytes;
	return ptr;
}

void ArenaAllocator_invalidate(ArenaAllocator *arena) {
	if (!arena->external_stack)
		free(arena->stack);
}
