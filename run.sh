TEST_NAME="test"
TEST_DIR="tests"
SRC_DIR="src"
BUILD_DIR="build"
INC_DIR="include"
PLUGIN="./build/DefUseGraph.so"

rm -f ${BUILD_DIR}/defuse_graph.dot result.png

clang-14 -S -emit-llvm ${TEST_DIR}/${TEST_NAME}.c -o ${TEST_DIR}/${TEST_NAME}.ll || exit 1

opt-14 -load-pass-plugin=${PLUGIN} -passes='defuse-graph' ${TEST_DIR}/${TEST_NAME}.ll -S -o ${BUILD_DIR}/test_instr.ll || exit 1

clang-14 ${BUILD_DIR}/test_instr.ll ${SRC_DIR}/runtime.c -I${INC_DIR} -o ${BUILD_DIR}/run_time_dump || exit 1

./build/run_time_dump

if [ -f "${BUILD_DIR}/defuse_graph.dot" ]; then
    dot -Tpng ${BUILD_DIR}/defuse_graph.dot -o result.png
    echo "Graph was built"
else
    echo "Error: .dot file was not found"
fi
