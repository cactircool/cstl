#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vector.h"
#include "array.h"
#include "strings.h"
#include "wptr.h"
#include "uptr.h"
#include "sptr.h"
#include "arena.h"
#include "pqueue.h"
#include "slab.h"
#include "error.h"

int int_comparator(void *a, void *b) {
    int x = *(int*)a;
    int y = *(int*)b;
    return y - x; // max-heap
}

int main() {
    printf("==== CSTL Test Suite ====\n");

    // ---- Vector test ----
    {
        Errable(Vector) vres = Vector_init(sizeof(int));
        assert(!vres.fail);
        Vector v = vres.success;
        int x = 42;
        assert(Vector_append(&v, &x) == VEC_ERR_SUCCESS);
        assert(Vector_size(&v) == 1);
        assert(Vector_get(&v, 0, int) == 42);
        Vector_invalidate(&v);
        printf("[Vector] Passed\n");
    }

    // ---- Array test ----
    {
        Errable(Array) ares = Array_init(sizeof(int), 4);
        assert(!ares.fail);
        Array a = ares.success;
        Array_data(&a, int)[0] = 7;
        assert(Array_get(&a, 0, int) == 7);
        Array_invalidate(&a);
        printf("[Array] Passed\n");
    }

    // ---- String test ----
    {
        STRING_CTX(s) {
            String_append_cstring(&s, "hello");
            assert(String_size(&s) == 5);
            Errable(CString) cstr = String_cstring(&s);
            assert(!cstr.fail);
            assert(strcmp(cstr.success, "hello") == 0);
        }
        printf("[String] Passed\n");
    }

    // ---- WeakPtr / UniquePtr / SharedPtr test ----
    {
        int *val = malloc(sizeof(int));
        *val = 123;

        // UniquePtr
        UniquePtr up = UniquePtr_init(val, free);
        WeakPtr *wp = UniquePtr_reader(&up);
        assert(*(int*)WeakPtr_extract(wp) == 123);
        UniquePtr_invalidate(&up);

        // SharedPtr
        int *v2 = malloc(sizeof(int));
        *v2 = 456;
        SharedPtr sp = SharedPtr_init(v2, free);
        SharedPtr_clone(&sp);
        WeakPtr *wsp = SharedPtr_reader(&sp);
        assert(*(int*)WeakPtr_extract(wsp) == 456);
        SharedPtr_invalidate(&sp);

        printf("[Ptr Types] Passed\n");
    }

    // ---- ArenaAllocator test ----
    {
        ArenaAllocator arena;
        ArenaAllocator_create(&arena, 1024);
        void *ptr = ArenaAllocator_alloc(&arena, 64);
        assert(ptr != NULL);
        ArenaAllocator_invalidate(&arena);
        printf("[ArenaAllocator] Passed\n");
    }

    // ---- PriorityQueue test ----
    {
        Errable(PriorityQueue) pqres = PriorityQueue_init(sizeof(int), int_comparator);
        assert(!pqres.fail);
        PriorityQueue pq = pqres.success;

        int a = 10, b = 20;
        PriorityQueue_push(&pq, &a);
        PriorityQueue_push(&pq, &b);

        int *top = PriorityQueue_top(&pq);
        assert(*top == 20);
        PriorityQueue_pop(&pq);
        top = PriorityQueue_top(&pq);
        assert(*top == 10);

        PriorityQueue_invalidate(&pq);
        printf("[PriorityQueue] Passed\n");
    }

    // ---- SlabAllocator test ----
    {
        Errable(SlabAllocator) sares = SlabAllocator_init(128);
        assert(!sares.fail);
        SlabAllocator sa = sares.success;

        void *p1 = SlabAllocator_alloc(&sa, 64);
        void *p2 = SlabAllocator_alloc(&sa, 32);
        assert(p1 && p2);

        SlabAllocator_invalidate(&sa);
        printf("[SlabAllocator] Passed\n");
    }

    printf("==== All tests passed ====\n");
    return 0;
}
