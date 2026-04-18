#include "Common.hpp"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static pthread_mutex_t dump_mutex = PTHREAD_MUTEX_INITIALIZER;

void dump_end() {
  pthread_mutex_lock(&dump_mutex);
  FILE *file = fopen(get_defuse_path(), "a");
  if (file) {
    fprintf(file, "}\n");
    fclose(file);
  }
  pthread_mutex_unlock(&dump_mutex);
}

void dump_val(long long val, unsigned long long id) {
  static int is_registered = 0;
  pthread_mutex_lock(&dump_mutex);
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
  pthread_mutex_unlock(&dump_mutex);
}
