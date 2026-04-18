#ifndef DEF_USE_GRAPH_H
#define DEF_USE_GRAPH_H

#include "llvm/IR/PassManager.h"
#include <string_view>

namespace llvm {
class DefUseGraphPass : public PassInfoMixin<DefUseGraphPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
  static constexpr std::string_view DUMP_FUNC = "dump_val";
  static constexpr std::string_view DUMP_END = "dump_end";

private:
  static std::string Sanitize(std::string_view Str);
  static std::string GetLabel(const llvm::Value *V);
};
} // namespace llvm

#endif
