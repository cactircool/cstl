#pragma once

#include <stddef.h>

typedef struct Arena {
	char *stack; // stack buffer
	size_t size; // full stack size
	size_t sp; // stack pointer
} Arena;

/**
 * Populates and stabilizes passed in arena object.
 * Convenience function for the case of preexisting stack memory to be used as the arena.
 * Only real use case is to minimize dynamic allocation overhead by preallocating a stack buffer, and passing that in to the Arena
 */
void Arena_create_stack(Arena *arena, char *stack, size_t size);

/**
 * Populates and stabilizes passed in arena object.
 */
void Arena_create(Arena *arena, size_t size);

/**
 * Convenience function to be optimized at compile time.
 * Equivalent to: ```c
 * Arena a;
 * Arena_create_stack(&a, stack, size);
 * ```
 */
Arena Arena_init_stack(char *stack, size_t size);
/**
 * Convenience function to be optimized at compile time.
 * Equivalent to: ```c
 * Arena a;
 * Arena_create(&a, size);
 * ```
 */
Arena Arena_init(size_t size);

/**
 * How much free space this arena has left
 */
size_t Arena_space(Arena *arena);

/**
 * Allocate bytes on this Arena (malloc style)
 */
void *Arena_alloc(Arena *arena, size_t bytes);
/**
 * Allocate bytes on this Arena and set the allocated bytes to 0 (calloc style)
 */
void *Arena_calloc(Arena *arena, size_t bytes);

/**
 * Release resources used by the arena
 */
void Arena_invalidate(Arena *arena);
