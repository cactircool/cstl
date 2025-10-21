#include "error.h"
#include "strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fatal(struct String msg) {
	const char *prefix = "\033[31mfatal: \033[0m";
	fwrite(prefix, sizeof(char), strlen(prefix), stderr);
	fwrite(String_raw_data(&msg), sizeof(char), String_size(&msg), stderr);
	fputc('\n', stderr);

	exit(1);
}
