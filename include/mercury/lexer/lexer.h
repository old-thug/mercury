#ifndef LEXER_H
#define LEXER_H

#include "mercury/module.h"
#include "mercury/lexer/span.h"
#include "mercury/lexer/token.h"

typedef struct {
    ModuleId id;
    const array(char) source;

    usize offset;
    usize endoffset;
    usize line;
    usize col, pcol;
} Lexer;

struct lex_error_t {
    const char *message;
    Span        span;
};

Lexer lexer_init(ModuleId id, const array(char) source);

/// Trys to lex the next token from the source
/// returns true if successful and [out] is set to the lex'ed token
/// otherwise returns false on failure and [err_out] is populated
bool  lexer_next(Lexer *lexer, Token *out, struct lex_error_t *err_out);

bool lexer_is_done(Lexer *lexer);

#endif /* LEXER_H */
