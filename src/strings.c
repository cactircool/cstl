#include "strings.h"
#include "error.h"
#include "view.h"
#include <stdlib.h>
#include <string.h>

Errable(String) String_init() {
	String s;
	StringError result;
	if ((result = String_create(&s)))
		return Err(result, String);
	return Ok(s, String);
}

void String_default(String *s) {
	s->data = NULL;
	s->size = 0;
	s->_capacity = 0;
}

StringError String_create(String *s) {
	void *data = (char *) malloc(sizeof(char) * 32);
	if (!data) return STR_ERR_OOM;
	s->data = data;
	s->size = 0;
	s->_capacity = 32;
	return STR_ERR_SUCCESS;
}

StringError String_cpy(String *dest, const String *src) {
	if (!src || src->size == 0) {
		String_default(dest);
		return STR_ERR_SUCCESS;
	}

	void *data = realloc(dest->data, src->size);
	if (!data) return STR_ERR_OOM;
	dest->data = data;
	memcpy(dest->data, src->data, src->size);
	dest->size = src->size;
	dest->_capacity = 0;
	return STR_ERR_SUCCESS;
}

void String_mv(String *dest, String *src) {
	String_invalidate(dest);

	dest->data = src->data;
	dest->size = src->size;
	dest->_capacity = src->_capacity;

	String_default(src);
}

void String_invalidate(String *s) {
	free(s->data);
	String_default(s);
}

void String_clear(String *s) {
	s->_capacity = s->_capacity + s->size;
	s->size = 0;
}

StringError String_shrink(String *s) {
	if (s->_capacity == 0) return STR_ERR_SUCCESS;
	char *data = realloc(s->data, s->size * sizeof(char));
	if (!data) return STR_ERR_OOM;
	s->data = data;
	s->_capacity = 0;
	return STR_ERR_SUCCESS;
}

size_t String_size(const String *s) {
	return s->size;
}

const char *String_raw_data(const String *s) {
	return s->data;
}

Errable(CString) String_cstring(String *s) {
	if (String_size(s) == 0 || String_get(s, String_size(s) - 1) != '\0') {
		CStringError result;
		if ((result = (CStringError) String_reserve(s, 1)))
			return Err(result, CString);
		if ((result = (CStringError) String_append_char(s, '\0')))
			return Err(result, CString);
		if (String_get(s, String_size(s) - 1) != '\0')
			return Err(CSTR_ERR_MISSING_NULL_TERMINATOR, CString);
	}
	return Ok(s->data, CString);
}

char String_get(const String *s, size_t index) {
	return s->data[index];
}

void String_set(String *s, size_t index, char c) {
	s->data[index] = c;
}

StringError String_reserve(String *s, size_t capacity) {
	if (s->_capacity >= capacity) return STR_ERR_SUCCESS;
	void *data = realloc(s->data, s->size + capacity);
	if (!data) return STR_ERR_OOM;
	s->data = data;
	s->_capacity += (capacity - s->_capacity);
	return STR_ERR_SUCCESS;
}

StringError String_append_char(String *s, char c) {
	if (s->_capacity > 0) {
		s->data[s->size++] = c;
		--s->_capacity;
		return STR_ERR_SUCCESS;
	}

	size_t nlen = (s->size * 2) + 1;
	void *data = realloc(s->data, nlen);
	if (!data) return STR_ERR_OOM;
	s->data = data;
	s->_capacity = nlen - (s->size + 1);
	s->data[s->size++] = c;
	return STR_ERR_SUCCESS;
}

StringError String_append_string(String *s, String *a) {
	if (a->size == 0) return STR_ERR_SUCCESS; // implies a->data == NULL or full of air

	if (s->_capacity >= a->size) {
		memcpy(s->data + s->size, a->data, a->size);
		s->size += a->size;
		s->_capacity -= a->size;
		return STR_ERR_SUCCESS;
	}

	void *data = realloc(s->data, s->size + a->size);
	if (!data) return STR_ERR_OOM;
	s->data = data;
	memcpy(s->data + s->size, a->data, a->size);
	s->size += a->size;
	s->_capacity = 0;
	return STR_ERR_SUCCESS;
}

StringError String_append_cstring(String *s, const char *a) {
	if (!a) return STR_ERR_SUCCESS;

	while (*a && s->_capacity) {
		s->data[s->size++] = *a;
		--s->_capacity;
		++a;
	}

	if (!*a) return STR_ERR_SUCCESS;

	size_t len = strlen(a);
	void *data = realloc(s->data, s->size + len);
	if (!data) return STR_ERR_OOM;
	s->data = data;
	memcpy(s->data + s->size, a, len);
	s->size += len;
	s->_capacity = 0;
	return STR_ERR_SUCCESS;
}

bool String_eq_string(const String *a, const String *b) {
	if (a->size != b->size) return false;
	for (size_t i = 0; i < a->size; ++i)
		if (String_get(a, i) != String_get(b, i))
			return false;
	return true;
}

bool String_eq_cstring(const String *a, const char *b) {
	for (size_t i = 0; i < a->size; ++i, ++b) {
		if (!*b) return false;
		if (String_get(a, i) != *b)
			return false;
	}
	if (*b) return false;
	return true;
}

int String_cmp_string(const String *a, const String *b) {
	size_t len = a->size > b->size ? b->size : a->size;
	for (size_t i = 0; i < len; ++i)
		if (String_get(a, i) != String_get(b, i))
			return String_get(a, i) - String_get(b, i);
	if (a->size == b->size) return 0;
	if (a->size > b->size) return 1;
	return -1;
}

int String_cmp_cstring(const String *a, const char *b) {
	for (size_t i = 0; i < a->size; ++i, ++b)
		if (String_get(a, i) != *b)
			return String_get(a, i) - *b;
	if (!*b) return 0;
	return -*b;
}

Errable(String) String_substring(String *s, size_t from, size_t to) {
	String str = {
		.size = to - from,
		._capacity = 0,
	};
	str.data = malloc(str.size * sizeof(char));
	if (!str.data) return Err(STR_ERR_OOM, String);
	memcpy(str.data, s->data + from, to - from);
	return Ok(str, String);
}

Errable(String) String_from_cstring(const char *s) {
	size_t len = strlen(s);
	String str;
	str.size = len;
	str._capacity = 0;
	str.data = (char *) malloc(len * sizeof(char));
	if (!str.data)
		return Err(STR_ERR_OOM, String);
	memcpy(str.data, s, len);
	return Ok(str, String);
}

ViewOf(String) String_view(String *s, size_t from, size_t to) {
	return View(s->data + from, to - from, sizeof(char));
}

SliceOf(String) String_slice(String *s, size_t from, size_t to) {
	return Slice(s->data + from, to - from, sizeof(char));
}
