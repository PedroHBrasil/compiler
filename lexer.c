// LAB 2 START
#include "compiler.h"
#include <string.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#define LEX_GETC_IF(buffer, c, exp) \
	for (c = peekc(); exp; c = peekc()) { \
		buffer_write(buffer, c); \
		nextc(); \
	}

struct token* read_next_token();
static struct lex_process* lex_process;
static struct token tmp_token;

// pegar um caracter do arquivo, sem mudar a posição de leitura.
static char peekc() {
	return lex_process->function->peek_char(lex_process);
}

static char nextc() {
	char c = lex_process->function->next_char(lex_process);
	lex_process->pos.col += 1;
	if (c == '\n') {
		lex_process->pos.line += 1;
		lex_process->pos.col = 1;
	}
	return c;
}

// Adicionar um caracter no arquivo.
static void pushc(char c) {
	lex_process->function->push_char(lex_process, c);
}

bool is_keyword(const char* str){
	return S_EQ(str, "if") ||
		S_EQ(str, "else") ||
		S_EQ(str, "while") ||
		S_EQ(str, "for") ||
		S_EQ(str, "do") ||
		S_EQ(str, "return") ||
		S_EQ(str, "break") ||
		S_EQ(str, "continue") ||
		S_EQ(str, "switch") ||
		S_EQ(str, "case") ||
		S_EQ(str, "default") ||
		S_EQ(str, "goto") ||
		S_EQ(str, "sizeof") ||
		S_EQ(str, "typedef") ||
		S_EQ(str, "struct") ||
		S_EQ(str, "union") ||
		S_EQ(str, "enum") ||
		S_EQ(str, "const") ||
		S_EQ(str, "volatile") ||
		S_EQ(str, "unsigned") ||
		S_EQ(str, "signed") ||
		S_EQ(str, "char") ||
		S_EQ(str, "int") ||
		S_EQ(str, "float") ||
		S_EQ(str, "double") ||
		S_EQ(str, "void") ||
		S_EQ(str, "static") ||
		S_EQ(str, "extern") ||
		S_EQ(str, "long") ||
		S_EQ(str, "short") ||
		S_EQ(str, "__ignore_typecheck") ||
		S_EQ(str, "extern") ||
		S_EQ(str, "restrict");
}

static struct pos lex_file_position() {
	return lex_process->pos;
}

struct token* token_create(struct token* _token) {
	memcpy(&tmp_token, _token, sizeof(struct token));
	tmp_token.pos = lex_file_position();
	return &tmp_token;
}

static void lex_new_expression() {
	lex_process->current_expression_count ++;
	if (lex_process->current_expression_count == 1) {
		lex_process->parentheses_buffer = buffer_create();
	}
}

static void lex_finish_expression() {
	lex_process->current_expression_count --;

	// if (lex_process->current_expression_count < 0) {
	// 	compiler_error(lex_process->compiler, "Erro: parênteses não balanceados\n");
	// }
}

bool lex_is_in_expression() {
	return lex_process->current_expression_count > 0;
}

static struct token* lexer_last_token() {
	return vector_back_or_null(lex_process->token_vec);
}

static struct token* handler_whitespace() {
	struct token* last_token = lexer_last_token();
	if (last_token){
		last_token->whitespace = true;
	}
	nextc();
	return read_next_token();
	}

// Funcoes responsavel por ler um numero do arquivo e converte se for HEX ou BIN
const char* read_number_str() {
    struct buffer* buffer = buffer_create();
    char c = peekc();

    // Se começar com 0, pode ser hexadecimal ou binário
    if (c == '0') {
        buffer_write(buffer, nextc()); // consome '0'
        c = peekc();

        // Hexadecimal
        if (c == 'x' || c == 'X') {
            buffer_write(buffer, nextc()); // consome 'x' ou 'X'
            c = peekc();

            while ((c >= '0' && c <= '9') ||
                   (c >= 'a' && c <= 'f') ||
                   (c >= 'A' && c <= 'F')) {
                buffer_write(buffer, nextc());
                c = peekc();
            }

            buffer_write(buffer, 0x00);
            int value = (int)strtol(buffer->data, NULL, 16);

            static char result[32];
            snprintf(result, sizeof(result), "%d", value);
            printf("Token NU: %s (hex)\n", result);
            return result;
        }

        // Binário
        else if (c == 'b' || c == 'B') {
            buffer_write(buffer, nextc()); // consome 'b' ou 'B'
            c = peekc();

            while (c == '0' || c == '1') {
                buffer_write(buffer, nextc());
                c = peekc();
            }

            buffer_write(buffer, 0x00);
            int value = (int)strtol(buffer->data + 2, NULL, 2); // pula o "0b"

            static char result[32];
            snprintf(result, sizeof(result), "%d", value);
            printf("Token NU: %s (bin)\n", result);
            return result;
        }

        // Zero isolado ou seguido de número decimal
        // continua para leitura decimal abaixo
    }

    // Decimal padrão
    while (c >= '0' && c <= '9') {
        buffer_write(buffer, nextc());
        c = peekc();
    }

    buffer_write(buffer, 0x00);
    printf("Token NU: %s (dec)\n", buffer->data);
    return buffer_ptr(buffer);
}


unsigned long long read_number() {
	const char* s = read_number_str();
	return atoll(s);
}

struct token* token_make_number_for_value(unsigned long number){
	return token_create(&(struct token) {
		.type = TOKEN_TYPE_NUMBER,
		.llnum = number
	});
}

struct token* token_make_number() {
	return token_make_number_for_value(read_number());
}


// char *read_token_value()
// {
//     struct buffer *buf = buffer_create();
//     char c = peekc();

// //     // 1. Identificador (começa com letra ou _)
// //     if (isalpha(c) || c == '_') {
// //         while (isalnum(peekc()) || peekc() == '_') {
// //             buffer_write(buf, nextc());
// //         }
// //     }
// //     // 2. Número
// //     else if (isdigit(c)) {
// //         while (isdigit(peekc())) {
// //             buffer_write(buf, nextc());
// //         }
// //     }
// //     // 3. Operador
// //     else {
// //         c = nextc();  // Consome
// //         switch (c)
// //         {
// //         OPERATOR_CASE:
// //             buffer_write(buf, c);
// //             break;
// //         default:
// //             buffer_free(buf);
// //             return NULL;
// //         }

// //         char next = peekc();
// //         if ((c == '=' && next == '=') ||
// //             (c == '+' && next == '=') ||
// //             (c == '-' && next == '=') ||
// //             (c == '*' && next == '=') ||
// //             (c == '/' && next == '=') ||
// //             (c == '+' && next == '+') ||
// //             (c == '-' && next == '-') ||
// //             (c == '!' && next == '=') ||
// //             (c == '<' && next == '=') ||
// //             (c == '>' && next == '=') ||
// //             (c == '&' && next == '&') ||
// //             (c == '|' && next == '|')) {
// //             buffer_write(buf, nextc());
// //         }
// //     }

// //     buffer_write(buf, '\0');
// //     return buf->data;
// // }


char *read_token_value(enum token_type *type)
{
    struct buffer *buf = buffer_create();
    char c = peekc();

    if (isalpha(c) || c == '_') {
        *type = TOKEN_TYPE_STRING; // ou TOKEN_TYPE_IDENTIFIER se quiser diferenciar
        while (isalnum(peekc()) || peekc() == '_') {
            buffer_write(buf, nextc());
        }
    }
    else if (isdigit(c)) {
        *type = TOKEN_TYPE_NUMBER;
        while (isdigit(peekc())) {
            buffer_write(buf, nextc());
        }
    }
    else {
        c = nextc();
        switch (c)
        {
        OPERATOR_CASE:
            *type = TOKEN_TYPE_OPERATOR;
            buffer_write(buf, c);
            break;
        default:
            buffer_free(buf);
            return NULL;
        }

        char next = peekc();
        if ((c == '=' && next == '=') ||
            (c == '+' && next == '=') ||
            (c == '-' && next == '=') ||
            (c == '*' && next == '=') ||
            (c == '/' && next == '=') ||
            (c == '+' && next == '+') ||
            (c == '-' && next == '-') ||
            (c == '!' && next == '=') ||
            (c == '<' && next == '=') ||
            (c == '>' && next == '=') ||
            (c == '&' && next == '&') ||
            (c == '|' && next == '|')) {
            buffer_write(buf, nextc());
        }
    }

    buffer_write(buf, '\0');
    return buf->data;
}

const char* read_symbol_str() {
	struct buffer* buffer = buffer_create();
	char c = peekc();
	// Aqui lemos um único símbolo
	if (c == '{' || c == '}' || c == ':' || c == ';' ||
		c == '#' || c == ')' || c == ']') {
		buffer_write(buffer, c);
		nextc(); // consome o caractere
	}
	buffer_write(buffer, '\0');
	printf("Token SY: %s\n", buffer_ptr(buffer));
	return buffer_ptr(buffer);
}

const char* read_newline_str() {
	struct buffer* buffer = buffer_create();
	char c = peekc();
	if (c == '\n') {
		buffer_write(buffer, c);
		nextc(); // consome a quebra de linha
	}
	buffer_write(buffer, '\0');
	printf("Token NL %s", buffer_ptr(buffer));
	return buffer_ptr(buffer);
}

struct token* token_make_symbol_for_value(const char* symbol_str) {
	return token_create(&(struct token) {
		.type = TOKEN_TYPE_SYMBOL,
		.sval = symbol_str
	});
}

struct token* token_make_newline_for_value(const char* newline_str) {
	return token_create(&(struct token) {
		.type = TOKEN_TYPE_NEWLINE,
		.sval = newline_str
	});
}

// Funcao responsavel por ler um operador do arquivo
struct token* token_make_operator_for_value(const char* op_str) {
	return token_create(&(struct token) {
		.type = TOKEN_TYPE_OPERATOR,
		.sval = op_str
	});
}

struct token *token_make_string(char inicio, char fim)
{
	struct buffer *buf = buffer_create();
    assert(nextc() == inicio);
    char c = nextc();

    for (; c != fim && c != EOF; c = nextc())
    {
        if (c == '\\')
        {
            continue;
        }
        buffer_write(buf, c);
    }

    buffer_write(buf, 0x00);
    printf("Token ST: %s\n", buf->data);
    return token_create(&(struct token){.type = TOKEN_TYPE_STRING, .sval = buffer_ptr(buf)});
}

struct token* token_make_newline() {
	const char* newline_str = read_newline_str();
	return token_make_newline_for_value(newline_str);
}

struct token* token_make_symbol() {
	const char* symbol_str = read_symbol_str();
	return token_make_symbol_for_value(symbol_str);
}

// Funcao responsavel por ler um operador ou string do arquivo
// static struct token *token_make_operator_or_string()
// {
//     char op = peekc();

//     // Casos especiais como #include <...>
//     if (op == '<'){
//         struct token *last_token = lexer_last_token();
//         if (token_is_keyword(last_token, "include"))
//         {
//             return token_make_string('<', '>');
//         }
//     }

//     // Usa nova função genérica
//     char *val = read_token_value();
//     if (!val) return NULL;

//     struct token *token = token_create(
//         &(struct token){.type = TOKEN_TYPE_STRING, .sval = val});

//     printf("Token ST: %s\n", token->sval);

//     if (op == '('){
//         lex_new_expression();
//     }

//     return token;
// }

static struct token *token_make_operator_or_string()
{
    char op = peekc();

    // Casos especiais como #include <...>
    if (op == '<') {
        struct token *last_token = lexer_last_token();
        if (token_is_keyword(last_token, "include")) {
            return token_make_string('<', '>');
        }
    }

    enum token_type type;
    char *val = read_token_value(&type);  // Agora recebe o tipo também
    if (!val) return NULL;

    struct token *token = token_create(
        &(struct token){.type = type, .sval = val});

    if (type == TOKEN_TYPE_OPERATOR)
        printf("Token OP: %s\n", token->sval);
    else if (type == TOKEN_TYPE_STRING)
        printf("Token ST: %s\n", token->sval);

    if (op == '(') {
        lex_new_expression();
    }

    return token;
}

static struct token* make_identifier_or_keyword() {
	struct buffer* buffer = buffer_create();
	char c;
	LEX_GETC_IF(buffer, c, (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')

	buffer_write(buffer, 0X00);
	printf("Token: %s\n", buffer->data);

	if (is_keyword(buffer_ptr(buffer))) {
		return token_create(&(struct token) {
			.type = TOKEN_TYPE_KEYWORD,
			.sval = buffer_ptr(buffer)
		});
	}
	return token_create(&(struct token) {
		.type = TOKEN_TYPE_IDENTIFIER,
		.sval = buffer_ptr(buffer)
	});
}

struct token *single_line_comment()
{

    struct buffer *buf = buffer_create();
    char c = 0;

    LEX_GETC_IF(buf, c, c != '\n' && c != EOF);
    buffer_write(buf, 0x00);

    printf("Token CO: %s\n", buf->data);

    return token_create(&(struct token){.type = TOKEN_TYPE_COMMENT, .sval = buffer_ptr(buf)});
}

struct token *multiline_comment(char estrela)
{
    struct buffer *buf = buffer_create();
    if (!buf)
        return NULL; // checa se buffer foi criado

    char c = nextc();

    while (c != EOF)
    {
        if (c == estrela)
        {
            char next = peekc();
            if (next == '/')
            {
                nextc();
                buffer_write(buf, 0x00);
                printf("Token: %s\n", buf->data);
                return token_create(&(struct token){.type = TOKEN_TYPE_STRING, .sval = buffer_ptr(buf)});
            }
        }

        buffer_write(buf, c);
        c = nextc();
    }

    return NULL;
}

struct token *token_make_comment()
{

    char c = peekc();

    if (c == '/')
    {
        nextc();
        if (peekc() == '/')
        {
            nextc();
            return single_line_comment();
        }
        else if (peekc() == '*')
        {
            nextc();
            return multiline_comment('*');
        }

        pushc('/');
        return token_make_operator_or_string();
    }
}


struct token *read_special_token()
{
    char c = peekc();
    if (isalpha(c) || c == '_')
    {
        return make_identifier_or_keyword();
    }

    return NULL;
}


// Funcao responsavel por ler o proximo token do arquivo
// struct token* read_next_token(){
// 	struct token* token = NULL;
// 	char c = peekc();
	
// 	switch (c){
// 	case EOF:
// 	// FIM do arquivo.
// 	break;

//     case '"':
// 	token = token_make_string('"', '"');
// 	break;

// 	NUMERIC_CASE:
// 	token = token_make_number();
// 	break;

// 	OPERATOR_CASE:
// 	token = token_make_operator_or_string();
// 	break;
	
// 	SYMBOL_CASE:
// 	token = token_make_symbol();
// 	break;

// 	case ' ':
// 	token = handler_whitespace();
// 	break;

// 	case '\t':
// 		token = handler_whitespace();
// 	break;

// 	default:
// 		// token = read_special_token();
// 		// if (!token) {
// 		// 	compiler_error(lex_process->compiler, "Erro: token inválido '%c'", c);
// 		// }
// 		break;

// 	}
// 	return token;
// }

struct token* read_next_token(){
	struct token* token = NULL;
	char c = peekc();
	
	switch (c){
	case EOF:
		// FIM do arquivo
		break;

	case '\n':
		token = token_make_newline();
		break;

	case '/':
		token = token_make_comment();
		break;

	case '"':
		token = token_make_string('"', '"');
		break;

	NUMERIC_CASE:
		token = token_make_number();
		break;

	OPERATOR_CASE:
		token = token_make_operator_or_string();
		break;

	SYMBOL_CASE:
		token = token_make_symbol();
		break;

	case ' ':
	token = handler_whitespace();
	break;
		
	case '\t':
		token = handler_whitespace();
		break;

	default:
		// Aqui tratamos palavras-chave, identificadores e também
		// qualquer operador que não caiu nos casos anteriores
		token = token_make_operator_or_string();

		// Se read falhar, pode ser um caractere inválido
		if (!token) {
			// compiler_error(lex_process->compiler, "Erro: token inválido '%c'", c);
		}
		break;
	}

	return token;
}

int lex(struct lex_process* process) {
	process->current_expression_count = 0;
	process->parentheses_buffer = NULL;
	lex_process = process;
	process->pos.filename = process->compiler->cfile.abs_path;

	struct token* token = read_next_token();
	while (token) {
		vector_push(lex_process->token_vec, token);
		token = read_next_token();
	}

	return LEXICAL_ANALYSIS_ALL_OK;
}


