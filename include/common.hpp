#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

static const char* DEFAULT_GRAPH_FILENAME = "./build/defuse_graph.dot";

const char* get_defuse_path();

#ifdef __cplusplus
}
#endif

#endif
