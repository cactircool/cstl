#pragma once

#include "error.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct String {
	char *data;
	size_t size;
	size_t _capacity;
} String;

typedef const char * CString;

DefErr(String);
DefErr(CString);

ErrType(String) String_init();
void String_default(String *s);
Error String_create(String *s);

Error String_cpy(String *dest, const String *src);
void String_mv(String *dest, String *src);

void String_invalidate(String *s);
void String_clear(String *s);
Error String_shrink(String *s);

size_t String_size(const String *s);
const char *String_raw_data(const String *s);
ErrType(CString) String_cstring(String *s);

char String_get(const String *s, size_t index);
void String_set(String *s, size_t index, char c);
Error String_reserve(String *s, size_t capacity);

Error String_append_char(String *s, char c);
Error String_append_string(String *s, String *a);
Error String_append_cstring(String *s, const char *a);

bool String_eq_string(const String *a, const String *b);
bool String_eq_cstring(const String *a, const char *b);

int String_cmp_string(const String *a, const String *b);
int String_cmp_cstring(const String *a, const char *b);

char *String_begin(String *s);
char *String_end(String *s);

ErrType(String) String_substring(String *s, size_t from, size_t to);

#define STRING_CTX(name) \
	for (ErrType(String) __##name##__err__ = String_init(), *__once__##name__ = (void*)1; !__##name##__err__.fail && __once__##name__;) \
		for (String name = __##name##__err__.success; __once__##name__; __once__##name__ = NULL)
