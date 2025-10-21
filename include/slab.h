#pragma once

#include "arena.h"
#include "error.h"
#include "pqueue.h"

/**
 * @brief A slab allocator that manages fixed-size memory blocks using a priority queue of arenas.
 *
 * The SlabAllocator allocates memory in slabs of fixed size. Each slab is an ArenaAllocator.
 * Slabs are tracked in a PriorityQueue for efficient allocation. When a slab is full, a new
 * slab is added automatically.
 */
typedef struct SlabAllocator {
	PriorityQueue slabs;   /**< PriorityQueue of ArenaAllocator slabs. */
	size_t slab_size;      /**< Size of each slab in bytes. */
} SlabAllocator;

/**
 * @brief Error codes returned by SlabAllocator operations.
 */
typedef enum {
	SA_ERR_SUCCESS = 0,       /**< Operation completed successfully. */
	SA_ERR_OOM,               /**< Out of memory during allocation. */
	SA_ERR_EMPTY_POP_BACK,    /**< Attempted to pop from an empty slab queue. */
} SlabAllocatorError;

/** @brief Result type for SlabAllocator operations. */
Result(SlabAllocator, SlabAllocatorError);

/**
 * @brief Comparator function used internally for the slab PriorityQueue.
 *
 * Compares two ArenaAllocator slabs for heap ordering in the PriorityQueue.
 *
 * @param a Pointer to the first ArenaAllocator.
 * @param b Pointer to the second ArenaAllocator.
 * @return Negative if a < b, positive if a > b, zero if equal (based on free space or other criteria).
 */
int _SlabAllocator_slab_comparator(ArenaAllocator *a, ArenaAllocator *b);

/**
 * @brief Initializes a new SlabAllocator with a given slab size.
 *
 * Convenience inline function to create a SlabAllocator.
 *
 * @param slab_size Size in bytes for each slab.
 * @return An `Errable(SlabAllocator)` result containing either a valid allocator or an OOM error.
 */
Errable(SlabAllocator) SlabAllocator_init(size_t slab_size);

/**
 * @brief Populates an existing SlabAllocator structure to be stable.
 *
 * @param sa Pointer to the SlabAllocator to initialize.
 * @param slab_size Size in bytes for each slab.
 * @return `SA_ERR_SUCCESS` on success, `SA_ERR_OOM` if memory allocation fails.
 */
SlabAllocatorError SlabAllocator_create(SlabAllocator *sa, size_t slab_size);

/**
 * @brief Allocates memory from the slab allocator.
 *
 * Finds a slab with enough free space or creates a new slab if necessary.
 *
 * @param sa Pointer to the SlabAllocator.
 * @param bytes Number of bytes to allocate.
 * @return Pointer to the allocated memory, or NULL if allocation fails.
 */
void *SlabAllocator_alloc(SlabAllocator *sa, size_t bytes);

/**
 * @brief Releases all resources used by the SlabAllocator.
 *
 * Invalidates all underlying slabs and clears the PriorityQueue.
 *
 * @param sa Pointer to the SlabAllocator to invalidate.
 */
void SlabAllocator_invalidate(SlabAllocator *sa);

/**
 * @brief Simple inline macro to initialize a slab allocator.
 *
 * @param name Name of the SlabAllocator variable.
 * @param slab_size Size of each slab in bytes.
 *
 * @note
 * Calls `fatal()` if initialization fails due to OOM.
 */
#define SLAB_ALLOCATOR_BEGIN(name, slab_size) \
	Errable(SlabAllocator) __errable__##name##__ = SlabAllocator_init(slab_size); \
	if (!__errable__##name##__.fail) fatal(String_from_cstring("Slab allocator '" #name "' failed initialization with an OutOfMemory error")); \
	SlabAllocator name = __errable__##name##__.success;

/**
 * @brief Simple inline macro to invalidate a slab allocator.
 *
 * @param name Name of the SlabAllocator variable to invalidate.
 */
#define SLAB_ALLOCATOR_END(name) \
	SlabAllocator_invalidate(&name);

/**
 * @brief RAII-style slab allocator context macro.
 *
 * Automatically initializes and invalidates a slab allocator for the duration of a code block.
 *
 * @param name Name of the SlabAllocator variable.
 * @param slab_size Size of each slab in bytes.
 *
 * @note
 * - Do not use `return` statements inside the block, or the slab will not be invalidated.
 * - Calls `fatal()` on initialization failure.
 *
 * @code
 * SLAB_ALLOCATOR_CTX(my_slab, 4096) {
 *     void *mem = SlabAllocator_alloc(&my_slab, 128);
 *     // use memory
 * } // automatically invalidates the allocator
 * @endcode
 */
#define SLAB_ALLOCATOR_CTX(name, slab_size) \
	Errable(SlabAllocator) __errable__##name##__ = SlabAllocator_init(slab_size); \
	if (!__errable__##name##__.fail) fatal(String_from_cstring("Slab allocator '" #name "' failed initialization with an OutOfMemory error")); \
	for (SlabAllocator name = __errable__##name##__.success, *__once__##name##__ = (void*)1; __once__##name##__; SlabAllocator_invalidate(&name), __once__##name##__ = NULL)
