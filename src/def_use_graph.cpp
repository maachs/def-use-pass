// [flops]: Usually passes headers contain only declarations of methods, like:

// /path/to/include/HelloWorld.h
// class HelloWorldPass : public PassInfoMixin<HelloWorldPass> {
// public:
//   PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
// };

// /path/to/src/HelloWorld.cpp
// PreservedAnalyses HelloWorldPass::run(Function &F, FunctionAnalysisManager &AM) {
// ... 
// }

// FIXME[flops]: Don't use relative include paths, in Cmake you can use `include_directories(include/)` and include it like:
//      #include "def_use_graph.hpp"
#include "../include/def_use_graph.hpp"

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "DefUseGraph",
        "v0.1",
        [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::ModulePassManager &MPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                    if (Name == "defuse-graph") {
                        MPM.addPass(llvm::DefUseGraphPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}
