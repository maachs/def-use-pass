#include <stdio.h>

// TODO[flops]: The same logic can be applied to the end of dot file. 
// Now you're doing something like: `echo "}" >> defuse_graph.dot`, but you can automate it using pass too!
// You just need to make runtime function like:
// ~~~
// void dump_end() { <Dumps `}` to the file> }
// ~~~
// And insert it at the end of main function

void dump_val(int val, unsigned long long id) {
    FILE *file = fopen("defuse_graph.dot", "a");
    if (file) {
        fprintf(file, "  \"node_0x%llx\" [xlabel=<<font color=\"red\"><b>VAL: %d</b></font>>, color=red, penwidth=2];\n", id, val);
        fclose(file);
    }
}
