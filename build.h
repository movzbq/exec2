#ifndef EXEC2_BUILD_H
#define EXEC2_BUILD_H
#include "exec2.h"

void build_init_table (uint16_t, uint16_t);
void build_make_cell ();
void build_mark_row ();
void build_make_token (const char*, size_t, const TokenK);
void build_build ();

#endif
