#include "GraphTools.hpp"

void DotGraphWriter::WriteHeader() const {
    OS << "digraph DefUseGraph {\n";
    OS << "  node [shape=record, fontname=\"Courier\", fontsize=10];\n"
       << "  edge [fontsize=8];\n";
}

void DotGraphWriter::BeginFunction(std::string_view Func) const {
    OS << "  subgraph \"cluster_" << Func << "\" {\n"
       << "    label = \"Function: " << Func << "\";\n"
       << "    color = blue;\n";
}

void DotGraphWriter::EndFunction() const {
    OS << "  }\n";
}

void DotGraphWriter::DumpFunction(const llvm::Function &F, std::function<std::string(const llvm::Value*)> LabelCreator) const {
    BeginFunction(F.getName().str());

    for (auto &Arg : F.args()) {
        WriteArgumentNode(&Arg, LabelCreator(&Arg));
    }

}

void DotGraphWriter::DumpInstruction(const llvm::Instruction *I, std::string_view Label) const {
    WriteInstructionNode(I, Label);

    for (const llvm::Value *Op : I->operands()) {

        if (llvm::isa<llvm::Constant>(Op) || llvm::isa<llvm::BasicBlock>(Op)) continue; //skip const and bb
        WriteEdge(Op, I);
    }
}

void DotGraphWriter::WriteArgumentNode(const llvm::Argument *Arg, std::string_view Label) const {
    OS << "    \"node_" << Arg << "\" [label=\"{ARG: " << Arg->getName()
        << " | " << Label << "}\", color=blue, style=filled, fillcolor=lightcyan];\n";
}


void DotGraphWriter::WriteInstructionNode(const llvm::Instruction *I, std::string_view Label) const {
    OS << "    \"node_" << I << "\" [label=\"{INST | " << Label << "}\"];\n";
}

void DotGraphWriter::WriteEdge(const llvm::Value *Src, const llvm::Value *Dest) const {
    OS << "    \"node_" << Src << "\" -> \"node_" << Dest << "\" [label=\"" << "use" << "\"];\n";
}
