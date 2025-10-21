#pragma once

/**
 * @brief A non-owning weak reference to an object.
 *
 * A WeakPtr does not manage or extend the lifetime of the object it points to.
 * It is mainly used for safe read-only access without transferring ownership.
 */
typedef struct WeakPtr {
	const void *ptr; /**< Pointer to the target object (non-owning). */
} WeakPtr;

/**
 * @brief Initializes a WeakPtr referencing a given object.
 *
 * @param ptr Pointer to the target object (may be NULL).
 * @return A WeakPtr referencing the provided pointer.
 */
WeakPtr WeakPtr_init(void *ptr);

/**
 * @brief Extracts the raw pointer stored in a WeakPtr.
 *
 * @param wptr Pointer to the WeakPtr.
 * @return The raw pointer originally referenced by the WeakPtr.
 */
const void *WeakPtr_extract(WeakPtr *wptr);
