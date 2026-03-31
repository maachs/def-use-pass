# LLVM Def-Use Dynamic Analyzer



## Build and Run Instruction

# TODO[niten]: This seems to be not entirely true :)
```bash
mkdir build
cd build
ninja
```
Prepare tests/test.c file

# TODO[flops]: Make script for this
```bash
rm -f defuse_graph.dot && \
clang-14 -S -emit-llvm ../tests/test.c -o ../tests/test.ll && \
opt-14 -load-pass-plugin=./DefUseGraph.so -passes='defuse-graph' ../tests/test.ll -S -o test_instr.ll && \
clang-14 test_instr.ll ../src/runtime.c -o run_time_dump && \
./run_time_dump && \
echo "}" >> defuse_graph.dot && \
dot -Tpng defuse_graph.dot -o result.png

# TODO[flops]: Better move all .png in assets directory and use it in README to prove that your tool works!
#              Btw result graph is kinda pretty, good job!
