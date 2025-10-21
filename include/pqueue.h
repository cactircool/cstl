#pragma once

#include "error.h"
#include "vector.h"
#include <stdbool.h>

typedef struct PriorityQueue {
	Vector vec;
	int (*comparator)(void *, void *);
} PriorityQueue;

DefErr(PriorityQueue);

ErrType(PriorityQueue) PriorityQueue_init(size_t member_size, int (*comparator)(void *, void *));
Error PriorityQueue_create(PriorityQueue *pq, size_t member_size, int (*comparator)(void *, void *));

Error PriorityQueue_cpy(PriorityQueue *dest, const PriorityQueue *src);
void PriorityQueue_mv(PriorityQueue *dest, PriorityQueue *src);

void PriorityQueue_invalidate(PriorityQueue *pq);
void PriorityQueue_custom_invalidate(PriorityQueue *pq, void (*destructor)(void *));
void PriorityQueue_clear(PriorityQueue *pq);
Error PriorityQueue_shrink(PriorityQueue *pq);

size_t PriorityQueue_size(PriorityQueue *pq);
const void *PriorityQueue_raw_data(const PriorityQueue *pq);

int _PriorityQueue_compare(PriorityQueue *pq, size_t index_a, size_t index_b);
void _PriorityQueue_heapify_up(PriorityQueue *pq, size_t index);
void _PriorityQueue_heapify_down(PriorityQueue *pq, size_t index);

Error PriorityQueue_push(PriorityQueue *pq, void *data);
void PriorityQueue_pop(PriorityQueue *pq);
void *PriorityQueue_top(PriorityQueue *pq);

#define PriorityQueue_begin(pq, type) Vector_begin(&pq->vec, type)
#define PriorityQueue_end(pq, type) Vector_end(&pq->vec, type)
