#pragma once

typedef struct UniquePtr {
	void *ptr;
	void (*deletor)(void *);
} UniquePtr;

UniquePtr UniquePtr_init(void *ptr, void (*deletor)(void *));

void *UniquePtr_extract(UniquePtr *uptr);

const void *UniquePtr_reader(UniquePtr *uptr);
UniquePtr UniquePtr_writer(UniquePtr *uptr);

void UniquePtr_invalidate(UniquePtr *uptr);

#define UNIQUE_PTR_CTX(name, value, deletor) \
	for (UniquePtr name = UniquePtr_init((void *) value, (void (*)(void *)) deletor), *__once__##name__ = (void *)1; __once__##name__; UniquePtr_invalidate(&name), __once__##name__ = NULL)
