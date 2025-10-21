#pragma once

#include <stddef.h>
#include "error.h"

typedef struct Vector {
	void *data;
	size_t _member_size;
	size_t size;
	size_t _capacity;
} Vector;

DefErr(Vector);

ErrType(Vector) Vector_init(size_t member_size);
void Vector_default(Vector *v, size_t member_size);
Error Vector_create(Vector *v, size_t member_size);

Error Vector_cpy(Vector *dest, const Vector *src);
void Vector_mv(Vector *dest, Vector *src);

void Vector_invalidate(Vector *v);
void Vector_custom_invalidate(Vector *v, void (*destructor)(void *));
void Vector_clear(Vector *v);
Error Vector_shrink(Vector *v);

size_t Vector_size(Vector *v);
size_t Vector_member_size(Vector *v);
const void *Vector_raw_data(const Vector *v);

#define Vector_data(v, type) ((type *) v->data)
#define Vector_get(v, index, type) ((type *) Vector_offset(v, index))
#define Vector_get_value(v, index, type) (*Vector_get(v, index, type))

void *Vector_offset(Vector *v, size_t offset);
void Vector_set(Vector *v, size_t index, void *data);
Error Vector_reserve(Vector *v, size_t capacity);

Error Vector_append(Vector *v, void *data);
Error Vector_append_members(Vector *v, void *data, size_t n);
Error Vector_append_vector(Vector *v, Vector *a);

void Vector_swap(Vector *v, size_t index_a, size_t index_b);
Error Vector_pop_back(Vector *v);

#define Vector_begin(v, type) ((type *) v->data)
#define Vector_end(v, type) (((type *) v->data) + v->size)

#define VECTOR_CTX(name, type) \
	for (ErrType(Vector) __##name##__err__ = Vector_init(sizeof(type)), *__once__##name__ = (void*)1; !__##name##__err__.fail && __once__##name__;) \
		for (Vector name = __##name##__err__.success; __once__##name__; __once__##name__ = NULL)
