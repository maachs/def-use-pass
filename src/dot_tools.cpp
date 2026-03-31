#include "dot_tools.hpp"

void DotGraphWriter::writeHeader() {
    OS << "digraph DefUseGraph {\n";
    OS << "  node [shape=record, fontname=\"Courier\", fontsize=10];\n"
       << "  edge [fontsize=8];\n";
}

void DotGraphWriter::beginFunction(std::string Func) {
    OS << "  subgraph \"cluster_" << Func << "\" {\n"
       << "    label = \"Function: " << Func << "\";\n"
       << "    color = blue;\n";
}

void DotGraphWriter::endFunction() {
    OS << "  }\n";
}

void DotGraphWriter::writeArgumentNode(const llvm::Argument *Arg, std::string Label) {
    OS << "    \"node_" << Arg << "\" [label=\"{ARG: " << Arg->getName()
        << " | " << Label << "}\", color=blue, style=filled, fillcolor=lightcyan];\n";
}


void DotGraphWriter::writeInstructionNode(const llvm::Instruction *I, std::string Label) {
    OS << "    \"node_" << I << "\" [label=\"{INST | " << Label << "}\"];\n";
}

void DotGraphWriter::writeEdge(const llvm::Value *Src, const llvm::Value *Dest) {
    OS << "    \"node_" << Src << "\" -> \"node_" << Dest << "\" [label=\"" << "use" << "\"];\n";
}
