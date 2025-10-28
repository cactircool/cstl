#pragma once

#include "error.h"
#include "view.h"
#include "slice.h"
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief A dynamically allocated, mutable string type.
 *
 * Stores character data, current size, and capacity. The internal buffer
 * may not be null-terminated unless explicitly converted to a CString
 * using String_cstring().
 */
typedef struct String {
	char *data;         /**< Pointer to the string's character data. */
	size_t size;        /**< Number of characters currently in use (excluding null terminator). */
	size_t _capacity;   /**< Allocated capacity (in bytes). */
} String;

/**
 * @brief Alias for a null-terminated C-style string.
 */
typedef const char *CString;

/**
 * @brief Error codes for String operations.
 */
typedef enum {
	STR_ERR_SUCCESS = 0,  /**< Operation succeeded. */
	STR_ERR_OOM,          /**< Out of memory. */
} StringError;

/**
 * @brief Error codes for CString operations.
 */
typedef enum {
	CSTR_ERR_SUCCESS = 0,            /**< Operation succeeded. */
	CSTR_ERR_OOM,                    /**< Out of memory. */
	CSTR_ERR_MISSING_NULL_TERMINATOR /**< Missing null terminator in source string. */
} CStringError;

/** @brief Result type for String-returning functions. */
Result(String, StringError);
/** @brief Result type for CString-returning functions. */
Result(CString, CStringError);

/**
 * @brief Creates and initializes an empty String.
 *
 * @return A Result object containing either a valid String or an error code.
 */
Errable(String) String_init();

/**
 * @brief Initializes a String structure without preallocating padding.
 *
 * @param s Pointer to the String to initialize.
 */
void String_default(String *s);

/**
 * @brief Allocates and prepares internal memory for a String.
 *
 * @param s Pointer to the String to initialize.
 * @return STR_ERR_SUCCESS on success, STR_ERR_OOM on allocation failure.
 */
StringError String_create(String *s);

/**
 * @brief Performs a deep copy from one String to another.
 *
 * @param dest Destination String.
 * @param src Source String.
 * @return STR_ERR_SUCCESS on success, STR_ERR_OOM on failure.
 */
StringError String_cpy(String *dest, const String *src);

/**
 * @brief Transfers ownership of a String's data to another String.
 *
 * @param dest Destination String.
 * @param src Source String. This String is invalidated after the operation.
 */
void String_mv(String *dest, String *src);

/**
 * @brief Frees all resources and resets the String to an empty state.
 *
 * @param s Pointer to the String to invalidate.
 */
void String_invalidate(String *s);

/**
 * @brief Clears the contents of the String but keeps its allocated capacity.
 *
 * @param s Pointer to the String to clear.
 */
void String_clear(String *s);

/**
 * @brief Shrinks the String’s capacity to match its current size.
 *
 * @param s Pointer to the String to shrink.
 * @return STR_ERR_SUCCESS on success, STR_ERR_OOM on failure.
 */
StringError String_shrink(String *s);

/**
 * @brief Returns the number of characters currently in the String.
 *
 * @param s Pointer to the String.
 * @return The size of the String.
 */
size_t String_size(const String *s);

/**
 * @brief Returns a pointer to the String’s raw character buffer.
 *
 * @param s Pointer to the String.
 * @return Pointer to raw data (not guaranteed to be null-terminated).
 */
const char *String_raw_data(const String *s);

/**
 * @brief Ensures the String is null-terminated and returns it as a CString.
 *
 * May allocate memory and append a null terminator if necessary.
 * Inefficient for frequent use—prefer caching the CString if needed.
 *
 * @param s Pointer to the String.
 * @return A Result containing a CString pointer or an error code.
 */
Errable(CString) String_cstring(String *s);

/**
 * @brief Returns the character at a specified index.
 *
 * @param s Pointer to the String.
 * @param index Index of the character to retrieve.
 * @return The character at the specified position.
 */
char String_get(const String *s, size_t index);

/**
 * @brief Sets the character at a specified index.
 *
 * @param s Pointer to the String.
 * @param index Index of the character to modify.
 * @param c Character to assign at the given index.
 */
void String_set(String *s, size_t index, char c);

/**
 * @brief Ensures the String can hold additional capacity without reallocation.
 *
 * @param s Pointer to the String.
 * @param capacity Number of additional characters to reserve space for.
 * @return STR_ERR_SUCCESS on success, STR_ERR_OOM on allocation failure.
 */
StringError String_reserve(String *s, size_t capacity);

/**
 * @brief Appends a single character to the String.
 *
 * @param s Pointer to the String.
 * @param c Character to append.
 * @return STR_ERR_SUCCESS on success, STR_ERR_OOM on allocation failure.
 */
StringError String_append_char(String *s, char c);

/**
 * @brief Appends the contents of another String to this one.
 *
 * @param s Destination String.
 * @param a Source String to append.
 * @return STR_ERR_SUCCESS on success, STR_ERR_OOM on allocation failure.
 */
StringError String_append_string(String *s, String *a);

/**
 * @brief Appends a null-terminated C string to this String.
 *
 * @param s Destination String.
 * @param a Source C string (must be null-terminated).
 * @return STR_ERR_SUCCESS on success, STR_ERR_OOM on allocation failure.
 */
StringError String_append_cstring(String *s, const char *a);

/**
 * @brief Checks if two Strings are equal.
 *
 * @param a First String.
 * @param b Second String.
 * @return true if equal, false otherwise.
 */
bool String_eq_string(const String *a, const String *b);

/**
 * @brief Checks if a String equals a C-style string.
 *
 * @param a String to compare.
 * @param b C-style string (must be null-terminated).
 * @return true if equal, false otherwise.
 */
bool String_eq_cstring(const String *a, const char *b);

/**
 * @brief Lexicographically compares two Strings.
 *
 * @param a First String.
 * @param b Second String.
 * @return <0 if a < b, 0 if equal, >0 if a > b.
 */
int String_cmp_string(const String *a, const String *b);

/**
 * @brief Lexicographically compares a String and a C-style string.
 *
 * @param a String to compare.
 * @param b C-style string (must be null-terminated).
 * @return <0 if a < b, 0 if equal, >0 if a > b.
 */
int String_cmp_cstring(const String *a, const char *b);

/**
 * @brief Creates a substring from an existing String.
 *
 * Creates a new String containing the range [from, to).
 *
 * @param s Source String.
 * @param from Starting index (inclusive).
 * @param to Ending index (exclusive).
 * @return A Result containing the new substring or an error code.
 */
Errable(String) String_substring(String *s, size_t from, size_t to);

/**
 * @brief Creates a String from a null-terminated C string.
 *
 * @param s Source C string.
 * @return A Result containing the new String or an error code.
 */
Errable(String) String_from_cstring(const char *s);

ViewOf(String) String_view(String *s, size_t from, size_t to);

SliceOf(String) String_slice(String *s, size_t from, size_t to);

/**
 * @brief Declares a scoped String that auto-invalidates at block exit.
 *
 * @param name Name of the scoped String variable.
 *
 * @note
 * - Automatically invalidates at the end of the scope.
 * - Early returns or gotos within the block are not allowed.
 *
 * @code
 * STRING_CTX(my_str) {
 *     String_append_cstring(&my_str, "Hello, world!");
 * }
 * @endcode
 */
#define STRING_CTX(name) \
	for (Errable(String) __##name##__err__ = String_init(), *__once__##name__ = (void*)1; \
	     !__##name##__err__.fail && __once__##name__;) \
		for (String name = __##name##__err__.success; __once__##name__; __once__##name__ = NULL)
