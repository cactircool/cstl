#pragma once

#include "error.h"
#include <stddef.h>

typedef struct Array {
	void *data; // array data
	size_t _member_size; // sizeof(T)
	const size_t size; // array size relative to sizeof(T)
} Array;

DefErr(Array); // (Array, Error) tuple structure

// Idiomatic Array<T>[size] style initialization but with macros -> Array(T, size)
#define Array(type, size) (Array_init(sizeof(type), size))

/**
 * TODO: finish documentation I'm so lazy jesus christ
 */
ErrType(Array) Array_init(size_t member_size, size_t size);
Error Array_create(Array *arr, size_t member_size, size_t size);

Error Array_cpy(Array *dest, const Array *src);
void Array_mv(Array *dest, Array *src);

size_t Array_size(Array *arr);
#define Array_data(arr, type) ((type *) arr->data)
#define Array_get(arr, index, type) (Array_data(arr, type)[index])

void Array_invalidate(Array *arr);
