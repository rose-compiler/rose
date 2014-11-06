/* Reads an executable, library, archive, core dump, or raw buffer of machine instructions and disassembles according to
 * command-line switches.  Although ROSE does the disassembly and partitioning by default, this test disables that automation
 * and does everything the hard way.  This allows us more control over what happens and even allows us to disassemble raw
 * buffers of machine instructions.  This test also does a variety of analyses and Robb uses it as a general tool and staging
 * area for testing new features before they're added to ROSE. */

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <errno.h>
#include <inttypes.h>
#include <ostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sawyer/CommandLine.h>

#include "AsmFunctionIndex.h"
#include "AsmUnparser.h"
#include "BinaryLoader.h"
#include "PartialSymbolicSemantics.h"
#include "SMTSolver.h"
#include "BinaryControlFlow.h"
#include "BinaryFunctionCall.h"
#include "BinaryDominance.h"
#include "Disassembler.h"
#include "DisassemblerPowerpc.h"
#include "DisassemblerMips.h"
#include "DisassemblerX86.h"
#include "DisassemblerM68k.h"
#include "Diagnostics.h"

/*FIXME: Rose cannot parse this file.*/
#ifndef CXX_I_ROSE_ANALYSIS

using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics;
using namespace Sawyer::Message::Common;
using namespace StringUtility;

static Sawyer::Message::Facility mlog; // diagnostics at the tool level; initialization in main()

enum DisassembleDriver { DDRIVE_PD, DDRIVE_DP, DDRIVE_D, DDRIVE_NONE };
enum SyscallMethod { SYSCALL_NONE, SYSCALL_LINUX32 };

/* Convert a SHA1 digest to a string. */
std::string
digest_to_str(const unsigned char digest[20])
{
    std::string digest_str;
    for (size_t i=20; i>0; --i) {
        digest_str += "0123456789abcdef"[(digest[i-1] >> 4) & 0xf];
        digest_str += "0123456789abcdef"[digest[i-1] & 0xf];
    }
    return digest_str;
}

/** Computes the SHA1 digest for the semantics of a single basic block. Returns true if the hash was computed, false
 *  otherwise.  When the hash is not computed, @p digest is set to all zeros. */
bool
block_hash(SgAsmBlock *blk, unsigned char digest[20]) 
{

    if (!blk || blk->get_statementList().empty() || !isSgAsmX86Instruction(blk->get_statementList().front())) {
        memset(digest, 0, 20);
        return false;
    }
    const SgAsmStatementPtrList &stmts = blk->get_statementList();

    typedef PartialSymbolicSemantics::Policy<PartialSymbolicSemantics::State, PartialSymbolicSemantics::ValueType> Policy;
    typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
    Policy policy;
    policy.set_discard_popped_memory(true);
    Semantics semantics(policy);
    try {
        for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(*si);
            ASSERT_not_null(insn);
            semantics.processInstruction(insn);
        }
    } catch (const Semantics::Exception&) {
        memset(digest, 0, 20);
        return false;
    } catch (const Policy::Exception&) {
        memset(digest, 0, 20);
        return false;
    }

    /* If the last instruction is a x86 CALL or FARCALL then change the return address that's at the top of the stack so that
     * two identical blocks located at different memory addresses generate equal hashes (at least as far as the function call
     * is concerned. */
    bool ignore_final_ip = true;
    SgAsmX86Instruction *last_insn = isSgAsmX86Instruction(stmts.back());
    if (last_insn->get_kind()==x86_call || last_insn->get_kind()==x86_farcall) {
        policy.writeMemory(x86_segreg_ss, policy.readRegister<32>("esp"), policy.number<32>(0), policy.true_());
        ignore_final_ip = false;
    }

    /* Set original IP to a constant value so that hash is never dependent on the true original IP.  If the final IP doesn't
     * matter, then make it the same as the original so that the difference between the original and final does not include the
     * IP (SHA1 is calculated in terms of the difference). */
    policy.get_orig_state().registers.ip = policy.number<32>(0);
    if (ignore_final_ip)
        policy.get_state().registers.ip = policy.get_orig_state().registers.ip;
    return policy.SHA1(digest);
}

/* Compute a hash value for a function. Return false if the hash cannot be computed. */
bool
function_hash(SgAsmFunction *func, unsigned char digest[20])
{
    memset(digest, 0, 20);
    std::set<std::string> seen;
    const SgAsmStatementPtrList &stmts = func->get_statementList();
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        SgAsmBlock *bb = isSgAsmBlock(*si);
        ASSERT_not_null2(bb, "SgAsmFunction should contain basic blocks");
        unsigned char bb_digest[20];
        if (block_hash(bb, bb_digest)) {
            std::string key = digest_to_str(bb_digest);
            if (seen.find(key)==seen.end()) {
                seen.insert(key);
                for (size_t i=0; i<20; i++)
                    digest[i] ^= bb_digest[i];
            }
        } else {
            memset(digest, 0, 20);
            return false;
        }
    }
    return true;
}

/* Aguments the AsmFunctionIndex by always sorting functions by entry address, and adding an extra column named "Hash" that
 * contains the hash value (if known) of the function. */
class ShowFunctions: public AsmFunctionIndex {
public:
    struct HashCallback: public OutputCallback {
        HashCallback(): OutputCallback("Hash", 16, "Experimental semantic hash of the entire function.") {}
        virtual bool operator()(bool enabled, const DataArgs &args) {
            if (enabled) {
                unsigned char sha1[20];
                if (function_hash(args.func, sha1)) {
                    args.output <<data_prefix <<std::setw(width) <<digest_to_str(sha1).substr(0, 16);
                } else {
                    args.output <<data_prefix <<std::setw(width) <<"";
                }
            }
            return enabled;
        }
    } hashCallback;

    ShowFunctions(SgNode *ast): AsmFunctionIndex(ast) {
        sort_by_entry_addr();
        output_callbacks.before(&nameCallback, &hashCallback, 1);
    }

    virtual void print(std::ostream &o) const {
        static const size_t width = 130;
        std::string sep_line(width, '=');
        std::string title("Function Index");
        std::string title_line(width, ' ');
        title_line.replace(0, 3, "===");
        title_line.replace(width-3, 3, "===");
        title_line.replace((std::max(width, title.size())-title.size())/2, title.size(), title);
        o <<"\n\n" <<sep_line <<"\n" <<title_line <<"\n" <<sep_line <<"\n";
        AsmFunctionIndex::print(o);
        o <<sep_line <<"\n\n";
    }
};

/* Unparser that outputs some extra information */
class MyAsmUnparser: public AsmUnparser {
public:
    MyAsmUnparser(bool show_hashes, bool show_syscall_names) {
        if (show_hashes) {
            function_callbacks.pre.append(&functionHash);
            basicblock_callbacks.pre.append(&blockHash);
        }
        if (show_syscall_names)
            insn_callbacks.unparse.append(&syscallName);
        basicblock_callbacks.pre.append(&dominatorBlock);
    }

private:
    /* Functor to add a hash to the beginning of basic block output. */
    class BlockHash: public UnparserCallback {
    public:
        bool operator()(bool enabled, const BasicBlockArgs &args) {
            unsigned char sha1[20];
            if (enabled && block_hash(args.block, sha1))
                args.output <<addrToString(args.block->get_address()) <<": " <<digest_to_str(sha1) <<"\n";
            return enabled;
        }
    };
    
    /* Functor to add a hash to the beginning of every function. */
    class FunctionHash: public UnparserCallback {
    public:
        bool operator()(bool enabled, const FunctionArgs &args) {
            unsigned char sha1[20];
            if (enabled && function_hash(args.func, sha1)) {
                args.output <<addrToString(args.func->get_entry_va())
                            <<": ============================ " <<digest_to_str(sha1) <<"\n";
            }
            return enabled;
        }
    };

    /* Functor to add syscall name after "INT 80" instructions */
    class SyscallName: public UnparserCallback {
    public:
        bool operator()(bool enabled, const InsnArgs &args) {
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(args.insn);
            SgAsmBlock *block = SageInterface::getEnclosingNode<SgAsmBlock>(args.insn);
            if (enabled && insn && block && insn->get_kind()==x86_int) {
                const SgAsmExpressionPtrList &opand_list = insn->get_operandList()->get_operands();
                SgAsmExpression *expr = opand_list.size()==1 ? opand_list[0] : NULL;
                if (expr && expr->variantT()==V_SgAsmIntegerValueExpression &&
                    0x80==isSgAsmIntegerValueExpression(expr)->get_value()) {

                    const SgAsmStatementPtrList &stmts = block->get_statementList();
                    size_t int_n;
                    for (int_n=0; int_n<stmts.size(); int_n++)
                        if (isSgAsmInstruction(stmts[int_n])==args.insn)
                            break;

                    typedef PartialSymbolicSemantics::Policy<PartialSymbolicSemantics::State,
                                                             PartialSymbolicSemantics::ValueType> Policy;
                    typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
                    Policy policy;
                    Semantics semantics(policy);

                    try {
                        semantics.processBlock(stmts, 0, int_n);
                        if (policy.readRegister<32>("eax").is_known()) {
                            int nr = policy.readRegister<32>("eax").known_value();
                            extern std::map<int, std::string> linux32_syscalls; // defined in linux_syscalls.C
                            const std::string &syscall_name = linux32_syscalls[nr];
                            if (!syscall_name.empty())
                                args.output <<" <" <<syscall_name <<">";
                        }
                    } catch (const Semantics::Exception&) {
                    } catch (const Policy::Exception&) {
                    }
                }
            }
            return enabled;
        }
    };

    /* Functor to emit immediate dominator of each basic block. */
    class DominatorBlock: public UnparserCallback {
    public:
        bool operator()(bool enabled, const BasicBlockArgs &args) {
            SgAsmBlock *idom = args.block->get_immediate_dominator();
            if (enabled && idom)
                args.output <<args.unparser->line_prefix()
                            <<"Dominator block: " <<addrToString(idom->get_address()) <<"\n";
            return enabled;
        }
    };
    
private:
    BlockHash blockHash;
    FunctionHash functionHash;
    SyscallName syscallName;
    DominatorBlock dominatorBlock;
};

/* Generate the "label" attribute for a function node in a *.dot file. */
static std::string
function_label_attr(SgAsmFunction *func)
{
    std::string retval;
    if (func) {
        char buf[64];
        sprintf(buf, "F%08"PRIx64, func->get_entry_va());
        retval += std::string("label = \"") + buf;
        if (func->get_name().size()>0)
            retval += std::string(" <") + func->get_name() + ">";
        retval += std::string("\\n(") + func->reason_str(false) + ")";
        sprintf(buf, "\\n%zu instructions", SageInterface::querySubTree<SgAsmInstruction>(func, V_SgAsmInstruction).size());

        unsigned char sha1[20];
        if (function_hash(func, sha1))
            retval += std::string(buf) + "\\n" + digest_to_str(sha1).substr(0, 16) + "...";
        retval += "\"";
    }
    return retval;
}

/* Generate the "URL" attribute for a function node in a *.dot file */
static std::string
function_url_attr(SgAsmFunction *func)
{
    char buf[64];
    sprintf(buf, "F%08"PRIx64, func->get_entry_va());
    return std::string("URL=\"") + buf + ".html\"";
}


        

/* Prints a graph node for a function. If @p verbose is true then the basic blocks of the funtion are displayed along with
 * control flow edges within the function. */
static std::string
dump_function_node(std::ostream &sout, SgAsmFunction *func, rose::BinaryAnalysis::ControlFlow::Graph &global_cfg,
                   bool verbose) 
{
    using namespace StringUtility;

    class Unparser: public AsmUnparser {
    private:
        std::set<std::string> semantics_seen;

        /* Generates the GraphViz node for a basic block, and the HTML table header for the node's label. */
        struct BasicBlockGraphvizNodeStart: public UnparserCallback {
            std::set<std::string> &semantics_seen;
            BasicBlockGraphvizNodeStart(std::set<std::string> &semantics_seen)
                : semantics_seen(semantics_seen) {}
            virtual bool operator()(bool enabled, const BasicBlockArgs &args) {
                if (enabled) {
                    std::string semantics_color = "green";
                    unsigned char sha1[20];
                    block_hash(args.block, sha1);
                    std::string sha1_str = digest_to_str(sha1);
                    if (semantics_seen.find(sha1_str)!=semantics_seen.end()) {
                        semantics_color = "orange";
                    } else {
                        semantics_seen.insert(sha1_str);
                    }
                    args.output <<"B" <<addrToString(args.block->get_address()) <<" [ label=<<table border=\"0\"";
                    SgAsmFunction *func = args.block->get_enclosing_function();
                    if (func && args.block->get_address()==func->get_entry_va())
                        args.output <<" bgcolor=\"lightskyblue1\"";
                    args.output <<"><tr><td align=\"left\" bgcolor=\"" <<semantics_color <<"\">"
                                <<sha1_str.substr(0, 16) <<"...</td></tr>";
                }
                return enabled;
            }
        } basicBlockGraphvizNodeStart;

        /* Generates the body of the basic block inside the GraphViz node. */
        struct BasicBlockGraphvizNodeBody: public UnparserCallback {
            virtual bool operator()(bool enabled, const BasicBlockArgs &args) {
                if (enabled) {
                    for (size_t i=0; i<args.insns.size(); i++) {
                        std::ostringstream ss;
                        args.unparser->unparse_insn(enabled, ss, args.insns[i], i);
                        bool is_noop = i<args.unparser->insn_is_noop.size() && args.unparser->insn_is_noop[i];
                        args.output <<"<tr>"
                                    <<"<td align=\"left\"" <<(is_noop?" bgcolor=\"gray50\"":"") <<">"
                                    <<htmlEscape(ss.str())
                                    <<"</td></tr>";
                    }
                }
                return enabled;
            }
        } basicBlockGraphvizNodeBody;

        /* Generates the end of the HTML table for a GraphViz node label, and the rest of the information for the node. */
        struct BasicBlockGraphvizNodeEnd: public UnparserCallback {
            virtual bool operator()(bool enabled, const BasicBlockArgs &args) {
                if (enabled) {
                    SgAsmFunction *func = args.block->get_enclosing_function();
                    args.output <<"</table>>";
                    if (!args.block->get_successors_complete()) {
                        ASSERT_forbid2(args.block->get_statementList().empty(), "basic blocks should not be empty");
                        SgAsmInstruction *last_insn = isSgAsmInstruction(args.block->get_statementList().back());
                        if (isSgAsmX86Instruction(last_insn) && isSgAsmX86Instruction(last_insn)->get_kind()==x86_ret) {
                            args.output <<", color=blue"; /*function return statement, not used as an unconditional branch*/
                        } else {
                            args.output <<", color=red"; /*red implies that we don't have complete information for successors*/
                        }
                    } else if (func && args.block->get_address()==func->get_entry_va()) {
                        args.output <<", color=blue"; /*function entry node*/
                    }
                    args.output <<" ];\n";
                }
                return enabled;
            }
        } basicBlockGraphvizNodeEnd;

        /* Clear the per-function unparsing information. */
        struct FunctionCleanup: public UnparserCallback {
            std::set<std::string> &semantics_seen;
            FunctionCleanup(std::set<std::string> &semantics_seen)
                : semantics_seen(semantics_seen) {}
            virtual bool operator()(bool enabled, const FunctionArgs &args) {
                semantics_seen.clear();
                return enabled;
            }
        } functionCleanup;

    public:
        Unparser()
            : basicBlockGraphvizNodeStart(semantics_seen),
              functionCleanup(semantics_seen) {

            insn_callbacks.pre
                .clear()
                .append(&insnAddress);
            insn_callbacks.post
                .clear();

            basicblock_callbacks.pre
                .clear()
                .append(&basicBlockNoopUpdater)         /* calculate no-op subsequences needed by insns */
                .append(&basicBlockGraphvizNodeStart);  /* beginning of GraphViz node */
            basicblock_callbacks.unparse
                .replace(&basicBlockBody, &basicBlockGraphvizNodeBody);
            basicblock_callbacks.post
                .clear()
                .append(&basicBlockGraphvizNodeEnd);    /* end of GraphViz node */

            datablock_callbacks.pre
                .clear();
            datablock_callbacks.unparse
                .clear();
            datablock_callbacks.post
                .clear();

            staticdata_callbacks.pre
                .clear();
            staticdata_callbacks.unparse
                .clear();
            staticdata_callbacks.post
                .clear();

            function_callbacks.pre
                .clear();
            function_callbacks.post
                .clear()
                .append(&functionCleanup);
        }
    } unparser;

    std::string label_attr = function_label_attr(func);

    if (verbose) {
        sout <<"  subgraph clusterF" <<addrToString(func->get_entry_va()) <<" {\n"
             <<"    style=filled; color=gray90;\n"
             <<"    " <<label_attr.c_str() <<";\n";

        /* Write the node definitions (basic blocks of this function) */
        unparser.unparse(sout, func);

        /* Write the edge definitions for internal (intra-function) flow control. Fall-through edges are black,
         * non-fall-throughs are orange. We could have just as easily used Boost's depth_first_search(), but our nested loops
         * here allow us to short circuit the traversal and consider only the edges originating from blocks within this
         * function. */
        boost::graph_traits<rose::BinaryAnalysis::ControlFlow::Graph>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(global_cfg); vi!=vi_end; ++vi) {
            SgAsmBlock *src_block = get(boost::vertex_name, global_cfg, *vi);
            SgAsmFunction *src_func = src_block->get_enclosing_function();
            if (src_func==func) {
                rose_addr_t src_fallthrough_va = src_block->get_fallthrough_va();
                boost::graph_traits<rose::BinaryAnalysis::ControlFlow::Graph>::out_edge_iterator ei, ei_end;
                for (boost::tie(ei, ei_end)=out_edges(*vi, global_cfg); ei!=ei_end; ++ei) {
                    SgAsmBlock *dst_block = get(boost::vertex_name, global_cfg, target(*ei, global_cfg));
                    SgAsmFunction *dst_func = dst_block->get_enclosing_function();
                    if (src_func==dst_func) {
                        sout <<"    B" <<addrToString(src_block->get_address())
                             <<" -> B" <<addrToString(dst_block->get_address());
                        if (dst_block->get_address()!=src_fallthrough_va)
                            sout <<" [ color=orange ]"; /* black for fall-through; orange for other */
                        sout <<";\n";
                    }
                }
            }
        }
        sout <<" };\n"; /*subgraph*/
    } else {
        sout <<"B" <<addrToString(func->get_entry_va())
             <<" [ " <<label_attr <<", " <<function_url_attr(func) <<" ];\n";
    }
    return std::string("B") + addrToString(func->get_entry_va());
}

/* Create a graphvis *.dot file of the control-flow graph for the specified function, along with the call graph edges into and
 * out of the specified function. */
static void
dump_function_cfg(const std::string &fileprefix, SgAsmFunction *func,
                  rose::BinaryAnalysis::ControlFlow::Graph &global_cfg)
{
    using namespace StringUtility;

    char func_node_name[64];
    sprintf(func_node_name, "F%08"PRIx64, func->get_entry_va());
    mlog[TRACE] <<"dumping graphviz file for function " <<func_node_name <<"\n";
    FILE *out = fopen((fileprefix+"-"+func_node_name+".dot").c_str(), "w");
    ASSERT_not_null2(out, "cannot open file: " + fileprefix+"-"+func_node_name+".dot");
    std::stringstream sout;
    sout <<"digraph " <<func_node_name <<" {\n"
         <<"  node [ shape = box ];\n";

    std::string my_node = dump_function_node(sout, func, global_cfg, true);
    std::set<SgAsmFunction*> node_defined;
    node_defined.insert(func);

    /* Add nodes and edges for functions that this function calls. The edges each go from one of this function's basic blocks
     * to either the entry node of another function or to the address of a block which has not been disassembled. The nodes
     * for the former case are collapsed function nodes with names beginning with "F"; while the latter case nodes have names
     * beginning with "B" and are shaded pink for higher visibility. */
    boost::graph_traits<rose::BinaryAnalysis::ControlFlow::Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end)=vertices(global_cfg); vi!=vi_end; ++vi) {
        SgAsmBlock *src_block = get(boost::vertex_name, global_cfg, *vi);
        SgAsmFunction *src_func = src_block->get_enclosing_function();
        if (src_func==func) {
            boost::graph_traits<rose::BinaryAnalysis::ControlFlow::Graph>::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=out_edges(*vi, global_cfg); ei!=ei_end; ++ei) {
                SgAsmBlock *dst_block = get(boost::vertex_name, global_cfg, target(*ei, global_cfg));
                SgAsmFunction *dst_func = dst_block->get_enclosing_function();
                if (dst_block==dst_func->get_entry_block()) {
                    /* This is a function call edge of the CFG */
                    if (node_defined.find(dst_func)==node_defined.end()) {
                        dump_function_node(sout, dst_func, global_cfg, false);
                        node_defined.insert(dst_func);
                    }
                    sout <<"B" <<addrToString(src_block->get_address())
                         <<" -> B" <<addrToString(dst_block->get_address())
                         <<" [ color=blue ];\n";
                }
            }
        }
    }

#if 0
    /* Add nodes and edges for functions that call this function and the edge cardinality. */
    BinaryCG::CalleeMap::const_iterator callee_i = cg.callee_edges.find(func);
    if (callee_i!=cg.callee_edges.end()) {
        for (BinaryCG::CallFromEdges::const_iterator ei=callee_i->second.begin(); ei!=callee_i->second.end(); ++ei) {
            SgAsmFunction *src_func = ei->first;
            rose_addr_t src_addr = src_func->get_entry_va();
            if (node_defined.find(src_addr)==node_defined.end()) {
                dump_function_node(sout, src_func, cfg, false);
                node_defined.insert(src_addr);
            }
            sout <<"B" <<addrToString(src_addr)
                 <<" -> B" <<addrToString(func->get_entry_va())
                 <<" [ color=blue, label=\"" <<ei->second <<" call" <<(1==ei->second?"":"s") <<"\" ];\n";
        }
    }
#endif

    sout <<"}\n";
    fputs(sout.str().c_str(), out);
    fclose(out);
}

/* Create control flow graphs for each function, one per file.  Also creates a function call graph. */
static void
dump_CFG_CG(SgNode *ast)
{
    using namespace StringUtility;
    typedef rose::BinaryAnalysis::ControlFlow::Graph CFG;
    typedef rose::BinaryAnalysis::FunctionCall::Graph CG;

    /* Create the control flow graph, but exclude blocks that are part of the "unassigned blocks" function. Note that if the
     * "-rose:partitioner_search -unassigned" switch is passed to the disassembler then the unassigned blocks will already
     * have been pruned from the AST anyway. */
    struct UnassignedBlockFilter: public rose::BinaryAnalysis::ControlFlow::VertexFilter {
        bool operator()(rose::BinaryAnalysis::ControlFlow*, SgAsmNode *node) {
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(node);
            return !func || 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS);
        }
    } unassigned_block_filter;
    rose::BinaryAnalysis::ControlFlow cfg_analyzer;
    cfg_analyzer.set_vertex_filter(&unassigned_block_filter);
    CFG global_cfg = cfg_analyzer.build_block_cfg_from_ast<CFG>(ast);

    /* Get the base name for the output files. */
    SgFile *srcfile = SageInterface::getEnclosingNode<SgFile>(ast);
    std::string filename = srcfile ? srcfile->get_sourceFileNameWithoutPath() : "x";

    /* Generate a dot file for the function call graph. */
    std::stringstream sout;
    sout <<"digraph callgraph {\n"
         <<"node [ shape = box ];\n";
    CG cg = rose::BinaryAnalysis::FunctionCall().build_cg_from_cfg<CG>(global_cfg);
    {
        boost::graph_traits<CG>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(cg); vi!=vi_end; ++vi) {
            SgAsmFunction *func = get(boost::vertex_name, cg, *vi);
            dump_function_node(sout, func, global_cfg, false);
        }
    }
    {
        boost::graph_traits<CG>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end)=edges(cg); ei!=ei_end; ++ei) {
            SgAsmFunction *src_func = get(boost::vertex_name, cg, source(*ei, cg));
            SgAsmFunction *dst_func = get(boost::vertex_name, cg, target(*ei, cg));
            sout <<"B" <<addrToString(src_func->get_entry_va())
                 <<" -> B" <<addrToString(dst_func->get_entry_va())
                 <<" [label=\"0\" ];\n"; //FIXME: number of calls from src_func to dst_func
        }
    }
    sout <<"}\n";
    {
        FILE *out = fopen((filename + "-cg.dot").c_str(), "w");
        ASSERT_not_null2(out, "cannot open file: " + filename + "-cg.dot");
        fputs(sout.str().c_str(), out);
        fclose(out);
    }
    
    /* Generate a dot file for each function */
    std::vector<SgAsmFunction*> funcs = SageInterface::querySubTree<SgAsmFunction>(ast);
    for (std::vector<SgAsmFunction*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi) {
        if (0 == ((*fi)->get_reason() & SgAsmFunction::FUNC_LEFTOVERS))
            dump_function_cfg(filename, *fi, global_cfg);
    }
}

// A simple partitioner that creates a single function having all instructions.
class SimplePartitioner: public Partitioner {
protected:
    rose_addr_t entry_va;
    Function *function;

public:
    SimplePartitioner(rose_addr_t entry_va): entry_va(entry_va), function(NULL) {}

    virtual void pre_cfg(SgAsmInterpretation *interp) ROSE_OVERRIDE {
        function = add_function(entry_va, SgAsmFunction::FUNC_ENTRY_POINT);
    }

    virtual void post_cfg(SgAsmInterpretation *interp) ROSE_OVERRIDE {}

    // Organize instructions into basic blocks
    virtual void analyze_cfg(SgAsmBlock::Reason reason) ROSE_OVERRIDE {
        ASSERT_not_null(function);
        bool changed = true;
        for (size_t pass=0; changed; ++pass) {
            changed = false;
            for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
                rose_addr_t va = ii->first;
                Instruction *insn = ii->second;
                if (!insn->bblock) {
                    BasicBlock *bb = find_bb_starting(va);
                    ASSERT_not_null(bb);
                    append(function, bb, SgAsmBlock::BLK_USERDEF);
                    changed = true;
                }
            }
            if (pass>=100) {
                mlog[ERROR] <<"too many passes through simple partitioner\n";
                abort();
            }
        }
    }
};

static SgAsmBlock *
simple_partitioner(SgAsmInterpretation *interp, const Disassembler::InstructionMap &insns, MemoryMap *mmap=NULL)
{
    if (insns.empty())
        return NULL;
    rose_addr_t entry_va = insns.begin()->first;
    SimplePartitioner sp(entry_va);
    return sp.partition(interp, insns, mmap);
}

int
main(int argc, char *argv[]) 
{
    typedef Sawyer::CommandLine::Switch Switch;

    /*------------------------------------------------------------------------------------------------------------------------
     * Initialize ROSE and our own logging.  Our logging facility, "log", is tied into the librose logging facility so it
     * can be controlled by the same command-line switches that control ROSE.
     *------------------------------------------------------------------------------------------------------------------------*/
    rose::Diagnostics::initialize();                    // rose has to be initialize for the next line to work
    mlog = Sawyer::Message::Facility("tool", rose::Diagnostics::destination);
    rose::Diagnostics::mfacilities.insertAndAdjust(mlog);

    /*------------------------------------------------------------------------------------------------------------------------
     * Generic switches
     *------------------------------------------------------------------------------------------------------------------------*/
    Sawyer::CommandLine::SwitchGroup generic = CommandlineProcessing::genericSwitches();

    /*------------------------------------------------------------------------------------------------------------------------
     * Input switches
     *------------------------------------------------------------------------------------------------------------------------*/
    Sawyer::CommandLine::SwitchGroup input("Input switches");

    rose_addr_t rebase_va = (rose_addr_t)(-1);
    input.insert(Switch("base-va")
                 .argument("addr", Sawyer::CommandLine::nonNegativeIntegerParser(rebase_va))
                 .doc("Use the specified address as the base virtual address rather than the adress indicated in the "
                      "file header."));

    std::vector<std::string> library_paths;
    input.insert(Switch("link")
                 .argument("paths", Sawyer::CommandLine::listParser(Sawyer::CommandLine::anyParser(library_paths)))
                 .explosiveLists(true)
                 .doc("Specifies the directories in which to search for shared libraries.  If any directories are "
                      "specified, then the main executable will be linked with its required dynamic libraries "
                      "disassembling. This argument may appear more than once, or the individual paths may be separated "
                      "from one another by colons."));

    rose_addr_t anon_pages = 8;
    input.insert(Switch("omit-anon")
                 .argument("npages", Sawyer::CommandLine::nonNegativeIntegerParser(anon_pages))
                 .doc("If the memory being disassembled is anonymously mapped (contains all zero bytes), then the "
                      "disassembler might spend substantial time creating code that's a single, large block of "
                      "instructions.  This switch can be used to discard regions of memory that are anonymously mapped. "
                      "Any distinct region larger than the specified size in kilobytes (1024 multiplier) will be "
                      "unmapped before disassembly starts.  This filter does not remove neighboring regions which are "
                      "individually smaller than the limit but whose combined size is larger.  The default is to omit "
                      "anonymous regions larger than " + numberToString(anon_pages) + "kB. Setting @v{npages} to zero "
                      "prevents any pages from being unmapped."));

    std::string protection_string = "x";
    input.insert(Switch("protection")
                 .argument("rwx", Sawyer::CommandLine::anyParser(protection_string))
                 .doc("Normally the disassembler will only consider data in memory that has execute permission.  This "
                      "switch allows the disassembler to use a different set of protection bits, all of which must be "
                      "set on any memory which is being considered for disassembly.  The @v{rwx} argument is one or more "
                      "of the letters: r (read), w (write), or x (execute), or '-' (ignored). @v{rwx} may be the word "
                      "\"any\", which has the same effect as not supplying any letters."));

    std::string raw_spec;
    input.insert(Switch("raw")
                 .argument("entries", Sawyer::CommandLine::anyParser(raw_spec))
                 .doc("Indicates that the specified file(s) contains raw machine instructions rather than a binary "
                      "container such as ELF or PE.  The @v{entries} argument specifies virtual addresses where "
                      "disassembly will be attempted. It is a comma-separated list of addresses or address ranges. An "
                      "address range is a low and high address separated by a hyphen and the range includes both "
                      "endpoints.  However, if the range is followed by a slash and an increment then assembly will be "
                      "tried only at the low address and positive offsets from the low address in multiples of the "
                      "increment.\n\n"
                      "The non-switch, positional arguments are either the name of an index file that was created by "
                      "MemoryMap::dump() (see documentation for MemoryMap::load() for details about the file format), or "
                      "pairs of file names and virtual addresses where the file contents are to be mapped.  The virtual "
                      "addresses can be suffixed with the letters 'r' (read), 'w' (write), and/or 'x' (execute) to specify "
                      "mapping permissions other than the default read and execute permission.  By default, ROSE will "
                      "only disassemble instructions appearing in parts of the memory address space containing execute "
                      "permission."));

    std::vector<rose_addr_t> reserved_vals;
    input.insert(Switch("reserve")
                 .argument("addr/size",
                           Sawyer::CommandLine::listParser(Sawyer::CommandLine::nonNegativeIntegerParser(reserved_vals)))
                 .explosiveLists(true)
                 .doc("Reserve the indicated virtual address range before attempting to load the specimen.  The memory is "
                      "mapped with no access rights. This switch may appear multiple times to reserve discontiguous "
                      "regions of the virtual address space."));

    /*------------------------------------------------------------------------------------------------------------------------
     * Disassembly control switches
     *------------------------------------------------------------------------------------------------------------------------*/
    Sawyer::CommandLine::SwitchGroup ctrl("Disassembly control switches");
    
    DisassembleDriver do_disassemble = DDRIVE_PD;
    ctrl.insert(Switch("disassemble")
                .argument("how", Sawyer::CommandLine::enumParser(do_disassemble)
                          ->with("pd", DDRIVE_PD)
                          ->with("dp", DDRIVE_DP)
                          ->with("d", DDRIVE_D)
                          ->with("none", DDRIVE_NONE),
                          "dp")
                .doc("Determines the interrelationship between the disassembler and the paritioner. The @v{how} should "
                     "be one of the following: \"pd\" runs the partitioner as the master driving the disassembler; "
                     "\"dp\" runs the disassembler first and then hands the results to the partitioner; \"d\" runs only "
                     "the disassembler and places all instructions in a single function; \"none\" skips both the "
                     "disassembly and partitioner and is useful if all you want to do is parse the container."));

    bool do_dos = false;
    ctrl.insert(Switch("dos")
                .intrinsicValue(true, do_dos)
                .doc("Normally, when the disassembler is invoked on a Windows PE or related container file it will "
                     "ignore the DOS interpretation. This switch causes the disassembler to use the DOS interpretation "
                     "instead of the default PE interpretation. The @s{no-dos} switch disables this feature (i.e., uses "
                     "the PE header)."));
    ctrl.insert(Switch("no-dos")
                .key("dos")
                .intrinsicValue(false, do_dos)
                .hidden(true));

    std::string isa;
    ctrl.insert(Switch("isa")
                .argument("architecture", Sawyer::CommandLine::anyParser(isa))
                .doc("Specify an instruction set architecture in order to choose a disassembler.  If an ISA is "
                     "specified then it overrides the disassembler that would have been chosen based on the file format. "
                     "Use \"@s{isa}=list\" to get a list of valid disassembler names."));

    std::string disassembler_search_str;
    ctrl.insert(Switch("rose:disassembler_search")
                .longPrefix("-")                    // librose uses hyphens to introduce named switches
                .argument("how", Sawyer::CommandLine::anyParser(disassembler_search_str))
                .doc("The argument for this switch is passed along to the ROSE library. See @s{rose-help} for details. "
                     "The use of this switch may or may not have any effect depending on how this tool calls the "
                     "disassembler."));

    std::string partitioner_config;
    ctrl.insert(Switch("rose:partitioner_config")
                .longPrefix("-")                    // librose uses hyphens to introduce named switches
                .argument("config", Sawyer::CommandLine::anyParser(partitioner_config))
                .doc("The argument for this switch is passed along to the ROSE library.  See @s{rose-help} for details. "
                     "The use of this switch may or may not have any effect depending on how this tool calls the "
                     "disassembler."));

    std::string partitioner_search_str;
    ctrl.insert(Switch("rose:partitioner_search")
                .longPrefix("-")                    // librose uses hyphens to introduce named switches
                .argument("how", Sawyer::CommandLine::anyParser(partitioner_search_str))
                .doc("The argument for this switch is passed along to the ROSE library. See @s{rose-help} for details. "
                     "The use of this switch may or may not have any effect depending on how this tool calls the "
                     "partitioner."));

    /*------------------------------------------------------------------------------------------------------------------------
     * Output switches
     *------------------------------------------------------------------------------------------------------------------------*/
    Sawyer::CommandLine::SwitchGroup output("Output switches");

    bool do_ast_dot = false;
    output.insert(Switch("ast-dot")
                  .intrinsicValue(true, do_ast_dot)
                  .doc("Generate GraphViz dot files for the entire AST.  This switch is applicable only when the input "
                       "file is a container such as ELF or PE.  The default is to not generate an AST dot file. Disabled "
                       "by @s{no-ast-dot}."));
    output.insert(Switch("no-ast-dot")
                  .key("ast-dot")
                  .intrinsicValue(false, do_ast_dot)
                  .hidden(true));

    bool do_cfg_dot = false;
    output.insert(Switch("cfg-dot")
                  .intrinsicValue(true, do_cfg_dot)
                  .doc("Generate GraphViz dot file containing the control flow graph of each function.  These files will "
                       "be named \"x-FXXXXXXXX.dot\" where \"XXXXXXXX\" is a function entry address.  This switch also "
                       "generates a function call graph with the name \"x-cg.dot\". These files can be converted to HTML "
                       "with the generate_html script found in tests/roseTests/binaryTests. The default is to not generate "
                       "these dot files.  This feature is disabled with @s{no-cfg-dot}."));
    output.insert(Switch("no-cfg-dot")
                  .key("cfg-dot")
                  .intrinsicValue(false, do_cfg_dot)
                  .hidden(true));

    std::string do_generate_ipd;
    output.insert(Switch("ipd")
                  .argument("filename", Sawyer::CommandLine::anyParser(do_generate_ipd))
                  .doc("Generate an IPD file from the disassembly results.  The IPD file can be modified by hand and then "
                       "fed back into another disassembly with the \"-rose:partitioner_config @v{filename}\" switch.  The "
                       "@s{no-ipd} disables this feature."));
    output.insert(Switch("no-ipd")
                  .key("ipd")
                  .intrinsicValue(std::string(), do_generate_ipd)
                  .hidden(true));

    bool do_linear = false;
    output.insert(Switch("linear")
                  .intrinsicValue(true, do_linear)
                  .doc("Causes output to be organized by address rather than hierarchically.  The output will be more "
                       "like traditional disassemblers."));

    bool do_quiet = false;
    output.insert(Switch("quiet")
                  .intrinsicValue(true, do_quiet)
                  .doc("Suppresses the instruction listing that is normally emitted to the standard output stream.  The "
                       "default is to not suppress, which is also the result when the @s{no-quiet} switch is used."));
    output.insert(Switch("no-quiet")
                  .key("quiet")
                  .intrinsicValue(false, do_quiet)
                  .hidden(true));

    bool show_bad = false;
    output.insert(Switch("show-bad")
                  .intrinsicValue(true, show_bad)
                  .doc("Show details about why instructions at certain addresses could not be disassembled.  The default "
                       "is to not show these details.  The @s{no-show-bad} switch disables this feature."));
    output.insert(Switch("no-show-bad")
                  .key("show-bad")
                  .intrinsicValue(true, show_bad)
                  .hidden(true));

    bool do_show_coverage = false;
    output.insert(Switch("show-coverage")
                  .intrinsicValue(true, do_show_coverage)
                  .doc("Show what percent of disassembly memory map was actually disassembled.  The default is to not show "
                       "this information.  The @s{no-show-coverage} switch disables this feature."));
    output.insert(Switch("no-show-coverage")
                  .key("show-coverage")
                  .intrinsicValue(false, do_show_coverage)
                  .hidden(true));

    bool do_show_extents = false;
    output.insert(Switch("show-extents")
                  .intrinsicValue(true, do_show_extents)
                  .doc("Show detailed information about what parts of the file were accessed during container parsing "
                       "and disassembly.  The default is to not show these details.  The @s{no-show-extents} switch "
                       "disables this feature."));
    output.insert(Switch("no-show-extents")
                  .key("show-extents")
                  .intrinsicValue(false, do_show_extents)
                  .hidden(true));

    bool do_show_functions = false;
    output.insert(Switch("show-functions")
                  .intrinsicValue(true, do_show_functions)
                  .doc("Display a list of functions in tabular format.  The default is to not show this information. The "
                       "@s{no-show-functions} switch disables this feature."));
    output.insert(Switch("no-show-functions")
                  .key("show-functions")
                  .intrinsicValue(false, do_show_functions)
                  .hidden(true));

    bool do_show_hashes = false;
    output.insert(Switch("show-hashes")
                  .intrinsicValue(true, do_show_hashes)
                  .doc("Display SHA1 hashes for basic blocks and functions in the assembly listing. These hashes are "
                       "based on basic block semantics.  The default is to not show these hashes in the listing. "
                       "Regardless of this switch, the hashes still appear in the function listing (@s{show-functions}) "
                       "and the CFG dot files (@s{cfg-dot}) if they can be computed. The @s{no-show-hashes} switch "
                       "disables this feature."));
    output.insert(Switch("no-show-hashes")
                  .key("show-hashes")
                  .intrinsicValue(false, do_show_hashes)
                  .hidden(true));

    bool do_show_instruction_addresses = false;
    output.insert(Switch("show-instruction-addresses")
                  .intrinsicValue(true, do_show_instruction_addresses)
                  .doc("Produce a listing of instruction addresses.  Each line of output will contain three space-separated "
                       "items: the address interval for the instruction (address followed by \"+\" followed by size), the "
                       "address of the basic block to which the instruction belongs, and the address of the function to which "
                       "the basic block belongs.  If the basic block doesn't belong to a function then the string \"nil\" is "
                       "printed for the function address field.  This listing is disabled with the "
                       "@s{no-show-instruction-addresses} switch.  The default is to " +
                       std::string(do_show_instruction_addresses?"":"not ") + "show this information."));
    output.insert(Switch("no-show-instruction-addresses")
                  .key("show-instruction-addresses")
                  .intrinsicValue(false, do_show_instruction_addresses)
                  .hidden(true));

    SyscallMethod do_syscall_names = SYSCALL_LINUX32;
    output.insert(Switch("syscalls")
                    .argument("method", Sawyer::CommandLine::enumParser(do_syscall_names)
                              ->with("none", SYSCALL_NONE)
                              ->with("linux32", SYSCALL_LINUX32))
                    .doc("Specifies how system calls are to be named in the disassembly output. The value \"linux32\" uses "
                         "system call numbers and names for 32-bit Linux, while the value \"none\" means no attempt is made "
                         "to determine system call names."));

    /*------------------------------------------------------------------------------------------------------------------------
     * Miscellaneous switches
     *------------------------------------------------------------------------------------------------------------------------*/
    Sawyer::CommandLine::SwitchGroup misc("Miscellaneous switches");

    bool do_reassemble = false;
    misc.insert(Switch("reassemble")
                .intrinsicValue(true, do_reassemble)
                .doc("Assemble each disassembled instruction and compare the generated machine code with the bytes "
                     "originally disassembled.  This switch is intended mostly to check the consistency of the "
                     "disassembler with the assembler.  The default is to not reassemble. The @s{no-reassemble} switch "
                     "disables this feature."));
    misc.insert(Switch("no-reassemble")
                .key("reassemble")
                .intrinsicValue(false, do_reassemble)
                .hidden(true));

    bool do_rose_help = false;
    misc.insert(Switch("rose-help")
                .intrinsicValue(true, do_rose_help)
                .doc("Show the ROSE library-recognized command-line switch documentation and exit."));

    /*------------------------------------------------------------------------------------------------------------------------
     * Parse the command-line.
     *------------------------------------------------------------------------------------------------------------------------*/

    Sawyer::CommandLine::Parser cmdline_parser;
    cmdline_parser.errorStream(mlog[FATAL]);
    cmdline_parser.purpose("disassemble binary specimens");
    cmdline_parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    cmdline_parser.chapter(1, "ROSE Command-line Tools");
    cmdline_parser.doc("Synopsis",
                       "@prop{programName} [@v{tool_switches}] [-- @v{rose_switches}] @v{specimen}\n\n"
                       "@prop{programName} @s{raw}... [@v{tool_switches}] [-- @v{rose_switches}] @v{map_file}\n\n"
                       "@prop{programName} @s{raw}... [@v{tool_switches}] [-- @v{rose_switches}] @v{name_addr_pairs}...");
    cmdline_parser.doc("Description",
                       "Disassembles a binary specimen to produce a listing file.  If the \"--raw\" switch is supplied "
                       "then the positional command-line arguments are either the name of a single memory-map dump, or "
                       "pairs of file names and virtual addresses (see the @s{raw} switch for details).  When the "
                       "@s{raw} switch is not specified, the command-line argument is the name of a specimen of a "
                       "recognized file format such as ELF or PE.\n\n"
                       "Because librose does not (yet) describe its command-line in terms that can be exported easily "
                       "to command-line parsers, this tool requires that all tool-related switches appear before all "
                       "librose switches, and that if rose-related switches are specified they are preceded by a \"--\" "
                       "switch.  This helps detect misspelled switches that are intended for the tool; on the other "
                       "hand, ROSE's frontend function does very minimal checking; @b{don't expect an error if you mistype "
                       "a switch intended for frontend!}");
    cmdline_parser.doc("Options",
                       "These are the switches recognized by the @prop{programName} tool itself. Documentation for switches "
                       "that can be passed to ROSE's frontend() function, if it's even called by this tool, is available "
                       "from this tool's @s{rose-help} switch.");

    Sawyer::CommandLine::ParserResult cmdline = cmdline_parser
                                                .with(generic)
                                                .with(input)
                                                .with(ctrl)
                                                .with(output)
                                                .with(misc)
                                                .parse(argc, argv)
                                                .apply();

    // Some switches require some extra parsing...  We could have written specific parsers for these and called them from
    // switch declarations above, but it's easier to just copy the code that previously existed...

    if (do_rose_help) {
        std::vector<std::string> args;
        args.push_back(argv[0]);
        args.push_back("--help");
        frontend(args);
        exit(0);
    }

    bool do_link = !library_paths.empty();

    unsigned protection = MemoryMap::EXECUTABLE;
    if (!protection_string.empty()) {
        protection = 0;
        BOOST_FOREACH (char ch, protection_string) {
            switch (ch) {
                case 'r': protection |= MemoryMap::READABLE;  break;
                case 'w': protection |= MemoryMap::WRITABLE; break;
                case 'x': protection |= MemoryMap::EXECUTABLE;  break;
                case '-': break;
                default:
                    mlog[ERROR] <<"invalid --protection bit: " <<ch <<"\n";
                    exit(1);
            }
        }
    }
    
    bool do_rebase = rebase_va != (rose_addr_t)(-1);

    Disassembler::AddressSet raw_entries;
    if (!raw_spec.empty()) {
        std::vector<std::string> parts = StringUtility::split(',', raw_spec, (size_t)(-1), true);
        for (size_t i=0; i<parts.size(); ++i) {
            // each part is: LO[-HI[/INC]]
            char *rest;
            const char *s = parts[i].c_str();
            errno = 0;
            rose_addr_t lo = strtoull(s, &rest, 0);
            if (errno || rest==s) {
                mlog[ERROR] <<"malformed --raw specification: " <<parts[i] <<"\n";
                exit(1);
            }
            rose_addr_t hi = lo, inc = 1;
            if ('-'==*rest) {
                s = rest+1;
                errno = 0;
                hi = strtoull(s, &rest, 0);
                if (errno || rest==s || hi<lo) {
                    mlog[ERROR] <<"malformed --raw specification: " <<parts[i] <<"\n";
                    exit(1);
                }
                if ('/'==*rest) {
                    s = rest+1;
                    errno = 0;
                    inc = strtoull(s, &rest, 0);
                    if (errno || rest==s || 0==inc) {
                        mlog[ERROR] <<"malformed --raw specification: " <<parts[i] <<"\n";
                        exit(1);
                    }
                }
            }
            if (*rest) {
                mlog[ERROR] <<"malformed --raw specification: " <<parts[i] <<"\n";
                exit(1);
            }
            for (rose_addr_t va=lo; va<=hi; va+=inc)
                raw_entries.insert(va);
        }
    }

    AddressIntervalSet reserved;
    if (!reserved_vals.empty()) {
        if (reserved_vals.size() % 2) {
            mlog[ERROR] <<"expected an even number of arguments for '--reserved' switch(es)\n";
            exit(1);
        }
        for (size_t i=0; i<reserved_vals.size(); i+=2)
            reserved.insert(AddressInterval::baseSize(reserved_vals[i], reserved_vals[i+1]));
    }

    unsigned disassembler_search = Disassembler::SEARCH_DEFAULT;
    if (!disassembler_search_str.empty()) {
        try {
            disassembler_search = Disassembler::parse_switches(disassembler_search_str, disassembler_search);
        } catch (const Disassembler::Exception &e) {
            mlog[ERROR] <<"disassembler exception: " <<e <<"\n";
            exit(1);
        }
    }

    unsigned partitioner_search = SgAsmFunction::FUNC_DEFAULT;
    if (!partitioner_search_str.empty()) {
        try {
            partitioner_search = Partitioner::parse_switches(partitioner_search_str, partitioner_search);
        } catch (const Partitioner::Exception &e) {
            mlog[ERROR] <<"partitioner exception: " <<e <<"\n";
            exit(1);
        }
    }

    Disassembler *disassembler = NULL;
    if (!isa.empty()) {
        disassembler = Disassembler::lookup(isa);
        if (!disassembler) {
            mlog[ERROR] <<"invalid isa specified on command line: " <<isa <<"\n";
            exit(1);
        }
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Build the command-line for frontend().  Because librose doesn't describe its command-line with Sawyer::CommandLine,
     * we're somewhat restricted in how we operate.  All tool switches must appear before all frontend() switches, and we must
     * assume that all tool switches are spelled correctly (because we have no way to distinguish between a valid frontend()
     * switch and a misspelled tool switch).  FIXME[Robb P. Matzke 2014-04-28]
     *------------------------------------------------------------------------------------------------------------------------*/

    std::vector<std::string> args = cmdline.unreachedArgs();
    std::vector<std::string> frontend_args;
    frontend_args.push_back(argv[0]);
    frontend_args.push_back("-rose:binary");
    frontend_args.push_back("-rose:read_executable_file_format_only");
    if (!disassembler_search_str.empty()) {
        frontend_args.push_back("-rose:disassembler_search");
        frontend_args.push_back(disassembler_search_str);
    }
    if (!partitioner_config.empty()) {
        frontend_args.push_back("-rose:partitioner_config");
        frontend_args.push_back(partitioner_config);
    }
    if (!partitioner_search_str.empty()) {
        frontend_args.push_back("-rose:partitioner_search");
        frontend_args.push_back(partitioner_search_str);
    }
    size_t argno = 0;
    for (/*void*/; argno<args.size() && !args[argno].empty() && '-'==args[argno][0]; ++argno) {
        if (0==args[argno].compare("--")) {
            ++argno;
            break;
        } else if (argno+2 < args.size() && CommandlineProcessing::isOptionTakingThirdParameter(args[argno])) {
            frontend_args.push_back(args[argno++]);
            frontend_args.push_back(args[argno++]);
            frontend_args.push_back(args[argno]);
        } else if (argno+1 < args.size() && CommandlineProcessing::isOptionTakingSecondParameter(args[argno])) {
            frontend_args.push_back(args[argno++]);
            frontend_args.push_back(args[argno]);
        } else {
            frontend_args.push_back(args[argno]);
        }
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Everything that's left on the command-line is a non-switch program argument.
     *------------------------------------------------------------------------------------------------------------------------*/

    MemoryMap raw_map;
    size_t nposargs = 0;
    for (/*void*/; argno<args.size(); ++argno) {
        if (!raw_spec.empty()) {
            ++nposargs;
            std::string raw_filename = args[argno];
            if (boost::ends_with(raw_filename, ".index")) {
#if 1 // [Robb P. Matzke 2014-09-03]: no longer supported
                throw std::runtime_error("loading memory map dumps is not currently supported");
#else
                std::string basename = raw_filename.substr(0, raw_filename.size()-6);
                try {
                    raw_map.load(basename);
                } catch (const MemoryMap::Exception &e) {
                    mlog[ERROR] <<e <<"\n";
                    exit(1);
                }
#endif
            } else {
                /* The --raw command-line args come in pairs consisting of the file name containing the raw machine
                 * instructions and the virtual address where those instructions are mapped.  The virtual address can be
                 * suffixed with any combination of the characters 'r' (read), 'w' (write), and 'x' (execute). The default when
                 * no suffix is present is 'rx'. */
                if (++argno >= args.size()) {
                    mlog[ERROR] <<"virtual address required for raw buffer " <<raw_filename <<"\n";
                    exit(1);
                }
                const char *s = args[argno].c_str();
                char *suffix;
                errno = 0;
                rose_addr_t start_va = strtoull(s, &suffix, 0);
                if (suffix==s || errno) {
                    mlog[ERROR] <<"virtual address required for raw buffer " <<raw_filename <<"\n";
                    exit(1);
                }
                unsigned perm = 0;
                while (suffix && *suffix) {
                    switch (*suffix++) {
                        case 'r': perm |= MemoryMap::READABLE; break;
                        case 'w': perm |= MemoryMap::WRITABLE; break;
                        case 'x': perm |= MemoryMap::EXECUTABLE; break;
                        default: mlog[ERROR] <<"invalid map permissions: " <<(suffix-1) <<"\n"; exit(1);
                    }
                }
                std::string base_name = StringUtility::stripPathFromFileName(raw_filename);
                if (!perm) perm = MemoryMap::READABLE | MemoryMap::EXECUTABLE;
                size_t raw_file_size = raw_map.insertFile(raw_filename, start_va, false, base_name);
                unsigned raw_file_access = MemoryMap::READABLE | MemoryMap::EXECUTABLE;
                raw_map.at(start_va).limit(raw_file_size).changeAccess(raw_file_access, ~raw_file_access);
            }
        } else {
            nposargs++;
            frontend_args.push_back(args[argno]);
        }
    }

    if (0==nposargs && !do_rose_help) {
        mlog[ERROR] <<"incorrect usage; see --help for details.\n";
        exit(1);
    }
    if (do_rebase && !raw_entries.empty())
        mlog[WARN] <<"--base-va ignored in raw buffer mode\n";
    if (!reserved.isEmpty() && !raw_entries.empty())
        mlog[WARN] <<"--reserve ignored in raw buffer mode\n";

    /*------------------------------------------------------------------------------------------------------------------------
     * Parse, link, remap, relocate
     *------------------------------------------------------------------------------------------------------------------------*/

    SgProject *project = NULL;                  /* Project if not disassembling a raw buffer */
    SgAsmInterpretation *interp = NULL;         /* Interpretation to disassemble if not disassembling a raw buffer */
    if (raw_entries.empty()) {
        /* Choose a disassembler based on the SgAsmInterpretation that we're disassembling */
        project = frontend(frontend_args); /*parse container but do not disassemble yet*/
        std::vector<SgAsmInterpretation*> interps
            = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation);

        /* Use the last header if there's more than one. Windows files often have a DOS header first followed by another
         * header such as PE.  If the "--dos" command-line switch is present then use the first header instead. */
        ASSERT_forbid2(interps.empty(), "a binary specimen must have at least one SgAsmInterpretation");
        interp = do_dos ? interps.front() : interps.back();

        /* Clear the interpretation's memory map because frontend() may have already done the mapping. We want to re-do the
         * mapping here because we may want to see debugging output, etc. */
        MemoryMap *map = interp->get_map();
        if (map!=NULL) {
            map->clear();
        } else {
            interp->set_map(map = new MemoryMap);
        }

        /* Adjust the base VA for the primary file header if requested. */
        if (do_rebase) {
            const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
            ASSERT_require2(1==hdrs.size(), "rebasing only works when the specimen has one file header");
            hdrs[0]->set_base_va(rebase_va);
        }

        /* Reserve parts of the address space. */
        BOOST_FOREACH (const AddressInterval &interval, reserved.intervals())
            map->insert(interval, MemoryMap::Segment::anonymousInstance(interval.size(), 0, "reserved area"));

        /* Run the loader */
        BinaryLoader *loader = BinaryLoader::lookup(interp)->clone();
        try {
            if (do_link) {
                for (size_t i=0; i<library_paths.size(); ++i)
                    loader->add_directory(library_paths[i]);
                loader->link(interp);
            }
            loader->remap(interp);
            if (do_link || !reserved.isEmpty()) {
                BinaryLoader::FixupErrors errors;
                loader->fixup(interp, &errors);
                if (!errors.empty()) {
                    mlog[WARN] <<"encountered " <<plural(errors.size(), "relocation fixup errors") <<"\n";
                }
            }
        } catch (const BinaryLoader::Exception &e) {
            mlog[ERROR] <<"BinaryLoader exception: " <<e <<"\n";
            exit(1);
        }
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Choose a disassembler
     *------------------------------------------------------------------------------------------------------------------------*/

    if (disassembler) {
        // already chosen above
    } else if (!raw_entries.empty() && !do_rose_help) {
        /* We don't have any information about the architecture, so assume the ROSE defaults (i386) */
        disassembler = Disassembler::lookup(new SgAsmPEFileHeader(new SgAsmGenericFile()));
        ASSERT_not_null(disassembler);
    } else {
        disassembler = Disassembler::lookup(interp);
        if (!disassembler) {
            mlog[ERROR] <<"no suitable disassembler found for interpretation\n";
            exit(1);
        }
    }
    ASSERT_not_null(disassembler);
    disassembler = disassembler->clone();

    /*------------------------------------------------------------------------------------------------------------------------
     * Configure the disassembler and its partitioner.
     *------------------------------------------------------------------------------------------------------------------------*/

    /* Set the disassembler instruction searching hueristics from the "-rose:disassembler_search" switch. We saved these
     * above, but they're also available via SgFile::get_disassemblerSearchHeuristics() if we called frontend(). */
    disassembler->set_search(disassembler_search);
    disassembler->set_alignment(1);      /*alignment for SEARCH_WORDS (default is four)*/

    /* What kind of memory can be disassembled, as specified by the --protection switch. */
    disassembler->set_protection(protection);

    /* Build the instruction partitioner and initialize it based on the -rose:partitioner_search and
     * -rose:partitioner_confg switches.  Similar to the disassembler switches, these are also available via
     * SgFile::get_partitionerSearchHeuristics() and SgFile::get_partitionerConfigurationFileName() if we had called
     * frontend(). */
    Partitioner *partitioner = new Partitioner();
    partitioner->set_search(partitioner_search);
    if (!partitioner_config.empty()) {
        try {
            partitioner->load_config(partitioner_config);
        } catch (const Partitioner::IPDParser::Exception &e) {
            mlog[ERROR] <<e <<"\n";
            exit(1);
        }
    }

    /* Note that because we call a low-level disassembly function (disassembleBuffer) the partitioner isn't invoked
     * automatically. However, we set it here just to be thorough. */
    disassembler->set_partitioner(partitioner);

    /*------------------------------------------------------------------------------------------------------------------------
     * Decide what to disassemble.
     *------------------------------------------------------------------------------------------------------------------------*/

    /* Note that if we using an active partitioner that calls the disassembler whenever an instruction is needed, then there's
     * no need to populate a work list.  The partitioner's pre_cfg() method will do the same things we're doing here.  We make
     * a copy of the MemoryMap because we might want to modify some of the permissions for disassembling; the new copy shares
     * the data (but not meta-data) with the original MemoryMap. */
    MemoryMap map;
    Disassembler::AddressSet worklist;

    if (!raw_entries.empty()) {
         /* We computed the memory map when we processed command-line arguments. */
        map = raw_map;
        for (Disassembler::AddressSet::iterator i=raw_entries.begin(); i!=raw_entries.end(); i++) {
            worklist.insert(*i);
            partitioner->add_function(*i, SgAsmFunction::FUNC_ENTRY_POINT, "entry_function");
        }
    } else {
        ASSERT_not_null2(interp->get_map(), "SgAsmInterpretation must have a memory map by now");
        map = *interp->get_map();

        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            /* Seed disassembler work list with entry addresses */
            SgRVAList entry_rvalist = (*hi)->get_entry_rvas();
            for (size_t i=0; i<entry_rvalist.size(); i++) {
                rose_addr_t entry_va = (*hi)->get_base_va() + entry_rvalist[i].get_rva();
                worklist.insert(entry_va);
            }

            /* Seed disassembler work list with addresses of function symbols if desired */
            if (disassembler->get_search() & Disassembler::SEARCH_FUNCSYMS)
                disassembler->search_function_symbols(&worklist, &map, *hi);
        }
    }

    /* Should we filter away any anonymous regions? */
    if (anon_pages > 0)
        map.eraseZeros(anon_pages*1024);

    /* If we did dynamic linking, then mark the ".got.plt" section as read-only.  This makes the disassembler treat it as
     * constant data so that dynamically-linked function thunks get known successor information.  E.g., a thunk like this:
     *     abort@plt:
     *        jmp DWORD PTR ds:[0x080600f8<.got.plt+0x0c>]
     * will have as its successor, the address stored at .got.plt+12.  If .got.plt had been left as read/write, then the
     * disassembler must assume that the address at .got.plt+12 changes while the program runs, and therefore the successors of
     * the JMP instruction are unknown. */
    if (do_link) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
            SgAsmGenericSectionPtrList sections = (*hi)->get_sections_by_name(".got.plt");      // ELF
            SgAsmGenericSectionPtrList s2 = (*hi)->get_sections_by_name(".got");                // ELF
            SgAsmGenericSectionPtrList s3 = (*hi)->get_sections_by_name(".import");             // PE
            sections.insert(sections.end(), s2.begin(), s2.end());
            sections.insert(sections.end(), s3.begin(), s3.end());
            for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
                if ((*si)->is_mapped()) {
                    AddressInterval mapped_va = AddressInterval::baseSize((*si)->get_mapped_actual_va(),
                                                                          (*si)->get_mapped_size());
                    map.within(mapped_va).changeAccess(MemoryMap::READABLE, ~MemoryMap::READABLE);
                }
            }
        }
    }

    std::cout <<"using this memory map for disassembly:\n";
    map.dump(std::cout, "    ");

    /*------------------------------------------------------------------------------------------------------------------------
     * Run the disassembler and partitioner
     *------------------------------------------------------------------------------------------------------------------------*/
    SgAsmBlock *block = NULL;
    Disassembler::BadMap bad;
    Disassembler::InstructionMap insns;

    try {
        if (DDRIVE_PD==do_disassemble) {
            block = partitioner->partition(interp, disassembler, &map);
            insns = partitioner->get_instructions();
            bad = partitioner->get_disassembler_errors();
        } else if (DDRIVE_DP==do_disassemble || DDRIVE_D==do_disassemble) {
            insns = disassembler->disassembleBuffer(&map, worklist, NULL, &bad);
            if (DDRIVE_DP==do_disassemble) {
                block = partitioner->partition(interp, insns, &map);
            } else {
                block = simple_partitioner(interp, insns, &map);
            }
        }
    } catch (const Partitioner::Exception &e) {
        mlog[ERROR] <<"partitioner exception: " <<e <<"\n";
        exit(1);
    }

    /* Link instructions into AST if possible */
    if (interp && block) {
        interp->set_global_block(block);
        block->set_parent(interp);
    }

#if 1 /* TESTING NEW FEATURES [RPM 2011-05-23] */
    {
        struct CalculateDominance: public AstSimpleProcessing {
            rose::BinaryAnalysis::ControlFlow &cfg_analysis;
            rose::BinaryAnalysis::Dominance &dom_analysis;
            CalculateDominance(rose::BinaryAnalysis::ControlFlow &cfg_analysis,
                               rose::BinaryAnalysis::Dominance &dom_analysis)
                : cfg_analysis(cfg_analysis), dom_analysis(dom_analysis)
                {}
            void visit(SgNode *node) {
                using namespace rose::BinaryAnalysis;
                typedef ControlFlow::Graph CFG;
                typedef boost::graph_traits<CFG>::vertex_descriptor CFG_Vertex;
                SgAsmFunction *func = isSgAsmFunction(node);
                if (func && func->get_entry_block()) {
                    CFG cfg = cfg_analysis.build_block_cfg_from_ast<CFG>(func);
                    CFG_Vertex entry = 0; /* see build_block_cfg_from_ast() */
                    ASSERT_require(get(boost::vertex_name, cfg, entry) == func->get_entry_block());
                    Dominance::Graph dg = dom_analysis.build_idom_graph_from_cfg<Dominance::Graph>(cfg, entry);
                    dom_analysis.clear_ast(func);
                    dom_analysis.apply_to_ast(dg);
                }
            }
        };
        rose::BinaryAnalysis::ControlFlow cfg_analysis;
        rose::BinaryAnalysis::Dominance   dom_analysis;
        //dom_analysis.set_debug(stderr);
        CalculateDominance(cfg_analysis, dom_analysis).traverse(interp, preorder);
    }
#elif 0
    {
        /* Initializes the p_immediate_dominator data member of SgAsmBlock objects in the entire AST as follows:
         *     For each function
         *       1. compute the CFG over that function
         *       2. calculate the DG with that CFG
         *       3. apply the DG to the AST */
        rose::BinaryAnalysis::ControlFlow cfg_analysis;
        rose::BinaryAnalysis::Dominance dom_analysis;
        //dom_analysis.set_debug(stderr);
        std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(interp);
        for (size_t i=0; i<functions.size(); i++) {
            SgAsmFunction *func = functions[i];
            rose::BinaryAnalysis::ControlFlow::Graph cfg = cfg_analysis.build_graph(func);
            rose::BinaryAnalysis::ControlFlow::Vertex start = (rose::BinaryAnalysis::ControlFlow::Vertex)0;
            ASSERT_require(get(boost::vertex_name, cfg, start)==func->get_entry_block());
            rose::BinaryAnalysis::Dominance::Graph dg = dom_analysis.build_idom_graph(cfg, start);
            dom_analysis.clear_ast(func);
            dom_analysis.apply_to_ast(dg);
        }
    }
#elif 0
    {
        /* Initialize the p_immediate_dominator data member of SgAsmBlock objects in the entire AST as follows:
         *     1. Compute the CFG over the entire AST
         *     2. calculate the DG starting with the program entry point
         *     3. apply the DG to the AST */
        rose::BinaryAnalysis::ControlFlow cfg_analysis;
        rose::BinaryAnalysis::ControlFlow::Graph global_cfg = cfg_analysis.build_graph(interp);
        rose::BinaryAnalysis::Dominance dom_analysis;
        dom_analysis.set_debug(stderr);
        cfg_analysis.cache_vertex_descriptors(global_cfg);
        dom_analysis.clear_ast(interp);
        std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(interp);
        for (size_t i=0; i<functions.size(); i++) {
            SgAsmFunction *func = functions[i];
            if (func->get_reason() & SgAsmFunction::FUNC_ENTRY_POINT) {
                SgAsmBlock *block = func->get_entry_block();
                ASSERT_not_null(block);
                rose::BinaryAnalysis::ControlFlow::Vertex start = block->get_cached_vertex();
                ASSERT_require(get(boost::vertex_name, global_cfg, start)==block);
                rose::BinaryAnalysis::Dominance::Graph dg = dom_analysis.build_idom_graph(global_cfg, start);
                dom_analysis.apply_to_ast(dg);
            }
        }
    }
#endif

    /*------------------------------------------------------------------------------------------------------------------------
     * Show the results
     *------------------------------------------------------------------------------------------------------------------------*/
    mlog[INFO] <<"disassembled " <<plural(insns.size(), "instructions") <<" and " <<plural(bad.size(), "failures") <<"\n";
    if (!bad.empty()) {
        if (show_bad && mlog[INFO]) {
            for (Disassembler::BadMap::const_iterator bmi=bad.begin(); bmi!=bad.end(); ++bmi)
                mlog[INFO] <<"    " <<addrToString(bmi->first) <<": " <<bmi->second.what() <<"\n";
        } else {
            mlog[INFO] <<"use --show-bad to see errors\n";
        }
    }

    if (do_show_functions && block)
        std::cout <<ShowFunctions(block);

    if (!do_quiet && block) {
        typedef rose::BinaryAnalysis::ControlFlow::Graph CFG;
        CFG cfg = rose::BinaryAnalysis::ControlFlow().build_block_cfg_from_ast<CFG>(block);
        MyAsmUnparser unparser(do_show_hashes, do_syscall_names);
        unparser.set_registers(disassembler->get_registers());
        unparser.add_function_labels(block);
        unparser.set_organization(do_linear ? AsmUnparser::ORGANIZED_BY_ADDRESS : AsmUnparser::ORGANIZED_BY_AST);
        unparser.add_control_flow_graph(cfg);
        unparser.staticDataDisassembler.init(disassembler); // disassemble static data blocks
        fputs("\n\n", stdout);
        unparser.unparse(std::cout, block);
        fputs("\n\n", stdout);
    }

    /* Figure out what part of the memory mapping does not have instructions. We do this by getting the extents (in
     * virtual address space) for the memory map used by the disassembler, then subtracting out the bytes referred to by
     * each instruction.  We cannot just take the sum of the sizes of the sections minus the sum of the sizes of
     * instructions because (1) sections may overlap in the memory map and (2) instructions may overlap in the virtual
     * address space.
     *
     * We use the list of instructions from the SgAsmBlock produced by partitioning rather than the list of instructions
     * actually disassembled. The lists are the same unless the partitioner's SEARCH_LEFTOVERS bit is clear, in which case we
     * only consider instructions that are part of a function. Cleared with "-rose:partitioner_search -leftovers".
     *
     * We also calculate the "percentageCoverage", which is the percent of the bytes represented by instructions to the
     * total number of bytes represented in the disassembly memory map. Although we store it in the AST, we don't
     * actually use it anywhere else. */
    if ((do_show_extents || do_show_coverage) && block) {
        AddressIntervalSet extents_tmp(map);
        ExtentMap extents=toExtentMap(extents_tmp);
        size_t disassembled_map_size = extents.size();

        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(block, V_SgAsmInstruction);
        for (std::vector<SgAsmInstruction*>::iterator ii=insns.begin(); ii!=insns.end(); ++ii)
            extents.erase(Extent((*ii)->get_address(), (*ii)->get_size()));
        size_t unused = extents.size();
        if (do_show_extents && unused>0 && mlog[INFO]) {
            mlog[INFO] <<"These addresses (" <<plural(unused, "bytes") <<") do not contain instructions:\n";
            extents.dump_extents(mlog[INFO], "    ", "");
        }

        if (do_show_coverage && disassembled_map_size>0) {
            double disassembled_coverage = 100.0 * (disassembled_map_size - unused) / disassembled_map_size;
            if (interp) {
                interp->set_percentageCoverage(disassembled_coverage);
                interp->set_coverageComputed(true);
            }
            mlog[INFO] <<"Disassembled coverage: " <<disassembled_coverage <<" percent\n";
        }
    }

    if (!do_generate_ipd.empty()) {
        std::ofstream ipdfile(do_generate_ipd.c_str());
        Partitioner::IPDParser::unparse(ipdfile, block);
    }

    if (do_show_instruction_addresses) {
        struct: AstSimpleProcessing {
            void visit(SgNode *node) {
                if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
                    SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(insn);
                    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(bb);
                    ASSERT_not_null(bb);
                    std::cout <<addrToString(insn->get_address()) <<"+" <<insn->get_size()
                              <<"\t" <<addrToString(bb->get_address())
                              <<"\t" <<(func ? addrToString(func->get_entry_va()) : std::string("nil")) <<"\n";
                }
            }
        } t1;
        t1.traverse(block, preorder);
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Generate the *.dump file for debugging
     *------------------------------------------------------------------------------------------------------------------------*/

    /* Note that backend() also currently [2010-07-21] generates this *.dump file, but it does so after giving sections an
     * opportunity to reallocate themselves.   We want the dump to contain the original data, prior to any normalizations that
     * might occur, so we generate the dump here explicitly. */
    if (interp) {
        struct T1: public SgSimpleProcessing {
            void visit(SgNode *node) {
                SgAsmGenericFile *file = isSgAsmGenericFile(node);
                if (file)
                    file->dump_all(true, ".dump");
            }
        };
        mlog[INFO] <<"generating ASCII dump...\n";
        T1().traverse(project, preorder);
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Generate dot files
     *------------------------------------------------------------------------------------------------------------------------*/
    
    if (do_ast_dot && project) {
        mlog[INFO] <<"generating GraphViz dot files for the AST...\n";
        generateDOT(*project);
        //generateAstGraph(project, INT_MAX);
    }
        
    if (do_cfg_dot && block) {
        mlog[INFO] <<"generating GraphViz dot files for control flow graphs...\n";
        dump_CFG_CG(block);
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Test the assembler
     *------------------------------------------------------------------------------------------------------------------------*/

    if (do_reassemble) {
        size_t assembly_failures = 0;

        /* Choose an encoding that must match the encoding used originally by the disassembler. If such an encoding cannot
         * be found by the assembler then assembleOne() will throw an exception. */
        Assembler *asmb = NULL;
        if (interp) {
            asmb = Assembler::create(interp);
        } else {
            asmb = Assembler::create(new SgAsmPEFileHeader(new SgAsmGenericFile()));
        }
        ASSERT_not_null2(asmb, "no appropriate assembler found");
        asmb->set_encoding_type(Assembler::ET_MATCHES);

        for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            /* Attempt to encode the instruction silently since most attempts succeed and we only want to produce
             * diagnostics for failures.  If there's a failure, turn on diagnostics and do the same thing again. */
            SgAsmInstruction *insn = ii->second;
            SgUnsignedCharList bytes;
            try {
                bytes = asmb->assembleOne(insn);
            } catch(const Assembler::Exception &e) {
                assembly_failures++;
                if (show_bad) {
                    mlog[ERROR] <<"assembly failed at " + addrToString(insn->get_address()) <<": " <<e.what() <<"\n";
                    FILE *old_debug = asmb->get_debug();
                    asmb->set_debug(stderr);
                    try {
                        (void)asmb->assembleOne(insn);
                    } catch(...) {
                        /*void*/
                    }
                    asmb->set_debug(old_debug);
                }
            }
        }
        if (assembly_failures>0) {
            mlog[WARN] <<"reassembly failed for " <<plural(assembly_failures, "instructions")
                       <<(show_bad ? "" : " (use --show-bad to see details)") <<"\n";
        } else {
            mlog[INFO] <<"reassembly succeeded for all instructions.\n";
        }
        delete asmb;
        if (assembly_failures>0)
            exit(1);
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Final statistics
     *------------------------------------------------------------------------------------------------------------------------*/
    
    size_t solver_ncalls = rose::BinaryAnalysis::SMTSolver::get_class_stats().ncalls;
    if (solver_ncalls>0)
        mlog[INFO] <<"SMT solver was called " <<plural(solver_ncalls, "times") <<"\n";
    return 0;
}




#endif
