#pragma once

#include "error.h"
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief A lightweight stack-based memory allocator (arena allocator).
 *
 * The ArenaAllocator provides a simple mechanism for fast, linear allocation
 * of temporary memory. It allocates a contiguous block of memory (the “arena”)
 * and satisfies allocation requests from the top of that block using a
 * stack-pointer-like mechanism.
 *
 * Memory can only be released all at once using `ArenaAllocator_invalidate()`,
 * making it ideal for short-lived allocations or frame-based memory management.
 *
 * @note
 * - No individual frees: memory is freed when the entire arena is invalidated.
 * - Very fast allocation with no fragmentation.
 * - Optionally supports externally provided stack buffers to avoid dynamic allocation.
 */
typedef struct ArenaAllocator {
	char *stack;        /**< Pointer to the arena’s memory buffer. */
	size_t size;        /**< Total size of the arena in bytes. */
	size_t sp;          /**< Current stack pointer (offset from start of buffer). */
	bool external_stack;/**< True if the buffer was provided externally and should not be freed. */
} ArenaAllocator;

/**
 * @brief Error codes for ArenaAllocator operations.
 */
typedef enum {
	ARENA_ERR_SUCCESS = 0, /**< Operation completed successfully. */
	ARENA_ERR_OOM,         /**< Out of memory — arena allocation failed. */
} ArenaAllocatorError;

/** @brief Result type for ArenaAllocator operations. */
Result(ArenaAllocator, ArenaAllocatorError);

/**
 * @brief Initializes an arena allocator using an existing stack buffer.
 *
 * Populates an ArenaAllocator using a caller-provided memory region.
 * This avoids any heap allocations and is useful for embedded or
 * performance-critical scenarios.
 *
 * @param arena Pointer to the ArenaAllocator to initialize.
 * @param stack Pointer to the preallocated memory buffer to use.
 * @param size Size of the memory buffer, in bytes.
 *
 * @note
 * This function does not allocate memory; it simply configures the arena
 * to use the provided buffer.
 */
void ArenaAllocator_create_stack(ArenaAllocator *arena, char *stack, size_t size);

/**
 * @brief Initializes an arena allocator with an internally allocated buffer.
 *
 * Allocates a new memory region of the specified size and prepares it
 * for allocation operations.
 *
 * @param arena Pointer to the ArenaAllocator to initialize.
 * @param size Total size of the arena in bytes.
 * @return `ARENA_ERR_SUCCESS` on success, or `ARENA_ERR_OOM` if allocation fails.
 */
ArenaAllocatorError ArenaAllocator_create(ArenaAllocator *arena, size_t size);

/**
 * @brief Creates a compile-time-optimizable arena using an existing stack buffer.
 *
 * This is a convenience function equivalent to:
 * @code
 * ArenaAllocator a;
 * ArenaAllocator_create_stack(&a, stack, size);
 * @endcode
 *
 * @param stack Pointer to a preallocated memory buffer.
 * @param size Size of the buffer in bytes.
 * @return An initialized ArenaAllocator configured to use the given buffer.
 */
ArenaAllocator ArenaAllocator_init_stack(char *stack, size_t size);

/**
 * @brief Creates a new ArenaAllocator with an internally allocated buffer.
 *
 * This is a convenience function equivalent to:
 * @code
 * ArenaAllocator a;
 * ArenaAllocator_create(&a, size);
 * @endcode
 *
 * @param size Total size of the arena in bytes.
 * @return An `Errable(ArenaAllocator)` result, which may contain either
 *         a valid allocator or an out-of-memory error.
 */
Errable(ArenaAllocator) ArenaAllocator_init(size_t size);

/**
 * @brief Returns the number of bytes of free space remaining in the arena.
 *
 * @param arena Pointer to the ArenaAllocator.
 * @return The number of unused bytes left in the arena.
 */
size_t ArenaAllocator_space(ArenaAllocator *arena);

/**
 * @brief Allocates a block of memory from the arena.
 *
 * Returns a pointer to a newly allocated block of the specified size.
 * The memory is uninitialized, similar to `malloc()`.
 *
 * @param arena Pointer to the ArenaAllocator.
 * @param bytes Number of bytes to allocate.
 * @return Pointer to the allocated memory, or `NULL` if there is not enough space.
 *
 * @note
 * This allocation does not support freeing individual blocks.
 */
void *ArenaAllocator_alloc(ArenaAllocator *arena, size_t bytes);

/**
 * @brief Allocates and zero-initializes a block of memory from the arena.
 *
 * Equivalent to `calloc()` for arenas — allocates a block of memory and
 * sets all bytes to zero.
 *
 * @param arena Pointer to the ArenaAllocator.
 * @param bytes Number of bytes to allocate.
 * @return Pointer to the zero-initialized memory, or `NULL` if there is not enough space.
 */
void *ArenaAllocator_calloc(ArenaAllocator *arena, size_t bytes);

/**
 * @brief Frees all resources associated with the ArenaAllocator.
 *
 * Resets the arena and deallocates its memory if it was dynamically allocated.
 * If the arena was using an external stack buffer, only internal state is reset.
 *
 * @param arena Pointer to the ArenaAllocator to invalidate.
 *
 * @note
 * After invalidation, the arena must be reinitialized before reuse.
 */
void ArenaAllocator_invalidate(ArenaAllocator *arena);
