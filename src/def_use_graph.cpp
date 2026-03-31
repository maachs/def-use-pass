#include "def_use_graph.hpp"
#include "dot_tools.hpp"

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
        std::error_code err_code;

        llvm::raw_fd_ostream Out(GRAPH_FILENAME, err_code, llvm::sys::fs::OF_None);

        if (err_code) {
            errs() << "ERROR: cannot open file " << err_code.message() << "\n";
            return PreservedAnalyses::all();
        }

        DotGraphWriter graph_write(Out);

        LLVMContext &Ctx = M.getContext();
        Type *VoidTy = Type::getVoidTy(Ctx);
        Type *Int32Ty = Type::getInt32Ty(Ctx);
        Type *Int64Ty = Type::getInt64Ty(Ctx);
        FunctionType *PrintFuncTy = FunctionType::get(VoidTy, {Int32Ty, Int64Ty}, false);
        FunctionCallee PrintFunc = M.getOrInsertFunction("dump_val", PrintFuncTy);

        FunctionType *EndFuncTy = FunctionType::get(VoidTy, false);
        FunctionCallee EndFunc = M.getOrInsertFunction("dump_end", EndFuncTy);

        bool Modified = false;

        graph_write.writeHeader();

        for (auto &F : M) {

            if (F.isDeclaration()) continue; //skip ex func

            graph_write.beginFunction(F.getName().str());

            for (auto &Arg : F.args()) {
                graph_write.writeArgumentNode(&Arg, getLabel(&Arg));
            }

            for (auto &BB : F) {
                for (auto &I : BB) {

                    graph_write.writeInstructionNode(&I, getLabel(&I));

                    for (Value *Op : I.operands()) {

                        if (llvm::isa<llvm::Constant>(Op) || llvm::isa<llvm::BasicBlock>(Op)) continue; //skip const and bb
                        graph_write.writeEdge(Op, &I);
                    }

                    if (I.getType()->isIntegerTy(32)) {
                        if (isa<PHINode>(&I) || I.isTerminator()) continue;

                        IRBuilder<> Builder(I.getNextNode());

                        Value *AddrAsInt = Builder.getInt64(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&I)));
                        Builder.CreateCall(PrintFunc, {&I, AddrAsInt});

                        Modified = true;
                    }
                    if (F.getName() == "main") {
                        if (auto *Ret = dyn_cast<ReturnInst>(&I)) {
                            IRBuilder<> Builder(Ret);
                            Builder.CreateCall(EndFunc);
                            Modified = true;
                        }
                    }
                }
            }
            graph_write.endFunction();
        }

        Out.close();

        if (Modified) {
            errs() << "Graph was written\n";
            return PreservedAnalyses::none();
        }

        return PreservedAnalyses::all();
    }

    std::string DefUseGraphPass::sanitize(std::string str) {
        std::string res;
        size_t len = str.size();
        for (size_t i = 0; i < len; ++i) {
            char cur_char = str[i];
            if (cur_char == '"') res += "\\\"";
            else if (cur_char == '\n') res += "\\l";
            else res += cur_char;
        }
        return res;
    }

    std::string DefUseGraphPass::getLabel(const llvm::Value *V) {
        std::string str;
        llvm::raw_string_ostream OS(str); //output
        V->print(OS);
        return sanitize(str);
    }
}
