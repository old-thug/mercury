#include "mercury/lexer/lexer.h"

#include <stdio.h>
#include <assert.h>

#include "./test.h"

void lex_identifier() {
    array(char) source = create_source("main");
    Lexer lexer = lexer_init(0, source);
    Token tok;
    
    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_Id && "lexer::lex_identifier: failed");
}

void lex_function() {
    array(char) source = create_source(
				       "proc main() {\n"
				       "}\n");
    Lexer lexer = lexer_init(0, source);
    Token tok;

    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_Proc);
    
    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_Id);
    
    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_Lparen);
    
    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_Rparen);
    
    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_Lbrace);
}

void lex_numerics() {
    array(char) source = create_source("0xFFF 0d1234 0o777 0b010011");

    Lexer lexer = lexer_init(0, source);
    Token tok;

    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_IntLiteral);

    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_IntLiteral);

    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_IntLiteral);

    assert(lexer_next(&lexer, &tok, nullptr));
    assert(tok.kind == TOK_IntLiteral);
}

int main() {
    lex_numerics();
    lex_identifier();
    lex_function();
}
