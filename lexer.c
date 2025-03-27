// LAB 2 START
#include "compiler.h"
#include <string.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"

#define LEX_GETC_IF(buffer, c, exp) \
	for (c = peekc(); exp; c = peekc()) { \
		buffer_write(buffer, c); \
		nextc(); \
	}


struct token* read_next_token();
static struct lex_process* lex_process;
static struct token tmp_token;
