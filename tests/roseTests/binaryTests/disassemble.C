/* Reads an executable, library, archive, core dump, or raw buffer of machine instructions and disassembles according to
 * command-line switches.  Although ROSE does the disassembly and partitioning by default, this test disables that automation
 * and does everything the hard way.  This allows us more control over what happens and even allows us to disassemble raw
 * buffers of machine instructions.  This test also does a variety of analyses and Robb uses it as a general tool and staging
 * area for testing new features before they're added to ROSE. */

static const char *usage = "\
Synopsis:\n\
  %s [SWITCHES] CONTAINER_FILE\n\
  %s [SWITCHES] --raw=ENTRIES RAW_FILE ADDRESS ...\n\
  %s [SWITCHES] --raw=ENTRIES MEMORY_MAP.index\n\
\n\
Description:\n\
  Disassembles machine instructions from a container such as ELF or PE, or from\n\
  a raw buffer such as a memory dump.\n\
\n\
  --ast-dot\n\
  --no-ast-dot\n\
    Generate (or don't generate) GraphViz dot files for the entire AST. This\n\
    switch is applicable only when the input file is a container such as ELF or\n\
    PE.  The default is to not generate an AST dot file.\n\
\n\
  --cfg-dot\n\
  --no-cfg-dot\n\
    Generate (or don't generate) a GraphViz dot file containing the control flow\n\
    graph of each function.  These files will be named \"x-FXXXXXXXX.dot\" where\n\
    \"XXXXXXXX\" is a function entry address.  This switch also generates a\n\
    function call graph with the name \"x-cg.dot\". These files can be converted\n\
    to HTML with the generate_html script found in tests/roseTests/binaryTests.\n\
    The default is to not generate these dot files.\n\
\n\
  --debug-disassembler\n\
  --no-debug-disassembler\n\
    Causes the disassembler to spew (or not) diagnostics to standard error.\n\
    This is intended for ROSE developers. The default is to not spew.\n\
\n\
  --debug-loader\n\
  --no-debug-loader\n\
    Causes the loader/linker to spew (or not) diagnostics to standard error.\n\
    This is intended for ROSE developers. The default is to not spew.\n\
\n\
  --debug-partitioner\n\
  --no-debug-partitioner\n\
    Causes the instruction partitioner to spew (or not) diagnostics to standard\n\
    error.  This is intended for ROSE developers. The default is to not spew.\n\
\n\
  --debug\n\
  --no-debug\n\
    Convenience switch that turns on (or off) the --debug-disassembler,\n\
    --debug-loader, and --debug-partitioner switches.\n\
\n\
  --disassemble\n\
    Call the disassembler explicitly, using the instruction search flags\n\
    specified with the -rose:disassembler_search switch.  Without this\n\
    --disassemble switch, the disassembler is called by the instruction\n\
    partitioner whenever the partitioner needs an instruction. When the\n\
    partitioner drives the disassembly we might spend substantially less time\n\
    disassembling, but fail to discover functions that are never statically\n\
    called.\n\
\n\
  --dos\n\
  --no-dos\n\
    Normally, when the disassembler is invoked on a Windows PE or related\n\
    container file it will ignore the DOS interpretation. This switch causes\n\
    the disassembler to use (or not use) the DOS interpretation instead of the\n\
    PE interpretation.\n\
\n\
  --dot\n\
  --no-dot\n\
    Convenience switch that is equivalent to --ast-dot and --cfg-dot (or\n\
    --no-ast-dot and --no-cfg-dot).\n\
\n\
  --quiet\n\
  --no-quiet\n\
    Suppresses (or not) the instruction listing that is normally emitted to the\n\
    standard output stream.  The default is to not suppress.\n\
\n\
  --raw=ENTRIES\n\
    Indicates that the specified file(s) contains raw machine instructions\n\
    rather than a binary container such as ELF or PE.  The ENTRIES argument\n\
    is a comma-separated list of one or more virtual addresses that will be\n\
    used to seed the recursive disassembler and instruction partitioner. The\n\
    non-switch, positional arguments are either the name of an index file that\n\
    was created by MemoryMap::dump() (see documentation for MemoryMap::load()\n\
    for details about the file format), or pairs of file names and virtual\n\
    addresses where the file contents are to be mapped.  The virtual addresses\n\
    can be suffixed with the letters 'r' (read), 'w' (write), and/or 'x'\n\
    (execute) to specify mapping permissions other than the default read and\n\
    execute permission.  By default, ROSE will only disassemble instructions\n\
    appearing in parts of the memory address space containing execute\n\
    permission.\n\
\n\
  --reassemble\n\
  --no-reassemble\n\
    Assemble (or not) each disassembled instruction and compare the generated\n\
    machine code with the bytes originally disassembled.  This switch is\n\
    intended mostly to check the consistency of the disassembler with the\n\
    assembler.  The default is to not reassemble.\n\
\n\
  --show-bad\n\
  --no-show-bad\n\
    Show (or not) details about why instructions at certain addresses could not\n\
    be disassembled.  The default is to not show these details.\n\
\n\
  --show-coverage\n\
  --no-show-coverage\n\
    Show (or not) what percent of the disassembly memory map was actually\n\
    disassembled.  The default is to not show this information.\n\
\n\
  --show-extents\n\
  --no-show-extents\n\
    Show (or not) detailed information about what parts of the file were not\n\
    disassembled.  The default is to not show these details.\n\
\n\
  --show-functions\n\
  --no-show-functions\n\
    Display (or not) a list of functions in tabular format.  The default is to\n\
    not show this list.\n\
\n\
  --show-hashes\n\
  --no-show-hashes\n\
    Display (or not) SHA1 hashes for basic blocks and functions in the assembly\n\
    listing. These hashes are based on basic block semantics.  The default is\n\
    to not show these hashes in the listing. Regardless of this switch, the\n\
    hashes still appear in the function listing (--show-functions) and the\n\
    CFG dot files (--cfg-dot) if they can be computed.\n\
\n\
In addition to the above switches, this disassembler tool passes all other\n\
switches to the underlying ROSE library's frontend() function if that function\n\
is actually called.  Of particular note are the following. Documentation for\n\
these switches can be obtained by specifying the \"--rose-help\" switch.\n\
  -rose:disassembler_search FLAGS\n\
  -rose:partitioner_search FLAGS\n\
  -rose:partitioner_config IPD_FILE\n\
";

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <errno.h>
#include <inttypes.h>
#include <ostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "AsmUnparser.h"
#include "BinaryLoader.h"
#include "VirtualMachineSemantics.h"
#include "SMTSolver.h"
#include "bincfg.h"

/*FIXME: Rose cannot parse this file.*/
#ifndef CXX_IS_ROSE_ANALYSIS

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
    using namespace VirtualMachineSemantics;

    if (!blk || blk->get_statementList().empty() || !isSgAsmx86Instruction(blk->get_statementList().front())) {
        memset(digest, 0, 20);
        return false;
    }
    const SgAsmStatementPtrList &stmts = blk->get_statementList();

    typedef X86InstructionSemantics<Policy, ValueType> Semantics;
    Policy policy;
    policy.set_discard_popped_memory(true);
    Semantics semantics(policy);
    try {
        for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
            SgAsmx86Instruction *insn = isSgAsmx86Instruction(*si);
            ROSE_ASSERT(insn!=NULL);
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
    SgAsmx86Instruction *last_insn = isSgAsmx86Instruction(stmts.back());
    if (last_insn->get_kind()==x86_call || last_insn->get_kind()==x86_farcall) {
        VirtualMachineSemantics::RenameMap rmap;
        policy.writeMemory(x86_segreg_ss, policy.readGPR(x86_gpr_sp), policy.number<32>(0), policy.true_());
        ignore_final_ip = false;
    }

    /* Set original IP to a constant value so that hash is never dependent on the true original IP.  If the final IP doesn't
     * matter, then make it the same as the original so that the difference between the original and final does not include the
     * IP (SHA1 is calculated in terms of the difference). */
    policy.get_orig_state().ip = policy.number<32>(0);
    if (ignore_final_ip)
        policy.get_state().ip = policy.get_orig_state().ip;
    return policy.SHA1(digest);
}

/* Compute a hash value for a function. Return false if the hash cannot be computed. */
bool
function_hash(SgAsmFunctionDeclaration *func, unsigned char digest[20])
{
    memset(digest, 0, 20);
    std::set<std::string> seen;
    const SgAsmStatementPtrList &stmts = func->get_statementList();
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        SgAsmBlock *bb = isSgAsmBlock(*si);
        ROSE_ASSERT(bb!=NULL);
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

/* Traversal prints information about each SgAsmFunctionDeclaration node. */
class ShowFunctions : public SgSimpleProcessing {
public:
    size_t nfuncs;
    ShowFunctions()
        : nfuncs(0)
        {}
    void show(SgNode *node) {
        printf("Functions detected in this interpretation:\n");
        printf("    Key for reason(s) address is a suspected function:\n");
        printf("      E = entry address         C = call target           X = exception frame\n");
        printf("      S = function symbol       P = instruction pattern   G = interblock branch graph\n");
        printf("      U = user-def detection    N = NOP/Zero padding      D = discontiguous blocks\n");
        printf("      H = insn sequence head    I = imported/dyn-linked   L = leftover blocks\n");
        printf("\n");
        printf("    Num  Low-Addr   End-Addr  Insns/Bytes  Reason      Kind     Hash             Name\n");
        printf("    --- ---------- ---------- ------------ ----------- -------- ---------------- --------------------------------\n");
        traverse(node, preorder);
        printf("    --- ---------- ---------- ------------ ----------- -------- ---------------- --------------------------------\n");
    }
    void visit(SgNode *node) {
        SgAsmFunctionDeclaration *defn = isSgAsmFunctionDeclaration(node);
        if (defn) {
            /* Scan through the function's instructions to find the range of addresses for the function. */
            rose_addr_t func_start=~(rose_addr_t)0, func_end=0;
            size_t ninsns=0, nbytes=0;
            SgAsmStatementPtrList func_stmts = defn->get_statementList();
            for (size_t i=0; i<func_stmts.size(); i++) {
                SgAsmBlock *bb = isSgAsmBlock(func_stmts[i]);
                if (bb) {
                    SgAsmStatementPtrList block_stmts = bb->get_statementList();
                    for (size_t j=0; j<block_stmts.size(); j++) {
                        SgAsmInstruction *insn = isSgAsmInstruction(block_stmts[j]);
                        if (insn) {
                            ninsns++;
                            func_start = std::min(func_start, insn->get_address());
                            func_end = std::max(func_end, insn->get_address()+insn->get_raw_bytes().size());
                            nbytes += insn->get_raw_bytes().size();
                        }
                    }
                }
            }

            /* Reason that this is a function */
            printf("    %3zu 0x%08"PRIx64" 0x%08"PRIx64" %5zu/%-6zu ", ++nfuncs, func_start, func_end, ninsns, nbytes);
            fputs(defn->reason_str(true).c_str(), stdout);

            /* Kind of function */
            switch (defn->get_function_kind()) {
              case SgAsmFunctionDeclaration::e_unknown:    fputs("  unknown", stdout); break;
              case SgAsmFunctionDeclaration::e_standard:   fputs(" standard", stdout); break;
              case SgAsmFunctionDeclaration::e_library:    fputs("  library", stdout); break;
              case SgAsmFunctionDeclaration::e_imported:   fputs(" imported", stdout); break;
              case SgAsmFunctionDeclaration::e_thunk:      fputs("    thunk", stdout); break;
              default:                                     fputs("    other", stdout); break;
            }

            /* First 16 bytes of the function hash */
            unsigned char sha1[20];
            if (function_hash(defn, sha1)) {
                printf(" %-16s", digest_to_str(sha1).substr(0, 16).c_str());
            } else {
                printf(" %-16s", "");
            }
            
            /* Function name if known */
            if (defn->get_name()!="")
                printf(" %s", defn->get_name().c_str());
            fputc('\n', stdout);
        }
    }
};

/* Unparser that outputs some extra information */
class MyAsmUnparser: public AsmUnparser {
private:
    bool show_hashes;
public:
    MyAsmUnparser(bool show_hashes)
        : show_hashes(show_hashes) {
        blk_detect_noop_seq = true;
    }
    virtual void pre(std::ostream &o, SgAsmBlock *blk) {
        unsigned char sha1[20];
        if (show_hashes && block_hash(blk, sha1)) {
            o <<StringUtility::addrToString(blk->get_address()) 
              <<": " <<digest_to_str(sha1) <<"\n";
        }
    }
    virtual void pre(std::ostream &o, SgAsmFunctionDeclaration *func) {
        unsigned char sha1[20];
        if (show_hashes && function_hash(func, sha1)) {
            o <<StringUtility::addrToString(func->get_entry_va())
              <<": ============================ " <<digest_to_str(sha1) <<"\n";
        }
    }
};

/* Generate the "label" attribute for a function node in a *.dot file. */
static std::string
function_label_attr(SgAsmFunctionDeclaration *func)
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
function_url_attr(SgAsmFunctionDeclaration *func)
{
    char buf[64];
    sprintf(buf, "F%08"PRIx64, func->get_entry_va());
    return std::string("URL=\"") + buf + ".html\"";
}


        

/* Prints a graph node for a function. If @p verbose is true then the basic blocks of the funtion are displayed along with
 * control flow edges within the function. */
static std::string
dump_function_node(std::ostream &sout, SgAsmFunctionDeclaration *func, BinaryCFG &cfg, bool verbose) 
{
    using namespace StringUtility;

    struct Unparser: public AsmUnparser {
        std::set<std::string> semantics_seen;
        SgAsmFunctionDeclaration *cur_func;
        Unparser(): cur_func(NULL) {
            insn_show_bytes = false;
            insn_linefeed = false;
            blk_detect_noop_seq = true;
            blk_remove_noop_seq = false;
            blk_show_noop_warning = false;
            blk_show_successors = false;
            func_show_title = false;
            interp_show_title = false;
        }
        virtual void pre(std::ostream &o, SgAsmInstruction*) {
            o <<"<tr>"
              <<"<td align=\"left\"" <<(insn_is_noop_seq?" bgcolor=\"gray50\"":"") <<">";
        }
        virtual void post(std::ostream &o, SgAsmInstruction*) {
            o <<"</td></tr>";
        }
        virtual void pre(std::ostream &o, SgAsmBlock *b) {
            std::string semantics_color = "green";
            unsigned char sha1[20];
            block_hash(b, sha1);
            std::string sha1_str = digest_to_str(sha1);
            if (semantics_seen.find(sha1_str)!=semantics_seen.end()) {
                semantics_color = "orange";
            } else {
                semantics_seen.insert(sha1_str);
            }
            o <<"B" <<StringUtility::addrToString(b->get_address()) <<" [ label=<<table border=\"0\"";
            if (b->get_address()==cur_func->get_entry_va()) o <<" bgcolor=\"lightskyblue1\"";
            o <<"><tr><td align=\"left\" bgcolor=\"" <<semantics_color <<"\">" <<sha1_str <<"</td></tr>";
        }
        virtual void post(std::ostream &o, SgAsmBlock*b) {
            SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(b->get_parent());
            o <<"</table>>";
            if (!b->get_complete_successors()) {
                SgAsmInstruction *last_insn = isSgAsmInstruction(b->get_statementList().back());
                if (isSgAsmx86Instruction(last_insn) && isSgAsmx86Instruction(last_insn)->get_kind()==x86_ret) {
                    o <<", color=blue"; /*function return statement, not used as an unconditional branch*/
                } else {
                    o <<", color=red"; /*red implies that we don't have complete information for successors*/
                }
            } else if (func && b->get_address()==func->get_entry_va()) {
                o <<", color=blue"; /*function entry node*/
            }
            o <<" ];\n";
        }
        virtual void pre(std::ostream&, SgAsmFunctionDeclaration *func) {
            semantics_seen.clear();
            cur_func = func;
        }
    } unparser;

    std::string label_attr = function_label_attr(func);

    if (verbose) {
        sout <<"  subgraph clusterF" <<addrToString(func->get_entry_va()) <<" {\n"
             <<"    style=filled; color=gray90;\n"
             <<"    " <<label_attr.c_str() <<";\n";

        /* Write the node definitions (basic blocks of this function) */
        unparser.unparse(sout, func);

        /* Write the edge definitions for internal flow control. Fall-through edges are black, non-fall-throughs are orange. */
        std::vector<SgAsmBlock*> bbs = SageInterface::querySubTree<SgAsmBlock>(func, V_SgAsmBlock);
        for (std::vector<SgAsmBlock*>::iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
            const SgAddressList &sucs = (*bbi)->get_cached_successors();
            rose_addr_t fall_through_va = (*bbi)->get_fallthrough_va();
            for (SgAddressList::const_iterator si=sucs.begin(); si!=sucs.end(); ++si) {
                SgAsmBlock *target_block = cfg.block(*si);
                SgAsmFunctionDeclaration *target_func = target_block ?
                                                        isSgAsmFunctionDeclaration(target_block->get_parent()) : NULL;
                if (target_func==func) {
                    sout <<"    B" <<addrToString((*bbi)->get_address())
                         <<" -> B" <<addrToString(*si);
                    if (*si!=fall_through_va)
                        sout <<" [ color=orange ]"; /* black for fall-through; orange for other */
                    sout <<";\n";
                }
            }
        }
        sout <<"  };\n"; /*subgraph*/
    } else {
        sout <<"B" <<addrToString(func->get_entry_va())
             <<" [ " <<label_attr <<", " <<function_url_attr(func) <<" ];\n";
    }
    return std::string("B") + addrToString(func->get_entry_va());
}

/* Create a graphvis *.dot file of the control-flow graph for the specified function, along with the call graph edges into and
 * out of the specified function. */
static void
dump_function_cfg(const std::string &fileprefix, SgAsmFunctionDeclaration *func, BinaryCFG &cfg, BinaryCG &cg)
{
    using namespace StringUtility;

    char func_node_name[64];
    sprintf(func_node_name, "F%08"PRIx64, func->get_entry_va());
    fprintf(stderr, " %s", func_node_name);
    FILE *out = fopen((fileprefix+"-"+func_node_name+".dot").c_str(), "w");
    ROSE_ASSERT(out!=NULL);
    std::stringstream sout;
    sout <<"digraph " <<func_node_name <<" {\n"
         <<"  node [ shape = box ];\n";

    std::string my_node = dump_function_node(sout, func, cfg, true);
    Disassembler::AddressSet node_defined;      /* nodes (virtual addresses) that we've defined in this graph so far */
    node_defined.insert(func->get_entry_va());

    /* Add nodes and edges for functions that this function calls. The edges each go from one of this function's basic blocks
     * to either the entry node of another function or to the address of a block which has not been disassembled. The nodes
     * for the former case are collapsed function nodes with names beginning with "F"; while the latter case nodes have names
     * beginning with "B" and are shaded pink for higher visibility. */
    BinaryCG::CallerMap::const_iterator caller_i = cg.caller_edges.find(func);
    if (caller_i!=cg.caller_edges.end()) {
        for (BinaryCG::CallToEdges::const_iterator ei=caller_i->second.begin(); ei!=caller_i->second.end(); ++ei) {
            SgAsmBlock *src_bb = ei->first;
            rose_addr_t dst_addr = ei->second;
            if (node_defined.find(dst_addr)==node_defined.end()) {
                SgAsmBlock *dst_bb = cfg.block(dst_addr);
                SgAsmFunctionDeclaration *dst_func = dst_bb ? isSgAsmFunctionDeclaration(dst_bb->get_parent()) : NULL;
                if (dst_func) {
                    dump_function_node(sout, dst_func, cfg, false);
                } else {
                    /* Node is not present in the CFG, so print a "B" (block) node rather than an "F" (function) node. */
                    sout <<"B" <<addrToString(dst_addr) <<" [ style=filled, color=lightpink ];\n";
                }
                node_defined.insert(dst_addr);
            }
            sout <<"B" <<addrToString(src_bb->get_address()) <<" -> B" <<addrToString(dst_addr)
                 <<" [ color=blue ];\n";
        }
    }

    /* Add nodes and edges for functions that call this function and the edge cardinality. */
    BinaryCG::CalleeMap::const_iterator callee_i = cg.callee_edges.find(func);
    if (callee_i!=cg.callee_edges.end()) {
        for (BinaryCG::CallFromEdges::const_iterator ei=callee_i->second.begin(); ei!=callee_i->second.end(); ++ei) {
            SgAsmFunctionDeclaration *src_func = ei->first;
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

    sout <<"}\n";
    fputs(sout.str().c_str(), out);
    fclose(out);
}

/* Create control flow graphs for each function, one per file.  Also creates a function call graph. */
static void
dump_CFG_CG(SgNode *ast)
{
    using namespace StringUtility;

    std::vector<SgAsmFunctionDeclaration*> funcs = SageInterface::querySubTree<SgAsmFunctionDeclaration>
                                                   (ast, V_SgAsmFunctionDeclaration);

    /* Create the control flow graph, but exclude blocks that are part of the "unassigned blocks" function. Note that if the
     * "-rose:partitioner_search -unassigned" switch is passed to the disassembler then the unassigned blocks will already
     * have been pruned from the AST anyway. */
    BinaryCFG cfg(ast);
    for (std::vector<SgAsmFunctionDeclaration*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi) {
        if (0 != ((*fi)->get_reason() & SgAsmFunctionDeclaration::FUNC_LEFTOVERS))
            cfg.erase(*fi);
    }
    BinaryCG cg(cfg);

    /* Get the base name for the output files. */
    SgFile *srcfile = NULL;
    for (SgNode *n=ast; n && !srcfile; n=n->get_parent())
        srcfile = isSgFile(n);
    std::string filename = srcfile ? srcfile->get_sourceFileNameWithoutPath() : "x";

    /* Generate a dot file for the function call graph. This is a slight bit complex because the CG has edges from blocks to
     * functions but we need edges from functions to functions. Also, we want to annotate the edge with the number of calls. */
    std::set<rose_addr_t> cg_defined_nodes;
    fprintf(stderr, "  generating: cg");
    FILE *out = fopen((filename+"-cg.dot").c_str(), "w");
    ROSE_ASSERT(out);
    std::stringstream sout;
    sout <<"digraph callgraph {\n"
         <<"node [ shape = box ];\n";
    for (BinaryCG::CallerMap::const_iterator i1=cg.caller_edges.begin(); i1!=cg.caller_edges.end(); ++i1) {
        SgAsmFunctionDeclaration *caller = i1->first;
        if (cg_defined_nodes.find(caller->get_entry_va())==cg_defined_nodes.end()) {
            cg_defined_nodes.insert(caller->get_entry_va());
            dump_function_node(sout, caller, cfg, false);
        }
        typedef std::map<rose_addr_t/*callee_addr*/, size_t/*count*/> CalleeCounts;
        CalleeCounts callee_counts;
        for (BinaryCG::CallToEdges::const_iterator i2=i1->second.begin(); i2!=i1->second.end(); ++i2) {
            callee_counts[i2->second]++;
        }
        for (CalleeCounts::iterator cci=callee_counts.begin(); cci!=callee_counts.end(); ++cci) {
            rose_addr_t callee_addr = cci->first;
            if (cg_defined_nodes.find(callee_addr)==cg_defined_nodes.end()) {
                cg_defined_nodes.insert(callee_addr);
                SgAsmBlock *callee_bb = cfg.block(callee_addr);
                SgAsmFunctionDeclaration *callee_func = callee_bb ? isSgAsmFunctionDeclaration(callee_bb->get_parent()) : NULL;
                if (callee_func) {
                    dump_function_node(sout, callee_func, cfg, false);
                } else {
                    sout <<"  B" <<addrToString(callee_addr) <<" [ style=filled, color=lightpink ];\n";
                }
            }
            sout <<"  B" <<addrToString(caller->get_entry_va()) <<" -> B" <<addrToString(callee_addr)
                 <<" [ label=\"" <<cci->second <<"\" ];\n";
        }
    }
    sout <<"}\n";
    fputs(sout.str().c_str(), out);
    fclose(out);
    
    /* Generate a dot file for each function */
    for (std::vector<SgAsmFunctionDeclaration*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi) {
        if (0 == ((*fi)->get_reason() & SgAsmFunctionDeclaration::FUNC_LEFTOVERS))
            dump_function_cfg(filename, *fi, cfg, cg);
    }

    fprintf(stderr, "\n");
}

int
main(int argc, char *argv[]) 
{
    bool show_bad = false;
    bool do_debug_disassembler=false, do_debug_partitioner=false, do_debug_loader=false;
    bool do_reassemble = false;
    bool do_ast_dot = false;
    bool do_cfg_dot = false;
    bool do_quiet = false;
    bool do_dos = false;
    bool do_show_extents = false;
    bool do_show_coverage = false;
    bool do_show_functions = false;
    bool do_rose_help = false;
    bool do_call_disassembler = false;
    bool do_show_hashes = false;

    Disassembler::AddressSet raw_entries;
    MemoryMap raw_map;
    unsigned disassembler_search = Disassembler::SEARCH_DEFAULT;
    unsigned partitioner_search = SgAsmFunctionDeclaration::FUNC_DEFAULT;
    char *partitioner_config = NULL;

    /*------------------------------------------------------------------------------------------------------------------------
     * Parse and remove the command-line switches intended for this executable, but leave the switches we don't
     * understand so they can be handled by ROSE if frontend() is called.
     *------------------------------------------------------------------------------------------------------------------------*/
    char **new_argv = (char**)calloc(argc+2, sizeof(char*));
    int new_argc=0, nposargs=0;
    char *arg0 = strrchr(argv[0], '/') ? strrchr(argv[0], '/')+1 : argv[0];
    new_argv[new_argc++] = argv[0];
    new_argv[new_argc++] = strdup("-rose:read_executable_file_format_only");
    for (int i=1; i<argc; i++) {
        if (!strncmp(argv[i], "--search-", 9) || !strncmp(argv[i], "--no-search-", 12)) {
            fprintf(stderr, "%s: search-related switches have been moved into ROSE's -rose:disassembler_search switch\n", arg0);
            exit(1);
        } else if (!strcmp(argv[i], "--ast-dot")) {             /* generate GraphViz dot files for the AST */
            do_ast_dot = true;
        } else if (!strcmp(argv[i], "--no-ast-dot")) {
            do_ast_dot = false;
        } else if (!strcmp(argv[i], "--cfg-dot")) {             /* generate dot files for control flow graph of each function */
            do_cfg_dot = true;
        } else if (!strcmp(argv[i], "--no-cfg-dot")) {
            do_cfg_dot = false;
        } else if (!strcmp(argv[i], "--disassemble")) {         /* call disassembler explicitly; use a passive partitioner */
            do_call_disassembler = true;
        } else if (!strcmp(argv[i], "--dot")) {                 /* generate all dot files (backward compatibility switch) */
            do_ast_dot = true;
            do_cfg_dot = true;
        } else if (!strcmp(argv[i], "--no-dot")) {
            do_ast_dot = false;
            do_cfg_dot = false;
        } else if (!strcmp(argv[i], "--dos")) {                 /* use MS-DOS header in preference to PE when both exist */
            do_dos = true;
        } else if (!strcmp(argv[i], "--no-dos")) {
            do_dos = false;
        } else if (!strcmp(argv[i], "-?") ||
                   !strcmp(argv[i], "-help") ||
                   !strcmp(argv[i], "--help")) {
            printf(usage, arg0, arg0, arg0);
            exit(0);
        } else if (!strcmp(argv[i], "--rose-help")) {
            new_argv[new_argc++] = strdup("--help");
            do_rose_help = true;
        } else if (!strcmp(argv[i], "--skip-dos")) {
            fprintf(stderr, "%s: --skip-dos has been replaced by --no-dos, which is now the default.\n", arg0);
            do_dos = false;
        } else if (!strcmp(argv[i], "--show-bad")) {            /* show details about failed disassembly or assembly */
            show_bad = true;
        } else if (!strcmp(argv[i], "--no-show-bad")) {
            show_bad = false;
        } else if (!strcmp(argv[i], "--show-coverage")) {       /* show disassembly coverage */
            do_show_coverage = true;
        } else if (!strcmp(argv[i], "--no-show-coverage")) {
            do_show_coverage = false;
        } else if (!strcmp(argv[i], "--show-functions")) {      /* show function summary */
            do_show_functions = true;
        } else if (!strcmp(argv[i], "--no-show-functions")) {
            do_show_functions = false;
        } else if (!strcmp(argv[i], "--show-extents")) {        /* show parts of file that were not disassembled */
            do_show_extents = true;
        } else if (!strcmp(argv[i], "--no-show-extents")) {
            do_show_extents = false;
        } else if (!strcmp(argv[i], "--show-hashes")) {         /* show SHA1 hashes in assembly listing */
            do_show_hashes = true;
        } else if (!strcmp(argv[i], "--no-show-hashes")) {
            do_show_hashes = false;
        } else if (!strcmp(argv[i], "--reassemble")) {          /* reassemble in order to test the assembler */
            do_reassemble = true;
        } else if (!strcmp(argv[i], "--no-reassemble")) {
            do_reassemble = false;
        } else if (!strcmp(argv[i], "--raw") || !strncmp(argv[i], "--raw=", 6)) {
            char *s = !strncmp(argv[i], "--raw=", 6) ? argv[i]+6 : (i+1<argc ? argv[++i] : NULL);
            if (!s || !*s) {
                fprintf(stderr, "%s: raw entry address(es) expceted for --raw switch\n", arg0);
                exit(1);
            }
            while (*s) {
                char *rest;
                errno = 0;
                rose_addr_t raw_entry_va = strtoull(s, &rest, 0);
                if (rest==s || errno!=0) {
                    fprintf(stderr, "%s: raw entry address expected at: %s\n", arg0, s);
                    exit(0);
                }
                raw_entries.insert(raw_entry_va);
                if (','==*rest) rest++;
                while (isspace(*rest)) rest++;
                s = rest;
            }
        } else if (!strcmp(argv[i], "--debug")) {               /* dump lots of debugging information */
            do_debug_disassembler = true;
            do_debug_loader = true;
            do_debug_partitioner = true;
        } else if (!strcmp(argv[i], "--no-debug")) {
            do_debug_disassembler = false;
            do_debug_loader = false;
            do_debug_partitioner = false;
        } else if (!strcmp(argv[i], "--debug-disassembler")) {
            do_debug_disassembler = true;
        } else if (!strcmp(argv[i], "--no-debug-disassembler")) {
            do_debug_disassembler = false;
        } else if (!strcmp(argv[i], "--debug-loader")) {
            do_debug_loader = true;
        } else if (!strcmp(argv[i], "--no-debug-loader")) {
            do_debug_loader = false;
        } else if (!strcmp(argv[i], "--debug-partitioner")) {
            do_debug_partitioner = true;
        } else if (!strcmp(argv[i], "--no-debug-partitioner")) {
            do_debug_partitioner = false;
        } else if (!strcmp(argv[i], "--quiet")) {               /* do not emit instructions to stdout */
            do_quiet = true;
        } else if (!strcmp(argv[i], "--no-quiet")) {
            do_quiet = false;
        } else if (!strcmp(argv[i], "-rose:disassembler_search")) {
            /* Keep track of disassembler search flags because we need them even if we don't invoke frontend(), but
             * also pass them along to the frontend() call. */
            ROSE_ASSERT(i+1<argc);
            try {
                disassembler_search = Disassembler::parse_switches(argv[i+1], disassembler_search);
            } catch (const Disassembler::Exception &e) {
                std::cerr <<"disassembler exception: " <<e <<"\n";
                exit(1);
            }
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i];
        } else if (!strcmp(argv[i], "-rose:partitioner_search")) {
            /* Keep track of partitioner heuristics because we need them even if we don't invoke frontend(), but
             * also pass them along to the frontend() call. */
            ROSE_ASSERT(i+1<argc);
            try {
                partitioner_search = Partitioner::parse_switches(argv[i+1], partitioner_search);
            } catch (const Partitioner::Exception &e) {
                std::cerr <<"partitioner exception: " <<e <<"\n";
                exit(1);
            }
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i];
        } else if (!strcmp(argv[i], "-rose:partitioner_config")) {
            /* Keep track of partitioner configuration file name because we need it even if we don't invoke frontend(), but
             * also pass them along to the frontend() call. */
            ROSE_ASSERT(i+1<argc);
            partitioner_config = argv[i+1];
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i];
        } else if (i+2<argc && CommandlineProcessing::isOptionTakingThirdParameter(argv[i])) {
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i];
        } else if (i+1<argc && CommandlineProcessing::isOptionTakingSecondParameter(argv[i])) {
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i];
        } else if (argv[i][0]=='-') {
            new_argv[new_argc++] = argv[i];
        } else if (!raw_entries.empty()) {
            nposargs++;
            char *raw_filename = argv[i];
            char *extension = strrchr(raw_filename, '.');
            if (extension && !strcmp(extension, ".index")) {
                std::string basename(raw_filename, extension-raw_filename);
                try {
                    raw_map.load(basename);
                } catch (const MemoryMap::Exception &e) {
                    std::cerr <<e <<"\n";
                    exit(1);
                }
            } else {
                /* The --raw command-line args come in pairs consisting of the file name containing the raw machine instructions
                 * and the virtual address where those instructions are mapped.  The virtual address can be suffixed with any
                 * combination of the characters 'r' (read), 'w' (write), and 'x' (execute). The default when no suffix is present
                 * is 'rx'. */
                if (++i>=argc) {
                    fprintf(stderr, "%s: virtual address required for raw buffer %s\n", arg0, raw_filename);
                    exit(1);
                }
                char *suffix;
                errno = 0;
                rose_addr_t start_va = strtoull(argv[i], &suffix, 0);
                if (suffix==argv[i] || errno) {
                    fprintf(stderr, "%s: virtual address required for raw buffer %s\n", arg0, raw_filename);
                    exit(1);
                }
                unsigned perm = 0;
                while (suffix && *suffix) {
                    switch (*suffix++) {
                        case 'r': perm |= MemoryMap::MM_PROT_READ;  break;
                        case 'w': perm |= MemoryMap::MM_PROT_WRITE; break;
                        case 'x': perm |= MemoryMap::MM_PROT_EXEC;  break;
                        default: fprintf(stderr, "%s: invalid map permissions: %s\n", arg0, suffix-1); exit(1);
                    }
                }
                if (!perm) perm = MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC;
                int fd = open(raw_filename, O_RDONLY);
                if (fd<0) {
                    fprintf(stderr, "%s: cannot open %s: %s\n", arg0, raw_filename, strerror(errno));
                    exit(1);
                }
                struct stat sb;
                if (fstat(fd, &sb)<0) {
                    fprintf(stderr, "%s: cannot stat %s: %s\n", arg0, raw_filename, strerror(errno));
                    exit(1);
                }
                uint8_t *buffer = new uint8_t[sb.st_size];
                ssize_t nread = read(fd, buffer, sb.st_size);
                ROSE_ASSERT(nread==sb.st_size);
                close(fd);
                MemoryMap::MapElement melmt(start_va, sb.st_size, buffer, 0, perm);
                melmt.set_name(strrchr(raw_filename, '/')?strrchr(raw_filename, '/')+1:raw_filename);
                raw_map.insert(melmt);
            }
        } else {
            nposargs++;
            new_argv[new_argc++] = argv[i];
        }
    }
    if (0==nposargs) {
        fprintf(stderr, "%s: incorrect usage; see --help for details.\n", arg0);
        exit(1);
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Parse, link, remap, relocate
     *------------------------------------------------------------------------------------------------------------------------*/

    SgProject *project = NULL;                  /* Project if not disassembling a raw buffer */
    SgAsmInterpretation *interp = NULL;         /* Interpretation to disassemble if not disassembling a raw buffer */
    if (raw_entries.empty()) {
        /* Choose a disassembler based on the SgAsmInterpretation that we're disassembling */
        project = frontend(new_argc, new_argv); /*parse container but do not disassemble yet*/
        std::vector<SgAsmInterpretation*> interps
            = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation);

        /* Use the last header if there's more than one. Windows files often have a DOS header first followed by another
         * header such as PE.  If the "--dos" command-line switch is present then use the first header instead. */
        ROSE_ASSERT(!interps.empty());
        interp = do_dos ? interps.front() : interps.back();

        /* Clear the interpretation's memory map because frontend() may have already done the mapping. We want to re-do the
         * mapping here because we may want to see debugging output, etc. */
        if (interp->get_map()!=NULL)
            interp->get_map()->clear();

        BinaryLoader *loader = BinaryLoader::lookup(interp)->clone();
        if (do_debug_loader) loader->set_debug(stderr);
        //loader->set_perform_dynamic_linking(true);
        //loader->add_directory("/lib32");
        loader->load(interp);
    }


    /*------------------------------------------------------------------------------------------------------------------------
     * Choose a disassembler
     *------------------------------------------------------------------------------------------------------------------------*/

    Disassembler *disassembler = NULL;
    if (!raw_entries.empty() && !do_rose_help) {
        /* We don't have any information about the architecture, so assume the ROSE defaults (i386) */
        disassembler = Disassembler::lookup(new SgAsmPEFileHeader(new SgAsmGenericFile()))->clone();
    } else {
        disassembler = Disassembler::lookup(interp)->clone();
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Configure the disassembler and its partitioner.
     *------------------------------------------------------------------------------------------------------------------------*/

    /* Set the disassembler instruction searching hueristics from the "-rose:disassembler_search" switch. We saved these
     * above, but they're also available via SgFile::get_disassemblerSearchHeuristics() if we called frontend(). */
    disassembler->set_search(disassembler_search);
    disassembler->set_alignment(1);      /*alignment for SEARCH_WORDS (default is four)*/
    if (do_debug_disassembler)
        disassembler->set_debug(stderr);
    
    /* Build the instruction partitioner and initialize it based on the -rose:partitioner_search and
     * -rose:partitioner_confg switches.  Similar to the disassembler switches, these are also available via
     * SgFile::get_partitionerSearchHeuristics() and SgFile::get_partitionerConfigurationFileName() if we had called
     * frontend(). */
    Partitioner *partitioner = new Partitioner();
    partitioner->set_search(partitioner_search);
    if (do_debug_partitioner)
        partitioner->set_debug(stderr);
    if (partitioner_config) {
        try {
            partitioner->load_config(partitioner_config);
        } catch (const Partitioner::IPDParser::Exception &e) {
            std::cerr <<e <<"\n";
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
     * no need to populate a work list.  The partitioner's pre_cfg() method will do the same things we're doing here. */
    MemoryMap *map = NULL;
    Disassembler::AddressSet worklist;

    if (!raw_entries.empty()) {
         /* We computed the memory map when we processed command-line arguments. */
        map = &raw_map;
        for (Disassembler::AddressSet::iterator i=raw_entries.begin(); i!=raw_entries.end(); i++) {
            worklist.insert(*i);
            partitioner->add_function(*i, SgAsmFunctionDeclaration::FUNC_ENTRY_POINT, "entry_function");
        }
    } else {
        map = interp->get_map();
        assert(map!=NULL);


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
                disassembler->search_function_symbols(&worklist, map, *hi);
        }
    }

    /* If the partitioner needs to execute a success program (defined in an IPD file) then it must be able to provide the
     * program with a window into the specimen's memory.  We do that by supplying the same memory map that was used for
     * disassembly. It is redundant to call set_map() with an activer paritioner, but doesn't hurt anything. */
    partitioner->set_map(map);

    printf("using this memory map for disassembly:\n");
    map->dump(stdout, "    ");

    /*------------------------------------------------------------------------------------------------------------------------
     * Run the disassembler and partitioner
     *------------------------------------------------------------------------------------------------------------------------*/
    SgAsmBlock *block = NULL;
    Disassembler::BadMap bad;
    Disassembler::InstructionMap insns;

    try {
        if (do_call_disassembler) {
            insns = disassembler->disassembleBuffer(map, worklist, NULL, &bad);
            block = partitioner->partition(interp, insns);
        } else {
            block = partitioner->partition(interp, disassembler, map);
            insns = partitioner->get_instructions();
            bad = partitioner->get_disassembler_errors();
        }
    } catch (const Partitioner::Exception &e) {
        std::cerr <<"partitioner exception: " <<e <<"\n";
        exit(1);
    }

    /* Link instructions into AST if possible */
    if (interp) {
        interp->set_global_block(block);
        block->set_parent(interp);
    }
    
    /*------------------------------------------------------------------------------------------------------------------------
     * Show the results
     *------------------------------------------------------------------------------------------------------------------------*/

    printf("disassembled %zu instruction%s and %zu failure%s",
           insns.size(), 1==insns.size()?"":"s", bad.size(), 1==bad.size()?"":"s");
    if (!bad.empty()) {
        if (show_bad) {
            printf(":\n");
            for (Disassembler::BadMap::const_iterator bmi=bad.begin(); bmi!=bad.end(); ++bmi)
                printf("    0x%08"PRIx64": %s\n", bmi->first, bmi->second.mesg.c_str());
        } else {
            printf(" (use --show-bad to see errors)\n");
        }
    } else {
        printf("\n");
    }

    if (do_show_functions)
        ShowFunctions().show(block);

    if (!do_quiet) {
        MyAsmUnparser unparser(do_show_hashes);
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
    if (do_show_extents || do_show_coverage) {
        ExtentMap extents=map->va_extents();
        size_t disassembled_map_size = extents.size();

        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(block, V_SgAsmInstruction);
        for (std::vector<SgAsmInstruction*>::iterator ii=insns.begin(); ii!=insns.end(); ++ii)
            extents.erase((*ii)->get_address(), (*ii)->get_raw_bytes().size());
        size_t unused = extents.size();
        if (do_show_extents && unused>0) {
            printf("These addresses (%zu byte%s) do not contain instructions:\n", unused, 1==unused?"":"s");
            extents.dump_extents(stdout, "    ", NULL, 0);
        }

        if (do_show_coverage && disassembled_map_size>0) {
            double disassembled_coverage = 100.0 * (disassembled_map_size - unused) / disassembled_map_size;
            if (interp) {
                interp->set_percentageCoverage(disassembled_coverage);
                interp->set_coverageComputed(true);
            }
            printf("Disassembled coverage: %0.1f%%\n", disassembled_coverage);
        }
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
        printf("generating ASCII dump...\n");
        T1().traverse(project, preorder);
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Generate dot files
     *------------------------------------------------------------------------------------------------------------------------*/
    
    if (do_ast_dot && project) {
        printf("generating GraphViz dot files for the AST...\n");
        generateDOT(*project);
        //generateAstGraph(project, INT_MAX);
    }
        
    if (do_cfg_dot) {
        printf("generating GraphViz dot files for control flow graphs...\n");
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
        ROSE_ASSERT(asmb!=NULL);
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
                    fprintf(stderr, "assembly failed at 0x%08"PRIx64": %s\n", insn->get_address(), e.mesg.c_str());
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
            printf("reassembly failed for %zu instruction%s.%s\n",
                   assembly_failures, 1==assembly_failures?"":"s", 
                   show_bad ? "" : " (use --show-bad to see details)");
        } else {
            printf("reassembly succeeded for all instructions.\n");
        }
        delete asmb;
        if (assembly_failures>0)
            exit(1);
    }

    /*------------------------------------------------------------------------------------------------------------------------
     * Final statistics
     *------------------------------------------------------------------------------------------------------------------------*/
    
    if (SMTSolver::total_calls>0)
        printf("SMT solver was called %zu time%s\n", SMTSolver::total_calls, 1==SMTSolver::total_calls?"":"s");
    return 0;
}




#endif
