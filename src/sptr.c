#include "sptr.h"
#include "wptr.h"
#include <stdlib.h>

SharedPtr SharedPtr_init(void *ptr, void (*deletor)(void *)) {
	return (SharedPtr) {
		.ptr = ptr,
		.deletor = deletor,
		.ref_count = 1,
	};
}

void *SharedPtr_extract(SharedPtr *sptr) {
	return sptr->ptr;
}

SharedPtr *SharedPtr_clone(SharedPtr *sptr) {
	++sptr->ref_count;
	return sptr;
}

WeakPtr *SharedPtr_reader(SharedPtr *sptr) {
	return (WeakPtr *) sptr;
}

void SharedPtr_invalidate(SharedPtr *sptr) {
	--sptr->ref_count;
	if (sptr->ref_count != 0) return;
	sptr->deletor(sptr->ptr);
	sptr->ptr = NULL;
	sptr->deletor = free;
}
