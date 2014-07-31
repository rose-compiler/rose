// Runs specific functions with specific inputs.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "rose_getline.h"
#include "AST_FILE_IO.h"        // only for the clearAllMemoryPools() function [Robb P. Matzke 2013-06-17]

#include <cerrno>
#include <csignal>

using namespace rose;
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
              <<"    --call-graph=no|compute|compute-small|save|save-small\n"
              <<"            Determines whether dynamic function call information should be computed and saved in the\n"
              <<"            database.  Computing the information makes it available to analyses that might run during or\n"
              <<"            after a test.  The default is to neither compute nor save. Specifying \"--call-graph\" with\n"
              <<"            no equal sign is the same as \"--call-graph=save\".  The \"small\" variants compute and save\n"
              <<"            only the call edges that emanate from the function being tested, exluding edges from other\n"
              <<"            callers or from recursive calls of the function being tested.\n"
              <<"    --checkpoint[=NSEC,[NSEC2]]\n"
              <<"    --no-checkpoint\n"
              <<"            Commit test results to the database every NSEC seconds.  If NSEC2 is also present then the\n"
              <<"            checkpoint interval will be a random value between NSEC and NSEC2.  If neither NSEC nor NSEC2\n"
              <<"            are specified then a random interval between 5 and 15 minutes is chosen (this is also the default\n"
              <<"            if neither --checkpoint nor --no-checkpoint is specified). A random interval is useful when large\n"
              <<"            output groups and/or traces need to be saved in the database since it makes it more likely that\n"
              <<"            not all instances of a parallel run will hit the database server at the about the same time.\n"
              <<"    --consumed-inputs=no|compute|save\n"
              <<"            Accumulate the list of input values that were consumed during each test and optionally save\n"
              <<"            it in the semantic_fio_inputs table.\n"
              <<"    --coverage=no|compute|save\n"
              <<"            Determines whether instruction coverage information should be computed and/or saved in the\n"
              <<"            database.  The default \"no\" does not keep track of which instruction addresses were executed;\n"
              <<"            the value \"compute\" causes coverage to be computed but not saved; the value \"save\" causes\n"
              <<"            coverage to be computed and saved in the database.  Specifying just \"--coverage\" with no\n"
              <<"            value is the same as saying \"--coverage=save\".\n"
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
              <<"                returned: events indicating that the inner-most function was forced to return early.\n"
              <<"                fault: events indicating that the test terminated abnormally.\n"
              <<"                consumed: events indicating that input was consumed.\n"
              <<"                cfg: short-hand for \"reached,branched,returned\".\n"
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
        : verbosity(SILENT), progress(false), pointers(false), interactive(false), trace_events(0), dry_run(false),
          save_coverage(false), save_callgraph(false), save_consumed_inputs(false) {
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
    bool save_coverage;
    bool save_callgraph;
    bool save_consumed_inputs;
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
    bool operator==(const WorkItem &other) const {
        return specimen_id==other.specimen_id && func_id==other.func_id && igroup_id==other.igroup_id;
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
    BinaryAnalysis::SMTSolver *solver = NULL;
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
    // small list from personal experience
    names.insert("abort");
    names.insert("abs");
    names.insert("alloca");
    names.insert("bcmp");
    names.insert("bzero");
    names.insert("ffs");
    names.insert("fputc_unlocked");
    names.insert("index");
    names.insert("labs");
    names.insert("memcmp");
    names.insert("memcpy");
    names.insert("memset");
    names.insert("rindex");
    names.insert("strcat");
    names.insert("strchr");
    names.insert("strcmp");
    names.insert("strcpy");
    names.insert("strcspn");
    names.insert("strlen");
    names.insert("strncat");
    names.insert("strncmp");
    names.insert("strncpy");
    names.insert("strpbrk");
    names.insert("strrchr");
    names.insert("strspn");
    names.insert("strstr");

    // all from http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html, but incomplete
    names.insert("_Exit");
    names.insert("abort");
    names.insert("abs");
    names.insert("acos");
    names.insert("acosf");
    names.insert("acosh");
    names.insert("acoshf");
    names.insert("acoshl");
    names.insert("acosl");
    names.insert("alloca");
    names.insert("asin");
    names.insert("asinf");
    names.insert("asinh");
    names.insert("asinhf");
    names.insert("asinhl");
    names.insert("asinl");
    names.insert("atan");
    names.insert("atan2");
    names.insert("atan2f");
    names.insert("atan2l");
    names.insert("atanf");
    names.insert("atanh");
    names.insert("atanhf");
    names.insert("atanhl");
    names.insert("atanl");
    names.insert("bcmp");
    names.insert("bzero");
    names.insert("cabs");
    names.insert("cabsf");
    names.insert("cabsl");
    names.insert("cacos");
    names.insert("cacosf");
    names.insert("cacosh");
    names.insert("cacoshf");
    names.insert("cacoshl");
    names.insert("cacosl");
    names.insert("calloc");
    names.insert("carg");
    names.insert("cargf");
    names.insert("cargl");
    names.insert("casin");
    names.insert("casinf");
    names.insert("casinh");
    names.insert("casinhf");
    names.insert("casinhl");
    names.insert("casinl");
    names.insert("catan");
    names.insert("catanf");
    names.insert("catanh");
    names.insert("catanhf");
    names.insert("catanhl");
    names.insert("catanl");
    names.insert("cbrt");
    names.insert("cbrtf");
    names.insert("cbrtl");
    names.insert("ccos");
    names.insert("ccosf");
    names.insert("ccosh");
    names.insert("ccoshf");
    names.insert("ccoshl");
    names.insert("ccosl");
    names.insert("ceil");
    names.insert("ceilf");
    names.insert("ceill");
    names.insert("cexp");
    names.insert("cexpf");
    names.insert("cexpl");
    names.insert("cimag");
    names.insert("cimagf");
    names.insert("cimagl");
    names.insert("clog");
    names.insert("clogf");
    names.insert("clogl");
    names.insert("conj");
    names.insert("conjf");
    names.insert("conjl");
    names.insert("copysign");
    names.insert("copysignf");
    names.insert("copysignl");
    names.insert("cos");
    names.insert("cosf");
    names.insert("cosh");
    names.insert("coshf");
    names.insert("coshl");
    names.insert("cosl");
    names.insert("cpow");
    names.insert("cpowf");
    names.insert("cpowl");
    names.insert("cproj");
    names.insert("cprojf");
    names.insert("cprojl");
    names.insert("creal");
    names.insert("crealf");
    names.insert("creall");
    names.insert("csin");
    names.insert("csinf");
    names.insert("csinh");
    names.insert("csinhf");
    names.insert("csinhl");
    names.insert("csinl");
    names.insert("csqrt");
    names.insert("csqrtf");
    names.insert("csqrtl");
    names.insert("ctan");
    names.insert("ctanf");
    names.insert("ctanh");
    names.insert("ctanhf");
    names.insert("ctanhl");
    names.insert("ctanl");
    names.insert("dcgettext");
    names.insert("dgettext");
    names.insert("drem");
    names.insert("dremf");
    names.insert("dreml");
    names.insert("erf");
    names.insert("erfc");
    names.insert("erfcf");
    names.insert("erfcl");
    names.insert("erff");
    names.insert("erfl");
    names.insert("exit");
    names.insert("exit");
    names.insert("exp");
    names.insert("exp10");
    names.insert("exp10f");
    names.insert("exp10l");
    names.insert("exp2");
    names.insert("exp2f");
    names.insert("exp2l");
    names.insert("expf");
    names.insert("expl");
    names.insert("expm1");
    names.insert("expm1f");
    names.insert("expm1l");
    names.insert("fabs");
    names.insert("fabsf");
    names.insert("fabsl");
    names.insert("fdim");
    names.insert("fdimf");
    names.insert("fdiml");
    names.insert("ffs");
    names.insert("ffsl");
    names.insert("ffsll");
    names.insert("floor");
    names.insert("floorf");
    names.insert("floorl");
    names.insert("fma");
    names.insert("fmaf");
    names.insert("fmal");
    names.insert("fmax");
    names.insert("fmaxf");
    names.insert("fmaxl");
    names.insert("fmin");
    names.insert("fminf");
    names.insert("fminl");
    names.insert("fmod");
    names.insert("fmodf");
    names.insert("fmodl");
    names.insert("fprintf");
    names.insert("fprintf_unlocked");
    names.insert("fputs");
    names.insert("fputs_unlocked");
    names.insert("frexp");
    names.insert("frexpf");
    names.insert("frexpl");
    names.insert("fscanf");
    names.insert("gamma");
    names.insert("gamma_r");
    names.insert("gammaf");
    names.insert("gammaf_r");
    names.insert("gammal");
    names.insert("gammal_r");
    names.insert("gettext");
    names.insert("hypot");
    names.insert("hypotf");
    names.insert("hypotl");
    names.insert("ilogb");
    names.insert("ilogbf");
    names.insert("ilogbl");
    names.insert("imaxabs");
    names.insert("index");
    names.insert("isalnum");
    names.insert("isalpha");
    names.insert("isascii");
    names.insert("isblank");
    names.insert("iscntrl");
    names.insert("isdigit");
    names.insert("isgraph");
    names.insert("islower");
    names.insert("isprint");
    names.insert("ispunct");
    names.insert("isspace");
    names.insert("isupper");
    names.insert("iswalnum");
    names.insert("iswalpha");
    names.insert("iswblank");
    names.insert("iswcntrl");
    names.insert("iswdigit");
    names.insert("iswgraph");
    names.insert("iswlower");
    names.insert("iswprint");
    names.insert("iswpunct");
    names.insert("iswspace");
    names.insert("iswupper");
    names.insert("iswxdigit");
    names.insert("isxdigit");
    names.insert("j0");
    names.insert("j0f");
    names.insert("j0l");
    names.insert("j1");
    names.insert("j1f");
    names.insert("j1l");
    names.insert("jn");
    names.insert("jnf");
    names.insert("jnl");
    names.insert("labs");
    names.insert("ldexp");
    names.insert("ldexpf");
    names.insert("ldexpl");
    names.insert("lgamma");
    names.insert("lgamma_r");
    names.insert("lgammaf");
    names.insert("lgammaf_r");
    names.insert("lgammal");
    names.insert("lgammal_r");
    names.insert("llabs");
    names.insert("llrint");
    names.insert("llrintf");
    names.insert("llrintl");
    names.insert("llround");
    names.insert("llroundf");
    names.insert("llroundl");
    names.insert("log");
    names.insert("log10");
    names.insert("log10f");
    names.insert("log10l");
    names.insert("log1p");
    names.insert("log1pf");
    names.insert("log1pl");
    names.insert("log2");
    names.insert("log2f");
    names.insert("log2l");
    names.insert("logb");
    names.insert("logbf");
    names.insert("logbl");
    names.insert("logf");
    names.insert("logl");
    names.insert("lrint");
    names.insert("lrintf");
    names.insert("lrintl");
    names.insert("lround");
    names.insert("lroundf");
    names.insert("lroundl");
    names.insert("malloc");
    names.insert("memchr");
    names.insert("memcmp");
    names.insert("memcpy");
    names.insert("mempcpy");
    names.insert("memset");
    names.insert("modf");
    names.insert("modf");
    names.insert("modfl");
    names.insert("nearbyint");
    names.insert("nearbyintf");
    names.insert("nearbyintl");
    names.insert("nextafter");
    names.insert("nextafterf");
    names.insert("nextafterl");
    names.insert("nexttoward");
    names.insert("nexttowardf");
    names.insert("nexttowardl");
    names.insert("pow");
    names.insert("pow10");
    names.insert("pow10f");
    names.insert("pow10l");
    names.insert("powf");
    names.insert("powl");
    names.insert("printf");
    names.insert("printf_unlocked");
    names.insert("putchar");
    names.insert("puts");
    names.insert("remainder");
    names.insert("remainderf");
    names.insert("remainderl");
    names.insert("remquo");
    names.insert("remquof");
    names.insert("remquol");
    names.insert("rindex");
    names.insert("rint");
    names.insert("rintf");
    names.insert("rintl");
    names.insert("round");
    names.insert("roundf");
    names.insert("roundl");
    names.insert("scalb");
    names.insert("scalbf");
    names.insert("scalbl");
    names.insert("scalbln");
    names.insert("scalblnf");
    names.insert("scalblnl");
    names.insert("scalbn");
    names.insert("scalbnf");
    names.insert("scalbnl");
    names.insert("scanf");
    names.insert("signbit");
    names.insert("signbitd128");
    names.insert("signbitd32");
    names.insert("signbitd64");
    names.insert("signbitf");
    names.insert("signbitl");
    names.insert("significand");
    names.insert("significandf");
    names.insert("significandl");
    names.insert("sin");
    names.insert("sincos");
    names.insert("sincosf");
    names.insert("sincosl");
    names.insert("sinf");
    names.insert("sinh");
    names.insert("sinhf");
    names.insert("sinhl");
    names.insert("sinl");
    names.insert("snprintf");
    names.insert("snprintf");
    names.insert("sprintf");
    names.insert("sqrt");
    names.insert("sqrtf");
    names.insert("sqrtl");
    names.insert("sscanf");
    names.insert("stpcpy");
    names.insert("stpncpy");
    names.insert("strcasecmp");
    names.insert("strcat");
    names.insert("strchr");
    names.insert("strcmp");
    names.insert("strcpy");
    names.insert("strcspn");
    names.insert("strdup");
    names.insert("strfmon");
    names.insert("strlen");
    names.insert("strncasecmp");
    names.insert("strncat");
    names.insert("strncmp");
    names.insert("strncpy");
    names.insert("strndup");
    names.insert("strpbrk");
    names.insert("strrchr");
    names.insert("strspn");
    names.insert("strstr");
    names.insert("tan");
    names.insert("tanf");
    names.insert("tanh");
    names.insert("tanhf");
    names.insert("tanhl");
    names.insert("tanl");
    names.insert("tgamma");
    names.insert("tgammaf");
    names.insert("tgammal");
    names.insert("toascii");
    names.insert("tolower");
    names.insert("toupper");
    names.insert("towlower");
    names.insert("towupper");
    names.insert("trunc");
    names.insert("truncf");
    names.insert("truncl");
    names.insert("vfprintf");
    names.insert("vfscanf");
    names.insert("vprintf");
    names.insert("vscanf");
    names.insert("vsnprintf");
    names.insert("vsprintf");
    names.insert("vsscanf");
    names.insert("y0");
    names.insert("y0f");
    names.insert("y0l");
    names.insert("y1");
    names.insert("y1f");
    names.insert("y1l");
    names.insert("yn");
    names.insert("ynf");
    names.insert("ynl");
}

// Returns (mostly) virtual addresses in the .got.plt (ELF) or IAT (PE) sections.
//
// Finds all functions whose first instruction is an indirect jump, and returns the memory address through which the jump
// occurs.  Most of these will be dynamically-linked functions whose only instruction is the JMP and whose address is in the
// .got.plt or IAT.  Return the set of addresses for only those functions whose name appears in the specified set.
//
// Note: we could have also looked in the .dynamic and .dynsym sections, but that would have been ELF-specific
static Disassembler::AddressSet
get_import_addresses(SgAsmInterpretation *interp, const NameSet &whitelist_names)
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
                        gotplt_addr[f] = val->get_absoluteValue();
                }
            }
        }
    } t1;
    t1.traverse(interp, preorder);

    Disassembler::AddressSet retval;
    for (std::map<SgAsmFunction*, rose_addr_t>::iterator i=t1.gotplt_addr.begin(); i!=t1.gotplt_addr.end(); ++i) {
        std::string name = i->first->get_name();
        name = StringUtility::split("@", name, 2)[0];
        if (whitelist_names.find(name)!=whitelist_names.end())
            retval.insert(i->second);
    }
    return retval;
}

// Scan the interpretation to find all the locations that contain addresses of dynamically linked functions and replace them
// with a special address that can be recognized by the fetch-execute loop in fuzz_test().  We do this by adding some entries
// to the memory map that serves up values when needed by memory-reading instructions.  For ELF, we replace the .got.plt
// section; for PE we replace the "Import Address Table" section.  We always replace addresses that don't correspond to a valid
// instruction. On the other hand, when the address is valid, we replace it if either follow-calls is CALL_NONE or follow-calls
// is CALL_BUILTIN and the entry is not whitelisted.
//
// The whitelist_imports are addresses of slots in the import table (ELF .got.plt or PE IAT) that should not be affected.  The
// whitelist_exports is the set of function addresses from import table slots that were whitelisted and not modified.
static void
overmap_dynlink_addresses(SgAsmInterpretation *interp, const InstructionProvidor &insns, FollowCalls follow_calls,
                          MemoryMap *ro_map/*in,out*/, rose_addr_t special_value,
                          const Disassembler::AddressSet &whitelist_imports, Disassembler::AddressSet &whitelist_exports/*out*/)
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
                        rose_addr_t entry_va = base_va + 4*i;
                        rose_addr_t call_va = ByteOrder::le_to_host(buf[i]);
                        if (NULL==insns.get_instruction(call_va) || CALL_NONE==follow_calls) {
                            // Never try to call a function if there's no instruction there
                            buf[i] = special_value;
                            ++nchanges;
                        } else if (whitelist_imports.find(entry_va)!=whitelist_imports.end()) {
                            // Allow white-listed functions to be called (and remember their addresses)
                            whitelist_exports.insert(call_va);
                        } else if (CALL_BUILTIN==follow_calls) {
                            // Don't call dynamically-linked functions that are not white-listed
                            buf[i] = special_value;
                            ++nchanges;
                        }
                    }
                    if (nchanges>0) {
                        MemoryMap::BufferPtr mmbuf = MemoryMap::ByteBuffer::create(buf, nbytes);
                        ro_map->insert(AddressInterval::baseSize(base_va, nbytes),
                                       MemoryMap::Segment(mmbuf, 0, MemoryMap::MM_PROT_READ,
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
          const AddressIdMap &entry2id, const Disassembler::AddressSet &whitelist_exports, FuncAnalyses &funcinfo,
          InsnCoverage &insn_coverage, DynamicCallGraph &dynamic_cg, ConsumedInputs &consumed_inputs)
{
    AddressIdMap::const_iterator id_found = entry2id.find(function->get_entry_va());
    assert(id_found!=entry2id.end());
    int func_id = id_found->second;
    FuncAnalysis &finfo = funcinfo[func_id];
    ++finfo.ntests;
    ClonePolicy policy(opt.params, entry2id, tracer, funcinfo, insn_coverage, dynamic_cg, consumed_inputs);
    policy.set_map(ro_map);
    CloneSemantics semantics(policy);
    AnalysisFault::Fault fault = AnalysisFault::NONE;
    policy.reset(interp, function, &inputs, &insns, pointers, whitelist_exports);
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
    } catch (const BinaryAnalysis::SMTSolver::Exception &e) {
        if (opt.verbosity>=EFFUSIVE)
            std::cerr <<"CloneDetection: analysis terminated by SMT solver exception: " <<e.mesg <<"\n";
        fault = AnalysisFault::SMTSOLVER;
    }

    if (fault) {
        rose_addr_t va = policy.state.registers.ip.is_known() ? policy.state.registers.ip.known_value() : 0;
        if (fault==AnalysisFault::DISASSEMBLY) {
            // We need to assign disassembly faults to the last good address, otherwise they'll never get attached to anything
            // in the listings.  We'll save the actual fault address as the value.
            tracer.emit(last_good_va, EV_FAULT, va, (int)fault);
        } else {
            // Non-disassembly faults will be assigned to the address where they occur, and the previous instruction's address
            // is stored as the value of the fault.
            tracer.emit(va, EV_FAULT, last_good_va, (int)fault);
        }
    }
    
    // Gather the function's outputs before restoring machine state.
    OutputGroup outputs = policy.get_outputs();
    outputs.set_fault(fault);
    if (!outputs.get_retval().first)
        ++finfo.nvoids;
    return outputs;
}

// Commit everything and return a new transaction
static SqlDatabase::TransactionPtr
checkpoint(const SqlDatabase::TransactionPtr &tx, Switches &opt, OutputGroups &ogroups, Tracer &tracer,
           InsnCoverage &insn_coverage, DynamicCallGraph &dynamic_cg, ConsumedInputs &consumed_inputs,
           Progress &progress, size_t ntests_ran, int64_t cmd_id)
{
    SqlDatabase::ConnectionPtr conn = tx->connection();

    progress.message("checkpoint: saving output groups");
    ogroups.save(tx);

    progress.message("checkpoint: saving trace events");
    tracer.flush(tx);

    if (opt.save_coverage && !opt.dry_run) {
        progress.message("checkpoint: saving instruction coverage");
        insn_coverage.flush(tx);
    } else {
        insn_coverage.clear();
    }

    if (opt.save_callgraph && !opt.dry_run) {
        progress.message("checkpoint: saving dynamic call graph");
        dynamic_cg.flush(tx);
    } else {
        dynamic_cg.clear();
    }

    if (opt.save_consumed_inputs && !opt.dry_run) {
        progress.message("checkpoint: saving consumed inputs");
        consumed_inputs.flush(tx);
    } else {
        consumed_inputs.clear();
    }
    
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
        } else if (!strcmp(argv[argno], "--call-graph") || !strcmp(argv[argno], "--call-graph=save")) {
            opt.params.compute_callgraph = opt.save_callgraph = true;
            opt.params.top_callgraph = false;
        } else if (!strcmp(argv[argno], "--call-graph=save-small")) {
            opt.params.compute_callgraph = opt.params.top_callgraph = opt.save_callgraph = true;
        } else if (!strcmp(argv[argno], "--call-graph=compute")) {
            opt.params.compute_callgraph = true;
            opt.params.top_callgraph = opt.save_callgraph = false;
        } else if (!strcmp(argv[argno], "--call-graph=compute-small")) {
            opt.params.compute_callgraph = opt.params.top_callgraph = true;
            opt.save_callgraph = false;
        } else if (!strcmp(argv[argno], "--call-graph=no") || !strcmp(argv[argno], "--no-call-graph")) {
            opt.params.compute_callgraph = opt.params.top_callgraph = opt.save_callgraph = false;
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
        } else if (!strcmp(argv[argno], "--consumed-inputs") || !strcmp(argv[argno], "--consumed-inputs=save")) {
            opt.params.compute_consumed_inputs = opt.save_consumed_inputs = true;
        } else if (!strcmp(argv[argno], "--consumed-inputs=compute")) {
            opt.params.compute_consumed_inputs = true;
            opt.save_consumed_inputs = false;
        } else if (!strcmp(argv[argno], "--no-consumed-inputs") || !strcmp(argv[argno], "--consumed-inputs=no")) {
            opt.params.compute_consumed_inputs = opt.save_consumed_inputs = false;
        } else if (!strcmp(argv[argno], "--coverage") || !strcmp(argv[argno], "--coverage=save")) {
            opt.params.compute_coverage = opt.save_coverage = true;
        } else if (!strcmp(argv[argno], "--coverage=compute")) {
            opt.params.compute_coverage = true;
            opt.save_coverage = false;
        } else if (!strcmp(argv[argno], "--coverage=no") || !strcmp(argv[argno], "--no-coverage")) {
            opt.params.compute_coverage = opt.save_coverage = false;
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
            opt.trace_events = ALL_EVENTS;
        } else if (!strcmp(argv[argno], "--no-trace")) {
            opt.trace_events = 0;
        } else if (!strncmp(argv[argno], "--trace=", 8)) {
            std::vector<std::string> words = StringUtility::split(",", argv[argno]+8, (size_t)-1, true);
            for (size_t i=0; i<words.size(); ++i) {
                unsigned events = EV_NONE;
                bool status = true;
                if (!words[i].empty() && '-'==words[i][0]) {
                    status = false;
                    words[i] = words[i].substr(1);
                } else if (!words[i].empty() && '+'==words[i][0]) {
                    status = true;
                    words[i] = words[i].substr(1);
                }
                if (0==words[i].compare("none")) {
                    events = EV_NONE;
                } else if (0==words[i].compare("reached")) {
                    events = EV_REACHED;
                } else if (0==words[i].compare("branched")) {
                    events = EV_BRANCHED;
                } else if (0==words[i].compare("returned")) {
                    events = EV_RETURNED;
                } else if (0==words[i].compare("fault") || 0==words[i].compare("faults")) {
                    events = EV_FAULT;
                } else if (0==words[i].compare("consume") || 0==words[i].compare("consumed")) {
                    events = EV_CONSUME_INPUT;
                } else if (0==words[i].compare("cfg")) {
                    events = CONTROL_FLOW;
                } else if (0==words[i].compare("all")) {
                    events = ALL_EVENTS;
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
    Disassembler::AddressSet whitelist_exports;         // dynamic functions that should be called
    InputGroup igroup;
    InstructionProvidor insns;
    SgAsmInterpretation *prev_interp = NULL;
    MemoryMap ro_map;
    AddressIdMap entry2id;                              // maps function entry address to function ID
    Tracer tracer;
    InsnCoverage insn_coverage;
    DynamicCallGraph dynamic_cg;
    ConsumedInputs consumed_inputs;
    time_t last_checkpoint = time(NULL);
    size_t ntests_ran=0;
    FuncAnalyses funcinfo;
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
            if (!project) {
                std::cerr <<argv0 <<": problems loading specimen\n";
                exit(1);
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
            Disassembler::AddressSet whitelist_imports = get_import_addresses(interp, builtin_function_names);
            whitelist_exports.clear(); // imports are addresses of import table slots; exports are functions
            overmap_dynlink_addresses(interp, insns, opt.params.follow_calls, &ro_map, GOTPLT_VALUE,
                                      whitelist_imports, whitelist_exports/*out*/);
        }
        
        // Get the results of pointer analysis.  We could have done this before any fuzz testing started, but by doing
        // it here we only need to do it for functions that are actually tested.
        PointerDetectors::iterator ip = pointers.find(func);
        if (ip==pointers.end())
            ip = pointers.insert(std::make_pair(func, detect_pointers(func, function_ids, opt))).first;
        assert(ip!=pointers.end());

        // Run the test
        insn_coverage.current_test(work.func_id, work.igroup_id);
        dynamic_cg.current_test(work.func_id, work.igroup_id);
        tracer.current_test(work.func_id, work.igroup_id, opt.trace_events);
        consumed_inputs.current_test(work.func_id, work.igroup_id);
        timeval start_time, stop_time;
        clock_t start_ticks = clock();
        gettimeofday(&start_time, NULL);
        OutputGroup ogroup = fuzz_test(interp, func, igroup, tracer, insns, &ro_map, ip->second, opt, entry2id,
                                       whitelist_exports, funcinfo, insn_coverage, dynamic_cg, consumed_inputs);
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
        stmt->bind(2, igroup.nconsumed_virtual(IQ_ARGUMENT));
        stmt->bind(3, igroup.nconsumed_virtual(IQ_LOCAL));
        stmt->bind(4, igroup.nconsumed_virtual(IQ_GLOBAL));
        stmt->bind(5, igroup.nconsumed_virtual(IQ_FUNCTION));
        stmt->bind(6, igroup.nconsumed_virtual(IQ_POINTER));
        stmt->bind(7, igroup.nconsumed_virtual(IQ_INTEGER));
        stmt->bind(8, ogroup.get_ninsns());
        stmt->bind(9, ogroup_id);
        stmt->bind(10, ogroup.get_fault());
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
                tx = checkpoint(tx, opt, ogroups, tracer, insn_coverage, dynamic_cg, consumed_inputs,
                                progress, ntests_ran, cmd_id);
            last_checkpoint = time(NULL);
        }
        if (do_exit) {
            tx->rollback();
            break;
        }
        
        prev_work = work;
    }

    // Store results for the analysis that tries to determine whether a function returns a value.
    if (!tx->is_terminated()) {
        SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_funcpartials"
                                                       " (func_id, ncalls, nretused, ntests, nvoids) values"
                                                       " (?,       ?,      ?,        ?,      ?)");
        for (FuncAnalyses::iterator fi=funcinfo.begin(); fi!=funcinfo.end(); ++fi) {
            stmt->bind(0, fi->first);
            stmt->bind(1, fi->second.ncalls);
            stmt->bind(2, fi->second.nretused);
            stmt->bind(3, fi->second.ntests);
            stmt->bind(4, fi->second.nvoids);
            stmt->execute();
        }
    }

    // Cleanup
    if (!tx->is_terminated() && !opt.dry_run)
        tx = checkpoint(tx, opt, ogroups, tracer, insn_coverage, dynamic_cg, consumed_inputs,
                        progress, ntests_ran, cmd_id);
    progress.clear();

    return 0;
}
