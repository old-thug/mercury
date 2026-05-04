#ifndef TOKEN_H
#define TOKEN_H

#include "mercury/lexer/span.h"

typedef enum {
#define TOKENX(Enum, Str) TOK_##Enum,
#    include "./token_kind.def"
#undef TOKENX
} TokenKind;

typedef struct {
    TokenKind kind;
    Span      span;
} Token;

const char *tokenkind_name(TokenKind kind);
Token token_init(TokenKind kind, Span span);

#endif /* TOKEN_H */
