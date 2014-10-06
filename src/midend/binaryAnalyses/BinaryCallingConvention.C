#include "sage3basic.h"
#include "threadSupport.h"  // for __attribute__ on Visual Studio
#include "BinaryCallingConvention.h"
#include "BinaryControlFlow.h"
#include "FindRegisterDefs.h"

using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics;

typedef ControlFlow::Graph CFG;
typedef boost::graph_traits<CFG>::vertex_descriptor CFGVertex;
typedef FindRegisterDefs::Policy Policy;
typedef X86InstructionSemantics<Policy, FindRegisterDefs::ValueType> Semantics;

/** Determines which registers are defined.  Evaluates a basic block to determine which registers have values assigned to them
 *  in that block. */
static void
evaluate_bblock(SgAsmBlock *bblock, Policy &policy)
{
    Semantics semantics(policy);
    const SgAsmStatementPtrList &insns = bblock->get_statementList();
    for (size_t i=0; i<insns.size(); ++i) {
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(insns[i]);
        if (insn) {
            try {
                semantics.processInstruction(insn);
            } catch (const Semantics::Exception &e) {
                std::cerr <<e <<" (instruction skipped)\n";
            }
        }
    }
}

static void
solve_flow_equation_iteratively(SgAsmFunction *func)
{
#if 1
    std::cerr <<"ROBB: solving flow equations for function " <<StringUtility::addrToString(func->get_entry_va()) <<"\n";
#endif
    rose::BinaryAnalysis::ControlFlow cfg_analyzer;
    CFG cfg = cfg_analyzer.build_block_cfg_from_ast<CFG>(func);

    CFGVertex entry_vertex = 0;
    assert(get(boost::vertex_name, cfg, entry_vertex)==func->get_entry_block());
    std::vector<CFGVertex> flowlist = cfg_analyzer.flow_order(cfg, entry_vertex);
    size_t nvertices = num_vertices(cfg); // flowlist might be smaller if CFG is not fully connected by forward edges
#if 0
    if (flowlist.size()!=num_vertices(cfg)) {
        std::cerr <<"flowlist.size() = " <<flowlist.size() <<"; nvertices = " <<num_vertices(cfg) <<"\n";
        std::cerr <<"flowlist vertices = {";
        for (size_t i=0; i<flowlist.size(); ++i) {
            std::cerr <<" " <<flowlist[i] <<"->{";
            boost::graph_traits<CFG>::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=out_edges(flowlist[i], cfg); ei!=ei_end; ++ei) {
                CFGVertex successor = target(*ei, cfg);
                std::cerr <<" " <<successor;
            }
            std::cerr <<" } ";
        }

        std::cerr <<" }\n" <<"graph vertices = {";
        for (size_t i=0; i<num_vertices(cfg); ++i) {
            SgAsmBlock *bblock = get(boost::vertex_name, cfg, (CFGVertex)i);
            assert(bblock!=NULL);
            std::cerr <<" [" <<i <<"]" <<StringUtility::addrToString(bblock->get_address());
        }
        std::cerr <<" }\n";
    }
#endif
    std::vector<Policy> policies(nvertices);            // output state indexed by vertex
    std::vector<bool> pending(nvertices, true);         // what vertices need to be (re)processed? (indexed by vertex)

    /* Initialize the incoming policy of the function entry block by defining certain registers.  The values we use to define
     * the registers are irrelevant since the policy doesn't track values, only bitmasks describing which bits of each register
     * are defined. */
    Policy entry_policy;
    entry_policy.writeRegister(entry_policy.findRegister("esp", 32), entry_policy.number<32>(0));
    entry_policy.writeRegister(entry_policy.findRegister("ebp", 32), entry_policy.number<32>(0));
    entry_policy.writeRegister(entry_policy.findRegister("es", 16), entry_policy.number<16>(0));
    entry_policy.writeRegister(entry_policy.findRegister("cs", 16), entry_policy.number<16>(0));
    entry_policy.writeRegister(entry_policy.findRegister("ss", 16), entry_policy.number<16>(0));
    entry_policy.writeRegister(entry_policy.findRegister("ds", 16), entry_policy.number<16>(0));
    entry_policy.writeRegister(entry_policy.findRegister("fs", 16), entry_policy.number<16>(0));
    entry_policy.writeRegister(entry_policy.findRegister("gs", 16), entry_policy.number<16>(0));
    
    /* Solve the flow equation iteratively to find out what's defined at the end of every basic block.   The policies[] stores
     * this info for each vertex. */
    bool changed;
    do {
        changed = false;
        for (size_t i=0; i<flowlist.size(); ++i) {
            CFGVertex vertex = flowlist[i];
            if (!pending[vertex])
                continue;
            pending[vertex] = false;
            SgAsmBlock *bblock = get(boost::vertex_name, cfg, vertex);
            assert(bblock!=NULL);

            /* Incoming policy for the block.  This is the merge of all out policies of predecessor vertices, with special
             * consideration for the function entry block. */
            Policy policy;
            if (bblock==func->get_entry_block())
                policy = entry_policy;
            boost::graph_traits<CFG>::in_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=in_edges(vertex, cfg); ei!=ei_end; ++ei) {
                CFGVertex predecessor = source(*ei, cfg);
                policy.merge(policies[predecessor]);
            }

            /* Compute output policy of this block. */
            policy.get_rdundef_state().clear();
            evaluate_bblock(bblock, policy/*in,out*/);

            /* If output of this block changed what we previously calculated, then mark all its children as pending. */
            if (!policy.equal_states(policies[vertex])) {
                changed = true;
                policies[vertex] = policy;
                boost::graph_traits<CFG>::out_edge_iterator ei, ei_end;
                for (boost::tie(ei, ei_end)=out_edges(vertex, cfg); ei!=ei_end; ++ei) {
                    CFGVertex successor = target(*ei, cfg);
                    pending[successor] = true;
                }
            }
        }
    } while (changed);

    /* Get a list of all parts of all registers that were read without being defined. But only do this for the basic blocks
     * that are reachable from the entry block.  I.e., those CFG vertices that are in the flowlist array. */
    FindRegisterDefs::State rdundef;
    for (size_t i=0; i<flowlist.size(); ++i) {
        CFGVertex vertex = flowlist[i];
        rdundef.merge(policies[vertex].get_rdundef_state());
#if 0
        if (func->get_entry_va()==0x411690) {
            std::cerr <<"================================================== DEBUG ===========================================\n";
            SgAsmBlock *blk = get(boost::vertex_name, cfg, vertex);
            AsmUnparser().unparse(std::cerr, func);
            std::cerr <<"Block " <<StringUtility::addrToString(blk->get_address()) <<":\n" <<policies[vertex]
                      <<"Undef reads:\n" <<policies[vertex].get_rdundef_state()
                      <<"Merged undef:\n" <<rdundef;
        }
#endif
    }

    for (size_t i=0; i<rdundef.n_gprs; ++i) {
        if (rdundef.gpr[i].defbits!=0) {
            std::cerr <<"    " <<std::setw(5) <<std::left <<gprToString((X86GeneralPurposeRegister)i)
                      <<" undef reads = " <<StringUtility::addrToString(rdundef.gpr[i].defbits) <<"\n";
        }
    }
}




#if 0
static void
test(SgAsmFunction *func)
{
    const SgAsmStatementPtrList &blocks = func->get_statementList();
    for (size_t i=0; i<blocks.size(); ++i) {
        SgAsmBlock *blk = isSgAsmBlock(blocks[i]);
        assert(blk!=NULL);
        regs_defined_in_bblock(blk);
    }
}
#endif




BinaryCallingConvention::Convention *
BinaryCallingConvention::analyze_callee(SgAsmFunction *func)
{
    solve_flow_equation_iteratively(func);
    return NULL;
}
