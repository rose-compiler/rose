// This file contains binary analysis utilities that are either not a critical part of the analysis framework, or which cannot
// be easily incorporated into the main header files because they would introduce circular #include dependencies.

#ifndef ROSE_BinaryAnalysisUtils_H
#define ROSE_BinaryAnalysisUtils_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "AsmUnparser_compat.h"

namespace Rose {
namespace BinaryAnalysis {                      // documented elsewhere

/** A vertex property writer for instruction-based CFGs.  This causes vertex names to be instructions rather than
 *  numbers, and it colors function entry and exit nodes to make them stand out better. */
template<class Graph>
struct InsnCFGVertexWriter {
    const Graph &cfg;
    InsnCFGVertexWriter(Graph &cfg): cfg(cfg) {}
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    void operator()(std::ostream &output, const Vertex &v) const {
        SgAsmInstruction *insn = get_ast_node(cfg, v);
        SgAsmX86Instruction *x86 = isSgAsmX86Instruction(insn);
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
        output <<"[ label=\"[" <<v << "] " <<insn->toString() <<"\"";
        if (insn->get_address()==func->get_entry_va()) {
            output <<", style=filled, color=\"#cd853f\"";
        } else if (x86 && x86_ret==x86->get_kind()) {
            output <<", style=filled, color=\"#fed3a7\"";
        }
        output <<" ]";
    }
};

} // namespace
} // namespace

#endif
#endif
