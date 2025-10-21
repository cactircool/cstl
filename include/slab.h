#pragma once

#include "arena.h"
#include "error.h"
#include "pqueue.h"

typedef struct SlabAllocator {
	PriorityQueue slabs;
	size_t slab_size;
} SlabAllocator;

DefErr(SlabAllocator);

int _SlabAllocator_slab_comparator(Arena *a, Arena *b);

ErrType(SlabAllocator) SlabAllocator_init(size_t slab_size);

Error SlabAllocator_create(SlabAllocator *sa, size_t slab_size);

void *SlabAllocator_alloc(SlabAllocator *sa, size_t bytes);

void SlabAllocator_invalidate(SlabAllocator *sa);
