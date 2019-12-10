// Runs a program under ptrace starting at random functions.

#include <rose.h>
#include <rose_config.h>


#include <BinaryDebugger.h>
#include <Combinatorics.h>
#include <CommandLine.h>
#include <Partitioner2/Engine.h>
#include <rose_strtoull.h>

#include <boost/algorithm/string/predicate.hpp>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Optional.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Stopwatch.h>

// Instruction pointer member from user_regs_struct in <sys/user.h>
#if __WORDSIZE == 64
# define INSTRUCTION_POINTER rip
#else
# define INSTRUCTION_POINTER eip
#endif

using namespace Rose;
using namespace StringUtility;
using namespace BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;

struct Settings {
    bool performLink;                                   // should dynamic linking be performed inside ROSE?
    std::vector<std::string> libDirs;                   // directories where libraries are stored
    bool showMaps;                                      // show memory maps?
    bool allowSyscalls;                                 // are system calls allowed?
    size_t maxInsns;                                    // max number of instructions to execute; zero means no limit
    size_t nFunctionsTested;                            // number of functions to test
    size_t nInsnsTested;                                // number of instructions to test
    std::string initFunction;                           // when to branch to another function
    bool showInsnTrace;                                 // show executed instructions?
    Settings(): performLink(false), showMaps(false), allowSyscalls(false), maxInsns(0),
                nFunctionsTested(-1), nInsnsTested(0), showInsnTrace(false) {}
};

// Describe and parse the command-line
static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings)
{
    using namespace Sawyer::CommandLine;

    // Generic switches
    SwitchGroup gen = Rose::CommandLine::genericSwitches();

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

    tool.insert(Switch("link")
                .intrinsicValue(true, settings.performLink)
                .doc("Perform dynamic linking within ROSE.  This is in addition to any dynamic linking that might occur when "
                     "the specimen is run natively and which is controlled by the @s{init} switch.  The @s{no-link} switch "
                     "disables linking in ROSE.  The default is to " +
                     std::string(settings.performLink?"":"not ") + "perform the linking step.\n\n"

                     "Note that ROSE's internal dynamic linking algorithm doesn't usually produce the same memory map as "
                     "the Linux loader/dynamic linker.  Even with @man(setarch){8} the mappings are different.  The "
                     "result of using linking in ROSE is therefore twofold: (1) any random function or instruction address that "
                     "this tool chooses from a dynamic library is likely not the same address in the natively loaded "
                     "specimen and will likely result in an immediate segmentation fault, and (2) if the "
                     "@s{no-allow-syscalls} is in effect, any instructions that the native process executes from inside a "
                     "dynamic library will not match instructions inside ROSE and this tool will complain that the "
                     "specimen is executing at an unknown address (or at the very least, the syscall detection will be "
                     "incorrect).  Therefore it is recommended that you carefully check the output from @s{show-maps} "
                     "when using the @s{link} switch."));
    tool.insert(Switch("no-link")
                .key("link")
                .intrinsicValue(false, settings.performLink)
                .hidden(true));

    tool.insert(Switch("libdir")
                .longName("libdirs")
                .argument("directories", anyParser(settings.libDirs))
                .whichValue(SAVE_ALL)
                .doc("Directories that contain libraries that are used if @s{link} is specified. This switch can be specfied "
                     "more than once, and each argument can be a list of colon-separated directory names."));

    tool.insert(Switch("init")
                .argument("name", anyParser(settings.initFunction))
                .doc("If specified, allow the specimen to run until the specified function or address is reached.  Keep in "
                     "mind that binary specimens seldom have function names, and those that do exist are often not the same "
                     "as what would appear in source code.  If @v{name} looks like an address then it will be used as such; "
                     "otherwise if @v{name} cannot be found the specimen's original entry point is used, thus allowing the "
                     "dynamic linker to run before branching to a function.  When not specified, branching occurs "
                     "immediately after the specimen is loaded."));

    tool.insert(Switch("functions")
                .argument("n", nonNegativeIntegerParser(settings.nFunctionsTested))
                .doc("Run at most @v{n} functions selected at random.  The default is to run " +
                     std::string(settings.nFunctionsTested==size_t(-1)?"all functions":
                                 "at most "+plural(settings.nFunctionsTested, "functions")) + "."));

    tool.insert(Switch("instructions")
                .argument("n", nonNegativeIntegerParser(settings.nInsnsTested))
                .doc("Run at most @v{n} instructions selected at random.  The default is to try running " +
                     std::string(settings.nInsnsTested==size_t(-1)?"each instruction":
                                 "at most "+plural(settings.nInsnsTested, "instructions")) + "."));

    tool.insert(Switch("trace")
                .intrinsicValue(true, settings.showInsnTrace)
                .doc("Output each instruction that is executed.  The instructions are output to standard output along "
                     "and precede each line containing the respective run total and termination reason.  Each instruction "
                     "output line begins with the string \"at \".  The @s{no-trace} switch disables this feature.  The "
                     "default is to " + std::string(settings.showInsnTrace?"":"not ") + "not show this information."));
    tool.insert(Switch("no-trace")
                .key("trace")
                .intrinsicValue(false, settings.showInsnTrace)
                .hidden(true));

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
        .doc("Specimens", P2::Engine::specimenNameDocumentation())
        .doc("Bugs",
             "@bullet{Only Linux ELF x86 32-bit executable specimens are supported.}"
             "@bullet{Won't work correctly when address randomization is in effect. Run under \"i386 -R\" to disable "
             "randomization.}");
    
    return parser.with(gen).with(tool).parse(argc, argv).apply();
}

// Run natively and return number of instructions executed and reason for termination.
static std::pair<size_t, std::string>
runNatively(const Settings &settings, const std::string &specimenName, Sawyer::Optional<rose_addr_t> initVa,
            const P2::Partitioner &partitioner, rose_addr_t randomAddress) {
    Stream debug(mlog[DEBUG]);

    Debugger debugger(specimenName, Debugger::CLOSE_FILES);
    if (debugger.isTerminated()) {
        mlog[FATAL] <<"child " <<debugger.isAttached() <<" " <<debugger.howTerminated() <<" before we could gain control\n";
        exit(1);
    }

    // Allow child to run until we hit the desired address.
    if (initVa) {
        debugger.setBreakpoint(*initVa);
        debugger.runToBreakpoint();
        debugger.clearBreakpoint(*initVa);
        if (debugger.isTerminated()) {
            mlog[FATAL] <<"child " <<debugger.isAttached() <<" " <<debugger.howTerminated()
                        <<" without reaching " <<addrToString(*initVa) <<"\n";
            exit(1);
        }
    }
    
    // Show specimen address map so we can verify that the Linux loader used the same addresses we used.
    // We could have shown it earlier, but then we wouldn't have seen the results of dynamic linking.
    if (settings.showMaps) {
        std::cout <<"Linux loader specimen memory map:\n";
        system(("cat /proc/" + numberToString(debugger.isAttached()) + "/maps").c_str());
    }

    // Branch to the starting address
    debug <<"branching to " <<addrToString(randomAddress) <<"\n";
    debugger.executionAddress(randomAddress);

    std::string terminationReason;
    size_t nExecuted = 0;                               // number of instructions executed
    while (1) {
        // Check for and avoid system calls if necessary
        if (!settings.allowSyscalls) {
            rose_addr_t eip = debugger.executionAddress();
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.instructionProvider()[eip]);
            if (!insn || insn->isUnknown()) {
                if (settings.showInsnTrace)
                    std::cout <<"at " <<addrToString(eip) <<": " <<(insn?"no":"unknown") <<" instruction\n";
                terminationReason = "executed at " + addrToString(eip) +" which we don't know about";
                break;
            }
            if (settings.showInsnTrace)
                std::cout <<"at " <<unparseInstructionWithAddress(insn) <<"\n";
            if (insn->get_kind() == x86_int || insn->get_kind() == x86_sysenter) {
                terminationReason = "tried to execute a system call";
                break;
            }
        }

        // Single-step
        if (debug)
            debug <<"single stepping at " <<addrToString(debugger.executionAddress()) <<"\n";
        debugger.singleStep();
        if (debugger.isTerminated()) {
            terminationReason = debugger.howTerminated();
            break;
        }
        ++nExecuted;
        if (settings.maxInsns!=0 && nExecuted>=settings.maxInsns) {
            terminationReason = "reached instruction limit";
            break;
        }
    }
    debugger.terminate();
    return std::make_pair(nExecuted, terminationReason);
}

static bool
isUnnamed(const P2::Function::Ptr &function) {
    return function->name().empty();
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    // Parse command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    if (specimenNames.size() != 1)
        throw std::runtime_error("exactly one binary specimen file should be specified; see --help");
    std::string specimenName = boost::starts_with(specimenNames[0], "run:") ? specimenNames[0].substr(4) : specimenNames[0];
    Stream info(mlog[INFO]);

    // Parse, map, link, and/or relocate
    info <<"performing parse, map, and optional link steps";
    engine.parseContainers(specimenNames);
    Sawyer::Stopwatch loadTimer;
    if (settings.performLink) {
        BinaryLoader *loader = engine.obtainLoader();
        ASSERT_not_null(loader);
        loader->set_perform_dynamic_linking(true);
#if 0 // [Robb P. Matzke 2014-10-09]: not always working, but maybe not needed for this analysis
        loader->set_perform_relocations(true);
#endif
        BOOST_FOREACH (const std::string &paths, settings.libDirs) {
            BOOST_FOREACH (const std::string &path, split(':', paths)) {
                loader->add_directory(path);
            }
        }
    }
    engine.loadSpecimens(specimenNames);
    info <<"; completed in " <<loadTimer <<" seconds\n";

    // Disassemble, partition, and get list of functions.  No need to create an AST.
    info <<"performing disassemble and partition";
    Sawyer::Stopwatch partitionTimer;
    P2::Partitioner partitioner = engine.partition();
    if (settings.showMaps) {
        std::cout <<"ROSE loader specimen memory map:\n";
        partitioner.memoryMap()->dump(std::cout);
    }
    std::vector<P2::Function::Ptr> functions = partitioner.functions();
    info <<"; completed in " <<partitionTimer <<" seconds.\n";
    info <<"found " <<plural(functions.size(), "functions") <<"\n";

    // Consider only functions that have names
    functions.erase(std::remove_if(functions.begin(), functions.end(), isUnnamed), functions.end());
    info <<"functions with names: " <<functions.size() <<"\n";

    // How far should we let the specimen run before branching to a function?
    Sawyer::Optional<rose_addr_t> initVa;
    if (!settings.initFunction.empty()) {
        const char *s = settings.initFunction.c_str();
        char *rest;
        rose_addr_t va = rose_strtoull(s, &rest, 0);
        if (!*rest) {
            initVa = va;
        } else {
            BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
                if (function->name() == settings.initFunction) {
                    initVa = function->address();
                    break;
                }
            }
            if (!initVa) {
                mlog[FATAL] <<"could not find function \"" <<settings.initFunction <<"\"\n";
                exit(1);
            }
        }
    }

    if (settings.nFunctionsTested > 0) {
        // Select the functions to run.
        if (settings.nFunctionsTested < functions.size()) {
            info <<"limiting to " <<plural(settings.nFunctionsTested, "functions") <<" selected at random.\n";
            Combinatorics::shuffle(functions);
            functions.resize(settings.nFunctionsTested);
        }

        // Run the specimen natively under a debugger.
        info <<"running selected functions natively under a debugger\n";
        Sawyer::ProgressBar<size_t> progress(functions.size(), mlog[MARCH]);
        BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
            mlog[WHERE] <<"running function " <<addrToString(function->address()) <<" \"" <<function->name() <<"\"\n";
            ++progress;
            std::pair<size_t, std::string> result = runNatively(settings, specimenName, initVa, partitioner,
                                                                function->address());
            std::cout <<addrToString(function->address()) <<", " <<result.first <<", "
                      <<"\"" <<cEscape(function->name()) <<"\" " <<result.second <<"\n";
        }
    }

    if (settings.nInsnsTested > 0) {
        // Get list of all instructions.
        std::vector<SgAsmInstruction*> insns;
        BOOST_FOREACH (const P2::BasicBlock::Ptr &bblock, partitioner.basicBlocks()) {
            const std::vector<SgAsmInstruction*> &bi = bblock->instructions();
            insns.insert(insns.end(), bi.begin(), bi.end());
        }
        info <<"found " <<plural(insns.size(), "instructions") <<"\n";


        // Select instructions to run
        if (settings.nInsnsTested < insns.size()) {
            info <<"limiting to " <<plural(settings.nInsnsTested, "instructions") <<" selected at random.\n";
            Combinatorics::shuffle(insns);
            insns.resize(settings.nInsnsTested);
        }

        // Run the speciment natively at each selected instruction.
        info <<"running selected instructions natively under a debugger\n";
        Sawyer::ProgressBar<size_t> progress(insns.size(), mlog[MARCH]);
        BOOST_FOREACH (SgAsmInstruction *insn, insns) {
            mlog[WHERE] <<"running instruction " <<addrToString(insn->get_address()) <<"\"\n";
            ++progress;
            std::pair<size_t, std::string> result = runNatively(settings, specimenName, initVa, partitioner,
                                                                insn->get_address());
            std::cout <<addrToString(insn->get_address()) <<", " <<result.first <<", " <<"instruction " <<result.second <<"\n";
        }
    }
}
