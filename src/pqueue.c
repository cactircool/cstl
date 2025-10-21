#include "pqueue.h"
#include "error.h"
#include "vector.h"
#include <string.h>

Errable(PriorityQueue) PriorityQueue_init(size_t member_size, int (*comparator)(void *, void *)) {
	PriorityQueue pq;
	PriorityQueueError result;
	if ((result = PriorityQueue_create(&pq, member_size, comparator)))
		return Err(result, PriorityQueue);
	return Ok(pq, PriorityQueue);
}

PriorityQueueError PriorityQueue_create(PriorityQueue *pq, size_t member_size, int (*comparator)(void *, void *)) {
	pq->comparator = comparator;
	return (PriorityQueueError) Vector_create(&pq->vec, member_size);
}

PriorityQueueError PriorityQueue_cpy(PriorityQueue *dest, const PriorityQueue *src) {
	dest->comparator = src->comparator;
	return (PriorityQueueError) Vector_cpy(&dest->vec, &src->vec);
}

void PriorityQueue_mv(PriorityQueue *dest, PriorityQueue *src) {
	dest->comparator = src->comparator;
	Vector_mv(&dest->vec, &src->vec); // invalidates src implicitly
}

void PriorityQueue_invalidate(PriorityQueue *pq) {
	Vector_invalidate(&pq->vec);
	pq->comparator = NULL;
}

void PriorityQueue_custom_invalidate(PriorityQueue *pq, void (*destructor)(void *)) {
	Vector_custom_invalidate(&pq->vec, destructor);
	pq->comparator = NULL;
}

void PriorityQueue_clear(PriorityQueue *pq) {
	Vector_clear(&pq->vec);
}

PriorityQueueError PriorityQueue_shrink(PriorityQueue *pq) {
	return (PriorityQueueError) Vector_shrink(&pq->vec);
}

size_t PriorityQueue_size(PriorityQueue *pq) {
	return Vector_size(&pq->vec);
}

const void *PriorityQueue_raw_data(const PriorityQueue *pq) {
	return Vector_raw_data(&pq->vec);
}

int _PriorityQueue_compare(PriorityQueue *pq, size_t index_a, size_t index_b) {
	void *a = Vector_offset(&pq->vec, index_a);
	void *b = Vector_offset(&pq->vec, index_b);
	return pq->comparator(a, b);
}

void _PriorityQueue_heapify_up(PriorityQueue *pq, size_t index) {
	if (index == 0) return;
	size_t parent = (index - 1) / 2;
	if (_PriorityQueue_compare(pq, index, parent) < 0) {
		Vector_swap(&pq->vec, index, parent);
		_PriorityQueue_heapify_up(pq, parent);
	}
}

void _PriorityQueue_heapify_down(PriorityQueue *pq, size_t index) {
	size_t left = 2 * index + 1;
	size_t right = 2 * index + 2;
	size_t largest = index;

	if (left < PriorityQueue_size(pq) && _PriorityQueue_compare(pq, left, largest) < 0)
		largest = left;
	if (right < PriorityQueue_size(pq) && _PriorityQueue_compare(pq, right, largest) < 0)
		largest = right;

	if (largest != index) {
		Vector_swap(&pq->vec, index, largest);
		_PriorityQueue_heapify_down(pq, largest);
	}
}

PriorityQueueError PriorityQueue_push(PriorityQueue *pq, void *data) {
	PriorityQueueError result = (PriorityQueueError) Vector_append(&pq->vec, data);
	if (result) return result;
	_PriorityQueue_heapify_up(pq, PriorityQueue_size(pq) - 1);
	return PQ_ERR_SUCCESS;
}

void PriorityQueue_pop(PriorityQueue *pq) {
	if (PriorityQueue_size(pq) == 0) return;
	Vector_set(&pq->vec, 0, Vector_offset(&pq->vec, Vector_size(&pq->vec) - 1));
	(void) Vector_pop_back(&pq->vec); // ignore return since empty case checked
	_PriorityQueue_heapify_down(pq, 0);
}

void *PriorityQueue_top(PriorityQueue *pq) {
	if (PriorityQueue_size(pq) == 0) return NULL;
	return Vector_offset(&pq->vec, 0);
}
