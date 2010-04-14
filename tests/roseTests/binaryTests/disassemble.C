/* Reads a binary file and disassembles according to command-line switches */

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

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
        printf("      H = insn sequence head\n");
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


/* Extra info about a basic block */
struct BlockInfo {
    SgAsmFunctionDeclaration *func;             /* Function to which this block belongs. */
    std::vector<SgAsmInstruction*> insns;       /* Instruction in this basic block. */
    Disassembler::AddressSet successors;        /* List of all known successors. */
    bool complete;                              /* True if successors list is complete. */
};

/* Get information about every basic block */
class AllBlockInfo: public SgSimpleProcessing {
public:
    typedef std::map<rose_addr_t, BlockInfo> map_type;
    typedef map_type::iterator iterator;
    
    explicit AllBlockInfo(SgNode *ast) {
        traverse(ast, preorder);
    }

    /* Find all functions that call the specified function, including this function itself. */
    std::set<SgAsmFunctionDeclaration*> callers(SgAsmFunctionDeclaration *callee) {
        std::set<SgAsmFunctionDeclaration*> retval;
        for (iterator bi=bb_info.begin(); bi!=bb_info.end(); ++bi) {
            BlockInfo &src_block = bi->second;
            if (src_block.func!=NULL) {
                for (Disassembler::AddressSet::iterator si=src_block.successors.begin(); si!=src_block.successors.end(); ++si) {
                    iterator dst_i = bb_info.find(*si);
                    if (dst_i!=bb_info.end()) {
                        BlockInfo &dst_block = dst_i->second;
                        if (dst_block.func==callee && dst_block.insns.front()->get_address()==callee->get_entry_va())
                            retval.insert(src_block.func);
                    }
                }
            }
        }
        return retval;
    }

    BlockInfo & operator[](rose_addr_t block_va) {
        return bb_info[block_va];
    }

protected:
    void visit(SgNode *node) {
        SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
        if (func) {
            std::vector<SgAsmBlock*> bbs = SageInterface::querySubTree<SgAsmBlock>(func, V_SgAsmBlock);
            for (std::vector<SgAsmBlock*>::iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
                BlockInfo &block = bb_info[(*bbi)->get_address()];
                block.func = func;
                for (size_t i=0; i<(*bbi)->get_statementList().size(); i++) {
                    SgAsmInstruction *insn = isSgAsmInstruction((*bbi)->get_statementList()[i]);
                    if (insn) block.insns.push_back(insn);
                }
                block.successors = block.insns.front()->get_successors(block.insns, &(block.complete));
            }
        }
    }

private:
    map_type bb_info;
};

static std::string
function_label_attr(SgAsmFunctionDeclaration *func)
{
    std::string retval;
    if (func) {
        char addr_str[64];
        sprintf(addr_str, "F%08"PRIx64, func->get_entry_va());
        retval += std::string("label = \"") + addr_str;
        if (func->get_name().size()>0)
            retval += std::string(" <") + func->get_name() + ">";
        retval += std::string("\\n(") + func->reason_str(false) + ")\"";
    }
    return retval;
}

/** Prints a graph node for a function. If @p verbose is true then the basic blocks of the funtion are displayed along with
 *  control flow edges within the function. */
static std::string
dump_function_node(FILE *out, SgAsmFunctionDeclaration *func, AllBlockInfo &blocks, bool verbose) 
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
            BlockInfo &block = blocks[(*bbi)->get_address()];
            fprintf(out, "    B%08"PRIx64" [ label=<<table border=\"0\">", (*bbi)->get_address());
            for (size_t i=0; i<block.insns.size(); i++) {
                std::string s = unparseInstructionWithAddress(block.insns[i]).c_str();
                for (size_t j=0; j<s.size(); j++) if ('\t'==s[j]) s[j]=' ';
                fprintf(out, "<tr><td align=\"left\">%s</td></tr>", s.c_str());
            }
            fprintf(out, "</table>>");
            if (!block.complete) {
                if (isSgAsmx86Instruction(block.insns.back()) &&
                    isSgAsmx86Instruction(block.insns.back())->get_kind()==x86_ret) {
                    fprintf(out, ", color=blue"); /*function return statement, not used as an unconditional branch*/
                } else if (!block.complete) {
                    fprintf(out, ", color=red"); /*red implies that we don't have complete information for successors*/
                }
            }
            fprintf(out, " ];\n");
        }

        /* Write the edge definitions for internal flow control */
        for (std::vector<SgAsmBlock*>::iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
            BlockInfo &block = blocks[(*bbi)->get_address()];
            for (Disassembler::AddressSet::iterator si=block.successors.begin(); si!=block.successors.end(); ++si) {
                if (blocks[*si].func==func)
                    fprintf(out, "    B%08"PRIx64" -> B%08"PRIx64";\n", (*bbi)->get_address(), *si);
            }
        }
        fprintf(out, "  };\n"); /*subgraph*/
    } else {
        fprintf(out, "B%s [ %s ];\n", node_name, label_attr.c_str());
    }
    return std::string("B") + node_name;
}

/** Creates a GraphVis file (*.dot) containing the instructions and control flow information for a single function.  Control
 *  flowing into or our of this fuction from/to other functions are represented in the graph, although nodes for the other
 *  functions are simplified (containing only some summary information). */
static void
dump_function_cfg(SgAsmFunctionDeclaration *func, std::string filename, AllBlockInfo &blocks)
{
    char func_node_name[64];
    sprintf(func_node_name, "F%08"PRIx64, func->get_entry_va());
    filename += std::string("-") + func_node_name + ".dot";
    FILE *out = fopen(filename.c_str(), "w");
    ROSE_ASSERT(out!=NULL);
    fprintf(out, "digraph {\n");
    fprintf(out, "  node [ shape = box ];\n");

    std::map<SgAsmFunctionDeclaration*, std::string> fnodes;
    std::string my_node = dump_function_node(out, func, blocks, true);
    fnodes.insert(std::make_pair(func, my_node));

    /* Nodes for other functions that call this function */
    std::set<SgAsmFunctionDeclaration*> callers = blocks.callers(func);
    for (std::set<SgAsmFunctionDeclaration*>::iterator ci=callers.begin(); ci!=callers.end(); ++ci) {
        if (*ci!=func) {
            std::string caller_node = dump_function_node(out, *ci, blocks, false);
            fnodes.insert(std::make_pair(*ci, caller_node));
            fprintf(out, "  %s -> %s\n", caller_node.c_str(), my_node.c_str());
        }
    }

    /* Nodes for functions that this function calls */
    std::vector<SgAsmBlock*> bbs = SageInterface::querySubTree<SgAsmBlock>(func, V_SgAsmBlock);
    for (std::vector<SgAsmBlock*>::iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
        BlockInfo &src_block = blocks[(*bbi)->get_address()];
        for (Disassembler::AddressSet::iterator si=src_block.successors.begin(); si!=src_block.successors.end(); ++si) {
            BlockInfo &dst_block = blocks[*si];
            if (dst_block.func!=NULL && dst_block.func!=func) {
                if (fnodes.find(dst_block.func)==fnodes.end()) {
                    std::string callee_node = dump_function_node(out, dst_block.func, blocks, false);
                    fnodes.insert(std::make_pair(dst_block.func, callee_node));
                }
                fprintf(out, "  B%08"PRIx64" -> B%08"PRIx64";\n", (*bbi)->get_address(), *si);
            }
        }
    }

    fprintf(out, "}\n"); /*digraph*/
    fclose(out);
}

static void
dumpCFG(SgNode *ast)
{
    AllBlockInfo blocks(ast);

    std::vector<SgAsmFunctionDeclaration*> funcs = SageInterface::querySubTree<SgAsmFunctionDeclaration>
                                                   (ast, V_SgAsmFunctionDeclaration);
    for (std::vector<SgAsmFunctionDeclaration*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi)
        dump_function_cfg(*fi, "x", blocks);
}

int
main(int argc, char *argv[]) 
{
    bool show_bad = false;
    bool do_debug_disassembler = false, do_debug_partitioner=false;
    bool do_reassemble = false;
    bool do_dot = false;
    bool do_quiet = false;
    bool do_skip_dos = false;
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
        } if (!strcmp(argv[i], "--dot")) {
            do_dot = true;      /* generate dot files showing the AST */
        } else if (!strcmp(argv[i], "--skip-dos")) {
            do_skip_dos = true;
        } else if (!strcmp(argv[i], "--show-bad")) {
            show_bad = true;    /* show details about failed disassembly or assembly */
        } else if (!strcmp(argv[i], "--show-functions")) {
            do_show_functions = true; /*show function summary*/
        } else if (!strcmp(argv[i], "--reassemble")) {
            do_reassemble = true; /* reassemble what we disassembled in order to test the assembler */
        } else if (!strcmp(argv[i], "--debug")) {/* dump lots of debugging information */
            do_debug_disassembler = true;
            do_debug_partitioner = true;
        } else if (!strcmp(argv[i], "--debug-disassembler")) {
            do_debug_disassembler = true;
        } else if (!strcmp(argv[i], "--debug-partitioner")) {
            do_debug_partitioner = true;
        } else if (!strcmp(argv[i], "--quiet")) {
            do_quiet = true;    /* do not emit the instructions to stdout (they're still stored in the *.dump file) */
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

        /* Generate graph of the AST */
        if (do_dot) {
            printf("Generating DOT graphs...\n");
            //generateDOT(*project);
            //generateAstGraph(project, INT_MAX);
            dumpCFG(interp);
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
