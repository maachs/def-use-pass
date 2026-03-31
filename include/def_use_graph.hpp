#ifndef DEF_USE_GRAPH_H
#define DEF_USE_GRAPH_H

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"

#include "common.hpp"

namespace llvm {
    class DefUseGraphPass : public PassInfoMixin<DefUseGraphPass> {
    public:
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
    private:
        static std::string sanitize(std::string str);
        static std::string getLabel(const llvm:: Value *V);
    };
}

#endif
