#include "compiler.h"

bool token_is_keyword(struct token* token, const char* value) {
    return token->type == TOKEN_TYPE_KEYWORD && S_EQ(token->sval, value);
}

// LAB 4 starts here

bool token_is_symbol(struct token* token, char value) {
    return (token->type == TOKEN_TYPE_SYMBOL) && (token->cval == value);
}

bool discart_token(struct token* token) {
    return (token->type == TOKEN_TYPE_NEWLINE) ||
    (token->type == TOKEN_TYPE_COMMENT) ||
    token_is_symbol(token, '\\');
}

// LAB 4 ends here

// LAB 5 starts here

bool token_is_operator(struct token* token, const char* val) {
    return token && token->type == TOKEN_TYPE_OPERATOR && S_EQ(token->sval, val);
}

// LAB 5 ends here
