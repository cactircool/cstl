#include "vector.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

Errable(Vector) Vector_init(size_t member_size) {
	Vector v;
	VectorError result;
	if ((result = Vector_create(&v, member_size)))
		return Err(result, Vector);
	return Ok(v, Vector);
}

void Vector_default(Vector *v, size_t member_size) {
	v->data = NULL;
	v->_member_size = member_size;
	v->size = 0;
	v->_capacity = 0;
}

VectorError Vector_create(Vector *v, size_t member_size) {
	void *data = malloc(32 * member_size);
	if (!data) return VEC_ERR_OOM;

	v->data = data;
	v->_member_size = member_size;
	v->_capacity = 32;
	v->size = 0;
	return VEC_ERR_SUCCESS;
}

VectorError Vector_cpy(Vector *dest, const Vector *src) {
	if (!src) {
		Vector_default(dest, 0);
		return VEC_ERR_SUCCESS;
	}

	if (src->size == 0) {
		Vector_default(dest, src->_member_size);
		return VEC_ERR_SUCCESS;
	}

	void *data = realloc(dest->data, src->_member_size * src->size);
	if (!data) return VEC_ERR_OOM;
	dest->data = data;
	memcpy(dest->data, src->data, src->_member_size * src->size);
	dest->size = src->size;
	dest->_member_size = src->_member_size;
	dest->_capacity = 0;
	return VEC_ERR_SUCCESS;
}


void Vector_mv(Vector *dest, Vector *src) {
	Vector_invalidate(dest);

	dest->data = src->data;
	dest->size = src->size;
	dest->_member_size = src->_member_size;
	dest->_capacity = src->_capacity;

	Vector_default(src, src->_member_size);
}

void Vector_invalidate(Vector *v) {
	free(v->data);
	v->data = NULL;
	v->size = 0;
	v->_capacity = 0;
}

void Vector_custom_invalidate(Vector *v, void (*destructor)(void *)) {
	for (size_t i = 0; i < v->size; ++i)
		destructor(Vector_offset(v, i));
	Vector_invalidate(v);
}

void Vector_clear(Vector *v) {
	v->_capacity += v->size;
	v->size = 0;
}

VectorError Vector_shrink(Vector *v) {
	if (v->_capacity == 0) return VEC_ERR_SUCCESS;
	void *data = realloc(v->data, v->size * v->_member_size);
	if (!data) return VEC_ERR_OOM;
	v->data = data;
	v->_capacity = 0;
	return VEC_ERR_SUCCESS;
}

size_t Vector_size(Vector *v) {
	return v->size;
}

size_t Vector_member_size(Vector *v) {
	return v->_member_size;
}

const void *Vector_raw_data(const Vector *v) {
	return v->data;
}

void *Vector_offset(Vector *v, size_t offset) {
	return ((char *) v->data) + (offset * v->_member_size);
}

void Vector_set(Vector *v, size_t index, void *data) {
	void *ptr = Vector_offset(v, index);
	memcpy(ptr, data, v->_member_size);
}

VectorError Vector_reserve(Vector *v, size_t capacity) {
	if (v->_capacity >= capacity) return VEC_ERR_SUCCESS;
	void *data = realloc(v->data, v->_member_size * (v->size + capacity));
	if (!data) return VEC_ERR_OOM;
	v->data = data;
	v->_capacity = capacity;
	return VEC_ERR_SUCCESS;
}

VectorError Vector_append(Vector *v, void *data) {
	if (v->_capacity == 0) {
		VectorError result;
		if ((result = Vector_reserve(v, (v->size * 2) + 1)))
			return result;
	}
	memcpy(((char *) v->data) + (v->size++ * v->_member_size), data, v->_member_size);
	--v->_capacity;
	return VEC_ERR_SUCCESS;
}

VectorError Vector_append_members(Vector *v, void *data, size_t n) {
	size_t min = n > v->_capacity ? v->_capacity : n;
	memcpy(((char *) v->data) + (v->size * v->_member_size), data, min * v->_member_size);
	n -= min;
	v->_capacity -= min;
	v->size += min;
	data = ((char *) data) + (min * v->_member_size);

	if (!n) return VEC_ERR_SUCCESS;
	VectorError result;
	if ((result = Vector_reserve(v, n)))
		return result;
	memcpy(((char *) v->data) + (v->size * v->_member_size), data, v->_member_size * n);
	v->size += n;
	v->_capacity -= n;
	return VEC_ERR_SUCCESS;
}

VectorError Vector_append_vector(Vector *v, Vector *a) {
	VectorError result;
	if ((result = Vector_append_members(v, a->data, a->size)))
		return result;
	return VEC_ERR_SUCCESS;
}

void Vector_swap(Vector *v, size_t index_a, size_t index_b) {
	size_t m_size = Vector_member_size(v);
	char tmp[m_size];
	void *a = ((char *) v->data) + (index_a * m_size);
	void *b = ((char *) v->data) + (index_b * m_size);

	memcpy(tmp, a, m_size);
	memcpy(a, b, m_size);
	memcpy(b, tmp, m_size);
}

VectorError Vector_pop_back(Vector *v) {
	if (v->size == 0) return VEC_ERR_EMPTY_POP_BACK;
	++v->_capacity;
	--v->size;
	return VEC_ERR_SUCCESS;
}
