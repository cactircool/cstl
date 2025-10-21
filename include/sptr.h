#pragma once

#include <stddef.h>
#include "wptr.h"

typedef struct SharedPtr {
	void *ptr;
	void (*deletor)(void *);
	size_t ref_count;
} SharedPtr;

SharedPtr SharedPtr_init(void *ptr, void (*deletor)(void *));

void *SharedPtr_extract(SharedPtr *sptr);

SharedPtr *SharedPtr_clone(SharedPtr *sptr);
WeakPtr *SharedPtr_reader(SharedPtr *sptr);

void SharedPtr_invalidate(SharedPtr *sptr);
