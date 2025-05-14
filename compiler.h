#ifndef COMPILER_H
#define COMPILER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define NUMERIC_CASE \
	case '0':	\
	case '1':	\
	case '2':	\
	case '3':	\
	case '4':	\
	case '5':	\
	case '6':	\
	case '7':	\
	case '8':	\
	case '9' 

#define OPERATOR_CASE \
    case '+': \
    case '-': \
    case '*': \
    case '%': \
    case '!': \
    case '=': \
    case '>': \
    case '<': \
    case '~': \
    case '|': \
    case '&': \
    case '(': \
    case '[': \
    case ',': \
	case '.': \
    case '?'
	// case 'a': \
	// case 'b': \
	// case 'c': \
	// case 'd': \
	// case 'e': \
	// case 'f': \
	// case 'g': \
	// case 'h': \
	// case 'i': \
	// case 'j': \
	// case 'k': \
	// case 'l': \
	// case 'm': \
	// case 'n': \
	// case 'o': \
	// case 'p': \
	// case 'q': \
	// case 'r': \
	// case 's': \
	// case 't': \
	// case 'u': \
	// case 'v': \
	// case 'w': \
	// case 'x': \
	// case 'y': \
	// case 'z': \
	// case 'A': \
	// case 'B': \
	// case 'C': \
	// case 'D': \
	// case 'E': \
	// case 'F': \
	// case 'G': \
	// case 'H': \
	// case 'I': \
	// case 'J': \
	// case 'K': \
	// case 'L': \
	// case 'M': \
	// case 'N': \
	// case 'O': \
	// case 'P': \
	// case 'Q': \
	// case 'R': \
	// case 'S': \
	// case 'T': \
	// case 'U': \
	// case 'V': \
	// case 'W': \
	// case 'X': \
	// case 'Y': \
	// case 'Z'


#define SYMBOL_CASE \
    case '{': \
    case '}': \
    case ':': \
    case ';': \
    case '#': \
    case ')': \
    case ']'

#define NEWLINE_CASE \
	case '\n'


// LAB 2 STARTS HERE

#define NUMERIC_CASE \
	case '0':	\
	case '1':	\
	case '2':	\
	case '3':	\
	case '4':	\
	case '5':	\
	case '6':	\
	case '7':	\
	case '8':	\
	case '9'

struct pos {
	int line;
	int col;
	const char* filename;
};

enum {
	LEXICAL_ANALYSIS_ALL_OK,
	LEXICAL_ANALYSIS_INPUT_ERROR
};

enum token_type {
	TOKEN_TYPE_KEYWORD,
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_OPERATOR,
	TOKEN_TYPE_SYMBOL,
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_STRING,
	TOKEN_TYPE_COMMENT,
	TOKEN_TYPE_NEWLINE,
};

struct token {
	int type;
	int flags;

	struct pos pos;

	union {
		char cval;
		const char *sval;
		unsigned int inum;
		unsigned long lnum;
		unsigned long long llnum;
		void* any;
	};
	// true quando há um espaço em branco entre o token atual e o próximo token.
	bool whitespace;

	// Retira a string que estiver dentro de parênteses. Ex: (1+2+3) resulta em 1+2+3.
	const char* between_brackets;
};

struct lex_process;

// Definição de ponteiros para funções.
typedef char (*LEX_PROCESS_NEXT_CHAR) (struct lex_process* process);
typedef char (*LEX_PROCESS_PEEK_CHAR) (struct lex_process* process);
typedef void (*LEX_PROCESS_PUSH_CHAR) (struct lex_process* process, char c);

struct lex_process_functions {
	LEX_PROCESS_NEXT_CHAR next_char;
	LEX_PROCESS_PEEK_CHAR peek_char;
	LEX_PROCESS_PUSH_CHAR push_char;
};

struct lex_process {
	struct pos pos;
	struct vector* token_vec;
	struct compile_process* compiler;

	int current_expression_count;  // Quantos parênteses existem no momento.
	
	struct buffer* parentheses_buffer;
	struct lex_process_functions* function;

	void* private;  // Dados privados que o lexer não entende mas o programador entende.
};

// Funções do arquivo cprocess.c
char compile_process_next_char(struct lex_process* lex_process);
char compile_process_peek_char(struct lex_process* lex_process);
void compile_process_push_char(struct lex_process* lex_process, char c);

// Funções do arquivo lex_process.c
struct lex_process* lex_process_create(struct compile_process* compiler, struct lex_process_functions* functions, void *private);
void lex_process_free(struct lex_process* process);
void* lex_process_private(struct lex_process* process);
struct vector* lex_process_tokens(struct lex_process* process);

// Funções do arquivo lexer.c
int lex(struct lex_process* process);

// LAB 2 ENDS HERE

enum {
	COMPILER_FILE_COMPILED_OK,
	COMPILER_FAILED_WITH_ERRORS,
};

struct compile_process
{
    // Como o arquivo deve ser compilado
    int flags;

    /* LAB2*/
    struct pos pos;

    struct compile_process_imput_file
    {
        FILE *fp;
        const char *abs_path;
    } cfile;

    struct vector *token_vec;
    struct vector *node_vec;
    struct vector *node_tree_vec;

    FILE *ofile;
};

enum
{
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_EXPRESSION_PARENTHESIS,
    NODE_TYPE_NUMBER,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_STRING,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_VARIABLE_LIST,
    NODE_TYPE_FUNCTION,
    NODE_TYPE_BODY,
    NODE_TYPE_STATEMENT_RETURN,
    NODE_TYPE_STATEMENT_IF,
    NODE_TYPE_STATEMENT_ELSE,
    NODE_TYPE_STATEMENT_WHILE,
    NODE_TYPE_STATEMENT_DO_WHILE,
    NODE_TYPE_STATEMENT_FOR,
    NODE_TYPE_STATEMENT_BREAK,
    NODE_TYPE_STATEMENT_CONTINUE,
    NODE_TYPE_STATEMENT_SWITCH,
    NODE_TYPE_STATEMENT_CASE,
    NODE_TYPE_STATEMENT_DEFAULT,
    NODE_TYPE_STATEMENT_GOTO,
    NODE_TYPE_UNARY,
    NODE_TYPE_TENARY,
    NODE_TYPE_LABEL,
    NODE_TYPE_STRUCT,
    NODE_TYPE_UNION,
    NODE_TYPE_BRACKET,
    NODE_TYPE_CAST,
    NODE_TYPE_BLANK
};

enum {
    PARSE_ALL_OK,
    PARSE_GENERAL_ERROR
};

// Cada nó uma parte do inputfile.
struct node {
    int type;
    int flags;
    struct pos pos;

    struct node_binded {
        // Ponteiro para o body node.
        struct node* owner;

        // Ponteiro para a função que o nó está.
        struct node* funtion;
    } binded;

    // Estrutura similar ao token
    union {
        char cval;
        const char* sval;
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void* any;
    };
};

int compile_file(const char* filename, const char* out_filename, int flags);
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags);

int compile_error(struct compile_process* compiler, const char* msg, ...);
int compile_warning(struct compile_process* compiler, const char* msg, ...);

#endif



/* BEGIN - LAB 3*/
