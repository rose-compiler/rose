// Tests instruction semantics by running them on one basic block at a time.

#include <rose.h>

#include <Diagnostics.h>
#include <Disassembler.h>
#include <DispatcherX86.h>
#include <IntervalSemantics2.h>
#include <NullSemantics2.h>
#include <PartialSymbolicSemantics2.h>
#include <Partitioner2/Engine.h>
#include <SymbolicSemantics2.h>
#include <TestSemantics2.h>
#include <TraceSemantics2.h>
#include <YicesSolver.h>

//=============================================================================================================================
//                                      User-contributed semantics
//
// Instructions for adding your own semantics:
//   0. Choose a C preprocessor symbol and surround all your additions with #ifdef, but do not define that symbol within this
//      file -- define it on the compiler command-line instead.  The symbol name should probably start with your name or your
//      organization's name.
//
//   1. Include any header files you need.  Document your extension at this same location using EXAMPLE_EXTENSIONS as a
//      template.
//
//   2. Augment the makeProtoVal, makeRegisterState, makeMemoryState, and makeRiscOperators functions so they construct
//      instances of your classes.  Keep the lists sorted alphabetically.
//
//   3. Optionally augment testSemanticsApi to test that your semantics layers behave correctly as far as they can be tested
//      to satisfy the requirements of the samentics framework.
//
//   4. Optionally submit your changes to the ROSE compiler team so you don't need to keep patching this file.
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Added 2014-10-04 Walter Wego <nobody@example.com>
// Example.com supplies the following semantics available at http://example.com/downloads:
//    * com::example::semantics is our nifty, state of the art, super duper, all inclusive semanticizer.
//      This package uses semantics to solve Fermat's last theorem based on chaotic patterns found in machine instructions.
// More information is available at http://example.com/semantics.html.
#ifdef EXAMPLE_EXTENSIONS
# include <example/semantics.h>
#endif

//                                      End of user-contributed semantics
//=============================================================================================================================

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace Sawyer::Message::Common;                // DEBUG, INFO, WARN, ERROR etc.
namespace P2 = rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

// Command-line settings
struct Settings {
    std::string isaName;                                // name of instruction set architecture
    std::string valueClassName;                         // name of semantic values class, abbreviated
    std::string rstateClassName;                        // name of register state class, abbreviated
    std::string mstateClassName;                        // name of memory state class, abbreviated
    std::string opsClassName;                           // name of RiscOperators class, abbreviated
    std::string solverName;                             // name of SMT solver
    bool trace;                                         // use TraceSemantics?
    bool showUseDef;                                    // show use-def information if available?
    bool showInitialValues;                             // show initial values in register states?
    bool showStates;                                    // show register and memory state after each instruction?
    bool showInitialState;                              // show initial state if showStates is set?
    AddressInterval bblockInterval;                     // which basic blocks to process
    Settings()
        : trace(false), showUseDef(true), showInitialValues(false), showStates(true), showInitialState(false),
          bblockInterval(AddressInterval::whole()) {}
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    //------------------------------------------------
    SwitchGroup load("Specimen loading switches");
    load.insert(Switch("isa")
                .argument("architecture", anyParser(settings.isaName))
                .doc("Instruction set architecture.  If the specimen has a binary container (e.g., ELF, PE) then the "
                     "architecture is obtained from the container (overridden by this switch), otherwise the user must "
                     "specify an architecture. Use \"@s{isa} list\" to see a list of recognized names."));

    //------------------------------------------------
    SwitchGroup sem("Semantics class switches");
    sem.insert(Switch("value")
               .argument("class", anyParser(settings.valueClassName))
               .doc("Name of the class that represents semantic values. If not specified, use the default value class for "
                    "the semantics class specified with @s{semantics}. Use \"@s{value} list\" to get a list of "
                    "recognized names."));

    sem.insert(Switch("rstate")
               .argument("class", anyParser(settings.rstateClassName))
               .doc("Name of the class that represents the register state.  If not specified, use the default register "
                    "class for the semantics class specified with @s{semantics}. Use \"@s{rstate} list\" to get a list "
                    "of recognized names."));

    sem.insert(Switch("mstate")
               .argument("class", anyParser(settings.mstateClassName))
               .doc("Name of the class that represents the memory state.  If not specified, use the default memory class "
                    "for the semantics class specified with @s{semantics}. Use \"@s{mstate} list\" to get a list "
                    "of recognized names."));

    sem.insert(Switch("semantics")
               .argument("class", anyParser(settings.opsClassName))
               .doc("Name of the class that represents the semantic RiscOperators.  This switch is required and provides "
                    "the default values for @s{value}, @s{rstate}, and @s{mstate}.  Use \"@s{semantics} list\" to get a "
                    "list of recognized names."));

    //------------------------------------------------
    SwitchGroup ctl("Semantics control switches");
    ctl.insert(Switch("solver")
               .argument("name", anyParser(settings.solverName))
               .doc("Enables use of an SMT solver of the specified class.  See \"@s{solver} list\" for the list of "
                    "recognized names.\n"));

    ctl.insert(Switch("trace")
               .intrinsicValue(true, settings.trace)
               .doc("Turns on tracing of the RiscOps by encapsulating it in TraceSemantics.  The @s{no-trace} switch "
                    "turns this feature off.  The default is to " + std::string(settings.trace?"":"not ") + "trace."));
    ctl.insert(Switch("no-trace")
               .key("trace")
               .intrinsicValue(false, settings.trace)
               .hidden(true));

    ctl.insert(Switch("bblock")
               .longName("bblocks")
               .argument("interval", P2::addressIntervalParser(settings.bblockInterval))
               .doc("Restrict semantics to only those basic blocks that start at the specified address(es). The @v{interval} "
                    "can be a single address, a pair of address separated by \",\" (inclusive) or \"-\" (end is not inclusive), "
                    "or an address and size separated by \"+\".  The default is to process all basic blocks.\n"));
               

    //------------------------------------------------
    SwitchGroup out("Output switches");
    out.insert(Switch("show-states")
               .intrinsicValue(true, settings.showStates)
               .doc("Turns on display of state information after each instruction is processed.  State output can be "
                    "disabled with @s{no-show-states}. The default is to " + std::string(settings.showStates?"":"not ") +
                    "show this information."));
    out.insert(Switch("no-show-states")
               .key("show-states")
               .intrinsicValue(false, settings.showStates)
               .hidden(true));

    out.insert(Switch("usedef")
               .intrinsicValue(true, settings.showUseDef)
               .doc("Turns on display of use-def information if the semantics value type supports it and @s{show-states} "
                    "is enabled.  The @s{no-usedef} switch disables this feature. The default is to " +
                    std::string(settings.showUseDef?"":"not ") + "show this information."));
    out.insert(Switch("no-usedef")
               .key("usedef")
               .intrinsicValue(false, settings.showUseDef)
               .hidden(true));

    out.insert(Switch("show-initial-values")
               .intrinsicValue(true, settings.showInitialValues)
               .doc("Turns on display of initial values stored in register states when @s{show-states} is enabled.  When "
                    "disabled (@s{no-show-initial-values}) registers whose value is a comment which is the name of the "
                    "register followed by \"_0\" are not listed.  The default is to " +
                    std::string(settings.showInitialValues?"":"not ") + "show initial values."));
    out.insert(Switch("no-show-initial-values")
               .key("show-initial-values")
               .intrinsicValue(false, settings.showInitialValues)
               .hidden(true));

    out.insert(Switch("show-initial-state")
               .intrinsicValue(true, settings.showInitialState)
               .doc("Turns on display of the initial register and memory state if @s{show-states} is enabled. This "
                    "output can be disabled with @s{no-show-initial-state}.  The default is to " +
                    std::string(settings.showInitialState?"":"not ") + "show this information."));
    out.insert(Switch("no-show-initial-state")
               .key("show-initial-state")
               .intrinsicValue(false, settings.showInitialState)
               .hidden(true));
    
    //------------------------------------------------
    Parser parser;
    parser
        .purpose("runs instruction semantics for testing")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Testing")
        .doc("synopsis",
             "@prop{programName} @s{semantics} @v{class} [@v{switches}] @v{specimen_name}")
        .doc("description",
             "Parses, disassembles and partitions the specimens given as positional arguments on the command-line and then "
             "instantiates and runs the specified instruction semantics for each basic block.  A semantic class must be "
             "specified with the @s{semantics} switch, and its default component types can be overridden with other "
             "class-specifying switches. However, not all combinations of semantic class and component types make sense (a "
             "well implemented class will complain when the combination is nonsensical).")
        .doc("Specimens", P2::Engine::specimenNameDocumentation());
    
    return parser.with(gen).with(load).with(sem).with(ctl).with(out).parse(argc, argv).apply();
}

static SMTSolver *
makeSolver(const Settings &settings) {
    if (settings.solverName == "list") {
        std::cout <<"SMT solver names:\n"
                  <<"  yices            rose::BinaryAnalysis::YicesSolver\n";
        exit(0);
    } else if (settings.solverName == "") {
        return NULL;                                    // solvers are optional
    } else if (settings.solverName == "yices") {
        YicesSolver *solver = new YicesSolver;
        solver->set_linkage(YicesSolver::LM_LIBRARY);
        return solver;
    } else {
        throw std::runtime_error("unrecognized SMT solver name \"" + settings.solverName + "\"; see --solver=list\n");
    }
}

static BaseSemantics::SValuePtr
makeProtoVal(const Settings &settings) {
    const std::string &className = settings.valueClassName;
    if (className == "list") {
        std::cout <<"value class names:\n"
#ifdef EXAMPLE_EXTENSIONS
                  <<"  example          com::example::semantics::SValue\n"
#endif
                  <<"  interval         rose::BinaryAnalysis::InstructionSemantics2::IntervalSemantics::SValue\n"
                  <<"  null             rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::SValue\n"
                  <<"  partial          rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics::SValue\n"
                  <<"  partitioner2     rose::BinaryAnalysis::Partitioner2::Semantics::SValue\n"
                  <<"  symbolic         rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::SValue\n";
        exit(0);
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::SValue::instance();
#endif
    } else if (className == "interval") {
        return IntervalSemantics::SValue::instance();
    } else if (className == "null") {
        return NullSemantics::SValue::instance();
    } else if (className == "partial") {
        return PartialSymbolicSemantics::SValue::instance();
    } else if (className == "partitioner2") {
        return P2::Semantics::SValue::instance();
    } else if (className == "symbolic") {
        return SymbolicSemantics::SValue::instance();
    } else {
        throw std::runtime_error("unrecognized value class \"" + className + "\"; see --value=list\n");
    }
}

static BaseSemantics::RegisterStatePtr
makeRegisterState(const Settings &settings, const BaseSemantics::SValuePtr &protoval, const RegisterDictionary *regdict) {
    const std::string &className = settings.rstateClassName;
    if (className == "list") {
        std::cout <<"register state class names:\n"
#ifdef EXAMPLE_EXTENSIONS
                  <<"  example          com::example::semantics::RegisterState\n"
#endif
                  <<"  generic          rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  interval         rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  null             rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::RegisterState\n"
                  <<"  partial          rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  partitioner2     rose::BinaryAnalysis::Partitioner2::Semantics::RegisterState\n"
                  <<"  symbolic         rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  x86              rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateX86\n";
        exit(0);
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::RegisterState::instance(protoval, regdict);
#endif
    } else if (className == "generic") {
        return BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
    } else if (className == "interval") {
        return BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
    } else if (className == "null") {
        return NullSemantics::RegisterState::instance(protoval, regdict);
    } else if (className == "partial") {
        return BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
    } else if (className == "partitioner2") {
        return P2::Semantics::RegisterState::instance(protoval, regdict);
    } else if (className == "symbolic") {
        return BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
    } else if (className == "x86") {
        return BaseSemantics::RegisterStateX86::instance(protoval, regdict);
    } else {
        throw std::runtime_error("unrecognized register state class name \"" + className + "\"; see --rstate=list\n");
    }
}

static BaseSemantics::MemoryStatePtr
makeMemoryState(const Settings &settings, const BaseSemantics::SValuePtr &protoval, const BaseSemantics::SValuePtr &protoaddr,
                const RegisterDictionary *regdict) {
    const std::string &className = settings.mstateClassName;
    if (className == "list") {
        std::cout <<"memory state class names:\n"
#ifdef EXAMPLE_EXTENSIONS
                  <<"  example          com::example::semantics::MemoryState\n"
#endif
                  <<"  interval         rose::BinaryAnalysis::InstructionSemantics2::IntervalSemantics::MemoryState\n"
                  <<"  null             rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::MemoryState\n"
                  <<"  partial          rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics default\n"
                  <<"  partitioner2     rose::BinaryAnalysis::Partitioner2::Semantics::MemoryState\n"
                  <<"  symbolic         rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::MemoryState\n";
        exit(0);
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::MemoryState::instance(protoval, protoaddr);
#endif
    } else if (className == "interval") {
        return IntervalSemantics::MemoryState::instance(protoval, protoaddr);
    } else if (className == "null") {
        return NullSemantics::MemoryState::instance(protoval, protoaddr);
    } else if (className == "partial") {
        BaseSemantics::RiscOperatorsPtr ops = PartialSymbolicSemantics::RiscOperators::instance(regdict);
        return ops->get_state()->get_memory_state();
    } else if (className == "partitioner2") {
        return P2::Semantics::MemoryState::instance(protoval, protoaddr);
    } else if (className == "symbolic") {
        return SymbolicSemantics::MemoryState::instance(protoval, protoaddr);
    } else {
        throw std::runtime_error("unrecognized memory state class name \"" + className + "\"; see --mstate=list\n");
    }
}

static BaseSemantics::RiscOperatorsPtr
makeRiscOperators(const Settings &settings, const RegisterDictionary *regdict) {
    const std::string &className = settings.opsClassName;
    if (className == "list") {
        std::cout <<"semantic class names:\n"
#ifdef EXAMPLE_EXTENSIONS
                  <<"  example          com::example::semantics::RiscOperators\n"
#endif
                  <<"  interval         rose::BinaryAnalysis::InstructionSemantics2::IntervalSemantics::RiscOperators\n"
                  <<"  null             rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::RiscOperators\n"
                  <<"  partial          rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics::RiscOperators\n"
                  <<"  partitioner2     rose::BinaryAnalysis::Partitioner2::Semantics::RiscOperators\n"
                  <<"  symbolic         rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RiscOperators\n";
        exit(0);
    } else if (className.empty()) {
        throw std::runtime_error("--semantics switch is required");
    }
    
    SMTSolver *solver = makeSolver(settings);
    BaseSemantics::SValuePtr protoval = makeProtoVal(settings);
    BaseSemantics::RegisterStatePtr rstate = makeRegisterState(settings, protoval, regdict);
    BaseSemantics::MemoryStatePtr mstate = makeMemoryState(settings, protoval, protoval, regdict);
    BaseSemantics::StatePtr state = BaseSemantics::State::instance(rstate, mstate);

    if (0) {
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::RiscOperators::instance(state, solver);
#endif
    } else if (className == "interval") {
        return IntervalSemantics::RiscOperators::instance(state, solver);
    } else if (className == "null") {
        return NullSemantics::RiscOperators::instance(state, solver);
    } else if (className == "partial") {
        return PartialSymbolicSemantics::RiscOperators::instance(state, solver);
    } else if (className == "partitioner2") {
        return P2::Semantics::RiscOperators::instance(state, solver);
    } else if (className == "symbolic") {
        return SymbolicSemantics::RiscOperators::instance(state, solver);
    } else {
        throw std::runtime_error("unrecognized semantic class name \"" + className + "\"; see --semantics=list\n");
    }
}

static void
adjustSettings(Settings &settings) {
    if (settings.opsClassName=="list") {
    } else if (settings.valueClassName=="list" || settings.rstateClassName=="list" || settings.mstateClassName=="list") {
        if (settings.opsClassName.empty())
            settings.opsClassName = "none";
    }
    if (settings.valueClassName.empty())
        settings.valueClassName = settings.opsClassName;
    if (settings.rstateClassName.empty())
        settings.rstateClassName = settings.opsClassName;
    if (settings.mstateClassName.empty())
        settings.mstateClassName = settings.opsClassName;
}

// Test the API for various combinations of classes.  Sorry this is so long and doesn't handle every case -- that's the pitfal
// of trying to mix runtime configuration and C++ templates.
static void
testSemanticsApi(const Settings &settings, const P2::Partitioner &partitioner) {
    std::cout <<"=====================================================================================\n"
              <<"=== Performing basic API tests                                                    ===\n"
              <<"=====================================================================================\n";
    const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
    BaseSemantics::RiscOperatorsPtr ops = makeRiscOperators(settings, regdict);
    if (settings.opsClassName == settings.valueClassName &&
        settings.opsClassName == settings.rstateClassName &&
        settings.opsClassName == settings.mstateClassName) {
        if (0) {
#ifdef EXAMPLE_EXTENSIONS
        } else if (settings.opsClassName == "example") {
            TestSemantics<com::example::semantics::SValuePtr, com::example::semantics::RegisterStatePtr,
                          com::example::semantics::MemoryStatePtr, com::example::semantics::StatePtr,
                          com::example::semantics::RiscOperatorsPtr> tester;
            tester.test(ops);
#endif
        } else if (settings.opsClassName == "interval") {
            TestSemantics<IntervalSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                          IntervalSemantics::MemoryStatePtr, BaseSemantics::StatePtr,
                          IntervalSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName == "null") {
            TestSemantics<
                NullSemantics::SValuePtr, NullSemantics::RegisterStatePtr, NullSemantics::MemoryStatePtr,
                BaseSemantics::StatePtr, NullSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName == "partial") {
            TestSemantics<PartialSymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                          BaseSemantics::MemoryCellListPtr, BaseSemantics::StatePtr,
                          PartialSymbolicSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName == "partitioner2") {
            TestSemantics<P2::Semantics::SValuePtr, P2::Semantics::RegisterStateGenericPtr,
                          P2::Semantics::MemoryStatePtr, P2::Semantics::StatePtr,
                          P2::Semantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName == "symbolic") {
            TestSemantics<SymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                          SymbolicSemantics::MemoryStatePtr, BaseSemantics::StatePtr,
                          SymbolicSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else {
            std::cout <<"tests skipped.\n\n";
            mlog[WARN] <<"API for " <<settings.opsClassName <<" cannot be tested\n";
        }
    } else {
        // There are many more combinations where the operators class need not be the same as the value or state classes. We
        // test only some of the more common ones.
        if (settings.opsClassName=="partial" && settings.valueClassName=="partial" &&
            settings.rstateClassName=="x86" && settings.mstateClassName=="partial") {
            TestSemantics<PartialSymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateX86Ptr,
                          BaseSemantics::MemoryCellListPtr, BaseSemantics::StatePtr,
                          PartialSymbolicSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName=="partitioner2" && settings.valueClassName=="partitioner2" &&
                   settings.rstateClassName=="x86" && settings.mstateClassName=="partitioner2") {
            TestSemantics<P2::Semantics::SValuePtr, BaseSemantics::RegisterStateX86Ptr,
                          P2::Semantics::MemoryStatePtr, P2::Semantics::StatePtr,
                          P2::Semantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName=="symbolic" && settings.valueClassName=="symbolic" &&
            settings.rstateClassName=="x86" && settings.mstateClassName=="symbolic") {
            TestSemantics<SymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateX86Ptr,
                          SymbolicSemantics::MemoryStatePtr, BaseSemantics::StatePtr,
                          SymbolicSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else {
            std::cout <<"tests skipped.\n\n";
            mlog[WARN] <<"API for " <<settings.opsClassName <<" semantics with";
            if (settings.valueClassName != settings.opsClassName)
                mlog[WARN] <<" value=" <<settings.valueClassName;
            if (settings.rstateClassName != settings.opsClassName)
                mlog[WARN] <<" rstate=" <<settings.rstateClassName;
            if (settings.mstateClassName != settings.opsClassName)
                mlog[WARN] <<" mstate=" <<settings.mstateClassName;
            mlog[WARN] <<" cannot be tested\n";
        }
    }
}

static void
runSemantics(const P2::BasicBlock::Ptr &bblock, const Settings &settings, const P2::Partitioner &partitioner) {
    if (!settings.bblockInterval.isContaining(bblock->address()))
        return;

    std::cout <<"=====================================================================================\n"
              <<"=== Starting a new basic block                                                    ===\n"
              <<"=====================================================================================\n";
    const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
    BaseSemantics::RiscOperatorsPtr ops = makeRiscOperators(settings, regdict);

    BaseSemantics::Formatter formatter;
    formatter.set_suppress_initial_values(!settings.showInitialValues);
    formatter.set_show_latest_writers(settings.showUseDef);

    BaseSemantics::DispatcherPtr dispatcher = partitioner.instructionProvider().dispatcher();
    if (!dispatcher)
        throw std::runtime_error("no semantics dispatcher available for architecture");
    if (settings.trace) {
        TraceSemantics::RiscOperatorsPtr trace = TraceSemantics::RiscOperators::instance(ops);
        trace->set_stream(stdout);
        dispatcher = dispatcher->create(trace);
    } else {
        dispatcher = dispatcher->create(ops);
    }

    // The fpstatus_top register must have a concrete value if we'll use the x86 floating-point stack (e.g., st(0))
    if (const RegisterDescriptor *REG_FPSTATUS_TOP = regdict->lookup("fpstatus_top")) {
        BaseSemantics::SValuePtr st_top = ops->number_(REG_FPSTATUS_TOP->get_nbits(), 0);
        ops->writeRegister(*REG_FPSTATUS_TOP, st_top);
    }

    // Process each instruction regardless of the value of the instruction pointer
    if (settings.showStates && settings.showInitialState)
        std::cout <<"Initial state:\n" <<(*ops+formatter) <<"\n";
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        std::cout <<unparseInstructionWithAddress(insn) <<"\n";
        try {
            dispatcher->processInstruction(insn);
        } catch (const BaseSemantics::Exception &e) {
            std::cout <<e <<"\n";
        }
        if (settings.showStates)
            std::cout <<(*ops+formatter) <<"\n";
    }
}

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();
    mlog = Sawyer::Message::Facility("tool");
    Diagnostics::mfacilities.insertAndAdjust(mlog);

    // Parse the command-line to load, disassemble, and partition the specimen
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, settings).unreachedArgs();
    adjustSettings(settings);
    P2::Engine engine;
    if (!settings.isaName.empty())
        engine.disassembler(Disassembler::lookup(settings.isaName));
    (void) makeRiscOperators(settings, RegisterDictionary::dictionary_i386());// for "list" side effects
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");
    P2::Partitioner partitioner = engine.partition(specimenNames);
    testSemanticsApi(settings, partitioner);
    
    // Run sementics on each basic block
    BOOST_FOREACH (const P2::BasicBlock::Ptr &bblock, partitioner.basicBlocks())
        runSemantics(bblock, settings, partitioner);
}
