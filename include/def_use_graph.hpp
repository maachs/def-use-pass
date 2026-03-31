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

namespace llvm {
    class DefUseGraphPass : public PassInfoMixin<DefUseGraphPass> {
    public:
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
            std::error_code err_code;

            // FIXME[flops]: Runtime and pass must work with the same dot file, so better to move it name in common include
            llvm::raw_fd_ostream Out("defuse_graph.dot", err_code, llvm::sys::fs::OF_None);

            if (err_code) {
                errs() << "ERROR: cannot open file " << err_code.message() << "\n";
                return PreservedAnalyses::all();
            }

            LLVMContext &Ctx = M.getContext();
            Type *VoidTy = Type::getVoidTy(Ctx);
            Type *Int32Ty = Type::getInt32Ty(Ctx);
            Type *Int64Ty = Type::getInt64Ty(Ctx);
            FunctionType *PrintFuncTy = FunctionType::get(VoidTy, {Int32Ty, Int64Ty}, false);
            FunctionCallee PrintFunc = M.getOrInsertFunction("dump_val", PrintFuncTy);

            bool Modified = false;

            // FIXME[flops]: Transfer all work with GraphViz dump in separate class/funcs
            Out << "digraph DefUseGraph {\n";
            Out << "  node [shape=record, fontname=\"Courier\", fontsize=10];\n";
            Out << "  edge [fontsize=8];\n";

            auto module_end = M.end(); // FIXME[flops]: You can use just `for (auto &Func: Module) { ... }`
            for (llvm::Module::iterator f_it = M.begin(); f_it != module_end; ++f_it) { 
                llvm::Function &func = *f_it;

                // FIXME[flops]: You should not perform this check: `func.getName() == "dump_val"`, 
                // because you are iterating through compilation source functions, so `dump_val` doesn't exist there
                if (func.isDeclaration() || func.getName() == "dump_val") continue; //skip ex func

                Out << "  subgraph \"cluster_" << func.getName() << "\" {\n";
                Out << "    label = \"Function: " << func.getName() << "\";\n";
                Out << "    color = blue;\n";

                auto arg_end = func.arg_end(); // TODO[flops]: for (auto &Arg : Func.args())
                for (llvm::Function::arg_iterator arg_it = func.arg_begin(); arg_it != arg_end; ++arg_it) {
                    llvm::Argument &arg = *arg_it;

                    Out << "    \"node_" << &arg << "\" [label=\"{ARG: " << arg.getName()
                        << " | " << getLabel(&arg) << "}\", color=blue, style=filled, fillcolor=lightcyan];\n";
                }

                auto bb_end = func.end(); // TODO[flops]: for (auto &BB : Func)
                for (llvm::Function::iterator bb_it = func.begin(); bb_it != bb_end; ++bb_it) {
                    llvm::BasicBlock &bb = *bb_it;

                    auto instr_end = bb.end(); // TODO[flops]: for (auto &I : BB)
                    for (llvm::BasicBlock::iterator instr_it = bb.begin(); instr_it != instr_end; ++instr_it) {
                        llvm::Instruction &instr = *instr_it;

                        // [flops]: You should not perform this check, 
                        // because you are iterating through compilation source instructions 
                        // that are not instrumented yet
                        if (auto *call = llvm::dyn_cast<llvm::CallInst>(&instr)) {
                            if (auto *callee = call->getCalledFunction()) {
                                if (callee->getName() == "dump_val") {
                                    continue;
                                }
                            }
                        }

                        Out << "    \"node_" << &instr << "\" [label=\"{INST | " << getLabel(&instr) << "}\"];\n";

                        auto op_end = instr.op_end(); // [flops]: for (llvm::User *U : I->users()) or for (llvm::Value *Op : I.operands())
                        for (llvm::User::op_iterator op_it = instr.op_begin(); op_it != op_end; ++op_it) {
                            llvm::Value *op = op_it->get();

                            // 
                            if (llvm::isa<llvm::Constant>(op) || llvm::isa<llvm::BasicBlock>(op)) continue; //skip const and bb
                            Out << "    \"node_" << op << "\" -> \"node_" << &instr << "\" [label=\"use\"];\n";
                        }

                        if (instr.getType()->isIntegerTy(32)) {
                            if (isa<PHINode>(&instr)) continue;
                            if (instr.isTerminator()) continue;

                            IRBuilder<> Builder(instr.getNextNode());

                            Value *AddrAsInt = Builder.getInt64((uintptr_t)&instr); // TODO[flops]: Use static_cast
                            Builder.CreateCall(PrintFunc, {&instr, AddrAsInt});

                            Modified = true;

                        }
                    }
                }
                Out << "  }\n";
            }

            Out.close();

            if (Modified) {
                errs() << "Graph is wrote\n";
                return PreservedAnalyses::none();
            }

            return PreservedAnalyses::all();
        }

    private:
        static std::string sanitize(std::string str) {
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

        static std::string getLabel(const llvm::Value *V) {
            std::string str;
            llvm::raw_string_ostream OS(str); //output
            V->print(OS);
            return sanitize(str);
        }
    };

}
#endif
