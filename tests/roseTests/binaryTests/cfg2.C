// Tests how to fix up a binary control flow graph so that:
//   * the CFG vertices are instructions rather than basic blocks
//   * function call vertices do not have an edge directly to the fall through instruction
//   * function return vertices point to the fall through instruction of all of their known call sites
#include "sage3basic.h"
#include "BinaryControlFlow.h"
#include "BinaryAnalysisUtils.h"

int main(int argc, char *argv[])
{
    // Use the last interpretation (ELF only has one; Windows has a DOS and PE and we want the PE).
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    assert(!interps.empty());
    SgAsmInterpretation *interp = interps.back();

    // Build an instruction-based control flow graph from control flow info stored in the AST
    typedef rose::BinaryAnalysis::ControlFlow::InsnGraph CFG;
    rose::BinaryAnalysis::ControlFlow analyzer;
    CFG cfg = analyzer.build_insn_cfg_from_ast<CFG>(interp);
    analyzer.write_graphviz(std::cout, cfg, rose::BinaryAnalysis::InsnCFGVertexWriter<CFG>(cfg));
}
