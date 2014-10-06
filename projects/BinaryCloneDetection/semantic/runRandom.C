// Runs a program under ptrace starting at random functions.

#include <rose.h>
#include <rose_config.h>

#if !defined(HAVE_ASM_LDT_H) || !defined(HAVE_SYS_TYPES_H) || !defined(HAVE_SYS_WAIT_H)

int main(int argc, char *argv[]) {
    std::cerr <<argv[0] <<" is not supported on this platform.\n";
    exit(1);
}

#else

#include <Partitioner2/Engine.h>
#include <rose_strtoull.h>

#include <errno.h>
#include <sawyer/CommandLine.h>
#include <sawyer/Optional.h>
#include <sawyer/ProgressBar.h>
#include <sawyer/Stopwatch.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

// Instruction pointer member from user_regs_struct in <sys/user.h>
#if __WORDSIZE == 64
# define INSTRUCTION_POINTER rip
#else
# define INSTRUCTION_POINTER eip
#endif

using namespace rose;
using namespace StringUtility;
using namespace Sawyer::Message::Common;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;

struct Settings {
    bool showMaps;                                      // show memory maps?
    bool allowSyscalls;                                 // are system calls allowed?
    size_t maxInsns;                                    // max number of instructions to execute; zero means no limit
    std::string initFunction;                           // when to branch to another function
    Settings(): showMaps(false), allowSyscalls(false), maxInsns(0) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    // Switches for this tool
    SwitchGroup tool("Tool-specific switches");

    tool.insert(Switch("show-maps")
                .intrinsicValue(true, settings.showMaps)
                .doc("Show the Linux memory map and the ROSE memory map for the specimen so that a human can compare "
                     "them and make sure that they're compatible.  The ROSE map will probably lack the dynamic loader, "
                     "the [vdso] and the [stack], but that's okay.  The default is to not show this information."));

    tool.insert(Switch("allow-syscalls")
                .intrinsicValue(true, settings.allowSyscalls)
                .doc("Allow child process to execute system calls (or any x86 INT instruction).  Allowing such things is "
                     "dangerous because the child is running on our system with our effective user ID "
                     "and is making system calls even though it is almost certainly not initialized properly.  We have no "
                     "idea what those system calls might do.  The @s{no-allow-syscalls} disables all INT instructions. The "
                     "default is to " + std::string(settings.allowSyscalls?"":"not ") + " allow system calls.  Note, "
                     "disallowing syscalls is based on whether the executable as originally loaded has an INT instruction "
                     "at the address being executed; it will not detect syscalls in self-modifying code."));
    tool.insert(Switch("no-allow-syscalls")
                .key("allow-syscalls")
                .intrinsicValue(false, settings.allowSyscalls)
                .hidden(true));

    tool.insert(Switch("limit")
                .argument("n", nonNegativeIntegerParser(settings.maxInsns), "1000000")
                .doc("Limit the number of instructions that should be executed for any particular function starting "
                     "address.  The limit can be a number, or defaults to one million.  If this switch is not specified then " +
                     (settings.maxInsns?
                      "at most " + plural(settings.maxInsns, "instructions") + "are executed.":
                      std::string("there is no limit."))));

    tool.insert(Switch("init")
                .argument("name", anyParser(settings.initFunction))
                .doc("If specified, allow the specimen to run until the specified function or address is reached.  Keep in "
                     "mind that binary specimens seldom have function names, and those that do exist are often not the same "
                     "as what would appear in source code.  If @v{name} looks like an address then it will be used as such; "
                     "otherwise if @v{name} cannot be found the specimen's original entry point is used, thus allowing the "
                     "dynamic linker to run before branching to a function.  When not specified, branching occurs "
                     "immediately after the specimen is loaded."));

    Parser parser;
    parser
        .purpose("load binary specimen and execute under debugger")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_name}")
        .doc("Description",
             "Parses, loads, and partitions the specified binary specimen to get a list of function addresses. Then tries to "
             "run the specimen under a debugger at each function address and count how many instructions can be executed before "
             "the specimen terminates.\n\n"

             "The results are printed on standard output with one line per function.  Each line is a list of three "
             "comma-separated ASCII values:  the address of the function tested, the number of instructions that were "
             "executed, and the reason for terminating the run.  The reason will begin with the function name as a "
             "C-escaped string in double quotes.  For instance,\n\n"

             "    0x080480a0, 3, \"\" terminated with segmentation fault")
        .doc("Bugs",
             "Only Linux ELF x86 32-bit executable specimens are supported.");
    
    return parser.with(gen).with(tool).parse(argc, argv).apply();
}

static void
sendCommand(enum __ptrace_request request, pid_t pid, void *addr=NULL, void *data=NULL) {
    if (-1 == ptrace(request, pid, addr, data)) {
        kill(pid, SIGKILL);
        std::string s;
        switch (request) {
            case PTRACE_TRACEME:        s = "traceme";          break;
            case PTRACE_GETREGS:        s = "getregs";          break;
            case PTRACE_SETREGS:        s = "setregs";          break;
            case PTRACE_SINGLESTEP:     s = "singlestep";       break;
            default:                    s = "?";                break;
        }
        throw std::runtime_error("ptrace(" + s + ") failed: " + strerror(errno));
    }
}

static int
waitForChild(pid_t pid) {
    while (1) {
        int wstat;
        if (-1 == (pid = waitpid(pid, &wstat, 0)))
            throw std::runtime_error("wait failed: " + std::string(strerror(errno)));
        if (WIFSTOPPED(wstat) && WSTOPSIG(wstat)!=SIGTRAP) {
            sendCommand(PTRACE_SINGLESTEP, pid, 0, (void*)WSTOPSIG(wstat)); // deliver signal
        } else {
            return wstat;
        }
    }
}

static bool
isTerminated(int wstat, std::string &why /*out*/) {
    if (WIFEXITED(wstat)) {
        why = "exit with status " + numberToString(WEXITSTATUS(wstat));
        return true;
    } else if (WIFSIGNALED(wstat)) {
        why = strsignal(WTERMSIG(wstat));
        if (!why.empty() && isupper(why[0]))
            why[0] = tolower(why[0]);
        why = "terminated with " + why;
        return true;
    }
    return false;
}

// Allow child to run until it hits the specified virtual address or dies.  Returns false if it died.
static bool
executeUntilAddress(pid_t pid, rose_addr_t va, std::string &terminationReason /*out*/) {
    terminationReason = "";
    while (1) {
        user_regs_struct regs;
        sendCommand(PTRACE_GETREGS, pid, 0, &regs);
        if (regs.INSTRUCTION_POINTER == va)
            return true;

        sendCommand(PTRACE_SINGLESTEP, pid);
        int wstat = waitForChild(pid);
        if (isTerminated(wstat, terminationReason /*out*/))
            return false;
    }
    return true;
}

static void
runNatively(const Settings &settings, const std::string &specimenName, Sawyer::Optional<rose_addr_t> initVa,
            const P2::Partitioner &partitioner, P2::Function::Ptr function) {
    mlog[WHERE] <<"running function " <<addrToString(function->address()) <<" \"" <<function->name() <<"\"\n";
    Stream debug(mlog[DEBUG]);

    // Fork and execute binary specimen expecting to be debugged with ptrace
    const pid_t child = fork();
    ASSERT_always_require2(child>=0, "fork failed");
    if (0==child) {
        sendCommand(PTRACE_TRACEME, 0);
        execl(specimenName.c_str(), specimenName.c_str(), NULL);
        throw std::runtime_error("execl failed: " + std::string(strerror(errno)));
    }

    // We will be notified as soon as child's exec occurs
    int wstat = waitForChild(child);
    std::string terminationReason;
    if (isTerminated(wstat, terminationReason /*out*/)) {
        mlog[FATAL] <<"child " <<child <<" " <<terminationReason <<" before we could gain control\n";
        exit(1);
    }
    ASSERT_require(WIFSTOPPED(wstat) && WSTOPSIG(wstat)==SIGTRAP);

    // Show specimen address map so we can verify that the Linux loader used the same addresses we used.
    if (settings.showMaps) {
        std::cout <<"Linux loader specimen memory map:\n";
        system(("cat /proc/" + numberToString(child) + "/maps").c_str());
        std::cout <<"ROSE loader specimen memory map:\n";
        partitioner.memoryMap().dump(std::cout);
    }

    // Allow child to run until we hit the desired address.
    if (initVa && !executeUntilAddress(child, *initVa, terminationReason /*out*/)) {
        mlog[FATAL] <<"child " <<child <<" " <<terminationReason <<" without reaching " <<addrToString(*initVa) <<"\n";
        exit(1);
    }

    // Branch to the function address
    debug <<"Branching to function entry point\n";
    user_regs_struct regs;
    sendCommand(PTRACE_GETREGS, child, 0, &regs);
    regs.INSTRUCTION_POINTER = function->address();
    sendCommand(PTRACE_SETREGS, child, 0, &regs);

    size_t nExecuted = 0;                               // number of instructions executed
    while (1) {
        // Check for and avoid system calls if necessary
        if (!settings.allowSyscalls) {
            sendCommand(PTRACE_GETREGS, child, 0, &regs);
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.instructionProvider()[regs.INSTRUCTION_POINTER]);
            if (!insn || insn->isUnknown()) {
                terminationReason = "executed at " + addrToString(regs.INSTRUCTION_POINTER) +" which we don't know about";
                break;
            }
            if (insn->get_kind() == x86_int) {
                terminationReason = "tried to execute a system call";
                break;
            }
        }

        // Single-step
        if (debug) {
            sendCommand(PTRACE_GETREGS, child, 0, &regs);
            debug <<"single stepping at " <<addrToString(regs.INSTRUCTION_POINTER) <<"\n";
        }
        sendCommand(PTRACE_SINGLESTEP, child, 0, 0);
        wstat = waitForChild(child);
        if (isTerminated(wstat, terminationReason /*out*/))
            break;
        ASSERT_require(WIFSTOPPED(wstat) && WSTOPSIG(wstat)==SIGTRAP);
        ++nExecuted;
        if (settings.maxInsns!=0 && nExecuted>=settings.maxInsns) {
            terminationReason = "reached instruction limit";
            break;
        }
    }
    kill(child, SIGKILL);                               // make sure child is really dead
    waitpid(child, &wstat, 0);                          // and reap it

    // Results
    std::cout <<addrToString(function->address()) <<", " <<nExecuted <<", "
              <<"\"" <<cEscape(function->name()) <<"\" " <<terminationReason <<"\n";
}

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();
    mlog = Sawyer::Message::Facility("tool");
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse command-line
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    if (specimenNames.size() != 1)
        throw std::runtime_error("exactly one binary specimen file should be specified; see --help");

    // Parse, load, disassemble, partition, and get list of functions.  No need to create an AST.
    mlog[INFO] <<"performing parse, load, disassemble and partition";
    Sawyer::Stopwatch partitionTimer;
    P2::Engine engine;
    P2::Partitioner partitioner = engine.loadAndPartition(specimenNames);
    std::vector<P2::Function::Ptr> functions = partitioner.functions();
    mlog[INFO] <<"; completed in " <<partitionTimer <<" seconds.\n";
    mlog[INFO] <<"found " <<plural(functions.size(), "functions") <<"\n";

    // Make sure the specimen is 32-bit x86 Linux ELF executable
    Sawyer::Optional<rose_addr_t> oep;                  // original entry point
    SgAsmInterpretation *interp = engine.interpretation();
    BOOST_FOREACH (SgAsmGenericHeader *hdr, interp->get_headers()->get_headers()) {
        SgAsmElfFileHeader *elf = isSgAsmElfFileHeader(hdr);
        if (!elf)
            throw std::runtime_error("only ELF binary specimens are supported");
        if (elf->get_exec_format()->get_purpose() != SgAsmGenericFormat::PURPOSE_EXECUTABLE &&
            elf->get_exec_format()->get_purpose() != SgAsmGenericFormat::PURPOSE_UNSPECIFIED) {
            throw std::runtime_error("only executables are supported");
        }
        if (elf->get_exec_format()->get_abi() != SgAsmGenericFormat::ABI_LINUX &&
            elf->get_exec_format()->get_abi() != SgAsmGenericFormat::ABI_UNSPECIFIED) {
            throw std::runtime_error("only Linux executables are supported");
        }
        if (0 == (elf->get_isa() & SgAsmGenericFormat::ISA_IA32_Family) &&
            elf->get_isa() != SgAsmGenericFormat::ISA_UNSPECIFIED) {
            throw std::runtime_error("only Intel x86 executables are supported");
        }
        if (32!=partitioner.instructionProvider().instructionPointerRegister().get_nbits())
            throw std::runtime_error("only 32-bit binary specimens are supported");
        if (!oep)
            oep = elf->get_entry_rva() + elf->get_base_va();
    }
    ASSERT_require2(oep, "no OEP");

    // How far should we let the specimen run before branching to a function?
    Sawyer::Optional<rose_addr_t> initVa;
    if (!settings.initFunction.empty()) {
        const char *s = settings.initFunction.c_str();
        char *rest;
        rose_addr_t va = rose_strtoull(s, &rest, 0);
        if (!*rest) {
            initVa = va;
        } else {
            bool foundFunction = false;
            BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
                if (function->name() == settings.initFunction) {
                    foundFunction = true;
                    initVa = function->address();
                    break;
                }
            }
            if (!foundFunction) {
                mlog[WARN] <<"could not find \"" <<settings.initFunction <<"\""
                           <<"; using original entry point instead (" <<addrToString(*oep) <<")\n";
            }
        }
    }
    
    // Run the specimen natively under a debugger.
    mlog[INFO] <<"running each function natively under a debugger\n";
    Sawyer::ProgressBar<size_t> progress(functions.size(), mlog[INFO]);
    BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
        ++progress;
        runNatively(settings, specimenNames[0], initVa, partitioner, function);
    }
}

#endif
