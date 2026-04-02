#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Module.h"
#include <cstdlib>

#include "DefUseGraph.hpp"
#include "GraphTools.hpp"

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

namespace llvm {
    PreservedAnalyses DefUseGraphPass::run(Module &M, ModuleAnalysisManager &AM) {
        const char* EnvPath = std::getenv("DEFUSE_PATH");
        std::string OutputFileName = EnvPath ? EnvPath : DEFAULT_GRAPH_FILENAME;
        std::error_code ErrorCode;

        llvm::raw_fd_ostream Out(OutputFileName, ErrorCode, llvm::sys::fs::OF_None);

        if (ErrorCode) {
            errs() << "ERROR: cannot open file " << ErrorCode.message() << "\n";
            return PreservedAnalyses::all();
        }

        DotGraphWriter GraphWrite(Out);

        LLVMContext &Ctx = M.getContext();
        Type *VoidTy = Type::getVoidTy(Ctx);
        Type *Int32Ty = Type::getInt32Ty(Ctx);
        Type *Int64Ty = Type::getInt64Ty(Ctx);
        FunctionType *PrintFuncTy = FunctionType::get(VoidTy, {Int32Ty, Int64Ty}, false);
        FunctionCallee PrintFunc = M.getOrInsertFunction("dump_val", PrintFuncTy);

        FunctionType *EndFuncTy = FunctionType::get(VoidTy, false);
        FunctionCallee EndFunc = M.getOrInsertFunction("dump_end", EndFuncTy);

        bool IsModified = false;

        GraphWrite.WriteHeader();

        for (auto &F : M) {

            if (F.isDeclaration()) continue; //skip ex func

            GraphWrite.DumpFunction(F, GetLabel);

            for (auto &BB : F) {
                for (auto &I : BB) {

                    GraphWrite.DumpInstruction(&I, GetLabel(&I));

                    if (I.getType()->isIntegerTy(cast<IntegerType>(Int32Ty)->getBitWidth())) {
                        if (isa<PHINode>(&I) || I.isTerminator()) continue;

                        IRBuilder<> Builder(I.getNextNode());

                        Value *AddrAsInt = Builder.getInt64(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&I)));
                        Builder.CreateCall(PrintFunc, {&I, AddrAsInt});

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
            if (CurChar == '"') Res += "\\\"";
            else if (CurChar == '\n') Res += "\\l";
            else Res += CurChar;
        }
        return Res;
    }

    std::string DefUseGraphPass::GetLabel(const llvm::Value *V) {
        std::string Str;
        llvm::raw_string_ostream OS(Str); //output
        V->print(OS);
        return Sanitize(Str);
    }
}

