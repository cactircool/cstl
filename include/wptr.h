#pragma once

typedef struct WeakPtr {
	const void *ptr;
} WeakPtr;

WeakPtr WeakPtr_init(void *ptr);

const void *WeakPtr_extract(WeakPtr *wptr);
