#include "slice.h"

Slice Slice_init(void *data, size_t size, size_t member_size) {
	return (Slice) {
		.data = data,
		.size = size,
		._member_size = member_size
	};
}

void Slice_create(Slice *s, void *data, size_t size, size_t member_size) {
	s->data = data;
	*((size_t *) &s->size) = size;
	*((size_t *) &s->_member_size) = member_size;
}

void *Slice_offset(Slice *s, size_t index) {
	return ((char *) s->data) + (index * s->_member_size);
}
