// Runs specific functions with specific inputs.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "rose_getline.h"
#include "AST_FILE_IO.h"        // only for the clearAllMemoryPools() function [Robb P. Matzke 2013-06-17]

#include <cerrno>
#include <csignal>

using namespace CloneDetection;
typedef CloneDetection::Policy<State, PartialSymbolicSemantics::ValueType> ClonePolicy;
typedef X86InstructionSemantics<ClonePolicy, PartialSymbolicSemantics::ValueType> CloneSemantics;
typedef std::set<std::string> NameSet;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE < FUNC_INPUT_PAIRS\n"
              <<"  This command runs the tests specified on standard input.\n"
              <<"\n"
              <<"    --checkpoint[=NSEC,[NSEC2]]\n"
              <<"    --no-checkpoint\n"
              <<"            Commit test results to the database every NSEC seconds.  If NSEC2 is also present then the\n"
              <<"            checkpoint interval will be a random value between NSEC and NSEC2.  If neither NSEC nor NSEC2\n"
              <<"            are specified then a random interval between 5 and 15 minutes is chosen (this is also the default\n"
              <<"            if neither --checkpoint nor --no-checkpoint is specified). A random interval is useful when large\n"
              <<"            output groups and/or traces need to be saved in the database since it makes it more likely that\n"
              <<"            not all instances of a parallel run will hit the database server at the about the same time.\n"
              <<"    --dry-run\n"
              <<"            Do not modify the database. This is really only useful with the --verbose switch in order to\n"
              <<"            re-run a test for debugging purposes.\n"
              <<"    --file=NAME\n"
              <<"            Load the FUNC_INPUT_PAIRS work list from this file rather than standard input.\n"
              <<"    --[no-]follow-calls\n"
              <<"    --follow-calls=none|all|builtin\n"
              <<"            Indicates which function calls (x86 CALL instructions) should be followed rather than skipped.\n"
              <<"            A value of \"all\" causes calls for which the call target is a known, disassembled, instruction\n"
              <<"            to be followed.  For functions that are skipped, an input value is consumed from the \"integer\"\n"
              <<"            queue and placed in the EAX register as the function's return value.  If \"builtin\" is specified\n"
              <<"            then all calls are followed unless the call is to a dynamically-loaded function which is not\n"
              <<"            on a white-list of built-in functions.  The \"--no-follow-calls\" is the same as specifying\n"
              <<"            \"--follow-calls=none\", and \"--follow-calls\" is the same as \"--follow-calls=all\".\n"
              <<"    --[no-]interactive\n"
              <<"            With the \"--interactive\" switch, pressing control-C (or otherwise sending SIGINT to the\n"
              <<"            process) will cause the process to finish executing the current test and then prompt the user\n"
              <<"            on the tty whether it should checkpoint and/or terminate. The default is --no-interactive.\n"
              <<"    --timeout=NINSNS\n"
              <<"            Any test for which more than NINSNS instructions are executed times out.  The default is 5000.\n"
              <<"            Tests that time out produce a fault output in addition to whatever normal output values were\n"
              <<"            produced.\n"
              <<"    --[no-]pointers\n"
              <<"            Perform [or not] pointer analysis on each function.  The pointer analysis is a binary-only\n"
              <<"            analysis that looks for memory addresses that hold pointers from the source code.  When this is\n"
              <<"            enabled, any read from such an address before it's initialized causes an input to be consumed\n"
              <<"            from the \"pointers\" input queue. This analysis slows down processing considerably and is\n"
              <<"            therefore disabled by default.\n"
              <<"    --[no-]progress\n"
              <<"            Show a progress bar even if standard error is not a terminal or the verbosity level is not silent.\n"
              <<"    --trace[=EVENTS]\n"
              <<"    --no-trace\n"
              <<"            Sets the events that are traced by each test.  The EVENTS is a comma-separated list of event\n"
              <<"            specifiers. Each event specifier is an optional '+' or '-' to indicate whether the event type will\n"
              <<"            be added or subtracted from the set (default is added), followed by either an event name or\n"
              <<"            integer bit vector.  The following event names are recognized:\n"
              <<"                reached: events indicating that a basic block has been reached\n"
              <<"                branched: events indicating that a branch has been taken, as opposed to falling through.\n"
              <<"                fault: events indicating that the test terminated abnormally.\n"
              <<"                consumed: events indicating that input was consumed.\n"
              <<"                cfg: short-hand for both \"reached\" and \"branched\" together.\n"
              <<"                all: all event types.\n"
              <<"    --verbose\n"
              <<"    --verbosity=(silent|laconic|effusive)\n"
              <<"            Determines how much diagnostic info to send to the standard error stream.  The --verbose\n"
              <<"            switch does the same thing as --verbosity=effusive.  The default is \"silent\".\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    FUNC_INPUT_PAIRS\n"
              <<"            A text file containing one line per test to be executed.  Each line contains three numbers: the\n"
              <<"            file ID for the specimen as a whole, the ID number for a function within that specimen, and the\n"
              <<"            ID number of the input group that contains the input  values to use for that test.  The '#'\n"
              <<"            character indroduces a comment that continues to the end of the line; blank lines are ignored.\n"
              <<"            Generally, this input is produced with the 02-pending-tests executable.\n";
    exit(exit_status);
}

struct Switches {
    Switches()
        : verbosity(SILENT), progress(false), pointers(false), interactive(false), trace_events(0), dry_run(false) {
        checkpoint = 300 + LinearCongruentialGenerator()()%600;
    }
    Verbosity verbosity;                        // semantic policy has a separate verbosity
    bool progress;
    bool pointers;
    time_t checkpoint;
    bool interactive;
    unsigned trace_events;
    bool dry_run;
    std::string input_file_name;
    PolicyParams params;
};

struct WorkItem {
    WorkItem(): specimen_id(-1), func_id(-1), igroup_id(-1) {}
    WorkItem(int specimen_id, int func_id, int igroup_id): specimen_id(specimen_id), func_id(func_id), igroup_id(igroup_id) {}
    bool operator<(const WorkItem &other) const {
        if (specimen_id!=other.specimen_id)
            return specimen_id < other.specimen_id;
        if (func_id!=other.func_id)
            return func_id < other.func_id;
        return igroup_id < other.igroup_id;
    }
    int specimen_id, func_id, igroup_id;
};

static int interrupted = -1;

static void
sig_handler(int signo)
{
    if (interrupted==SIGINT && signo==SIGINT) {
        // Pressing Ctrl-C twice will abort without committing.
        struct sigaction sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);
        raise(SIGINT);
        abort();
    }

    if (SIGINT==signo && isatty(2)) {
        static const char *s = "\nterminating after this test...\n";
        write(2, s, strlen(s));
    }

    interrupted = signo;
}

static WorkList<WorkItem>
load_worklist(const std::string &filename, FILE *f)
{
    WorkList<WorkItem> worklist;
    char *line = NULL;
    size_t line_sz = 0, line_num = 0;
    while (rose_getline(&line, &line_sz, f)>0) {
        ++line_num;
        if (char *c = strchr(line, '#'))
            *c = '\0';
        char *s = line + strspn(line, " \t\r\n"), *rest;
        if (!*s)
            continue; // blank line

        errno = 0;
        int specimen_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<filename <<":" <<line_num <<": syntax error: specimen file ID expected\n";
            exit(1);
        }
        s = rest;

        errno = 0;
        int func_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<filename <<":" <<line_num <<": syntax error: function ID expected\n";
            exit(1);
        }
        s = rest;

        errno = 0;
        int igroup_id = strtol(s, &rest, 0);
        if (errno!=0 || rest==s) {
            std::cerr <<argv0 <<": " <<filename <<":" <<line_num <<": syntax error: input group ID expected\n";
            exit(1);
        }

        while (isspace(*rest)) ++rest;
        if (*rest) {
            std::cerr <<argv0 <<": " <<filename <<":" <<line_num <<": syntax error: extra text after input group ID\n";
            exit(1);
        }

        worklist.push(WorkItem(specimen_id, func_id, igroup_id));
    }
    return worklist;
}

// Perform a pointer-detection analysis on the specified function. We'll need the results in order to determine whether a
// function input should consume a pointer or an integer from the input value set.
static PointerDetector *
detect_pointers(SgAsmFunction *func, const FunctionIdMap &function_ids, const Switches &opt)
{
    if (!opt.pointers)
        return NULL;

    // Choose an SMT solver. This is completely optional.  Pointer detection still seems to work fairly well (and much,
    // much faster) without an SMT solver.
    SMTSolver *solver = NULL;
#if 0   // optional code
    if (YicesSolver::available_linkage())
        solver = new YicesSolver;
#endif
    InstructionProvidor insn_providor(func);
    if (opt.verbosity>=LACONIC)
        std::cerr <<argv0 <<": " <<function_to_str(func, function_ids) <<" pointer detection analysis\n";
    PointerDetector *pd = new PointerDetector(&insn_providor, solver);
    pd->initial_state().registers.gpr[x86_gpr_sp] = SymbolicSemantics::ValueType<32>(opt.params.initial_stack);
    pd->initial_state().registers.gpr[x86_gpr_bp] = SymbolicSemantics::ValueType<32>(opt.params.initial_stack);
    //pd.set_debug(stderr);
    try {
        pd->analyze(func);
    } catch (...) {
        // probably the instruction is not handled by the semantics used in the analysis.  For example, the
        // instruction might be a floating point instruction that isn't handled yet.
        std::cerr <<argv0 <<": pointer analysis FAILED for " <<function_to_str(func, function_ids) <<"\n";
    }
    if (opt.verbosity>=EFFUSIVE) {
        const PointerDetector::Pointers plist = pd->get_pointers();
        for (PointerDetector::Pointers::const_iterator pi=plist.begin(); pi!=plist.end(); ++pi) {
            std::cerr <<argv0 <<":     ";
            if (pi->type & BinaryAnalysis::PointerAnalysis::DATA_PTR)
                std::cerr <<"data ";
            if (pi->type & BinaryAnalysis::PointerAnalysis::CODE_PTR)
                std::cerr <<"code ";
            std::cerr <<"pointer at " <<pi->address <<"\n";
        }
    }
    return pd;
}

// Returns (via argument) the names of functions built into the compiler.
static void
add_builtin_functions(NameSet &names/*in,out*/)
{
    names.insert("abs@plt");
    names.insert("labs@plt");
    names.insert("memcmp@plt");
    names.insert("memcpy@plt");
    names.insert("strcmp@plt");
    names.insert("strlen@plt");
    names.insert("strncmp@plt");
    names.insert("abort@plt");
    names.insert("memset@plt");
    names.insert("strcat@plt");
    names.insert("strcpy@plt");
    names.insert("strncpy@plt");
    names.insert("strchr@plt");
    names.insert("strspn@plt");
    names.insert("strcspn@plt");
    names.insert("strstr@plt");
    names.insert("strpbrk@plt");
    names.insert("strrchr@plt");
    names.insert("strncat@plt");
    names.insert("alloca@plt");
    names.insert("ffs@plt");
    names.insert("index@plt");
    names.insert("rindex@plt");
    names.insert("bcmp@plt");
    names.insert("bzero@plt");
}

// Find all functions whose first instruction is an indirect jump, and store the memory address through which the jump occurs.
// Most of these will be dynamically-linked functions whose only instruction is the JMP and whose address is in the .got.plt or
// IAT.  Return the set of addresses for only those functions whose name appears in the specified set.
static Disassembler::AddressSet
whitelist_imports(SgAsmInterpretation *interp, const NameSet &whitelist_names)
{
    struct T1: AstSimpleProcessing {
        std::map<SgAsmFunction*, rose_addr_t> gotplt_addr;      // return value; address in .got.plt for each function
        SgAsmFunction *func;    // current function, but only if we haven't seen it's entry instruction yet

        T1(): func(NULL) {}

        void visit(SgNode *node) {
            if (SgAsmFunction *f = isSgAsmFunction(node)) {
                func = f;
            } else if (SgAsmx86Instruction *insn = isSgAsmx86Instruction(node)) {
                SgAsmFunction *f = func; func = NULL;
                const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
                if (f && x86_jmp==insn->get_kind() && args.size()==1) {
                    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(args[0]);
                    SgAsmIntegerValueExpression *val = isSgAsmIntegerValueExpression(mre ? mre->get_address() : NULL);
                    if (val)
                        gotplt_addr[f] = val->get_absolute_value();
                }
            }
        }
    } t1;
    t1.traverse(interp, preorder);

    Disassembler::AddressSet retval;
    for (std::map<SgAsmFunction*, rose_addr_t>::iterator i=t1.gotplt_addr.begin(); i!=t1.gotplt_addr.end(); ++i) {
        if (whitelist_names.find(i->first->get_name())!=whitelist_names.end())
            retval.insert(i->second);
    }
    return retval;
}

// Scan the interpretation to find all the locations that contain addresses of dynamically linked functions and replace them
// with a special address that can be recognized by the fetch-execute loop in fuzz_test().  We do this by adding some entries
// to the memory map that serves up values when needed by memory-reading instructions.  For ELF, we replace the .got.plt
// section; for PE we replace the "Import Address Table" section.  We always replace addresses that don't correspond to a valid
// instruction. On the other hand, when the address is valid, we replace it if either follow-calls is CALL_NONE or follow-calls
// is CALL_BUILTIN and the function is not builtin.
static void
overmap_dynlink_addresses(SgAsmInterpretation *interp, const InstructionProvidor &insns, FollowCalls follow_calls,
                          const Disassembler::AddressSet &whitelist, MemoryMap *ro_map/*in,out*/, rose_addr_t special_value)
{
    const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=hdrs.begin(); hi!=hdrs.end(); ++hi) {
        const SgAsmGenericSectionPtrList &sections = (*hi)->get_sections()->get_sections();
        for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
            std::string name = (*si)->get_name()->get_string();
            if ((*si)->is_mapped() && (0==name.compare(".got.plt") || 0==name.compare("Import Address Table"))) {
                rose_addr_t base_va = (*si)->get_mapped_actual_va();
                size_t nbytes = (*si)->get_mapped_size();
                size_t nwords = nbytes / 4;
                if (nwords>0) {
                    size_t nchanges = 0;
                    uint32_t *buf = new uint32_t[nwords];
                    (*si)->read_content_local(0, buf, nbytes, false);
                    for (size_t i=0; i<nwords; ++i) {
                        rose_addr_t va = ByteOrder::le_to_host(buf[i]);
                        if (NULL==insns.get_instruction(va) || CALL_NONE==follow_calls ||
                            (CALL_BUILTIN==follow_calls && whitelist.find(va)==whitelist.end())) {
                            buf[i] = special_value;
                            ++nchanges;
                        }
                    }
                    if (nchanges>0) {
                        MemoryMap::BufferPtr mmbuf = MemoryMap::ByteBuffer::create(buf, nbytes);
                        ro_map->insert(Extent(base_va, nbytes), MemoryMap::Segment(mmbuf, 0, MemoryMap::MM_PROT_READ,
                                                                                   "analysis-mapped dynlink addresses"));
                    } else {
                        delete[] buf;
                    }
                }
            }
        }
    }
}

// Analyze a single function by running it with the specified inputs and collecting its outputs. */
static OutputGroup
fuzz_test(SgAsmInterpretation *interp, SgAsmFunction *function, InputGroup &inputs, Tracer &tracer,
          const InstructionProvidor &insns, MemoryMap *ro_map, const PointerDetector *pointers, const Switches &opt,
          const AddressIdMap &entry2id)
{
    ClonePolicy policy(opt.params, entry2id, tracer);
    policy.set_map(ro_map);
    CloneSemantics semantics(policy);
    AnalysisFault::Fault fault = AnalysisFault::NONE;
    policy.reset(interp, function, &inputs, &insns, pointers);
    rose_addr_t last_good_va = 0;
    try {
        while (1) {
            if (!policy.state.registers.ip.is_known()) {
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<"CloneDetection: EIP value is not concrete\n";
                fault = AnalysisFault::SEMANTICS;
                break;
            }

            rose_addr_t insn_va = policy.state.registers.ip.known_value();
            if (policy.FUNC_RET_ADDR==insn_va) {
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<"CloneDetection: function returned\n";
                fault = AnalysisFault::NONE;
                break;
            }

            if (SgAsmx86Instruction *insn = isSgAsmx86Instruction(insns.get_instruction(insn_va))) {
                semantics.processInstruction(insn);
            } else {
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<"CloneDetection: no instruction at " <<StringUtility::addrToString(insn_va) <<"\n";
                fault = AnalysisFault::DISASSEMBLY;
                break;
            }
            last_good_va = insn_va;
        }
    } catch (const FaultException &e) {
        if (opt.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: analysis terminated by " <<AnalysisFault::fault_name(e.fault) <<"\n";
        fault = e.fault;
    } catch (const Exception &e) {
        if (opt.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: analysis terminated by semantic exception: " <<e.mesg <<"\n";
        fault = AnalysisFault::SEMANTICS;
    } catch (const BaseSemantics::Policy::Exception &e) {
        // Some exception in the policy, such as division by zero.
        if (opt.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: analysis terminated by FAULT_SEMANTICS: " <<e.mesg <<"\n";
        fault = AnalysisFault::SEMANTICS;
    } catch (const CloneSemantics::Exception &e) { // X86InstructionSemantics<...>::Exception
        if (opt.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: analysis terminated by X86InstructionSemantics exception: " <<e.mesg <<"\n";
        fault = AnalysisFault::SEMANTICS;
    } catch (const SMTSolver::Exception &e) {
        if (opt.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: analysis terminated by SMT solver exception: " <<e.mesg <<"\n";
        fault = AnalysisFault::SMTSOLVER;
    }

    if (fault) {
        rose_addr_t va = policy.state.registers.ip.is_known() ? policy.state.registers.ip.known_value() : 0;
        if (fault==AnalysisFault::DISASSEMBLY) {
            // We need to assign disassembly faults to the last good address, otherwise they'll never get attached to anything
            // in the listings.  We'll save the actual fault address as the value.
            tracer.emit(last_good_va, Tracer::EV_FAULT, va, (int)fault);
        } else {
            // Non-disassembly faults will be assigned to the address where they occur, and the previous instruction's address
            // is stored as the value of the fault.
            tracer.emit(va, Tracer::EV_FAULT, last_good_va, (int)fault);
        }
    }
    
    // Gather the function's outputs before restoring machine state.
    OutputGroup outputs = policy.get_outputs();
    outputs.fault = fault;
    return outputs;
}

// Commit everything and return a new transaction
static SqlDatabase::TransactionPtr
checkpoint(const SqlDatabase::TransactionPtr &tx, OutputGroups &ogroups, Tracer &tracer, Progress &progress,
           size_t ntests_ran, int64_t cmd_id)
{
    SqlDatabase::ConnectionPtr conn = tx->connection();
    progress.message("checkpoint: saving output groups");
    ogroups.save(tx);
    progress.message("checkpoint: saving trace events");
    tracer.save(tx);

    progress.message("checkpoint: committing");
    std::string desc = "ran "+StringUtility::numberToString(ntests_ran)+" test"+(1==ntests_ran?"":"s");
    if (ntests_ran>0)
        finish_command(tx, cmd_id, desc);
    tx->commit();

    progress.message("");
    progress.clear();
    std::cerr <<argv0 <<": " <<desc <<"\n";
    return conn->transaction();
}

int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    // Parse command-line switches
    Switches opt;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--checkpoint")) {
            opt.checkpoint = 300 + LinearCongruentialGenerator()() % 600; // between 5 and 15 minutes
        } else if (!strcmp(argv[argno], "--no-checkpoint")) {
            opt.checkpoint = 0;
        } else if (!strncmp(argv[argno], "--checkpoint=", 13)) {
            char *s=argv[argno]+13, *rest;
            errno = 0;
            time_t c1 = strtoul(s, &rest, 0), c2=0;
            if (errno || rest==s) {
                std::cerr <<argv0 <<": invalid time spec for --checkpoint switch: " <<argv[argno]+13 <<"\n";
                exit(1);
            }
            while (isspace(*rest)) ++rest;
            if (','==*rest) {
                s = rest;
                errno = 0;
                c2 = strtoul(s, &rest, 0);
                if (errno || rest==s) {
                    std::cerr <<argv0 <<": invalid end time spec for --checkpoint switch: " <<argv[argno]+13 <<"\n";
                    exit(1);
                }
            } else if (*rest) {
                std::cerr <<argv0 <<": invalid time spec for --checkpoint switch: " <<argv[argno]+13 <<"\n";
                exit(1);
            } else {
                c2 = c1;
            }
            c1 = std::max((time_t)1, c1);
            c2 = std::max((time_t)1, c2);
            if (c1==c2) {
                opt.checkpoint = c1;
            } else {
                if (c1 > c2) std::swap(c1, c2);
                opt.checkpoint = c1 + LinearCongruentialGenerator()() % (c2-c1);
            }
        } else if (!strcmp(argv[argno], "--dry-run")) {
            opt.dry_run = true;
        } else if (!strncmp(argv[argno], "--file=", 7)) {
            opt.input_file_name = argv[argno]+7;
        } else if (!strcmp(argv[argno], "--follow-calls") || !strcmp(argv[argno], "--follow-calls=all")) {
            opt.params.follow_calls = CALL_ALL;
        } else if (!strcmp(argv[argno], "--no-follow-calls") || !strcmp(argv[argno], "--follow-calls=none")) {
            opt.params.follow_calls = CALL_NONE;
        } else if (!strcmp(argv[argno], "--follow-calls=builtin")) {
            opt.params.follow_calls = CALL_BUILTIN;
        } else if (!strcmp(argv[argno], "--interactive")) {
            opt.interactive = true;
        } else if (!strcmp(argv[argno], "--no-interactive")) {
            opt.interactive = false;
        } else if (!strncmp(argv[argno], "--timeout=", 10)) {
            opt.params.timeout = strtoull(argv[argno]+10, NULL, 0);
        } else if (!strcmp(argv[argno], "--pointers")) {
            opt.pointers = true;
        } else if (!strcmp(argv[argno], "--no-pointers")) {
            opt.pointers = false;
        } else if (!strcmp(argv[argno], "--progress")) {
            opt.progress = true;
        } else if (!strcmp(argv[argno], "--no-progress")) {
            opt.progress = false;
        } else if (!strcmp(argv[argno], "--trace")) {
            opt.trace_events = Tracer::ALL_EVENTS;
        } else if (!strcmp(argv[argno], "--no-trace")) {
            opt.trace_events = 0;
        } else if (!strncmp(argv[argno], "--trace=", 8)) {
            std::vector<std::string> words = StringUtility::split(",", argv[argno]+8, (size_t)-1, true);
            for (size_t i=0; i<words.size(); ++i) {
                unsigned events = Tracer::EV_NONE;
                bool status = true;
                if (!words[i].empty() && '-'==words[i][0]) {
                    status = false;
                    words[i] = words[i].substr(1);
                } else if (!words[i].empty() && '+'==words[i][0]) {
                    status = true;
                    words[i] = words[i].substr(1);
                }
                if (0==words[i].compare("none")) {
                    events = Tracer::EV_NONE;
                } else if (0==words[i].compare("reached")) {
                    events = Tracer::EV_REACHED;
                } else if (0==words[i].compare("branched")) {
                    events = Tracer::EV_BRANCHED;
                } else if (0==words[i].compare("fault") || 0==words[i].compare("faults")) {
                    events = Tracer::EV_FAULT;
                } else if (0==words[i].compare("consume") || 0==words[i].compare("consumed")) {
                    events = Tracer::EV_CONSUME_INPUT;
                } else if (0==words[i].compare("cfg")) {
                    events = Tracer::CONTROL_FLOW;
                } else if (0==words[i].compare("all")) {
                    events = Tracer::ALL_EVENTS;
                } else if (!words[i].empty() && isdigit(words[i][0])) {
                    const char *s = words[i].c_str();
                    char *rest;
                    errno = 0;
                    events = strtoul(s, &rest, 0);
                    if (errno || rest==s || *rest) {
                        std::cerr <<argv0 <<": invalid event spec: " <<words[i] <<"\n";
                        exit(1);
                    }
                } else {
                    std::cerr <<argv0 <<": invalid event spec: " <<words[i] <<"\n";
                    exit(1);
                }
                if (status) {
                    opt.trace_events |= events;
                } else {
                    opt.trace_events &= ~events;
                }
            }
        } else if (!strcmp(argv[argno], "--verbose")) {
            opt.verbosity = opt.params.verbosity = EFFUSIVE;
        } else if (!strcmp(argv[argno], "--verbosity=silent")) {
            opt.verbosity = opt.params.verbosity = SILENT;
        } else if (!strcmp(argv[argno], "--verbosity=laconic")) {
            opt.verbosity = opt.params.verbosity = LACONIC;
        } else if (!strcmp(argv[argno], "--verbosity=effusive")) {
            opt.verbosity = opt.params.verbosity = EFFUSIVE;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    SqlDatabase::TransactionPtr tx = conn->transaction();
    int64_t cmd_id = start_command(tx, argc, argv, "running tests");

    // Read list of tests from stdin
    WorkList<WorkItem> worklist;
    if (opt.input_file_name.empty()) {
        std::cerr <<argv0 <<": reading worklist from stdin...\n";
        worklist = load_worklist("stdin", stdin);
    } else {
        FILE *f = fopen(opt.input_file_name.c_str(), "r");
        if (NULL==f) {
            std::cerr <<argv0 <<": " <<strerror(errno) <<": " <<opt.input_file_name <<"\n";
            exit(1);
        }
        worklist = load_worklist(opt.input_file_name, f);
        fclose(f);
    }
    std::cerr <<argv0 <<": " <<worklist.size() <<(1==worklist.size()?" test needs":" tests need") <<" to be run\n";
    if (worklist.empty())
        return 0;
    Progress progress(worklist.size());
    progress.force_output(opt.progress);
    OutputGroups ogroups; // do not load from database (that might take a very long time)
    NameSet builtin_function_names;
    add_builtin_functions(builtin_function_names/*out*/);
    
    // Set up the interrupt handler
    if (opt.interactive) {
        struct sigaction sa;
        sa.sa_handler = sig_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(SIGINT, &sa, NULL);
    }

    // Process each item on the work list.
    typedef std::map<SgAsmFunction*, PointerDetector*> PointerDetectors;
    PointerDetectors pointers;
    FilesTable files(tx);
    WorkItem prev_work;
    IdFunctionMap functions;
    FunctionIdMap function_ids;
    InputGroup igroup;
    InstructionProvidor insns;
    SgAsmInterpretation *prev_interp = NULL;
    MemoryMap ro_map;
    AddressIdMap entry2id;                      // maps function entry address to function ID
    Tracer tracer;
    time_t last_checkpoint = time(NULL);
    size_t ntests_ran=0;
    while (!worklist.empty()) {
        ++progress;
        WorkItem work = worklist.shift();

        // If we're switching to a new specimen then we need to reinitialize the AST, throwing away the old information and
        // getting new information.  The new information comes from either a stored AST, or by extracting the binaries from
        // the database and reparsing them, or by reparsing existing binaries (in that order of preference).
        if (work.specimen_id!=prev_work.specimen_id) {
            if (opt.verbosity>=LACONIC) {
                progress.clear();
                if (opt.verbosity>=EFFUSIVE)
                    std::cerr <<argv0 <<": " <<std::string(100, '#') <<"\n";
                std::cerr <<argv0 <<": processing binary specimen \"" <<files.name(work.specimen_id) <<"\"\n";
            }

            if (prev_work.specimen_id>=0) {
#if 0 // [Robb P. Matzke 2013-06-17]
                // Is this the preferred way to delete a whole AST? It fails an assertion:
                // Cxx_GrammarTreeTraversalSuccessorContainer.C:23573:
                //     virtual SgNode* SgAsmGenericSectionList::get_traversalSuccessorByIndex(size_t):
                //     Assertion `idx < p_sections.size()' failed.
                SageInterface::deleteAST(SageInterface::getProject());
#else
                AST_FILE_IO::clearAllMemoryPools();
#endif
                prev_interp = NULL;
            }

            progress.message("loading AST");
            SgProject *project = files.load_ast(tx, work.specimen_id);
            progress.message("");

            if (!project) {
                progress.message("parsing specimen");
                project = open_specimen(tx, files, work.specimen_id, argv0);
                progress.message("");
            }
            
            // Get list of functions and initialize the instruction cache
            std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(project);
            functions = existing_functions(tx, files, all_functions);
            function_ids.clear();
            entry2id.clear();
            for (IdFunctionMap::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
                function_ids[fi->second] = fi->first;
                entry2id[fi->second->get_entry_va()] = fi->first;
            }
            insns = InstructionProvidor(all_functions);
            progress.message("");
        }

        // Load the input group from the database if necessary.
        if (work.igroup_id!=prev_work.igroup_id) {
            if (!igroup.load(tx, work.igroup_id)) {
                progress.clear();
                std::cerr <<argv0 <<": input group " <<work.igroup_id <<" is empty or does not exist\n";
                exit(1);
            }
        }

        // Find the function to test
        IdFunctionMap::iterator func_found = functions.find(work.func_id);
        assert(func_found!=functions.end());
        SgAsmFunction *func = func_found->second;
        if (opt.verbosity>=LACONIC) {
            progress.clear();
            if (opt.verbosity>=EFFUSIVE)
                std::cerr <<argv0 <<": " <<std::string(100, '=') <<"\n";
            std::cerr <<argv0 <<": processing function " <<function_to_str(func, function_ids) <<"\n";
        }
        SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);
        assert(interp!=NULL);

        // Do per-interpretation stuff
        if (interp!=prev_interp) {
            prev_interp = interp;
            assert(interp->get_map()!=NULL);
            ro_map = *interp->get_map();
            ro_map.prune(MemoryMap::MM_PROT_READ, MemoryMap::MM_PROT_WRITE);
            Disassembler::AddressSet whitelist = whitelist_imports(interp, builtin_function_names);
            overmap_dynlink_addresses(interp, insns, opt.params.follow_calls, whitelist, &ro_map, GOTPLT_VALUE);
        }
        
        // Get the results of pointer analysis.  We could have done this before any fuzz testing started, but by doing
        // it here we only need to do it for functions that are actually tested.
        PointerDetectors::iterator ip = pointers.find(func);
        if (ip==pointers.end())
            ip = pointers.insert(std::make_pair(func, detect_pointers(func, function_ids, opt))).first;
        assert(ip!=pointers.end());

        // Run the test
        tracer.reset(work.func_id, work.igroup_id, opt.trace_events);
        timeval start_time, stop_time;
        clock_t start_ticks = clock();
        gettimeofday(&start_time, NULL);
        OutputGroup ogroup = fuzz_test(interp, func, igroup, tracer, insns, &ro_map, ip->second, opt, entry2id);
        gettimeofday(&stop_time, NULL);
        clock_t stop_ticks = clock();
        double elapsed_time = (stop_time.tv_sec - start_time.tv_sec) +
                              ((double)stop_time.tv_usec - start_time.tv_usec) * 1e-6;

        // If clock_t is a 32-bit unsigned value then it will wrap around once every ~71.58 minutes. We expect clone
        // detection to take longer than that, so we need to be careful.
        double cpu_time = start_ticks <= stop_ticks ?
                                  (double)(stop_ticks-start_ticks) / CLOCKS_PER_SEC :
                                  (pow(2.0, 8*sizeof(clock_t)) - (start_ticks-stop_ticks)) / CLOCKS_PER_SEC;

        // Find a matching output group, or create a new one
        int64_t ogroup_id = ogroups.find(ogroup);
        if (ogroup_id<0)
            ogroup_id = ogroups.insert(ogroup);

        // Update the database with the test results
        SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_fio"
                                                       // 0        1          2                   3
                                                       " (func_id, igroup_id, arguments_consumed, locals_consumed,"
                                                       // 4               5                   6
                                                       "globals_consumed, functions_consumed, pointers_consumed,"
                                                       // 7                8                      9          10
                                                       "integers_consumed, instructions_executed, ogroup_id, status,"
                                                       // 11          12        13
                                                       "elapsed_time, cpu_time, cmd)"
                                                       " values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        stmt->bind(0, work.func_id);
        stmt->bind(1, work.igroup_id);
        stmt->bind(2, igroup.queue(IQ_ARGUMENT).nconsumed());
        stmt->bind(3, igroup.queue(IQ_LOCAL).nconsumed());
        stmt->bind(4, igroup.queue(IQ_GLOBAL).nconsumed());
        stmt->bind(5, igroup.queue(IQ_FUNCTION).nconsumed());
        stmt->bind(6, igroup.queue(IQ_POINTER).nconsumed());
        stmt->bind(7, igroup.queue(IQ_INTEGER).nconsumed());
        stmt->bind(8, ogroup.ninsns);
        stmt->bind(9, ogroup_id);
        stmt->bind(10, ogroup.fault);
        stmt->bind(11, elapsed_time);
        stmt->bind(12, cpu_time);
        stmt->bind(13, cmd_id);
        stmt->execute();
        ++ntests_ran;

        // Check for user interrupts
        bool do_checkpoint=false, do_exit=false;
        if (SIGINT==interrupted) {
            progress.clear();
            std::cout <<argv0 <<": interrupted by user.\n";
            if (isatty(1)) {
                FILE *f = fopen("/dev/tty", "r");
                if (f!=NULL) {
                    if (opt.dry_run) {
                        std::cout <<argv0 <<":"
                                  <<" c=continue in dry-run mode;"
                                  <<" d=turn dry-run mode off;"
                                  <<" q=commit and quit;"
                                  <<" a=abort w/out commit\n"
                                  <<argv0 <<": your choice? [C/d/q/a] ";
                    } else {
                        std::cout <<argv0 <<":"
                                  <<" c=commit and conintue;"
                                  <<" q=commit and quit;"
                                  <<" a=abort w/out commit\n"
                                  <<argv0 <<": your choice? [C/q/a] ";
                    }
                    char *line=NULL;
                    size_t line_sz=0;
                    if (rose_getline(&line, &line_sz, f)>0) {
                        do_checkpoint = NULL!=strchr("cCdDqQ\r\n", line[0]);
                        do_exit = NULL==strchr("cC\r\n", line[0]);
                        if (opt.dry_run && strchr("dD", line[0]))
                            opt.dry_run = false;
                    }
                }
            }
            interrupted = -1;
        }
        
        // Checkpoint
        if (do_checkpoint || (opt.checkpoint>0 && time(NULL)-last_checkpoint > opt.checkpoint)) {
            if (!opt.dry_run)
                tx = checkpoint(tx, ogroups, tracer, progress, ntests_ran, cmd_id);
            last_checkpoint = time(NULL);
        }
        if (do_exit) {
            tx->rollback();
            break;
        }
        
        prev_work = work;
    }

    // Cleanup
    if (!tx->is_terminated() && !opt.dry_run)
        tx = checkpoint(tx, ogroups, tracer, progress, ntests_ran, cmd_id);
    progress.clear();

    return 0;
}
