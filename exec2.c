#include "lexer.h"

static char* get_info_abt_file (FILE*, size_t*, uint16_t*, uint16_t*);

int main (int argc, char** argv) {
    if (argc != 2)
        EXEC2_ABORT("invalid execution arguments");

    FILE* file = fopen(argv[1], "r");
    if (!file)
        EXEC2_ABORT("invalid file to be processed");

    uint16_t rows = 0, cells = 0;
    size_t size = 0;

    char* content = get_info_abt_file(file, &size, &rows, &cells);
    lexer_lexer(content, size, rows, cells);
    return 0;
}

static char* get_info_abt_file (FILE* file, size_t* bytes, uint16_t* rows, uint16_t* cells) {
    fseek(file, 0, SEEK_END);
    *bytes = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (!*bytes)
        EXEC2_ABORT("there is nothing to be done");

    char* content = (char*) calloc(*bytes, 1);
    CHECK_4_MEM(content);

    fread(content, *bytes, 1, file);
    content[*bytes] = 0;
    fclose(file);

    size_t a = 0;
    while (a < *bytes) {
        /**/ if (content[a] == '|') *cells += 1;
        else if (content[a] == 10) *rows += 1;
        a++;
    }

    return content;
}
