#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>

#include "Common.hpp"
#include "DefUseGraph.hpp"
#include "GraphTools.hpp"

namespace llvm {
PreservedAnalyses DefUseGraphPass::run(Module &M, ModuleAnalysisManager &AM) {
  std::error_code ErrorCode;

  llvm::raw_fd_ostream Out(get_defuse_path(), ErrorCode,
                           llvm::sys::fs::OF_None);

  if (ErrorCode) {
    errs() << "ERROR: cannot open file " << ErrorCode.message() << "\n";
    return PreservedAnalyses::all();
  }

  DotGraphWriter GraphWrite(Out);

  LLVMContext &Ctx = M.getContext();
  Type *VoidTy = Type::getVoidTy(Ctx);
  Type *Int64Ty = Type::getInt64Ty(Ctx);
  FunctionType *PrintFuncTy =
      FunctionType::get(VoidTy, {Int64Ty, Int64Ty}, false);
  FunctionCallee PrintFunc = M.getOrInsertFunction(DUMP_FUNC, PrintFuncTy);

  FunctionType *EndFuncTy = FunctionType::get(VoidTy, false);
  FunctionCallee EndFunc = M.getOrInsertFunction(DUMP_END, EndFuncTy);

  bool IsModified = false;

  GraphWrite.WriteHeader();

  for (auto &F : M) {

    if (F.isDeclaration())
      continue; // skip ex func

    GraphWrite.DumpFunction(F, GetLabel);

    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *Call = dyn_cast<CallInst>(&I)) {
          Function *Callee = Call->getCalledFunction();
          if (Callee && Callee->getName() == llvm::StringRef(DUMP_FUNC)) {
            continue;
          }
        }
        if ((isa<PtrToIntInst>(&I) || isa<ZExtInst>(&I) ||
             isa<BitCastInst>(&I)) &&
            I.getName().empty()) {
          continue;
        }

        GraphWrite.DumpInstruction(&I, GetLabel(&I));

        if (!I.getType()->isVoidTy()) {
          if (isa<PHINode>(&I) || I.isTerminator())
            continue;

          IRBuilder<> Builder(I.getNextNode());

          Value *ValToLog = &I;

          if (ValToLog->getType()->isPointerTy()) {
            ValToLog = Builder.CreatePtrToInt(ValToLog, Int64Ty);
          } else if (ValToLog->getType()->isIntegerTy()) {
            ValToLog = Builder.CreateZExtOrTrunc(ValToLog, Int64Ty);
          } else if (ValToLog->getType()->isFloatingPointTy()) {
            ValToLog = Builder.CreateBitCast(ValToLog, Int64Ty);
          } else {
            continue;
          }

          Value *AddrAsInt = Builder.getInt64(reinterpret_cast<uintptr_t>(&I));
          Builder.CreateCall(PrintFunc, {ValToLog, AddrAsInt});

          IsModified = true;
        }
      }
    }
    GraphWrite.EndFunction();
  }

  if (IsModified) {
    errs() << "Graph was written\n";
    return PreservedAnalyses::none();
  }
  return PreservedAnalyses::all();
}

std::string DefUseGraphPass::Sanitize(std::string_view Str) {
  std::string Res;
  size_t Len = Str.size();
  for (size_t i = 0; i < Len; ++i) {
    char CurChar = Str[i];
    if (CurChar == '"')
      Res += "\\\"";
    else if (CurChar == '\n')
      Res += "\\l";
    else
      Res += CurChar;
  }
  return Res;
}

std::string DefUseGraphPass::GetLabel(const llvm::Value *V) {
  std::string Str;
  llvm::raw_string_ostream OS(Str); // output
  V->print(OS);
  return Sanitize(Str);
}
} // namespace llvm

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DefUseGraph", "v0.1",
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
          }};
}
