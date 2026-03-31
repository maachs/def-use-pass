#include <stdio.h>
#include "common.hpp"

void dump_val(int val, unsigned long long id) {
    FILE *file = fopen(GRAPH_FILENAME, "a");
    if (file) {
        fprintf(file, "  \"node_0x%llx\" [xlabel=<<font color=\"red\"><b>VAL: %d</b></font>>, color=red, penwidth=2];\n", id, val);
        fclose(file);
    }
}

void dump_end() {
    FILE *file = fopen(GRAPH_FILENAME, "a");
    if (file) {
        fprintf(file, "}\n");
        fclose(file);
    }
}
