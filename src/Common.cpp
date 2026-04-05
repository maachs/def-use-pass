#include "Common.hpp"
#include <stdlib.h>

extern "C" const char* get_defuse_path() {
    const char* env_path = getenv("DEFUSE_PATH");
    if (env_path) {
        return env_path;
    }
    return DEFAULT_GRAPH_FILENAME;
}
