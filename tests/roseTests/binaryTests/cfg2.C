// Tests how to fix up a binary control flow graph so that:
//   * the CFG vertices are instructions rather than basic blocks
//   * function call vertices do not have an edge directly to the fall through instruction
//   * function return vertices point to the fall through instruction of all of their known call sites
#include "sage3basic.h"
#include "BinaryControlFlow.h"
#include "AsmUnparser_compat.h"

// Label the graphviz vertices with the address and instruction.  Include function name for function entry instructions.
// This is used by boost::write_graphviz()
template<class Graph>
struct GraphvizVertexWriter {
    const Graph &cfg;
    GraphvizVertexWriter(Graph &cfg): cfg(cfg) {}
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    void operator()(std::ostream &output, const Vertex &v) const {
        SgAsmInstruction *insn = get(boost::vertex_name, cfg, v);
        SgAsmx86Instruction *x86 = isSgAsmx86Instruction(insn);
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
        output <<"[ label=\"" <<unparseInstructionWithAddress(insn) <<"\"";
        if (insn->get_address()==func->get_entry_va() && !func->get_name().empty()) {
            output <<", style=filled, color=\"#cd853f\"";
        } else if (x86 && x86_ret==x86->get_kind()) {
            output <<", style=filled, color=\"#fed3a7\"";
        }
        output <<" ]";
    }
};

int main(int argc, char *argv[])
{
    // Use the last interpretation (ELF only has one; Windows has a DOS and PE and we want the PE).
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    assert(!interps.empty());
    SgAsmInterpretation *interp = interps.back();

    // Build an instruction-based control flow graph from control flow info stored in the AST
    typedef BinaryAnalysis::ControlFlow::InsnGraph CFG;
    BinaryAnalysis::ControlFlow analyzer;
    CFG cfg = analyzer.build_insn_cfg_from_ast<CFG>(interp);
    analyzer.write_graphviz(std::cout, cfg, GraphvizVertexWriter<CFG>(cfg));
}
