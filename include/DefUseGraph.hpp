#ifndef DEF_USE_GRAPH_H
#define DEF_USE_GRAPH_H

#include <string_view>
#include "llvm/IR/PassManager.h"

inline constexpr std::string_view DUMP_FUNC = "dump_val";
inline constexpr std::string_view DUMP_END  = "dump_end";

namespace llvm {
    class DefUseGraphPass : public PassInfoMixin<DefUseGraphPass> {
    public:
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
    private:
        static std::string Sanitize(std::string_view Str);
        static std::string GetLabel(const llvm:: Value *V);
    };
}

#endif
