#pragma once

#include "error.h"
#include <stddef.h>

/**
 * @brief A fixed-size, dynamically allocated array container.
 *
 * The Array type represents a contiguous block of memory capable of holding
 * a fixed number of elements of equal size. Unlike a Vector, its size cannot
 * grow dynamically after creation.
 *
 * @note
 * - The `size` field is constant after initialization.
 * - Use `Array_invalidate()` to release all allocated memory.
 */
typedef struct Array {
	void *data;             /**< Pointer to the array’s allocated memory block. */
	size_t _member_size;    /**< Size in bytes of each element (e.g., `sizeof(T)`). */
	const size_t size;      /**< Total number of elements in the array. */
} Array;

/**
 * @brief Error codes returned by Array operations.
 */
typedef enum {
	ARRAY_ERR_SUCCESS = 0,  /**< Operation completed successfully. */
	ARRAY_ERR_OOM,          /**< Out of memory during allocation. */
} ArrayError;

/** @brief Result type for Array operations that may fail. */
Result(Array, ArrayError);

/**
 * @brief Macro for idiomatic array initialization.
 *
 * This macro provides convenient syntax similar to declaring an array in C.
 * For example:
 * @code
 * Array(int, 8); // Equivalent to Array_init(sizeof(int), 8)
 * @endcode
 *
 * @param type The element type of the array.
 * @param size The number of elements to allocate.
 * @return A new `Errable(Array)` result.
 */
#define Array(type, size) (Array_init(sizeof(type), size))

/**
 * @brief Creates and initializes a new Array.
 *
 * Allocates memory for an array of elements of a given size.
 *
 * @param member_size Size in bytes of each element (use `sizeof(T)`).
 * @param size Number of elements to allocate.
 * @return An `Errable(Array)` result containing either a valid array or an OOM error.
 *
 * @note
 * This is the preferred creation method when initializing arrays inline.
 */
Errable(Array) Array_init(size_t member_size, size_t size);

/**
 * @brief Initializes an existing Array object with the specified configuration.
 *
 * Allocates and populates the array at the provided memory location.
 *
 * @param arr Pointer to an uninitialized Array structure.
 * @param member_size Size in bytes of each element.
 * @param size Number of elements to allocate.
 * @return `ARRAY_ERR_SUCCESS` if successful, `ARRAY_ERR_OOM` if allocation fails.
 *
 * @note
 * This function allows stack-allocated Array objects to be manually constructed.
 */
ArrayError Array_create(Array *arr, size_t member_size, size_t size);

/**
 * @brief Performs a deep copy of an Array from `src` to `dest`.
 *
 * Copies both the metadata and the underlying data buffer.
 *
 * @param dest Pointer to the destination Array.
 * @param src Pointer to the source Array.
 * @return `ARRAY_ERR_SUCCESS` if successful, or `ARRAY_ERR_OOM` on allocation failure.
 */
ArrayError Array_cpy(Array *dest, const Array *src);

/**
 * @brief Transfers ownership of data from one Array to another.
 *
 * Moves the underlying data pointer from `src` to `dest`, leaving `src` empty.
 *
 * @param dest Pointer to the destination Array.
 * @param src Pointer to the source Array.
 *
 * @note
 * After this call, `src` becomes invalid and should not be used unless reinitialized.
 */
void Array_mv(Array *dest, Array *src);

/**
 * @brief Returns the number of elements in the Array.
 *
 * @param arr Pointer to the Array.
 * @return The total number of elements stored in the array.
 */
size_t Array_size(Array *arr);

/**
 * @brief Returns a typed pointer to the Array’s data buffer.
 *
 * This macro simplifies casting of the Array’s raw data to the desired element type.
 *
 * @param arr Pointer to the Array.
 * @param type Element type to cast to (e.g., `int`, `float`).
 * @return A typed pointer to the Array’s data.
 *
 * @code
 * int *nums = Array_data(&arr, int);
 * nums[0] = 42;
 * @endcode
 */
#define Array_data(arr, type) ((type *) (arr)->data)

/**
 * @brief Retrieves an element from the Array at a specific index.
 *
 * Provides safe, typed access to the element at the given index.
 * No bounds checking is performed.
 *
 * @param arr Pointer to the Array.
 * @param index Index of the desired element.
 * @param type Element type to cast to.
 * @return The element at the specified index.
 *
 * @code
 * int first = Array_get(&arr, 0, int);
 * @endcode
 */
#define Array_get(arr, index, type) (Array_data(arr, type)[index])

/**
 * @brief Frees the memory associated with the Array.
 *
 * Deallocates the internal data buffer and resets the array to an empty state.
 *
 * @param arr Pointer to the Array to invalidate.
 *
 * @note
 * Must be called once per Array when it goes out of scope to prevent memory leaks.
 */
void Array_invalidate(Array *arr);
