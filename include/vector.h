#pragma once

#include <stddef.h>
#include "error.h"

/**
 * @brief A dynamically resizing array of generic elements.
 *
 * A Vector stores elements of uniform size and handles memory allocation
 * automatically as elements are added or removed. Supports copy, move,
 * and capacity management operations.
 */
typedef struct Vector {
	void *data;           /**< Pointer to the underlying data buffer. */
	size_t _member_size;  /**< Size (in bytes) of each element. */
	size_t size;          /**< Number of elements currently in use. */
	size_t _capacity;     /**< Allocated capacity (in elements). */
} Vector;

/**
 * @brief Error codes for Vector operations.
 */
typedef enum {
	VEC_ERR_SUCCESS = 0,       /**< Operation succeeded. */
	VEC_ERR_OOM,               /**< Out of memory during allocation. */
	VEC_ERR_EMPTY_POP_BACK,    /**< Attempted to pop from an empty Vector. */
} VectorError;

/** @brief Result type for Vector-returning functions. */
Result(Vector, VectorError);

/**
 * @brief Creates and initializes a new Vector.
 *
 * @param member_size Size (in bytes) of each element.
 * @return A Result containing a Vector or an error code.
 */
Errable(Vector) Vector_init(size_t member_size);

/**
 * @brief Initializes an existing Vector to default values.
 *
 * @param v Pointer to the Vector to initialize.
 * @param member_size Size (in bytes) of each element.
 */
void Vector_default(Vector *v, size_t member_size);

/**
 * @brief Allocates internal memory for a Vector.
 *
 * @param v Pointer to the Vector to create.
 * @param member_size Size (in bytes) of each element.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_OOM on allocation failure.
 */
VectorError Vector_create(Vector *v, size_t member_size);

/**
 * @brief Performs a deep copy of one Vector into another.
 *
 * @param dest Destination Vector.
 * @param src Source Vector.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_OOM on failure.
 */
VectorError Vector_cpy(Vector *dest, const Vector *src);

/**
 * @brief Moves data from one Vector to another, invalidating the source.
 *
 * @param dest Destination Vector.
 * @param src Source Vector (invalidated after move).
 */
void Vector_mv(Vector *dest, Vector *src);

/**
 * @brief Frees all memory associated with the Vector and resets its fields.
 *
 * @param v Pointer to the Vector to invalidate.
 */
void Vector_invalidate(Vector *v);

/**
 * @brief Frees all memory after calling a custom destructor on each element.
 *
 * @param v Pointer to the Vector to invalidate.
 * @param destructor Function to destroy each element before freeing.
 */
void Vector_custom_invalidate(Vector *v, void (*destructor)(void *));

/**
 * @brief Clears all elements but retains allocated capacity.
 *
 * @param v Pointer to the Vector.
 */
void Vector_clear(Vector *v);

/**
 * @brief Shrinks the allocated capacity to match the current size.
 *
 * @param v Pointer to the Vector.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_OOM on failure.
 */
VectorError Vector_shrink(Vector *v);

/**
 * @brief Returns the number of elements in the Vector.
 *
 * @param v Pointer to the Vector.
 * @return Number of elements currently stored.
 */
size_t Vector_size(Vector *v);

/**
 * @brief Returns the size in bytes of each element.
 *
 * @param v Pointer to the Vector.
 * @return Element size in bytes.
 */
size_t Vector_member_size(Vector *v);

/**
 * @brief Returns a pointer to the raw data buffer.
 *
 * @param v Pointer to the Vector.
 * @return Const pointer to the internal data buffer.
 */
const void *Vector_raw_data(const Vector *v);

/**
 * @brief Type-safe macro for accessing the Vector's data.
 */
#define Vector_data(v, type) ((type *) (v)->data)

/**
 * @brief Type-safe macro for getting an element by index.
 */
#define Vector_get(v, index, type) (((type *) Vector_data(v, type))[index])

/**
 * @brief Returns a pointer to the element at a given byte offset.
 *
 * @param v Pointer to the Vector.
 * @param offset Offset in bytes.
 * @return Pointer to the element.
 */
void *Vector_offset(Vector *v, size_t offset);

/**
 * @brief Sets the element at a given index to provided data.
 *
 * @param v Pointer to the Vector.
 * @param index Index of the element to set.
 * @param data Pointer to the new element data.
 */
void Vector_set(Vector *v, size_t index, void *data);

/**
 * @brief Ensures the Vector can hold at least `capacity` elements.
 *
 * @param v Pointer to the Vector.
 * @param capacity Minimum capacity to reserve.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_OOM on failure.
 */
VectorError Vector_reserve(Vector *v, size_t capacity);

/**
 * @brief Appends a single element to the end of the Vector.
 *
 * @param v Pointer to the Vector.
 * @param data Pointer to the element to append.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_OOM on failure.
 */
VectorError Vector_append(Vector *v, void *data);

/**
 * @brief Appends multiple elements from a contiguous data block.
 *
 * @param v Pointer to the Vector.
 * @param data Pointer to the elements to append.
 * @param n Number of elements to append.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_OOM on failure.
 */
VectorError Vector_append_members(Vector *v, void *data, size_t n);

/**
 * @brief Appends the contents of one Vector to another.
 *
 * @param v Destination Vector.
 * @param a Source Vector.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_OOM on failure.
 */
VectorError Vector_append_vector(Vector *v, Vector *a);

/**
 * @brief Swaps two elements within the Vector.
 *
 * @param v Pointer to the Vector.
 * @param index_a Index of the first element.
 * @param index_b Index of the second element.
 */
void Vector_swap(Vector *v, size_t index_a, size_t index_b);

/**
 * @brief Removes the last element from the Vector.
 *
 * @param v Pointer to the Vector.
 * @return VEC_ERR_SUCCESS on success, VEC_ERR_EMPTY_POP_BACK if empty.
 */
VectorError Vector_pop_back(Vector *v);

/**
 * @brief Returns a typed pointer to the first element.
 */
#define Vector_begin(v, type) ((type *) v->data)

/**
 * @brief Returns a typed pointer to one past the last element.
 */
#define Vector_end(v, type) (((type *) v->data) + v->size)

/**
 * @brief Scoped Vector context macro with automatic cleanup.
 *
 * @param name Name of the scoped Vector variable.
 * @param type Type of elements in the Vector.
 *
 * @code
 * VECTOR_CTX(my_vec, int) {
 *     Vector_append(&my_vec, &value);
 * }
 * @endcode
 */
#define VECTOR_CTX(name, type) \
	for (Errable(Vector) __##name##__err__ = Vector_init(sizeof(type)), *__once__##name__ = (void*)1; !__##name##__err__.fail && __once__##name__;) \
		for (Vector name = __##name##__err__.success; __once__##name__; __once__##name__ = NULL)
