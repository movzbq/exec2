#ifndef EXEC2_EXEC2_H
#define EXEC2_EXEC2_H
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define EXEC2_ABORT(s)          do { fprintf(stderr, "exec2: err: %s.\n", s); exit(1); } while (0)
#define EXEC2_WARNG(s, ...)     fprintf(stderr, "exec2: wrg: " s, ##__VA_ARGS__)
#define CHECK_4_MEM(p)          do { if (!p) { EXEC2_ABORT("no memory enough anymore :("); exit(1); } } while (0)
#define IS_IT_CONST(k)          (k >= 1 && k <= 3)
#define IS_IT_LITERAL(k)        (k == 1 || k == 2 || k == -1)

#define EXEC2_MAX_TOKEN_LEN     64
#define EXEC2_MAX_TOKENS_CAP    16
#define EXEC2_MAX_SUBEXP_CAP    8

typedef enum TokenKind {
    kind_error = -1,
    kind_unknown,
    kind_strg,
    kind_real,
    kind_addr,

    kind_add,
    kind_sub,
    kind_mul,
    kind_div,
    kind_mod,
    kind_pow,

    kind_lpa,
    kind_rpa,
    kind_lbr,
    kind_rbr,
    kind_mth,
    kind_cnd,

    kind_grt,
    kind_gte,
    kind_lss,
    kind_lse,
    kind_equ,
    kind_neq,
} TokenK;

typedef struct Token {
    char    token[EXEC2_MAX_TOKEN_LEN];
    TokenK  kind;
} Token;

typedef struct Expression {
    Token               tokens[EXEC2_MAX_TOKENS_CAP];
    struct Expression*  subexp, *parent;
    uint16_t            tkns_i, subex_i;
} Expr;

typedef struct Cell {
    Expr    expression;
    char    cell_msg[EXEC2_MAX_TOKEN_LEN];
    TokenK  content;
    bool    made_row;
} Cell;

typedef struct Table {
    Cell*      cells;
    uint16_t*  row_marks;
    uint16_t   clls_i, rowmks_i;
    bool       new_row;
} Table;

#endif

