#ifndef DOT_TOOLS_
#define DOT_TOOLS_

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include <string>

class DotGraphWriter {
    llvm::raw_ostream &OS;
public:
    DotGraphWriter(llvm::raw_ostream &Out) : OS(Out) {}

    void writeHeader();
    void beginFunction(std::string Func);
    void endFunction();
    void writeArgumentNode(const llvm::Argument *Arg, std::string Label);
    void writeInstructionNode(const llvm::Instruction *I, std::string Label);
    void writeEdge(const llvm::Value *Src, const llvm::Value *Dest);
};

#endif
