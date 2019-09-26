// Tests instruction semantics by running them on one basic block at a time.

#include <rose.h>

#include <BinaryNoOperation.h>
#include <BinarySerialIo.h>
#include <BinaryYicesSolver.h>
#include <BinaryZ3Solver.h>
#include <CommandLine.h>
#include <ConcreteSemantics2.h>
#include <Diagnostics.h>
#include <Disassembler.h>
#include <DispatcherX86.h>
#include <IntervalSemantics2.h>
#include <MemoryCellList.h>
#include <NullSemantics2.h>
#include <PartialSymbolicSemantics2.h>
#include <Partitioner2/Engine.h>
#include <SymbolicSemantics2.h>
#include <TestSemantics2.h>
#include <TraceSemantics2.h>

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

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
using namespace Sawyer::Message::Common;                // DEBUG, INFO, WARN, ERROR etc.
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

typedef Sawyer::Container::Map<std::string /*mnemonic*/, size_t> InstructionHistogram;

Sawyer::Message::Facility mlog;

// Command-line settings
struct Settings {
    std::string valueClassName;                         // name of semantic values class, abbreviated
    std::string rstateClassName;                        // name of register state class, abbreviated
    std::string mstateClassName;                        // name of memory state class, abbreviated
    std::string opsClassName;                           // name of RiscOperators class, abbreviated
    bool trace;                                         // use TraceSemantics?
    bool showInitialValues;                             // show initial values in register states?
    bool showStates;                                    // show register and memory state after each instruction?
    bool showInitialState;                              // show initial state if showStates is set?
    bool showHistogram;                                 // show instruction histogram?
    bool showFailedHistogram;                           // show failed instructions?
    AddressInterval bblockInterval;                     // which basic blocks to process
    bool useMemoryMap;                                  // state uses MemoryMap to initialize memory?
    bool runNoopAnalysis;                               // run no-op analysis on each instruction individually?
    bool testAdaptiveRegisterState;                     // test RegisterStateGeneric
    SymbolicSemantics::DefinersMode computingDefiners;  // whether to track which instructions define each value
    SymbolicSemantics::WritersMode computingWriters;    // whether to track which instructions write to each location
    Settings()
        : trace(false), showInitialValues(false), showStates(true), showInitialState(false), showHistogram(false),
          showFailedHistogram(false), bblockInterval(AddressInterval::whole()), useMemoryMap(false), runNoopAnalysis(false),
          testAdaptiveRegisterState(false), computingDefiners(SymbolicSemantics::TRACK_NO_DEFINERS),
          computingWriters(SymbolicSemantics::TRACK_NO_WRITERS) {}
};

static bool
perInstructionOutput(const Settings &settings) {
    return settings.trace || settings.showInitialValues || settings.showStates || settings.testAdaptiveRegisterState;
}

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    std::string purpose = "runs instruction semantics for testing";
    std::string description =
        "Parses, disassembles and partitions the specimens given as positional arguments on the command-line and then "
        "instantiates and runs the specified instruction semantics for each basic block.  A semantic class must be "
        "specified with the @s{semantics} switch, and its default component types can be overridden with other "
        "class-specifying switches. However, not all combinations of semantic class and component types make sense (a "
        "well implemented class will complain when the combination is nonsensical).";

    Parser parser = engine.commandLineParser(purpose, description);

    //------------------------------------------------
    SwitchGroup sem("Semantics class switches");
    sem.name("class");
    sem.doc("These switches control which classes are used when constructing the instruction semantics framework.");

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
    ctl.name("ctl");
    ctl.doc("These switches control various operational characteristics of the instruction semantics framework. The "
            "applicability of some of these switches depends on the classes used to construct the framework.");

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

    ctl.insert(Switch("use-memory-map")
               .intrinsicValue(true, settings.useMemoryMap)
               .doc("Adds the specimen's memory map to the memory state and/or RISC operators if they support using a memory "
                    "map. For most domains that allow this, it essentially initializes memory based on values loaded from "
                    "the executable.  The @s{no-use-memory-map} turns this off. The default is to " +
                    std::string(settings.useMemoryMap?"":"not ") + "use the memory map."));
    ctl.insert(Switch("no-use-memory-map")
               .key("use-memory-map")
               .intrinsicValue(false, settings.useMemoryMap)
               .hidden(true));

    ctl.insert(Switch("noop-analysis")
               .intrinsicValue(true, settings.runNoopAnalysis)
               .doc("Runs the no-op analysis on each instruction and reports when an instruction has no effect on the "
                    "machine except to change the instruction pointer.  The @s{no-noop-analysis} switch turns this off. "
                    "The default is to " + std::string(settings.runNoopAnalysis?"":"not ") + "run this analysis."));
    ctl.insert(Switch("no-noop-analysis")
               .key("noop-analysis")
               .intrinsicValue(false, settings.runNoopAnalysis)
               .hidden(true));

    ctl.insert(Switch("test-adaptive-registers")
               .intrinsicValue(true, settings.testAdaptiveRegisterState)
               .doc("Allows the RegisterStateGeneric::accessModifiesExistingLocations property to be turned on or off based "
                    "on the contents of the stack pointer register.  While the stack pointer contains the value "
                    "0x137017c1 the set of stored register locations is not allowed to change. E.g., reading AL when the "
                    "register state is storing AX will not cause it to start storing AL and AH instead of AX. The "
                    "@s{no-test-adaptive-registers} switch disables this feature. The default is to " +
                    std::string(settings.testAdaptiveRegisterState?"":"not ") + " operate in this mode."));
    ctl.insert(Switch("no-test-adaptive-registers")
               .key("test-adaptive-registers")
               .intrinsicValue(false, settings.testAdaptiveRegisterState)
               .hidden(true));

    ctl.insert(Switch("track-definers")
               .argument("how", enumParser<SymbolicSemantics::DefinersMode>(settings.computingDefiners)
                         ->with("none", SymbolicSemantics::TRACK_NO_DEFINERS)
                         ->with("last", SymbolicSemantics::TRACK_LATEST_DEFINER)
                         ->with("all", SymbolicSemantics::TRACK_ALL_DEFINERS))
               .doc("Controls whether a domain that supports tracking value definers does so.  For instance, the "
                    "symbolic domain can keep a list of defining instructions for each symbolic value. Turning on any "
                    "form of definers also turns on the output of such definers.  The @v{how} can be one of these words: "
                    "@named{none}{Do not compute definers.}"
                    "@named{last}{Track only the latest defining instruction per value.}"
                    "@named{all}{Track all defining instructions per value.}"));

    ctl.insert(Switch("track-writers")
               .argument("how", enumParser<SymbolicSemantics::WritersMode>(settings.computingWriters)
                         ->with("none", SymbolicSemantics::TRACK_NO_WRITERS)
                         ->with("last", SymbolicSemantics::TRACK_LATEST_WRITER)
                         ->with("all", SymbolicSemantics::TRACK_ALL_WRITERS))
               .doc("Controls whether a domain that supports tracking writers does so.  This means that each location "
                    "(registers and memory) will have a list of instructions that wrote to that location. The allowed "
                    "arguments are:"
                    "@named{none}{Do not track writers.}"
                    "@named{last}{Track only the latest writing instruction per location.}"
                    "@named{all}{Track all writing instructions per location.}"));

    //------------------------------------------------
    SwitchGroup out("Output switches");
    out.name("out");
    out.doc("These switches control what kind of output is produced.");

    out.insert(Switch("show-states")
               .intrinsicValue(true, settings.showStates)
               .doc("Turns on display of state information after each instruction is processed.  State output can be "
                    "disabled with @s{no-show-states}. The default is to " + std::string(settings.showStates?"":"not ") +
                    "show this information."));
    out.insert(Switch("no-show-states")
               .key("show-states")
               .intrinsicValue(false, settings.showStates)
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

    CommandLine::insertBooleanSwitch(out, "show-histogram", settings.showHistogram,
                                     "Show a histogram of the instruction mnemonics.");

    CommandLine::insertBooleanSwitch(out, "show-failure-histogram", settings.showFailedHistogram,
                                     "Show a histogram of the instruction mnemonics for which semantics failed.");

    //------------------------------------------------
    parser.doc("Synopsis", "@prop{programName} @s{semantics} @v{class} [@v{switches}] @v{specimen_name}");
    parser.errorStream(::mlog[FATAL]);
    return parser.with(sem).with(ctl).with(out).parse(argc, argv).apply().unreachedArgs();
}

static SmtSolverPtr
makeSolver(const Settings &settings) {
    return SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
}

static BaseSemantics::SValuePtr
makeProtoVal(const Settings &settings) {
    const std::string &className = settings.valueClassName;
    if (className == "list") {
        std::cout <<"value class names:\n"
#ifdef EXAMPLE_EXTENSIONS
                  <<"  example          com::example::semantics::SValue\n"
#endif
                  <<"  concrete         Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::SValue\n"
                  <<"  interval         Rose::BinaryAnalysis::InstructionSemantics2::IntervalSemantics::SValue\n"
                  <<"  null             Rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::SValue\n"
                  <<"  partial          Rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics::SValue\n"
                  <<"  partitioner2     Rose::BinaryAnalysis::Partitioner2::Semantics::SValue\n"
                  <<"  symbolic         Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::SValue\n";
        return BaseSemantics::SValuePtr();
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::SValue::instance();
#endif
    } else if (className == "concrete") {
        return ConcreteSemantics::SValue::instance();
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
                  <<"  concrete         Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  generic          Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  interval         Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  null             Rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::RegisterState\n"
                  <<"  partial          Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n"
                  <<"  partitioner2     Rose::BinaryAnalysis::Partitioner2::Semantics::RegisterState\n"
                  <<"  symbolic         Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric\n";
        return BaseSemantics::RegisterStatePtr();
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::RegisterState::instance(protoval, regdict);
#endif
    } else if (className == "concrete") {
        return BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
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
    } else {
        throw std::runtime_error("unrecognized register state class name \"" + className + "\"; see --rstate=list\n");
    }
}

static BaseSemantics::MemoryStatePtr
makeMemoryState(const Settings &settings, const P2::Partitioner &partitioner, const BaseSemantics::SValuePtr &protoval,
                const BaseSemantics::SValuePtr &protoaddr, const RegisterDictionary *regdict) {
    const std::string &className = settings.mstateClassName;
    if (className == "list") {
        std::cout <<"memory state class names:\n"
#ifdef EXAMPLE_EXTENSIONS
                  <<"  example          com::example::semantics::MemoryState\n"
#endif
                  <<"  concrete         Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::MemoryState\n"
                  <<"  interval         Rose::BinaryAnalysis::InstructionSemantics2::IntervalSemantics::MemoryState\n"
                  <<"  null             Rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::MemoryState\n"
                  <<"  partial          Rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics default\n"
                  <<"  p2-list          Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryListState\n"
                  <<"  p2-map           Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryMapState\n"
                  <<"  symbolic-list    Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::MemoryListState\n"
                  <<"  symbolic-map     Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::MemoryMapState\n";
        return BaseSemantics::MemoryStatePtr();
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::MemoryState::instance(protoval, protoaddr);
#endif
    } else if (className == "concrete") {
        return ConcreteSemantics::MemoryState::instance(protoval, protoaddr);
    } else if (className == "interval") {
        return IntervalSemantics::MemoryState::instance(protoval, protoaddr);
    } else if (className == "null") {
        return NullSemantics::MemoryState::instance(protoval, protoaddr);
    } else if (className == "partial") {
        BaseSemantics::RiscOperatorsPtr ops = PartialSymbolicSemantics::RiscOperators::instance(regdict);
        return ops->currentState()->memoryState();
    } else if (className == "p2-list" || className == "partitioner2") {
        P2::Semantics::MemoryListStatePtr m = P2::Semantics::MemoryListState::instance(protoval, protoaddr);
        m->memoryMap(partitioner.memoryMap()->shallowCopy());
        return m;
    } else if (className == "p2-map") {
        P2::Semantics::MemoryMapStatePtr m = P2::Semantics::MemoryMapState::instance(protoval, protoaddr);
        m->memoryMap(partitioner.memoryMap()->shallowCopy());
        return m;
    } else if (className == "symbolic-list" || className == "symbolic") {
        return SymbolicSemantics::MemoryListState::instance(protoval, protoaddr);
    } else if (className == "symbolic-map") {
        return SymbolicSemantics::MemoryMapState::instance(protoval, protoaddr);
    } else {
        throw std::runtime_error("unrecognized memory state class name \"" + className + "\"; see --mstate=list\n");
    }
}

static void
listRiscOperators() {
    std::cout <<"semantic class names:\n"
#ifdef EXAMPLE_EXTENSIONS
              <<"  example          com::example::semantics::RiscOperators\n"
#endif
              <<"  concrete         Rose::BinaryAnalysis::InstructionSemantics2::ConcreteSemantics::RiscOperators\n"
              <<"  interval         Rose::BinaryAnalysis::InstructionSemantics2::IntervalSemantics::RiscOperators\n"
              <<"  null             Rose::BinaryAnalysis::InstructionSemantics2::NullSemantics::RiscOperators\n"
              <<"  partial          Rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics::RiscOperators\n"
              <<"  partitioner2     Rose::BinaryAnalysis::Partitioner2::Semantics::RiscOperators\n"
              <<"  symbolic         Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics::RiscOperators\n";
}

static BaseSemantics::RiscOperatorsPtr
makeRiscOperators(const Settings &settings, const P2::Partitioner &partitioner) {
    const std::string &className = settings.opsClassName;
    if (className.empty())
        throw std::runtime_error("--semantics switch is required");
    
    SmtSolverPtr solver = makeSolver(settings);
    const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
    BaseSemantics::SValuePtr protoval = makeProtoVal(settings);
    BaseSemantics::RegisterStatePtr rstate = makeRegisterState(settings, protoval, regdict);
    BaseSemantics::MemoryStatePtr mstate = makeMemoryState(settings, partitioner, protoval, protoval, regdict);
    BaseSemantics::StatePtr state = BaseSemantics::State::instance(rstate, mstate);

    if (0) {
#ifdef EXAMPLE_EXTENSIONS
    } else if (className == "example") {
        return com::example::semantics::RiscOperators::instance(state, solver);
#endif
    } else if (className == "concrete") {
        return ConcreteSemantics::RiscOperators::instance(state, solver);
    } else if (className == "interval") {
        return IntervalSemantics::RiscOperators::instance(state, solver);
    } else if (className == "null") {
        return NullSemantics::RiscOperators::instance(state, solver);
    } else if (className == "partial") {
        PartialSymbolicSemantics::RiscOperatorsPtr ops = PartialSymbolicSemantics::RiscOperators::instance(state, solver);
        if (settings.useMemoryMap)
            ops->set_memory_map(partitioner.memoryMap()->shallowCopy());
        return ops;
    } else if (className == "partitioner2") {
        return P2::Semantics::RiscOperators::instance(state, solver);
    } else if (className == "symbolic") {
        SymbolicSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(state, solver);
        ops->computingDefiners(settings.computingDefiners);
        ops->computingRegisterWriters(settings.computingWriters);
        ops->computingMemoryWriters(settings.computingWriters);
        return ops;
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

// Test the API for various combinations of classes.
static void
testSemanticsApi(const Settings &settings, const P2::Partitioner &partitioner) {
    BaseSemantics::RiscOperatorsPtr ops = makeRiscOperators(settings, partitioner);
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
        } else if (settings.opsClassName == "concrete") {
            TestSemantics<ConcreteSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                          ConcreteSemantics::MemoryStatePtr, BaseSemantics::StatePtr,
                          ConcreteSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
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
            TestSemantics<P2::Semantics::SValuePtr, P2::Semantics::RegisterStatePtr,
                          P2::Semantics::MemoryListStatePtr, P2::Semantics::StatePtr,
                          P2::Semantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName == "symbolic") {
            TestSemantics<SymbolicSemantics::SValuePtr, BaseSemantics::RegisterStatePtr,
                          SymbolicSemantics::MemoryListStatePtr, BaseSemantics::StatePtr,
                          SymbolicSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else {
            std::cout <<"tests skipped.\n\n";
            ::mlog[WARN] <<"API for " <<settings.opsClassName <<" cannot be tested\n";
        }
    } else {
        // There are many more combinations where the operators class need not be the same as the value or state classes. We
        // test only some of the more common ones.
        if (settings.opsClassName=="symbolic" && settings.valueClassName=="symbolic" &&
                   settings.rstateClassName=="symbolic" && settings.mstateClassName=="symbolic-map") {
            TestSemantics<SymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                          SymbolicSemantics::MemoryMapStatePtr, BaseSemantics::StatePtr,
                          SymbolicSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else if (settings.opsClassName=="symbolic" && settings.valueClassName=="symbolic" &&
                   settings.rstateClassName=="symbolic" && settings.mstateClassName=="symbolic-list") {
            TestSemantics<SymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                          SymbolicSemantics::MemoryListStatePtr, BaseSemantics::StatePtr,
                          SymbolicSemantics::RiscOperatorsPtr> tester;
            tester.test(ops);
        } else {
            std::cout <<"tests skipped.\n\n";
            ::mlog[WARN] <<"API for " <<settings.opsClassName <<" semantics with";
            if (settings.valueClassName != settings.opsClassName)
                ::mlog[WARN] <<" value=" <<settings.valueClassName;
            if (settings.rstateClassName != settings.opsClassName)
                ::mlog[WARN] <<" rstate=" <<settings.rstateClassName;
            if (settings.mstateClassName != settings.opsClassName)
                ::mlog[WARN] <<" mstate=" <<settings.mstateClassName;
            ::mlog[WARN] <<" cannot be tested\n";
        }
    }
}

static void
runSemantics(const P2::BasicBlock::Ptr &bblock, const Settings &settings, const P2::Partitioner &partitioner,
             InstructionHistogram &allInsns, InstructionHistogram &failedInsns) {
    if (!settings.bblockInterval.isContaining(bblock->address()))
        return;

    if (perInstructionOutput(settings)) {
        std::cout <<"=====================================================================================\n"
                  <<"=== Starting a new basic block                                                    ===\n"
                  <<"=====================================================================================\n";
    }
    const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
    BaseSemantics::RiscOperatorsPtr ops = makeRiscOperators(settings, partitioner);

    BaseSemantics::Formatter formatter;
    formatter.set_suppress_initial_values(!settings.showInitialValues);
    formatter.set_show_latest_writers(settings.computingWriters != SymbolicSemantics::TRACK_NO_WRITERS);

    BaseSemantics::DispatcherPtr dispatcher = partitioner.instructionProvider().dispatcher();
    if (!dispatcher)
        throw std::runtime_error("no semantics dispatcher available for architecture");
    if (settings.trace) {
        // Wrap the RISC operators in TraceSemantics::RiscOperators and adjust the output stream to be standard output since
        // the trace is considered the main output from this command and needs to be properly interleaved with the other output
        // on stdout.
        TraceSemantics::RiscOperatorsPtr trace = TraceSemantics::RiscOperators::instance(ops);
        trace->stream().destination(Sawyer::Message::FileSink::instance(stdout, Sawyer::Message::Prefix::silentInstance()));
        dispatcher = dispatcher->create(trace);
    } else {
        dispatcher = dispatcher->create(ops);
    }

    // The fpstatus_top register must have a concrete value if we'll use the x86 floating-point stack (e.g., st(0))
    if (const RegisterDescriptor *REG_FPSTATUS_TOP = regdict->lookup("fpstatus_top")) {
        BaseSemantics::SValuePtr st_top = ops->number_(REG_FPSTATUS_TOP->nBits(), 0);
        ops->writeRegister(*REG_FPSTATUS_TOP, st_top);
    }

    // Process each instruction regardless of the value of the instruction pointer
    if (settings.showStates && settings.showInitialState)
        std::cout <<"Initial state:\n" <<(*ops+formatter) <<"\n";
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions()) {
        if (perInstructionOutput(settings))
            std::cout <<insn->toString() <<"\n";
        ++allInsns.insertMaybe(insn->get_mnemonic(), 0);

        // See the comments in $ROSE/binaries/samples/x86-64-adaptiveRegs.s for details
        if (settings.testAdaptiveRegisterState) {
            BaseSemantics::RegisterStateGenericPtr regState =
                boost::dynamic_pointer_cast<BaseSemantics::RegisterStateGeneric>(ops->currentState()->registerState());
            if (regState) {
                RegisterDescriptor SP = partitioner.instructionProvider().stackPointerRegister();
                BaseSemantics::SValuePtr sp = ops->readRegister(SP);
                unsigned magic = sp->is_number() ? sp->get_number() : 0;
                unsigned settings = (magic & 0xfffffffc) == 0x137017c0 ? (magic & 3) : 3;
                regState->accessModifiesExistingLocations(settings & 1);
                regState->accessCreatesLocations(settings & 2);
                std::cout <<"RegisterStateGeneric: accessModifiesExistingLocations="
                          <<(regState->accessModifiesExistingLocations() ? "yes" : "no")
                          <<", accessCreatesLocations="
                          <<(regState->accessCreatesLocations() ? "yes" : "no") <<"\n";
            }
        }

        try {
            dispatcher->processInstruction(insn);
        } catch (const BaseSemantics::Exception &e) {
            //if (perInstructionOutput(settings) || !settings.showFailedHistogram)
                std::cout <<"Semantics error: " <<e <<"\n";
            ++failedInsns.insertMaybe(insn->get_mnemonic(), 0);
        } catch (const SmtSolver::Exception &e) {
            //if (perInstructionOutput(settings) || !settings.showFailedHistogram)
                std::cout <<"SMT solver error: " <<e.what() <<"\n";
            ++failedInsns.insertMaybe(insn->get_mnemonic(), 0);
        }
        if (settings.showStates)
            std::cout <<(*ops+formatter) <<"\n";

        if (settings.runNoopAnalysis) {
            // Use a different state for no-op analysis, otherwise it will end up messing with the state we're using for our
            // own semantics.
            BaseSemantics::RiscOperatorsPtr ops2 = makeRiscOperators(settings, partitioner);
            BaseSemantics::DispatcherPtr dispatcher2 = partitioner.instructionProvider().dispatcher();
            dispatcher2 = dispatcher2->create(ops2);
            NoOperation nopAnalyzer(dispatcher2);
            if (nopAnalyzer.isNoop(insn))
                std::cerr <<"Instruction has no effect (other than changing the instruction pointer register)\n";
        }
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");

    // Parse the command-line to load, disassemble, and partition the specimen
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine, settings);
    adjustSettings(settings);

    // Perhaps the user is only asking us to list available values for some switches
    bool listAndExit = false;
    if (settings.valueClassName == "list") {
        (void) makeProtoVal(settings);
        listAndExit = true;
    }
    if (settings.rstateClassName == "list") {
        (void) makeRegisterState(settings, BaseSemantics::SValuePtr(), NULL);
        listAndExit = true;
    }
    if (settings.mstateClassName == "list") {
        P2::Partitioner p;
        (void) makeMemoryState(settings, p, BaseSemantics::SValuePtr(), BaseSemantics::SValuePtr(), NULL);
        listAndExit = true;
    }
    if (settings.opsClassName == "list") {
        listRiscOperators();
        listAndExit = true;
    }
    if (listAndExit)
        exit(0);

    // Validate the command-line now that we know we're not just listing stuff
    if (specimenNames.empty())
        throw std::runtime_error("no specimen specified; see --help");

    // Parse, disassemble, and partition
    P2::Partitioner partitioner;
    if (specimenNames.size() == 1 && boost::ends_with(specimenNames[0], ".rba")) {
        SerialInput::Ptr input = SerialInput::instance();
        input->open(specimenNames[0]);
        partitioner = input->loadPartitioner();
        std::cerr <<"ROBB: " <<partitioner.nBasicBlocks() <<"\n";
    } else {
        partitioner = engine.partition(specimenNames);
    }
    
    testSemanticsApi(settings, partitioner);
    
    // Run sementics on each basic block
    Sawyer::ProgressBar<size_t> progress(partitioner.nBasicBlocks(), ::mlog[MARCH], "basic block semantics");
    InstructionHistogram allInsns, failedInsns;
    BOOST_FOREACH (const P2::BasicBlock::Ptr &bblock, partitioner.basicBlocks()) {
        runSemantics(bblock, settings, partitioner, allInsns /*in,out*/, failedInsns /*in,out*/);
        ++progress;
    }
    
    // Show histogram results
    if (settings.showHistogram) {
        std::cout <<"Processed instructions:\n";
        size_t total = 0;
        BOOST_FOREACH (const InstructionHistogram::Node &node, allInsns.nodes()) {
            Diagnostics::mfprintf(std::cout)("%-12zu %s\n", node.value(), node.key().c_str());
            total += node.value();
        }
        Diagnostics::mfprintf(std::cout)("%-12zu total\n", total);
    }
    if (settings.showFailedHistogram) {
        std::cout <<"Failed instructions:\n";
        size_t total = 0;
        BOOST_FOREACH (const InstructionHistogram::Node &node, failedInsns.nodes()) {
            Diagnostics::mfprintf(std::cout)("%-12zu %s\n", node.value(), node.key().c_str());
            total += node.value();
        }
        Diagnostics::mfprintf(std::cout)("%-12zu total\n", total);
    }
}
