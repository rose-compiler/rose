/* Reads a binary file and disassembles according to command-line switches */

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "bincfg.h"

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
        printf("      H = insn sequence head    L = leftover blocks\n");
        printf("\n");
        printf("    Num  Low-Addr   End-Addr  Insns/Bytes   Reason      Kind   Name\n");
        printf("    --- ---------- ---------- ------------ --------- -------- --------------------------------\n");
        traverse(node, preorder);
        printf("    --- ---------- ---------- ------------ --------- -------- --------------------------------\n");
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

            /* Function name if known */
            if (defn->get_name()!="")
                printf(" %s", defn->get_name().c_str());
            fputc('\n', stdout);
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
        retval += std::string(buf) + "\"";
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
dump_function_node(FILE *out, SgAsmFunctionDeclaration *func, BinaryCFG &cfg, bool verbose) 
{
    char node_name[64];
    sprintf(node_name, "%08"PRIx64, func->get_entry_va());
    std::string label_attr = function_label_attr(func);

    if (verbose) {
        fprintf(out, "  subgraph clusterF%s {\n", node_name);
        fprintf(out, "    style=filled; color=gray95;\n");
        fprintf(out, "    %s;\n", label_attr.c_str());

        /* Write the node definitions (basic blocks of this function) */
        std::vector<SgAsmBlock*> bbs = SageInterface::querySubTree<SgAsmBlock>(func, V_SgAsmBlock);
        for (std::vector<SgAsmBlock*>::iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
            fprintf(out, "    B%08"PRIx64" [ label=<<table border=\"0\">", (*bbi)->get_address());
            const SgAsmStatementPtrList &stmts = (*bbi)->get_statementList();
            for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
                SgAsmInstruction *insn = isSgAsmInstruction(*si);
                if (insn!=NULL) {
                    char addr_str[64];
                    sprintf(addr_str, "0x%08"PRIx64": ", insn->get_address());
                    std::string s = std::string(addr_str) + unparseInstruction(insn).c_str();
                    for (size_t j=0; j<s.size(); j++) if ('\t'==s[j]) s[j]=' ';
                    fprintf(out, "<tr><td align=\"left\">%s</td></tr>", s.c_str());
                }
            }
            fprintf(out, "</table>>");
            if (!(*bbi)->get_complete_successors()) {
                SgAsmInstruction *last_insn = isSgAsmInstruction((*bbi)->get_statementList().back());
                if (isSgAsmx86Instruction(last_insn) && isSgAsmx86Instruction(last_insn)->get_kind()==x86_ret) {
                    fprintf(out, ", color=blue"); /*function return statement, not used as an unconditional branch*/
                } else {
                    fprintf(out, ", color=red"); /*red implies that we don't have complete information for successors*/
                }
            } else if ((*bbi)->get_address()==func->get_entry_va()) {
                fprintf(out, ", color=darkgreen");
            }
            fprintf(out, " ];\n");
        }

        /* Write the edge definitions for internal flow control */
        for (std::vector<SgAsmBlock*>::iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
            const SgAddressList &sucs = (*bbi)->get_cached_successors();
            for (SgAddressList::const_iterator si=sucs.begin(); si!=sucs.end(); ++si) {
                SgAsmBlock *target_block = cfg.block(*si);
                SgAsmFunctionDeclaration *target_func = target_block ?
                                                        isSgAsmFunctionDeclaration(target_block->get_parent()) : NULL;
                if (target_func==func)
                    fprintf(out, "    B%08"PRIx64" -> B%08"PRIx64";\n", (*bbi)->get_address(), *si);
            }
        }
        fprintf(out, "  };\n"); /*subgraph*/
    } else {
        fprintf(out, "B%s [ %s, %s ];\n", node_name, label_attr.c_str(), function_url_attr(func).c_str());
    }
    return std::string("B") + node_name;
}

/* Create a graphvis *.dot file of the control-flow graph for the specified function, along with the call graph edges into and
 * out of the specified function. */
static void
dump_function_cfg(const std::string &fileprefix, SgAsmFunctionDeclaration *func, BinaryCFG &cfg, BinaryCG &cg)
{
    char func_node_name[64];
    sprintf(func_node_name, "F%08"PRIx64, func->get_entry_va());
    fprintf(stderr, " %s", func_node_name);
    FILE *out = fopen((fileprefix+"-"+func_node_name+".dot").c_str(), "w");
    ROSE_ASSERT(out!=NULL);
    fprintf(out, "digraph %s {\n", func_node_name);
    fprintf(out, "  node [ shape = box ];\n");

    std::string my_node = dump_function_node(out, func, cfg, true);
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
                    dump_function_node(out, dst_func, cfg, false);
                } else {
                    /* Node is not present in the CFG, so print a "B" (block) node rather than an "F" (function) node. */
                    fprintf(out, "B%08"PRIx64" [ style=filled, color=lightpink ];\n", dst_addr);
                }
                node_defined.insert(dst_addr);
            }
            fprintf(out, "B%08"PRIx64" -> B%08"PRIx64";\n", src_bb->get_address(), dst_addr);
        }
    }

    /* Add nodes and edges for functions that call this function and the edge cardinality. */
    BinaryCG::CalleeMap::const_iterator callee_i = cg.callee_edges.find(func);
    if (callee_i!=cg.callee_edges.end()) {
        for (BinaryCG::CallFromEdges::const_iterator ei=callee_i->second.begin(); ei!=callee_i->second.end(); ++ei) {
            SgAsmFunctionDeclaration *src_func = ei->first;
            rose_addr_t src_addr = src_func->get_entry_va();
            if (node_defined.find(src_addr)==node_defined.end()) {
                dump_function_node(out, src_func, cfg, false);
                node_defined.insert(src_addr);
            }
            fprintf(out, "B%08"PRIx64" -> B%08"PRIx64" [ label=\"%zu call%s\" ];\n",
                    src_addr, func->get_entry_va(), ei->second, 1==ei->second?"":"s");
        }
    }

    fprintf(out, "}\n"); /*digraph*/
    fclose(out);
}

/* Create control flow graphs for each function, one per file.  Also creates a function call graph. */
static void
dump_CFG_CG(SgNode *ast)
{
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

    /* Generate a dot file for the function call graph */
    std::set<rose_addr_t> cg_defined_nodes;
    fprintf(stderr, "  generating: cg");
    FILE *out = fopen((filename+"-cg.dot").c_str(), "w");
    ROSE_ASSERT(out);
    fprintf(out, "digraph callgraph {\n");
    fprintf(out, "node [ shape = box ];\n");
    for (BinaryCG::CallerMap::const_iterator i1=cg.caller_edges.begin(); i1!=cg.caller_edges.end(); ++i1) {
        SgAsmFunctionDeclaration *caller = i1->first;
        for (BinaryCG::CallToEdges::const_iterator i2=i1->second.begin(); i2!=i1->second.end(); ++i2) {
            rose_addr_t callee = i2->second;
            dump_function_node(out, caller, cfg, false);
            if (cg_defined_nodes.find(callee)==cg_defined_nodes.end()) {
                cg_defined_nodes.insert(callee);
                SgAsmBlock *callee_bb = cfg.block(callee);
                SgAsmFunctionDeclaration *callee_func = callee_bb ? isSgAsmFunctionDeclaration(callee_bb->get_parent()) : NULL;
                if (callee_func) {
                    dump_function_node(out, callee_func, cfg, false);
                } else {
                    fprintf(out, "  B%08"PRIx64" [ style=filled, color=lightpink ];\n", callee);
                }
            }
            fprintf(out, "  B%08"PRIx64" -> B%08"PRIx64";\n", caller->get_entry_va(), callee);
        }
    }
    fprintf(out, "}\n");
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
        SgNode *file = interp;
        while (file && !isSgFile(file)) file = file->get_parent();
        ROSE_ASSERT(file);
        d->set_search(isSgFile(file)->get_disassemblerSearchHeuristics());

        /* Build the instruction partitioner and set its search heuristics based on the "-rose:partitioner_search" switch as
         * stored in the SgFile node containing this interpretation. */
        Partitioner *p = new Partitioner();
        if (do_debug_partitioner)
            p->set_debug(stderr);
        p->set_search(isSgFile(file)->get_partitionerSearchHeuristics());
        d->set_partitioner(p);

        /* Disassemble instructions, linking them into the interpretation */
        Disassembler::BadMap bad;
        d->disassemble(interp, NULL, &bad);
        if (do_show_functions)
            ShowFunctions().show(interp);
        if (!do_quiet) {
            fputs(unparseAsmInterpretation(interp).c_str(), stdout);
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

        /* Figure out what part of the memory mapping does not have instructions. */
        if (do_show_extents) {
            ExtentMap extents=interp->get_map()->va_extents();
            std::vector<SgNode*> insns = NodeQuery::querySubTree(interp, V_SgAsmInstruction);
            for (size_t j=0; j<insns.size(); j++) {
                SgAsmInstruction *insn = isSgAsmInstruction(insns[j]);
                extents.erase(insn->get_address(), insn->get_raw_bytes().size());
            }
            size_t unused = extents.size();
            if (unused>0) {
                printf("These addresses (%zu byte%s) do not contain instructions:\n", unused, 1==unused?"":"s");
                extents.dump_extents(stdout, "    ", NULL, 0);
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

    printf("running back end...\n");
    int ecode = backend(project);
    return ecode>0 ? ecode : exit_status;
}
