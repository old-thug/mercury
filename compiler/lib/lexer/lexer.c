#include "mercury/lexer/lexer.h"

typedef u32 codepoint_t;

struct spec_t {
    const char *str;
    TokenKind   kind;
};

static const struct spec_t KEYWORDS[] = {
    { "proc",     TOK_Proc },
    { "return",   TOK_Return },
    { "match",    TOK_Match },
    { "if",       TOK_If },
    { "else",     TOK_Else },
    { "while",    TOK_While },
    { "for",      TOK_For },
    { "foreach",  TOK_Foreach },
    { "break",    TOK_Break },
    { "continue", TOK_Continue },
    { "import",   TOK_Import },
    { "export",   TOK_Export },
    { "module",   TOK_Module },
    { "extern",   TOK_Extern },
    { "auto",     TOK_Auto },
    { "Self",     TOK_Self },
    { "self",     TOK_Selfv },
    { "define",   TOK_Define },
    { "impl",     TOK_Impl },

    { "u8",       TOK_U8 },
    { "u16",      TOK_U16 },
    { "u32",      TOK_U32 },
    { "u64",      TOK_U64 },
    { "s8",       TOK_S8 },
    { "s16",      TOK_S16 },
    { "s32",      TOK_S32 },
    { "s64",      TOK_S64 },
    { "usize",    TOK_Usize },
    { "ssize",    TOK_Ssize },
    { "int",    TOK_Int },
    { "char",    TOK_Char },
    { "void",    TOK_Void },
    { "string",    TOK_String },
    { "zeroed",    TOK_Zeroed },
    { "null",    TOK_Null },
    { "none",    TOK_None },
};
static const int KEYWORD_COUNT = sizeof(KEYWORDS)/sizeof(struct spec_t);

static const struct spec_t PUNCTS[] = {
    { ">>=",      TOK_Shreq },
    { "<<=",      TOK_Shleq },
    { "<<=",      TOK_Shleq },
    { "::<",      TOK_TurboFi },
    { "...",      TOK_DotDotDot },
    
    { "+=",       TOK_AddEq },
    { "-=",       TOK_MinusEq },
    { "*=",       TOK_StarEq },
    { "/=",       TOK_SlashEq },
    { "!=",       TOK_BangEq },
    { "|=",       TOK_BitOrEq },
    { ".{",       TOK_DotBrace },
    { ".[",       TOK_DotBrack },
    { ".(",       TOK_DotParen },
    { "..",       TOK_DotDot },
    { "::",       TOK_Ns },
    { ">>",       TOK_Shr },
    { "<<",       TOK_Shl },
    { "||",       TOK_LogOr },
    { "&&",       TOK_LogAnd },
    
    { "<",       TOK_Lt },
    { ">",       TOK_Gt },
    { "+",       TOK_Add },
    { "-",       TOK_Minus },
    { "*",       TOK_Star },
    { "/",       TOK_Slash },
    { ".",       TOK_Dot },
    { ";",       TOK_Semicolon },
    { ":",       TOK_Colon },
    { "!",       TOK_Bang },
    { "|",       TOK_BitOr },
    { "&",       TOK_BitAnd },
    { "{",       TOK_Lbrace },
    { "[",       TOK_Lbrack },
    { "(",       TOK_Lparen },
    { "}",       TOK_Rbrace },
    { "]",       TOK_Rbrack },
    { ")",       TOK_Rparen },
};
static const int PUNCT_COUNT = sizeof(PUNCTS)/sizeof(struct spec_t);

static bool is_alpha(codepoint_t cp) {
    return
	(cp >= 'a' && cp <= 'z') ||
	(cp >= 'A' && cp <= 'Z') ||
	(cp == '_') ||
	(cp > 127);
}

static bool is_digit(codepoint_t cp, int base) {
    if (base == 2) {
	return (cp == '0' || cp == '1');
    }

    if (base == 8) {
	return (cp >= '0' && cp <= '7');
    }

    if (base == 10) {
	return (cp >= '0' && cp <= '9');
    }

    if (base == 16) {
	return
	    (cp >= '0' && cp <= '9') ||
	    (cp >= 'a' && cp <= 'f') ||
	    (cp >= 'A' && cp <= 'F');
    }

    return false;
}

int lexer_decode_utf8_at_point(Lexer *l, codepoint_t *cp) {
    const char   *s = &l->source[l->offset];
    unsigned char c = (unsigned char)*s;
    if (c <= 0x7F) {
        *cp = c;
        return 1;
    } else if ((c & 0xE0) == 0xC0) {
        *cp = ((c & 0x1F) << 6) | (s[1] & 0x3F);
        return 2;
    } else if ((c & 0xF0) == 0xE0) {
        *cp = ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        return 3;
    } else if ((c & 0xF8) == 0xF0) {
        *cp = ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
              ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        return 4;
    }
    return -1; // Invalid Utf8
}

static void lexer_token(Lexer *l, Token *out, TokenKind kind) {
    out->kind = kind;
    out->span = span_init(l->id, l->line, l->pcol, l->col);
}

Lexer lexer_init(ModuleId id, const array(char) source) {
    Lexer lexer = {0};
    lexer.source = source;
    lexer.endoffset = arr_len(source);
    lexer.line = 1;
    lexer.col = 1;
    lexer.pcol = 1;
    return lexer;
}

codepoint_t lexer_current(Lexer *l)
{
    if (lexer_is_done(l)) {
	return EOF;
    }

    codepoint_t cp;
    int len = lexer_decode_utf8_at_point(l, &cp);
    if (len <= 0) {
	return EOF;
    }
    return cp;
}

codepoint_t lexer_eat(Lexer *l)
{
    auto ch = lexer_current(l);
    if (ch == '\n') {
	l->line += 1;
	l->col   = 1;
    } else
	l->col += 1;

    l->offset += 1;
    return ch;
}

bool lexer_prefix(Lexer *l, const char *prefix)
{
    usize len = min(strlen(prefix), l->endoffset - l->offset);
    return !strncmp(&l->source[l->offset], prefix, len);
}

char lexer_take(Lexer *l, int count, ...)
{
    va_list args;
    va_start(args, count);
    
    for (usize n = 0; n < count; ++n) {
	char ch = va_arg(args, int);
	if (lexer_current(l) == ch) {
	    lexer_eat(l);
	    return ch;
	}
    }
    return EOF;
}

void lexer_eat_prefix(Lexer *l, const char *prefix) {
    if (!lexer_prefix(l, prefix)) {
	return;
    }

    for(int n = 0; n < strlen(prefix); ++n) {
	lexer_eat(l);
    }
}

Span lexer_here(Lexer *l) {
    return span_init(l->id, l->line, l->pcol, l->col);
}

bool lexer_next(Lexer *l, Token *out, struct lex_error_t *err_out) {
    if (lexer_is_done(l)) {
	lexer_token(l, out, TOK_EndofFile);
    }

    for(;;) {

	if (lexer_current(l) == ' ' ||
	    lexer_current(l) == '\n' ||
	    lexer_current(l) == '\r') {
	    lexer_eat(l);
	    continue;
	}

	if (lexer_prefix(l, "//")) {
	    lexer_eat_prefix(l, "//");
	    while (!lexer_is_done(l) &&
		   lexer_current(l) != '\n') {
		lexer_eat(l);
	    }
	    lexer_eat(l);
	    continue;
	}
	
	break;
    }
    
    if (lexer_is_done(l)) {
	lexer_token(l, out, TOK_EndofFile);
    }

    l->pcol = l->col;

    if (is_alpha(lexer_current(l))) {
	auto begin = l->offset;
	while (!lexer_is_done(l) && (is_alpha(lexer_current(l)) || is_digit(lexer_current(l), 10))) {
	    lexer_eat(l);
	}
	auto end = l->offset;
	StringView slice = sv_init(&l->source[begin], end - begin);
	for(usize n = 0; n < KEYWORD_COUNT; ++n) {
	    struct spec_t spec = KEYWORDS[n];
	    if (sv_equals_str(slice, spec.str)) {
		lexer_token(l, out, spec.kind);
		return true;
	    }
	}
	lexer_token(l, out, TOK_Id);
	return true;
    }

    if (is_digit(lexer_current(l), 10)) {
	int base = 10;
	if (lexer_current(l) == '0') {
	    lexer_eat(l);
	    codepoint_t ch = lexer_take(l, 8, 'x', 'X', 'b', 'B', 'd', 'D', 'o', 'O');
	    if (ch != EOF) {
		switch (ch) {
		case 'x':
		case 'X':
		    base = 16;
		    break;
		case 'b':
		case 'B':
		    base = 2;
		    break;
		case 'd':
		case 'D':
		    base = 10;
		    break;
		case 'o':
		case 'O':
		    base = 8;
		    break;
		default:
		    todo();
		}
	    }
	}

	if (lexer_is_done(l)) {
	    if (err_out) {
		err_out->message = "expected numeric literal after numeric prefix";
		err_out->span    = lexer_here(l);
	    }
	    return false;
	}

	while (!lexer_is_done(l) && is_digit(lexer_current(l), base)) {
	    lexer_eat(l);
	}

	lexer_token(l, out, TOK_IntLiteral);
	return true;
    }
    
    for(usize n = 0; n < PUNCT_COUNT; ++n) {
	struct spec_t spec = PUNCTS[n];
	if (lexer_prefix(l, spec.str)) {
	    lexer_eat_prefix(l, spec.str);
	    lexer_token(l, out, spec.kind);
	    return true;
	}
    }

    todo();
    return false;
}

bool lexer_is_done(Lexer *lexer)
{
    return lexer->offset >= lexer->endoffset;
}
