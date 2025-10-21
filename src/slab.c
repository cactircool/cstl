#include "slab.h"
#include "arena.h"
#include "error.h"
#include "pqueue.h"
#include <stdlib.h>
#include <string.h>

int _SlabAllocator_slab_comparator(Arena *a, Arena *b) {
	return Arena_space(a) - Arena_space(b);
}

ErrType(SlabAllocator) SlabAllocator_init(size_t slab_size) {
	SlabAllocator ha;
	Error result;
	if ((result = SlabAllocator_create(&ha, slab_size)))
		return Err(result, SlabAllocator);
	return Ok(ha, SlabAllocator);
}

Error SlabAllocator_create(SlabAllocator *sa, size_t slab_size) {
	sa->slab_size = slab_size;
	return PriorityQueue_create(&sa->slabs, sizeof(Arena), (int (*)(void *, void *)) _SlabAllocator_slab_comparator);
}

void *SlabAllocator_alloc(SlabAllocator *sa, size_t bytes) {
	while (true) {
		Arena *top = PriorityQueue_top(&sa->slabs);
		if (!top) return NULL;

		if (Arena_space(top) < bytes) {
			Arena a;
			Arena_create(&a, bytes > sa->slab_size ? bytes : sa->slab_size);
			PriorityQueue_push(&sa->slabs, &a);
			continue;
		}

		PriorityQueue_pop(&sa->slabs);
		void *alloc = Arena_alloc(top, bytes);
		PriorityQueue_push(&sa->slabs, top);
		return alloc;
	}
}

void SlabAllocator_invalidate(SlabAllocator *sa) {
	PriorityQueue_custom_invalidate(&sa->slabs, (void (*)(void *)) Arena_invalidate);
	sa->slab_size = 0;
}
