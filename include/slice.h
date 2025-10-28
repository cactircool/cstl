#pragma once

#include <stddef.h>

typedef struct Slice {
	void *data;
	const size_t size;
	const size_t _member_size;
} Slice;

#define SliceOf(T) Slice
#define Slice(data, size, member_size) (Slice_init(data, size, member_size))

Slice Slice_init(void *data, size_t size, size_t member_size);
void Slice_create(Slice *s, void *data, size_t size, size_t member_size);

#define Slice_get(s, index, type) (((type *) (s)->data)[index])
void *Slice_offset(Slice *s, size_t index);
