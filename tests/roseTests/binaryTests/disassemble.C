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

        /* Build the disassembler */
        Disassembler *d = Disassembler::create(interp);
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
            generateDOT(*project);
            generateAstGraph(project, INT_MAX);
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
