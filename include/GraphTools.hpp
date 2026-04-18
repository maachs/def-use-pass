#ifndef DOT_TOOLS_
#define DOT_TOOLS_

#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include <string>

class DotGraphWriter final {
  llvm::raw_ostream &OS;

public:
  DotGraphWriter(llvm::raw_ostream &Out) : OS(Out) {}

  void WriteHeader() const;
  void EndFunction() const;
  void DumpFunction(
      const llvm::Function &F,
      std::function<std::string(const llvm::Value *)> LabelCreator) const;
  void DumpInstruction(const llvm::Instruction *I,
                       std::string_view Label) const;

private:
  void BeginFunction(std::string_view Func) const;
  void WriteEdge(const llvm::Value *Src, const llvm::Value *Dest) const;
  void WriteArgumentNode(const llvm::Argument *Arg,
                         std::string_view Label) const;
  void WriteInstructionNode(const llvm::Instruction *I,
                            std::string_view Label) const;
};

#endif
