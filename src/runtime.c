#include "Common.hpp"
#include <stdio.h>
#include <stdlib.h>

void dump_end() {
  FILE *file = fopen(get_defuse_path(), "a");
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

  FILE *file = fopen(get_defuse_path(), "a");
  if (file) {
    fprintf(file,
            "  \"node_0x%llx\" [xlabel=<<font color=\"red\"><b>VAL: "
            "%lld</b></font>>, color=red, penwidth=2];\n",
            id, val);
    fclose(file);
  }
}
