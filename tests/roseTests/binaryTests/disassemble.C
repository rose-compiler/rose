/* Reads a binary file and disassembles according to command-line switches */

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <ostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "AsmUnparser.h"
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
    if (!blk || blk->get_statementList().empty() || !isSgAsmx86Instruction(blk->get_statementList().front())) {
        memset(digest, 0, 20);
        return false;
    }
    const SgAsmStatementPtrList &stmts = blk->get_statementList();

    typedef X86InstructionSemantics<VirtualMachineSemantics::Policy, VirtualMachineSemantics::ValueType> Semantics;
    VirtualMachineSemantics::Policy policy;
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
public:
    MyAsmUnparser() {
        blk_detect_noop_seq = true;
    }
    virtual void pre(std::ostream &o, SgAsmBlock *blk) {
        unsigned char sha1[20];
        if (block_hash(blk, sha1)) {
            o <<StringUtility::addrToString(blk->get_address()) 
              <<": " <<digest_to_str(sha1) <<"\n";
        }
    }
    virtual void pre(std::ostream &o, SgAsmFunctionDeclaration *func) {
        unsigned char sha1[20];
        if (function_hash(func, sha1)) {
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
    bool do_debug_disassembler = false, do_debug_partitioner=false;
    bool do_reassemble = false;
    bool do_ast_dot = false;
    bool do_cfg_dot = false;
    bool do_quiet = false;
    bool do_skip_dos = false;
    bool do_show_extents = false;
    bool do_show_coverage = false;
    bool do_show_functions = false;
    int exit_status = 0;

    /* Parse and remove the command-line switches intended for this executable, but leave the switches we don't
     * understand so they can be handled by ROSE's frontend(). */
    char **new_argv = (char**)calloc(argc+2, sizeof(char*));
    int new_argc=0;
    new_argv[new_argc++] = argv[0];
    new_argv[new_argc++] = strdup("-rose:read_executable_file_format_only");
    for (int i=1; i<argc; i++) {
        if (!strncmp(argv[i], "--search-", 9) || !strncmp(argv[i], "--no-search-", 12)) {
            fprintf(stderr, "%s: search-related switches have been moved into ROSE's -rose:disassembler_search switch\n", argv[0]);
            exit(1);
        } else if (!strcmp(argv[i], "--ast-dot")) {             /* generate GraphViz dot files for the AST */
            do_ast_dot = true;
        } else if (!strcmp(argv[i], "--cfg-dot")) {             /* generate dot files for control flow graph of each function */
            do_cfg_dot = true;
        } else if (!strcmp(argv[i], "--dot")) {                 /* generate all dot files (backward compatibility switch) */
            do_ast_dot = true;
            do_cfg_dot = true;
        } else if (!strcmp(argv[i], "--skip-dos")) {
            do_skip_dos = true;
        } else if (!strcmp(argv[i], "--show-bad")) {            /* show details about failed disassembly or assembly */
            show_bad = true;
        } else if (!strcmp(argv[i], "--show-coverage")) {       /* show disassembly coverage */
            do_show_coverage = true;
        } else if (!strcmp(argv[i], "--show-functions")) {      /* show function summary */
            do_show_functions = true;
        } else if (!strcmp(argv[i], "--show-extents")) {        /* show parts of file that were not disassembled */
            do_show_extents = true;
        } else if (!strcmp(argv[i], "--reassemble")) {          /* reassemble in order to test the assembler */
            do_reassemble = true;
        } else if (!strcmp(argv[i], "--debug")) {               /* dump lots of debugging information */
            do_debug_disassembler = true;
            do_debug_partitioner = true;
        } else if (!strcmp(argv[i], "--debug-disassembler")) {
            do_debug_disassembler = true;
        } else if (!strcmp(argv[i], "--debug-partitioner")) {
            do_debug_partitioner = true;
        } else if (!strcmp(argv[i], "--quiet")) {               /* do not emit instructions to stdout */
            do_quiet = true;
        } else if (i+2<argc && CommandlineProcessing::isOptionTakingThirdParameter(argv[i])) {
            printf("switch and args passed along to ROSE proper: %s %s %s\n", argv[i], argv[i+1], argv[i+2]);
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i];
        } else if (i+1<argc && CommandlineProcessing::isOptionTakingSecondParameter(argv[i])) {
            printf("switch and arg passed along to ROSE proper: %s %s\n", argv[i], argv[i+1]);
            new_argv[new_argc++] = argv[i++];
            new_argv[new_argc++] = argv[i];
        } else if (argv[i][0]=='-') {
            printf("switch passed along to ROSE proper: %s\n", argv[i]);
            new_argv[new_argc++] = argv[i];
        } else {
            new_argv[new_argc++] = argv[i];
        }
    }

    /* Parse container but do not disassemble anything. */
    SgProject *project = frontend(new_argc, new_argv);

    /* Process each interpretation individually */
    std::vector<SgNode*> interps = NodeQuery::querySubTree(project, V_SgAsmInterpretation);
    assert(interps.size()>0);
    for (size_t i=0; i<interps.size(); i++) {
        SgAsmInterpretation *interp = isSgAsmInterpretation(interps[i]);
        SgFile *file = SageInterface::getEnclosingFileNode(interp);
        ROSE_ASSERT(file);

        /* Should we skip this interpretation? */
        if (do_skip_dos) {
            bool is_dos = false;
            const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
            for (size_t j=0; j<headers.size() && !is_dos; j++) {
                if (isSgAsmDOSFileHeader(headers[j])) {
                    is_dos = true;
                }
            }
            if (is_dos)
                continue;
        }

        /* Get a copy of the disassembler so we can modify it locally. */
        Disassembler *d = Disassembler::lookup(interp)->clone();
        if (do_debug_disassembler)
            d->set_debug(stderr);

        /* Set the disassembler instruction searching heuristics from the "-rose:disassembler_search" switch as stored
         * in the SgFile node containing this interpretation. */
        d->set_search(file->get_disassemblerSearchHeuristics());

        /* Build the instruction partitioner and initialize it based on the -rose:partitioner_search and
         * -rose:partitioner_confg switches as stored in the SgFile node containing this interpretation. */
        Partitioner *p = new Partitioner();
        if (do_debug_partitioner)
            p->set_debug(stderr);
        p->set_search(file->get_partitionerSearchHeuristics());
        p->set_config(file->get_partitionerConfigurationFileName());
        d->set_partitioner(p);

        /* Disassemble instructions, linking them into the interpretation. Passing the BadMap as the third argument of
         * disassemble() causes the disassembler to not throw exceptions. However, the partitioner might still throw an
         * exception if it cannot parse the configuration file. */
        Disassembler::BadMap bad;
        try {
            d->disassemble(interp, NULL, &bad);
        } catch (const Partitioner::IPDParser::Exception &e) {
            std::cerr <<e <<"\n";
            exit(1);
        }

        /* Show disassembly if requested. */
        if (do_show_functions)
            ShowFunctions().show(interp);
        if (!do_quiet) {
            MyAsmUnparser unparser;
            unparser.unparse(std::cout, interp);
            fputs("\n\n", stdout);
        }

        /* Results */
        printf("disassembled %zu instruction%s + %zu failure%s for this interpretation",
               d->get_ndisassembled(), 1==d->get_ndisassembled()?"":"s", bad.size(), 1==bad.size()?"":"s");
        if (bad.size()>0) {
            if (show_bad) {
                printf(":\n");
                for (Disassembler::BadMap::iterator bmi=bad.begin(); bmi!=bad.end(); ++bmi) {
                    printf("    0x%08"PRIx64": %s\n", bmi->first, bmi->second.mesg.c_str());
                }
            } else {
                printf(" (use --show-bad to see errors)\n");
            }
        } else {
            printf("\n");
        }
        printf("used this memory map:\n");
        interp->get_map()->dump(stdout, "    ");

        /* Figure out what part of the memory mapping does not have instructions. We do this by getting the extents (in
         * virtual address space) for the memory map used by the disassembler, then subtracting out the bytes referred to by
         * each instruction.  We cannot just take the sum of the sizes of the sections minus the sum of the sizes of
         * instructions because (1) sections may overlap in the memory map and (2) instructions may overlap in the virtual
         * address space.
         *
         * We also calculate the "percentageCoverage", which is the percent of the bytes represented by instructions to the
         * total number of bytes represented in the disassembly memory map. Although this is stored in the AST, we don't
         * actually use it anywhere. */
        if (do_show_extents || do_show_coverage) {
            ExtentMap extents=interp->get_map()->va_extents();
            size_t disassembled_map_size = extents.size();

            std::vector<SgNode*> insns = NodeQuery::querySubTree(interp, V_SgAsmInstruction);
            for (size_t j=0; j<insns.size(); j++) {
                SgAsmInstruction *insn = isSgAsmInstruction(insns[j]);
                extents.erase(insn->get_address(), insn->get_raw_bytes().size());
            }
            size_t unused = extents.size();
            if (do_show_extents && unused>0) {
                printf("These addresses (%zu byte%s) do not contain instructions:\n", unused, 1==unused?"":"s");
                extents.dump_extents(stdout, "    ", NULL, 0);
            }

            if (do_show_coverage && disassembled_map_size>0) {
                double disassembled_coverage = 100.0 * (disassembled_map_size - unused) / disassembled_map_size;
                interp->set_percentageCoverage(disassembled_coverage);
                interp->set_coverageComputed(true);
                printf("Disassembled coverage: %0.1f%%\n", disassembled_coverage);
            }
        }

        /* Generate dot files */
        if (do_ast_dot) {
            printf("Generating GraphViz dot files for the AST...\n");
            generateDOT(*project);
            //generateAstGraph(project, INT_MAX);
        }
        if (do_cfg_dot) {
            printf("Generating GraphViz dot files for control flow graphs...\n");
            dump_CFG_CG(interp);
        }

        /* Test assembler */
        if (do_reassemble) {
            size_t assembly_failures = 0;

            /* Choose an encoding that must match the encoding used originally by the disassembler. If such an encoding cannot
             * be found by the assembler then assembleOne() will throw an exception. */
            Assembler *asmb = Assembler::create(interp);
            asmb->set_encoding_type(Assembler::ET_MATCHES);

            std::vector<SgNode*> insns = NodeQuery::querySubTree(interp, V_SgAsmInstruction);
            printf("reassembling to check consistency...\n");
            for (size_t j=0; j<insns.size(); j++) {
                /* Attempt to encode the instruction silently since most attempts succeed and we only want to produce
                 * diagnostics for failures.  If there's a failure, turn on diagnostics and do the same thing again. */
                SgAsmInstruction *insn = isSgAsmInstruction(insns[j]);
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
                exit_status = 1;
                printf("reassembly failed for %zu instruction%s.%s\n",
                       assembly_failures, 1==assembly_failures?"":"s", 
                       show_bad ? "" : " (use --show-bad to see details)");
            } else {
                printf("reassembly succeeded for all instructions.\n");
            }
            delete asmb;
        }
        delete d;
    }

    if (SMTSolver::total_calls>0)
        printf("SMT solver was called %zu time%s\n", SMTSolver::total_calls, 1==SMTSolver::total_calls?"":"s");

    /* Generate a *.dump file in the current directory. Note that backend() also currently [2010-07-21] generates this *.dump
     * file, but it does so after giving sections an opportunity to reallocate themselves.   We want the dump to contain the
     * original data, prior to any normalizations that might occur, so we generate the dump here explicitly. */
    struct T1: public SgSimpleProcessing {
        void visit(SgNode *node) {
            SgAsmGenericFile *file = isSgAsmGenericFile(node);
            if (file)
                file->dump_all(true, ".dump");
        }
    };
    printf("generating ASCII dump...\n");
    T1().traverse(project, preorder);

#if 0
    printf("running back end...\n");
    int ecode = backend(project);
    return ecode>0 ? ecode : exit_status;
#endif

    return 0;
}

#endif
