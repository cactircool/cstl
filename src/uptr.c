#include "uptr.h"
#include "wptr.h"

#include <stdlib.h>

UniquePtr UniquePtr_init(void *ptr, void (*deletor)(void *)) {
	return (UniquePtr) {
		.ptr = ptr,
		.deletor = deletor,
	};
}

void *UniquePtr_extract(UniquePtr *uptr) {
	return uptr->ptr;
}

WeakPtr *UniquePtr_reader(UniquePtr *uptr) {
	return (WeakPtr *) uptr;
}

UniquePtr UniquePtr_writer(UniquePtr *uptr) {
	UniquePtr new = {
		.ptr = uptr->ptr,
		.deletor = uptr->deletor,
	};
	uptr->ptr = NULL;
	uptr->deletor = free;
	return new;
}

void UniquePtr_invalidate(UniquePtr *uptr) {
	uptr->deletor(UniquePtr_extract(uptr));
	uptr->ptr = NULL;
	uptr->deletor = free;
}
