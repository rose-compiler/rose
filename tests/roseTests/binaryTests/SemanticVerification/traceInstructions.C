/* See README. The basic idea is that ROSE parses the executable and then attaches to a debugger, single stepping through the
 * program and comparing the real execution to ROSE's semantic analysis. */
#include "rose.h"
#include "Debugger.h"

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(true);
    std::string hostname = "localhost";
    short port = 32002;

    /* Process command-line arguments. ROSE will do most of the work, but we need to look for the --debugger switch. */
    for (int argno=1; argno<argc; argno++) {
        if (!strncmp(argv[argno], "--debugger=", 11)) {
            char *colon = strchr(argv[argno]+11, ':');
            if (colon) {
                hostname = std::string(argv[argno]+11, colon-(argv[argno]+11));
                char *rest;
                port = strtol(colon+1, &rest, 0);
                if (rest && *rest) {
                    fprintf(stderr, "invalid argument for --debugger=HOST:PORT switch\n");
                    exit(1);
                }
            } else {
                hostname = argv[argno]+11;
            }
            memmove(argv+argno, argv+argno+1, (argc-(argno+1))*sizeof(*argv));
            --argc;
            break;
        }
    }
    fprintf(stderr, "Parsing and disassembling executable...\n");
    SgProject *project = frontend(argc, argv);

    /* Connect to debugger */
    fprintf(stderr, "Connecting to debugger at %s:%d\n", hostname.c_str(), port);
    Debugger dbg(hostname, port);

    /* Choose an interpretation */
    SgAsmInterpretation *interp = isSgAsmInterpretation(NodeQuery::querySubTree(project, V_SgAsmInterpretation).back());
    assert(interp);
    Disassembler *disassembler = Disassembler::lookup(interp)->clone();
    assert(disassembler!=NULL);

    fprintf(stderr, "Setting break points...\n");
    int status = dbg.setbp(0, 0xffffffff);
    assert(status>=0);

    fprintf(stderr, "Starting executable...\n");
    uint64_t nprocessed = 0;

    dbg.cont(); /* Advance to the first breakpoint. */
    while (1) {
        nprocessed++;
        unsigned char insn_buf[15];
        size_t nread = dbg.memory(dbg.rip(), sizeof insn_buf, insn_buf);
        SgAsmInstruction *insn = NULL;
        try {
            insn = disassembler->disassembleOne(insn_buf, dbg.rip(), nread, dbg.rip(), NULL);
        } catch(const Disassembler::Exception &e) {
            std::cerr <<"disassembler exception: " <<e <<"\n";
        }
        if (!insn) {
            dbg.cont();
            continue;
        }
        /* Trace instructions */
        fprintf(stderr, "[%07"PRIu64"] 0x%08"PRIx64": %s\n", nprocessed, insn->get_address(), unparseInstruction(insn).c_str());

        /* Single step to cause the instruction to be executed remotely. Then compare our state with the remote state. */
        dbg.step();
    }


    exit(1); /*FIXME*/
}

