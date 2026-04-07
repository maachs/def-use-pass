#include <stdlib.h>
#include "Common.hpp"

extern "C" const char* get_defuse_path() {
    static const char* DEFAULT_GRAPH_FILENAME = "./build/defuse_graph.dot";
    const char* env_path = getenv("DEFUSE_PATH");
    if (env_path) {
        return env_path;
    }
    return DEFAULT_GRAPH_FILENAME;
}
