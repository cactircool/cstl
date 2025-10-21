#pragma once

typedef enum Error {
	SUCCESS = 0,

	OOM,
	CSTR_BAD_TERMINATOR,
	EMPTY_POP_BACK,
	NO_ARENAS,
} Error;

#define DefErr(type) typedef struct { \
	type success; \
	Error fail; \
} Err##type

#define ErrType(type) Err##type

#define Ok(value, type) (ErrType(type)) { \
	.success = value, \
	.fail = SUCCESS, \
}

#define Err(err, type) (ErrType(type)) { \
	.fail = err, \
}
