#include "wptr.h"

WeakPtr WeakPtr_init(void *ptr) {
	return (WeakPtr) {
		.ptr = ptr,
	};
}

const void *WeakPtr_extract(WeakPtr *wptr) {
	return wptr->ptr;
}
