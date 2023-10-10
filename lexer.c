#include "lexer.h"

static TokenK find_type_out (const char, const char);
static size_t get_ths_const (const char*, size_t*, const TokenK);
static void get_context_of (const char*, size_t);

void lexer_lexer (char* content, size_t len, uint16_t rows, uint16_t cells) {
    build_init_table(rows, cells);

    for (size_t i = 0; i < len; i++) {
        const char a = content[i];

        if (a == '|') { build_make_cell(); continue; }
        if (a == '\n') { build_mark_row(); continue; }
        if (isspace(a)) continue;

        TokenK kind = find_type_out(a, content[i + 1]);
        if (kind == kind_unknown) {
            get_context_of(content, i);
            continue;
        }

        if (IS_IT_CONST(kind)) {
            size_t pi = i, len = get_ths_const(content, &i, kind);
            build_make_token(content + pi, len, kind);
        }
        else {
            if (kind >= kind_grt)
                i++;
            build_make_token(NULL, 0, kind);
        }
    }

    free(content);
    build_build();
}

static TokenK find_type_out (const char a, const char b) {
    switch (a) {
        case '`': return kind_strg;
        case '&': return kind_addr;

        case '+': return kind_add;
        case '*': return kind_mul;
        case '/': return kind_div;
        case '%': return kind_mod;
        case '^': return kind_pow;

        case '(': return kind_lpa;
        case ')': return kind_rpa;
        case '{': return kind_lbr;
        case '}': return kind_rbr;
        case '$': return kind_mth;
        case '?': return kind_cnd;

        case '>': return (b == '=') ? kind_gte : kind_grt;
        case '<': return (b == '=') ? kind_lse : kind_lss;
        case '=': return (b == '!') ? kind_neq : kind_equ;
    }

    if (a == '-')
        return isdigit(b) ? kind_real : kind_sub;
    return isdigit(a) ? kind_real : kind_unknown;
}

static bool get_ths_real (const char a) { return isdigit(a); }
static bool get_ths_strg (const char a) { return a != '`'; }
static bool get_ths_addr (const char a) { return isdigit(a) || islower(a); }

static size_t get_ths_const (const char* content, size_t* _pos, const TokenK kind) {
    typedef bool (*fx) (const char);
    fx y = get_ths_addr;

    /**/ if (kind == kind_strg) y = get_ths_strg;
    else if (kind == kind_real) y = get_ths_real;

    size_t len = 0;
    do {
        len++;
        *_pos += 1;
    } while (y(content[*_pos]));

    if (kind != kind_strg) *_pos -= 1;
    else len++;

    return len;
}

static void get_context_of (const char* content, size_t _pos) {
    size_t position = _pos;
    uint8_t context = 0;

    do {
        context++;
        _pos++;
    } while (content[_pos] >= 32 && context <= 5);

    EXEC2_WARNG("unknown token type: '...%.*s...' (%ld byte).\n", context, content + position, position);
}
