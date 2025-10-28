#include "array.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

Errable(Array) Array_init(size_t member_size, size_t size) {
	Array arr = { .size = size };
	ArrayError result;
	if ((result = Array_create(&arr, member_size, size)))
		return Err(result, Array);
	return Ok(arr, Array);
}

ArrayError Array_create(Array *arr, size_t member_size, size_t size) {
	*((size_t *) &arr->size) = size;
	*((size_t *) &arr->_member_size) = member_size;
	arr->data = malloc(member_size * size);
	if (!arr->data) return ARRAY_ERR_OOM;
	return ARRAY_ERR_SUCCESS;
}

ArrayError Array_cpy(Array *dest, const Array *src) {
	if (dest->_member_size * dest->size != src->_member_size * src->size) {
		void *data = realloc(dest->data, src->_member_size * src->size);
		if (!data) return ARRAY_ERR_OOM;
		dest->data = data;
	}
	*((size_t *) &dest->size) = src->size;
	*((size_t *) &src->_member_size) = src->_member_size;

	memcpy(dest->data, src->data, src->size * src->_member_size);
	return ARRAY_ERR_SUCCESS;
}

void Array_mv(Array *dest, Array *src) {
	Array_invalidate(dest);
	memcpy(dest, src, sizeof(Array));
	src->data = NULL;
	*((size_t *) &src->_member_size) = 0;
	*((size_t *) &src->size) = 0;
}

size_t Array_size(Array *arr) {
	return arr->size;
}

void Array_invalidate(Array *arr) {
	free(arr->data);
	arr->data = NULL;
	*((size_t *) &arr->_member_size) = 0;
	*((size_t *) &arr->size) = 0;
}

Errable(Array) Array_sublist(Array *arr, size_t from, size_t to) {
	Array a = {
		.size = to - from,
		._member_size = arr->_member_size,
	};
	a.data = malloc(a.size * a._member_size);
	if (!a.data) return Err(ARRAY_ERR_OOM, Array);
	memcpy(a.data, arr->data + from, a.size);
	return Ok(a, Array);
}

ViewOf(Array) Array_view(Array *arr, size_t from, size_t to) {
	return View(((char *) arr->data) + (from * arr->_member_size), (to - from), arr->_member_size);
}

SliceOf(Array) Array_slice(Array *arr, size_t from, size_t to) {
	return Slice(((char *) arr->data) + (from * arr->_member_size), (to - from), arr->_member_size);
}
