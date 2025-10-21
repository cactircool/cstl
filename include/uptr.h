#pragma once

#include "wptr.h"

/**
 * @brief A smart pointer type that exclusively owns a resource.
 *
 * UniquePtr ensures unique ownership of a dynamically allocated object.
 * The resource is automatically released using a custom deletor function
 * when invalidated or when leaving scope via the UNIQUE_PTR_CTX macro.
 *
 * UniquePtr objects must be passed by value and invalidated explicitly
 * or automatically at the end of their scope.
 */
typedef struct UniquePtr {
	void *ptr;                   /**< Pointer to the managed resource. */
	void (*deletor)(void *);     /**< Custom function to delete or free the resource. */
} UniquePtr;

/**
 * @brief Creates a UniquePtr with a custom deletor.
 *
 * @param ptr Pointer to the resource to manage.
 * @param deletor Function to release the resource (e.g., free()).
 * @return A UniquePtr managing the provided resource.
 */
UniquePtr UniquePtr_init(void *ptr, void (*deletor)(void *));

/**
 * @brief Extracts the raw pointer from a UniquePtr without freeing it.
 *
 * @warning This is unsafe — the caller assumes full responsibility for
 * managing the returned pointer manually. Use only for compatibility
 * with raw C APIs.
 *
 * @param uptr Pointer to the UniquePtr.
 * @return The raw pointer managed by the UniquePtr.
 */
void *UniquePtr_extract(UniquePtr *uptr);

/**
 * @brief Creates a read-only WeakPtr observing the same resource.
 *
 * @param uptr Pointer to the UniquePtr.
 * @return A WeakPtr referencing the managed object (non-owning).
 */
WeakPtr *UniquePtr_reader(UniquePtr *uptr);

/**
 * @brief Transfers ownership of the managed resource to a new UniquePtr.
 *
 * The source UniquePtr is invalidated but remains safe to reuse or
 * invalidate again.
 *
 * @param uptr Pointer to the UniquePtr to move from.
 * @return A new UniquePtr that owns the resource.
 */
UniquePtr UniquePtr_writer(UniquePtr *uptr);

/**
 * @brief Releases the managed resource using the custom deletor.
 *
 * After invalidation, the UniquePtr is reset to a null state.
 *
 * @param uptr Pointer to the UniquePtr to invalidate.
 */
void UniquePtr_invalidate(UniquePtr *uptr);

/**
 * @brief Declares a scoped UniquePtr that automatically invalidates on exit.
 *
 * @param name Variable name for the scoped UniquePtr.
 * @param value Pointer to the allocated resource.
 * @param deletor Function to free or destroy the resource.
 *
 * @note
 * - The resource is automatically released when the scope ends.
 * - Early returns inside the block are not allowed.
 *
 * @code
 * UNIQUE_PTR_CTX(file_ptr, fopen("data.txt", "r"), fclose) {
 *     // Use file_ptr.ptr safely here
 * }
 * @endcode
 */
#define UNIQUE_PTR_CTX(name, value, deletor) \
	for (UniquePtr name = UniquePtr_init((void *) value, (void (*)(void *)) deletor), *__once__##name__ = (void *)1; \
	     __once__##name__; \
	     UniquePtr_invalidate(&name), __once__##name__ = NULL)
