#include "mercury/lexer/token.h"

const char *tokenkind_name(TokenKind kind) {
    return (const char *[]){
#define TOKENX(Enum, Str) [TOK_##Enum] = Str,
#    include "mercury/lexer/token_kind.def"
#undef  TOKENX
    } [kind];
}

Token token_init(TokenKind kind, Span span) {
    Token token;
    token.span = span;
    token.kind = kind;
    return token;
}
