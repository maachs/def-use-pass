#ifndef DEF_USE_GRAPH_H
#define DEF_USE_GRAPH_H

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Instructions.h"

namespace llvm {
    class DefUseGraphPass : public PassInfoMixin<DefUseGraphPass> {
    public:
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
            std::error_code err_code;

            llvm::raw_fd_ostream Out("defuse_graph.dot", err_code, llvm::sys::fs::OF_None);

            if (err_code) {
                errs() << "ERROR: cannot open file " << err_code.message() << "\n";
                return PreservedAnalyses::all();
            }

            Out << "digraph DefUseGraph {\n";
            Out << "  node [shape=record, fontname=\"Courier\", fontsize=10];\n";
            Out << "  edge [fontsize=8];\n";

            auto module_end = M.end();
            for (llvm::Module::iterator f_it = M.begin(); f_it != module_end; ++f_it) {
                llvm::Function &F = *f_it;

                if (F.isDeclaration()) continue; //skip ex func

                Out << "  subgraph \"cluster_" << F.getName() << "\" {\n";
                Out << "    label = \"Function: " << F.getName() << "\";\n";
                Out << "    color = blue;\n";

                auto arg_end = F.arg_end();
                for (llvm::Function::arg_iterator arg_it = F.arg_begin(); arg_it != arg_end; ++arg_it) {
                    llvm::Argument &arg = *arg_it;

                    Out << "    \"node_" << &arg << "\" [label=\"{ARG: " << arg.getName()
                        << " | " << getLabel(&arg) << "}\", color=blue, style=filled, fillcolor=lightcyan];\n";
                }

                auto bb_end = F.end();
                for (llvm::Function::iterator bb_it = F.begin(); bb_it != bb_end; ++bb_it) {
                    llvm::BasicBlock &bb = *bb_it;

                    auto instr_end = bb.end();
                    for (llvm::BasicBlock::iterator instr_it = bb.begin(); instr_it != instr_end; ++instr_it) {
                        llvm::Instruction &instr = *instr_it;

                        Out << "    \"node_" << &instr << "\" [label=\"{INST | " << getLabel(&instr) << "}\"];\n";

                        auto op_end = instr.op_end();
                        for (llvm::User::op_iterator op_it = instr.op_begin(); op_it != op_end; ++op_it) {
                            llvm::Value *op = op_it->get();

                            if (llvm::isa<llvm::Constant>(op) || llvm::isa<llvm::BasicBlock>(op)) continue; //skip const and bb
                            Out << "    \"node_" << op << "\" -> \"node_" << &instr << "\" [label=\"use\"];\n";
                        }
                    }
                }
                Out << "  }\n";
            }

            Out << "}\n";
            Out.close();

            errs() << "[DefUseGraph] Graph written to defuse_graph.dot\n";

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
