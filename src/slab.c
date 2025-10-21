#include "slab.h"
#include "arena.h"
#include "error.h"
#include "pqueue.h"
#include <stdlib.h>
#include <string.h>

int _SlabAllocator_slab_comparator(ArenaAllocator *a, ArenaAllocator *b) {
	return ArenaAllocator_space(a) - ArenaAllocator_space(b);
}

Errable(SlabAllocator) SlabAllocator_init(size_t slab_size) {
	SlabAllocator ha;
	SlabAllocatorError result;
	if ((result = SlabAllocator_create(&ha, slab_size)))
		return Err(result, SlabAllocator);
	return Ok(ha, SlabAllocator);
}

SlabAllocatorError SlabAllocator_create(SlabAllocator *sa, size_t slab_size) {
	sa->slab_size = slab_size;
	return (SlabAllocatorError) PriorityQueue_create(&sa->slabs, sizeof(ArenaAllocator), (int (*)(void *, void *)) _SlabAllocator_slab_comparator);
}

void *SlabAllocator_alloc(SlabAllocator *sa, size_t bytes) {
	while (true) {
		ArenaAllocator *top = PriorityQueue_top(&sa->slabs);
		if (!top || ArenaAllocator_space(top) < bytes) {
			ArenaAllocator a;
			ArenaAllocator_create(&a, bytes > sa->slab_size ? bytes : sa->slab_size);
			PriorityQueue_push(&sa->slabs, &a);
			continue;
		}

		PriorityQueue_pop(&sa->slabs);
		void *alloc = ArenaAllocator_alloc(top, bytes);
		PriorityQueue_push(&sa->slabs, top);
		return alloc;
	}
}

void SlabAllocator_invalidate(SlabAllocator *sa) {
	PriorityQueue_custom_invalidate(&sa->slabs, (void (*)(void *)) ArenaAllocator_invalidate);
	sa->slab_size = 0;
}
