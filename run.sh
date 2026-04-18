TEST_DIR="tests"
SRC_DIR="src"
BUILD_DIR="build"
INC_DIR="include"
PLUGIN="./build/DefUseGraph.so"
PATH_TO_DOT="./build/defuse_graph.dot"

export DEFUSE_PATH="$PATH_TO_DOT"

clang-format -i include/*.hpp src/*.cpp src/runtime.c

rm -f ${PATH_TO_DOT} result.png

clang-14 -S -emit-llvm ${TEST_DIR}/test1.c -o ${BUILD_DIR}/test1.ll || exit 1
clang-14 -S -emit-llvm ${TEST_DIR}/test2.c -o ${BUILD_DIR}/test2.ll || exit 1

llvm-link-14 ${BUILD_DIR}/test1.ll ${BUILD_DIR}/test2.ll -S -o ${BUILD_DIR}/merged.ll || exit 1

opt-14 -load-pass-plugin=${PLUGIN} -passes='defuse-graph' ${BUILD_DIR}/merged.ll -S -o ${BUILD_DIR}/instrumented.ll || exit 1

clang-14 ${BUILD_DIR}/instrumented.ll ${SRC_DIR}/runtime.c ${SRC_DIR}/Common.cpp \
  -DGRAPH_PATH="\"$PATH_TO_DOT\"" -I${INC_DIR} -pthread -o ${BUILD_DIR}/run_time_dump || exit 1

./build/run_time_dump

if [ -f ${PATH_TO_DOT} ]; then
    dot -Tpng "$PATH_TO_DOT" -o result.png
    echo "Graph was built successfully!"
else
    echo "Error: .dot file was not found"
fi
