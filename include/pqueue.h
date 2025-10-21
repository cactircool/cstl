#pragma once

#include "error.h"
#include "vector.h"
#include <stdbool.h>

/**
 * @brief A priority queue implemented as a binary heap on top of a Vector.
 *
 * This structure provides a generic, type-agnostic priority queue.
 * It relies on a user-provided comparator function to maintain heap ordering,
 * supporting both min-heaps and max-heaps.
 *
 * @note
 * - The comparator function must take two pointers to elements and return
 *   an integer: negative if the first element is less, positive if greater,
 *   and zero if equal.
 * - The underlying data is stored in a Vector for dynamic resizing.
 */
typedef struct PriorityQueue {
	Vector vec;                           /**< Underlying dynamic array storage. */
	int (*comparator)(void *, void *);    /**< Function to compare two elements (determines heap order). */
} PriorityQueue;

/**
 * @brief Error codes returned by PriorityQueue operations.
 */
typedef enum {
	PQ_ERR_SUCCESS = 0,       /**< Operation completed successfully. */
	PQ_ERR_OOM,               /**< Out of memory during allocation. */
	PQ_ERR_EMPTY_POP_BACK,    /**< Attempted to pop from an empty PriorityQueue. */
} PriorityQueueError;

/** @brief Result type for PriorityQueue operations that may fail. */
Result(PriorityQueue, PriorityQueueError);

/**
 * @brief Initializes a new PriorityQueue.
 *
 * Provides a convenient inline constructor. The comparator must accept
 * pointers to the elements.
 *
 * @param member_size Size in bytes of each element.
 * @param comparator Function to determine heap ordering.
 * @return An `Errable(PriorityQueue)` result containing either a valid queue or an OOM error.
 */
Errable(PriorityQueue) PriorityQueue_init(size_t member_size, int (*comparator)(void *, void *));

/**
 * @brief Populates an existing PriorityQueue structure.
 *
 * @param pq Pointer to the PriorityQueue to initialize.
 * @param member_size Size of each element in bytes.
 * @param comparator Comparator function for heap ordering.
 * @return `PQ_ERR_SUCCESS` on success, `PQ_ERR_OOM` if allocation fails.
 */
PriorityQueueError PriorityQueue_create(PriorityQueue *pq, size_t member_size, int (*comparator)(void *, void *));

/**
 * @brief Performs a deep copy of a PriorityQueue.
 *
 * Wrapper over `Vector_cpy`.
 *
 * @param dest Pointer to the destination PriorityQueue.
 * @param src Pointer to the source PriorityQueue.
 * @return `PQ_ERR_SUCCESS` on success, `PQ_ERR_OOM` if allocation fails.
 */
PriorityQueueError PriorityQueue_cpy(PriorityQueue *dest, const PriorityQueue *src);

/**
 * @brief Moves a PriorityQueue, invalidating the source.
 *
 * Wrapper over `Vector_mv`.
 *
 * @param dest Pointer to the destination PriorityQueue.
 * @param src Pointer to the source PriorityQueue (invalidated after move).
 */
void PriorityQueue_mv(PriorityQueue *dest, PriorityQueue *src);

/**
 * @brief Releases all resources associated with the PriorityQueue.
 *
 * Wrapper over `Vector_invalidate`.
 *
 * @param pq Pointer to the PriorityQueue to invalidate.
 */
void PriorityQueue_invalidate(PriorityQueue *pq);

/**
 * @brief Releases resources with a custom element destructor.
 *
 * Wrapper over `Vector_custom_invalidate`.
 *
 * @param pq Pointer to the PriorityQueue.
 * @param destructor Function to call for each element before freeing.
 */
void PriorityQueue_custom_invalidate(PriorityQueue *pq, void (*destructor)(void *));

/**
 * @brief Clears all elements from the PriorityQueue.
 *
 * Wrapper over `Vector_clear`.
 *
 * @param pq Pointer to the PriorityQueue.
 */
void PriorityQueue_clear(PriorityQueue *pq);

/**
 * @brief Shrinks the internal storage of the PriorityQueue.
 *
 * Wrapper over `Vector_shrink`.
 *
 * @param pq Pointer to the PriorityQueue.
 * @return `PQ_ERR_SUCCESS` on success, `PQ_ERR_OOM` if allocation fails.
 */
PriorityQueueError PriorityQueue_shrink(PriorityQueue *pq);

/**
 * @brief Returns the number of elements in the PriorityQueue.
 *
 * @param pq Pointer to the PriorityQueue.
 * @return Number of elements currently in the queue.
 */
size_t PriorityQueue_size(PriorityQueue *pq);

/**
 * @brief Returns a pointer to the underlying raw data array.
 *
 * @param pq Pointer to the PriorityQueue.
 * @return Const pointer to the underlying Vector data.
 */
const void *PriorityQueue_raw_data(const PriorityQueue *pq);

/**
 * @brief Compares two elements in the queue using the comparator function.
 *
 * @param pq Pointer to the PriorityQueue.
 * @param index_a Index of the first element.
 * @param index_b Index of the second element.
 * @return Negative, zero, or positive value according to the comparator.
 */
int _PriorityQueue_compare(PriorityQueue *pq, size_t index_a, size_t index_b);

/**
 * @brief Recursively restores heap property upwards from a given index.
 *
 * @param pq Pointer to the PriorityQueue.
 * @param index Index to heapify up from.
 */
void _PriorityQueue_heapify_up(PriorityQueue *pq, size_t index);

/**
 * @brief Recursively restores heap property downwards from a given index.
 *
 * @param pq Pointer to the PriorityQueue.
 * @param index Index to heapify down from.
 */
void _PriorityQueue_heapify_down(PriorityQueue *pq, size_t index);

/**
 * @brief Adds a new element to the PriorityQueue.
 *
 * Inserts the element at the end of the heap and then restores the heap property.
 *
 * @param pq Pointer to the PriorityQueue.
 * @param data Pointer to the element to insert.
 * @return `PQ_ERR_SUCCESS` on success, `PQ_ERR_OOM` if memory allocation fails.
 */
PriorityQueueError PriorityQueue_push(PriorityQueue *pq, void *data);

/**
 * @brief Removes the top element from the PriorityQueue.
 *
 * Pops the element at index 0 (root of the heap) and restores the heap property.
 *
 * @param pq Pointer to the PriorityQueue.
 *
 * @note Does nothing if the PriorityQueue is empty.
 */
void PriorityQueue_pop(PriorityQueue *pq);

/**
 * @brief Returns the top element of the PriorityQueue.
 *
 * @param pq Pointer to the PriorityQueue.
 * @return Pointer to the element with the highest priority (root of the heap),
 *         or NULL if the queue is empty.
 */
void *PriorityQueue_top(PriorityQueue *pq);
