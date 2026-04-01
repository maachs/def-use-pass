#include <stdio.h>
#include <stdlib.h>
#include "Common.hpp"

void dump_end() {
    FILE *file = fopen(GRAPH_FILENAME, "a");
    if (file) {
        fprintf(file, "}\n");
        fclose(file);
    }
}

void dump_val(long long val, unsigned long long id) {
    static int is_registered = 0;
    if (!is_registered) {
        atexit(dump_end);
        is_registered = 1;
    }

    FILE *file = fopen(GRAPH_FILENAME, "a");
    if (file) {
        fprintf(file, "  \"node_0x%llx\" [xlabel=<<font color=\"red\"><b>VAL: %lld</b></font>>, color=red, penwidth=2];\n", id, val);
        fclose(file);
    }
}

