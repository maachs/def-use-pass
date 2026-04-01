#ifndef DEF_USE_GRAPH_H
#define DEF_USE_GRAPH_H

#include "llvm/IR/PassManager.h"

#include "Common.hpp"

namespace llvm {
    class DefUseGraphPass : public PassInfoMixin<DefUseGraphPass> {
    public:
        PreservedAnalyses  run     (Module &M, ModuleAnalysisManager &AM);
    private:
        static std::string Sanitize(std::string_view Str);
        static std::string GetLabel(const llvm:: Value *V);
    };
}

#endif
