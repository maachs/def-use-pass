#include <stdio.h>

void dump_val(int val, unsigned long long id) {
    FILE *file = fopen("defuse_graph.dot", "a");
    if (file) {
        fprintf(file, "  \"node_0x%llx\" [xlabel=<<font color=\"red\"><b>VAL: %d</b></font>>, color=red, penwidth=2];\n", id, val);
        fclose(file);
    }
}
