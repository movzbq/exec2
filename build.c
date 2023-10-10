#include "build.h"
#define EMPTY_ERROR             0
#define TOKEN_BOUNDS_ERROR      1
#define SUBEX_BOUDNS_ERROR      2

static Table g_table    = {0};
static Expr* g_cexpr    = NULL;

static void set_cell_to_error (Cell*, uint8_t);
static void init_expreission (Expr*, Expr*);

void build_init_table (uint16_t rows, uint16_t cells) {
    g_table.cells = (Cell*) calloc(cells, sizeof(Cell));
    CHECK_4_MEM(&g_table.cells);

    g_table.row_marks = (uint16_t*) calloc(rows, sizeof(uint16_t));
    CHECK_4_MEM(&g_table.row_marks);

    /*
     * A row mark is the position where new row starts, as we
     * know the first cell is at (0, 0) where row = 0, calloc
     * starts all the elements to zero, so we can skip the first
     * position.
     * */
    g_table.rowmks_i = 1;
    g_table.clls_i   = 0;
    g_table.new_row  = true;
}

void build_make_cell () {
    Cell* cc = &g_table.cells[g_table.clls_i++];

    init_expreission(&cc->expression, NULL);
    set_cell_to_error(cc, EMPTY_ERROR);
    cc->content  = kind_unknown;
    cc->made_row = g_table.new_row;
}

void build_mark_row () {
    g_table.row_marks[g_table.rowmks_i++] = g_table.clls_i;
    g_table.new_row = true;
}

void build_make_token (const char* data, size_t len, const TokenK kind) {
    if (!g_table.clls_i) {
        EXEC2_WARNG("trying to save '%.*s' (%d) token but not cell made previously.\n", (int) len, data, kind);
        return;
    }

    uint16_t cell_counter = g_table.clls_i - 1;
    Cell* cc = &g_table.cells[cell_counter];

    if (IS_IT_LITERAL(cc->content)) {
        EXEC2_WARNG("the %d cell was already set to a(n) constant/error.\n", cell_counter);
        return;
    }

    /*
     * Cell are set to a constant value if the first token defined
     * on them is one constant value, if so the cell will no longer
     * accept more tokens to be saved.
     * */
    if (!cc->expression.tkns_i && IS_IT_LITERAL(kind)) {
        snprintf(cc->cell_msg, len + 1, "%.*s", (int) len, data);
        cc->content = kind;

        return;
    }

    if (g_cexpr->tkns_i == EXEC2_MAX_TOKENS_CAP) {
        EXEC2_WARNG("the %d cell has reached its limit (tokens).\n", cell_counter);
        set_cell_to_error(cc, TOKEN_BOUNDS_ERROR);
        return;
    }

    if (kind == kind_rbr) {
        if (!g_cexpr->parent) {
            EXEC2_WARNG("trying to go back but there is not route to go through (%d cell).\n", cell_counter);
            return;
        }
    }

    Token token = { .token = {0}, .kind  = kind };
    memcpy(token.token, data, len);
    g_cexpr->tokens[g_cexpr->tkns_i++] = token;

    if (kind == kind_lbr) {
        if (g_cexpr->subex_i == EXEC2_MAX_SUBEXP_CAP) {
            EXEC2_WARNG("the %d cell has reached its limit (subexprs).\n", cell_counter);
            set_cell_to_error(cc, SUBEX_BOUDNS_ERROR);
            return;
        }

        init_expreission(&g_cexpr->subexp[g_cexpr->subex_i++], g_cexpr);
    }
}

void build_build () {
    puts("-/-");
    for (uint16_t i = 0; i < g_table.clls_i; i++) {
        Cell* cc = &g_table.cells[i];

        printf("%s \t %p \t %d", cc->cell_msg, cc, cc->content);
        if (cc->made_row)
            putchar(10);
    }
}

static void set_cell_to_error (Cell* cc, uint8_t err) {
    char* const errors[] = {
        "[!EMPTY]",
        "[!TOKEN_BOUDNS]",
        "[!SUBEX_BOUNDS]"
    };

    const char* such = errors[err];
    const size_t len = strlen(such);

    snprintf(cc->cell_msg, len + 1, "%s", such);
    cc->content = kind_error;
}

static void init_expreission (Expr* self, Expr* parent) {
    self->subexp = (Expr*) calloc(EXEC2_MAX_SUBEXP_CAP, sizeof(Expr));
    CHECK_4_MEM(self->subexp);

    self->parent  = parent;
    self->tkns_i  = 0;
    self->subex_i = 0;

    g_cexpr = self;
}



