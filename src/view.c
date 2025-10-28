#include "view.h"

View View_init(const void *data, size_t size, size_t member_size) {
	return (View) {
		.data = data,
		.size = size,
		._member_size = member_size
	};
}

void View_create(View *v, const void *data, size_t size, size_t member_size) {
	v->data = data;
	*((size_t *) &v->size) = size;
	*((size_t *) &v->_member_size) = member_size;
}

void *View_offset(View *v, size_t index) {
	return ((char *) v->data) + (index * v->_member_size);
}
