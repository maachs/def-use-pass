# LLVM Def-Use Dynamic Analyzer



## Build and Run Instruction

```bash
mkdir build
cd build
ninja
```
Prepare tests/test.c file

```bash
rm -f defuse_graph.dot && \
clang-14 -S -emit-llvm ../tests/test.c -o ../tests/test.ll && \
opt-14 -load-pass-plugin=./DefUseGraph.so -passes='defuse-graph' ../tests/test.ll -S -o test_instr.ll && \
clang-14 test_instr.ll ../tests/runtime.c -o run_time_dump && \
./run_time_dump && \
echo "}" >> defuse_graph.dot && \
dot -Tpng defuse_graph.dot -o result.png
