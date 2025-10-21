#pragma once

struct String;

/**
 * @brief Defines a generic result type combining a success and an error value.
 *
 * This macro defines a new struct type named `Errable<SuccessType>` that
 * represents the result of an operation that may either succeed or fail.
 *
 * The generated struct has two members:
 * - `success` — a value of type `S`, representing the success result.
 * - `fail` — a value of type `F`, representing an error code.
 *
 * Example:
 * @code
 * Result(int, MyError); // defines Errableint
 * Errable(int) res = Ok(42, int);
 * if (res.fail == 0) { ... }
 * @endcode
 *
 * @param S The success type.
 * @param F The failure (error) type.
 */
#define Result(S, F) typedef struct { \
	S success; \
	F fail; \
} Errable##S

/**
 * @brief Refers to a previously defined `Errable` result type.
 *
 * This macro simplifies the use of a generated result type
 * by allowing concise syntax such as `Errable(String)` instead
 * of manually writing `ErrableString`.
 *
 * @param type The name of the success type used in the original `Result()` definition.
 * @return The corresponding `Errable<type>` type.
 *
 * Example:
 * @code
 * Result(String, StringError);
 * Errable(String) s = Ok(my_string, String);
 * @endcode
 */
#define Errable(type) Errable##type

/**
 * @brief Constructs a successful `Errable` result.
 *
 * This macro creates the "Ok" variant of an errorable type,
 * representing a successful operation outcome.
 *
 * @param value The success value.
 * @param type The type used in the `Result()` definition.
 * @return An `Errable(type)` struct initialized with a success value and zero failure.
 *
 * Example:
 * @code
 * Errable(int) res = Ok(10, int);
 * @endcode
 */
#define Ok(value, type) (Errable(type)) { \
	.success = value, \
	.fail = 0, \
}

/**
 * @brief Constructs a failed `Errable` result.
 *
 * This macro creates the "Err" variant of an errorable type,
 * representing an operation that failed with a specific error code.
 *
 * @param err The error value.
 * @param type The type used in the `Result()` definition.
 * @return An `Errable(type)` struct initialized with an error.
 *
 * Example:
 * @code
 * Errable(int) res = Err(MY_ERROR_CODE, int);
 * @endcode
 */
#define Err(err, type) (Errable(type)) { \
	.fail = err, \
}

/**
 * @brief Terminates the program with a fatal error message.
 *
 * This function reports an unrecoverable error and typically
 * halts execution. The exact termination behavior (e.g. printing
 * the message, aborting, etc.) depends on the implementation.
 *
 * @param msg A String object describing the fatal error.
 *
 * @note
 * This function does not return.
 */
void fatal(struct String msg);
