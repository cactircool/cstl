#pragma once

#include "wptr.h"
#include <stddef.h>

/**
 * @brief A reference-counted smart pointer providing shared ownership semantics.
 *
 * A SharedPtr maintains a reference count to manage the lifetime of a shared resource.
 * When the last SharedPtr referencing an object is invalidated, the associated deletor
 * is automatically called to release the resource.
 *
 * @note
 * - **Never pass SharedPtr by value** — always pass by pointer (e.g. `SharedPtr *`),
 *   since copying by value breaks the internal reference count consistency.
 * - You must call `SharedPtr_invalidate()` once a SharedPtr goes out of scope.
 * - For interoperability with non-owning access, use `SharedPtr_reader()` to obtain
 *   a `WeakPtr` that observes the same resource.
 */
typedef struct SharedPtr {
	void *ptr;                   /**< Pointer to the managed resource. */
	void (*deletor)(void *);     /**< Function used to release the resource (e.g., `free`). */
	size_t ref_count;            /**< Internal reference count for shared ownership. */
} SharedPtr;

/**
 * @brief Creates a new SharedPtr managing a resource with a custom deletor.
 *
 * @param ptr Pointer to the resource to manage.
 * @param deletor Function used to release the resource (e.g., `free`).
 * @return A new SharedPtr managing the provided resource with ref_count = 1.
 *
 * @code
 * SharedPtr sptr = SharedPtr_init(malloc(64), free);
 * @endcode
 */
SharedPtr SharedPtr_init(void *ptr, void (*deletor)(void *));

/**
 * @brief Extracts the raw pointer from a SharedPtr without affecting ownership.
 *
 * @warning
 * This is an **unsafe** operation — the caller assumes full responsibility
 * for ensuring the lifetime of the returned pointer.
 * Use only for low-level interoperability with C APIs that require raw pointers.
 *
 * @param sptr Pointer to the SharedPtr.
 * @return The raw pointer managed by the SharedPtr.
 */
void *SharedPtr_extract(SharedPtr *sptr);

/**
 * @brief Increments the reference count and returns the same SharedPtr pointer.
 *
 * This function must be called before passing the SharedPtr to another context
 * that will also manage its lifetime. The internal reference count is incremented
 * to reflect the additional owner.
 *
 * @param sptr Pointer to the SharedPtr to clone.
 * @return The same SharedPtr pointer (for convenience).
 *
 * @code
 * SharedPtr *shared_b = SharedPtr_clone(shared_a);
 * @endcode
 */
SharedPtr *SharedPtr_clone(SharedPtr *sptr);

/**
 * @brief Creates a read-only WeakPtr that references the same resource.
 *
 * The returned WeakPtr observes the resource managed by the SharedPtr
 * but does **not** affect its reference count. Writing to memory through
 * this WeakPtr is undefined behavior.
 *
 * @param sptr Pointer to the SharedPtr.
 * @return A WeakPtr referencing the managed resource.
 *
 * @note The WeakPtr remains valid only as long as the parent SharedPtr exists.
 */
WeakPtr *SharedPtr_reader(SharedPtr *sptr);

/**
 * @brief Decrements the reference count and releases the resource if it reaches zero.
 *
 * When the last SharedPtr referencing a resource is invalidated, the associated deletor
 * is invoked and the internal pointer is set to NULL.
 *
 * @param sptr Pointer to the SharedPtr to invalidate.
 *
 * @note
 * You **must** call this function once for every SharedPtr instance when it
 * goes out of scope to ensure proper resource cleanup.
 *
 * @code
 * SharedPtr_invalidate(&sptr);
 * @endcode
 */
void SharedPtr_invalidate(SharedPtr *sptr);
