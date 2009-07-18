/* Reads a binary file and disassembles according to command-line switches */

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int
main(int argc, char *argv[]) 
{
    unsigned search = Disassembler::SEARCH_DEFAULT;
    bool show_bad = false;
    bool do_debug = false;
    bool do_reassemble = false;

    char **new_argv = (char**)calloc(argc+2, sizeof(char*));
    int new_argc=0;
    new_argv[new_argc++] = argv[0];
    new_argv[new_argc++] = "-rose:read_executable_file_format_only";
    for (int i=1; i<argc; i++) {
        if (!strcmp(argv[i], "--search-following")) {
            search |= Disassembler::SEARCH_FOLLOWING;
        } else if (!strcmp(argv[i], "--no-search-following")) {
            search &= ~Disassembler::SEARCH_FOLLOWING;
        } else if (!strcmp(argv[i], "--search-immediate")) {
            search |= Disassembler::SEARCH_IMMEDIATE;
        } else if (!strcmp(argv[i], "--no-search-immediate")) {
            search &= ~Disassembler::SEARCH_IMMEDIATE;
        } else if (!strcmp(argv[i], "--search-words")) {
            search |= Disassembler::SEARCH_WORDS;
        } else if (!strcmp(argv[i], "--no-search-words")) {
            search &= ~Disassembler::SEARCH_WORDS;
        } else if (!strcmp(argv[i], "--search-allbytes")) {
            search |= Disassembler::SEARCH_ALLBYTES;
        } else if (!strcmp(argv[i], "--no-search-allbytes")) {
            search &= ~Disassembler::SEARCH_ALLBYTES;
        } else if (!strcmp(argv[i], "--search-unused")) {
            search |= Disassembler::SEARCH_UNUSED;
        } else if (!strcmp(argv[i], "--no-search-unused")) {
            search &= ~Disassembler::SEARCH_UNUSED;
        } else if (!strcmp(argv[i], "--search-nonexe")) {
            search |= Disassembler::SEARCH_NONEXE;
        } else if (!strcmp(argv[i], "--no-search-nonexe")) {
            search &= ~Disassembler::SEARCH_NONEXE;
        } else if (!strcmp(argv[i], "--search-deadend")) {
            search |= Disassembler::SEARCH_DEADEND;
        } else if (!strcmp(argv[i], "--no-search-deadend")) {
            search &= ~Disassembler::SEARCH_DEADEND;
        } else if (!strcmp(argv[i], "--search-unknown")) {
            search |= Disassembler::SEARCH_UNKNOWN;
        } else if (!strcmp(argv[i], "--no-search-unknown")) {
            search &= ~Disassembler::SEARCH_UNKNOWN;
        } else if (!strcmp(argv[i], "--show-bad")) {
            show_bad = true;
        } else if (!strcmp(argv[i], "--reassemble")) {
            do_reassemble = true;
        } else if (!strcmp(argv[i], "--debug")) {
            do_debug = true;
        } else {
            new_argv[new_argc++] = argv[i];
        }
    }
    
    /* The -rose:read_executable_file_format_only causes a prominent warning to be displayed:
     *   >
     *   >WARNING: Skipping instruction disassembly
     *   >
     */
    SgProject *project = frontend(new_argc, new_argv);
    printf("No, please ignore the previous warning; Rest assured, we're doing disassembly!\n\n");


    /* Process each interpretation individually */
    std::vector<SgNode*> interps = NodeQuery::querySubTree(project, V_SgAsmInterpretation);
    assert(interps.size()>0);
    for (size_t i=0; i<interps.size(); i++) {
        SgAsmInterpretation *interp = isSgAsmInterpretation(interps[i]);

        /* Build the disassembler */
        Disassembler *d = Disassembler::create(interp);
        if (do_debug)
            d->set_debug(stderr);
        d->set_search(search);

        /* Disassemble instructions, linking them into the interpretation */
        Disassembler::BadMap bad;
        d->disassemble(interp, NULL, &bad);

        /* Results */
        printf("disassembled %zu instruction%s + %zu failure%s",
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


        /* Test assembler */
        if (do_reassemble) {
            AssemblerX86 ass;
            std::vector<SgNode*> insns = NodeQuery::querySubTree(project, V_SgAsmInstruction);
            printf("reassembling to check consistency...\n");
            for (size_t j=0; j<insns.size(); j++) {
                SgAsmInstruction *insn = isSgAsmInstruction(insns[j]);

                /* Choose an encoding that must match the encoding used originally by the disassembler. If such an encoding cannot
                 * be found by the assembler then assembleOne() will throw an exception. However, NOP instructions are used for
                 * padding and therefore are not always encoded like other instructions (the assembler doesn't try every single
                 * possible encoding), so we won't enforce a strict matching for them. */
                if (x86_nop==isSgAsmx86Instruction(insn)->get_kind()) {
                    ass.set_encoding_type(Assembler::ET_SHORTEST);
                } else {
                    ass.set_encoding_type(Assembler::ET_MATCHES);
                }

                /* Attempt to encode the instruction silently since most attempts succeed and we only want to produce
                 * diagnostics for failures.  If there's a failure, turn on diagnostics and do the same thing again. */
                SgUnsignedCharList bytes;
                try {
                    bytes = ass.assembleOne(insn);
                } catch(const Assembler::Exception &e) {
                    fprintf(stderr, "assembly failed at 0x%08"PRIx64": %s\n", insn->get_address(), e.mesg.c_str());
                    ass.set_debug(stderr);
                    (void)ass.assembleOne(insn);
                    ROSE_ASSERT(!"an exception should have been thrown!");
                }
            }
        }
    }

    printf("running back end...\n");
    return backend(project);
}
