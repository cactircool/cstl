#pragma once

#include <stddef.h>

typedef struct View {
	const void *data;
	const size_t size;
	const size_t _member_size;
} View;

#define ViewOf(T) View
#define View(data, size, member_size) (View_init(data, size, member_size))

View View_init(const void *data, size_t size, size_t member_size);
void View_create(View *v, const void *data, size_t size, size_t member_size);

#define View_get(v, index, type) (((type *) (v)->data)[index])
void *View_offset(View *v, size_t index);
