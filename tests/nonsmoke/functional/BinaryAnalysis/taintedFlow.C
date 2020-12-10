// Demonstrates how to perform a binary data flow analysis by using tainted flow as an example.
//
// Binary data flow analysis differs from source code data flow analysis in that source code has well-defined variables and
// analysis is often restricted for simplicity to variable granularity (i.e., not individual array elements, and no regard for
// pointer aliasing).  Binary data flow has one big array (all of memory, no named variables) and registers that temporarily
// hold values from that array.  Therefore the dataflow must have array element granularity and support aliasing.
//
// Another difference is that source code has a relatively small number of basic operations while binaries have hundreds
// operations (the various instructions across the set of supported architectures). It is easy to encode the data flow paths
// through these source-level operations, but much more time consuming to encode the paths through the hundreds of different
// binary instructions.  Also, most of the instructions have multiple, data flow paths (like x86 "POP EAX", which has three
// paths: incrementing the instruction pointer, copying a value from memory to register, and incrementing the stack pointer).
// It is not sufficient to note only what the inputs and outputs of each instruction are since (1) many times the data flow
// paths within an instruction are independent of one another, and (2) sometimes input or output locations are a function of
// the input values (like the "POP EAX" example, where one of the input locations, the memory address, is a function of another
// input value, the stack pointer).
//
// A third difference from source code is that binary instructions routinely access multiple variables in gather/scatter
// patterns due to the mismatch in size between memory values (bytes) and register values (words).  For instance, the previous
// "POP EAX" instruction has four data flow paths that collectively flow into the 32-bit EAX register from four consecutive
// memory locations. A related issue is the recurring use of idioms like "XOR EAX, EAX" (which sets the value of the EAX
// register to zero without doing any actual data flow).
//
// The core data flow analysis requires a representation of a state that maps a finite number of variables to values that form
// a lattice.  For instance, if the core data flow is a tainted flow analysis, then the state should map a finite number of
// abstract locations to values that form a lattice: bottom, tainted, not-tainted, top.
//
// Binary tainted flow requires these steps:
//   1. Parse the ELF or PE container if necessary in order to locate the parts of the file that might contain instructions.
//
//   2. Discover instructions by recursive disassembly or some other method, and group the instructions into units on which the
//      data flow analysis will operate.  For instance, if we want to do intra-function tainted flow we need to discover what
//      collections of instructions form functions.
//
//   3. Create a control flow graph over the instructions of interest.  The global control flow graph is constructed by ROSE
//      internally as part of step #2, but we'll need an explicit representation, perhaps a subgraph, over which the data flow
//      engine will eventually operate.
//
//   4. Discover the variables.  Binaries don't normally have explicit variables, so they need to be discovered. The variables
//      are represented as a list of AbstractLocation objects that refer to either registers or memory addresses.  The
//      addresses can be in a domain chosen by the user (e.g., concrete vs. symbolic, etc.).
//
//   5. Instructions (or basic blocks, depending on the type of CFG we're using) usually have multiple, independent data flow
//      paths. We need to identify these paths because they're what will eventually drive the data flow transfer
//      function. Furthermore, we want to identify the paths without having to encode them explicitly for hundreds of different
//      CPU instructions.
//
//  Finally we are ready for the core data flow analysis, which requires:
//
//   * A control flow graph (step 3 above)
//
//   * A lattice of values (e.g., bottom, tainted, not-tainted, top) so that a value can be stored at each variable in a state.
//
//   * A representation for a state that maps variables to values. The variables are the abstract locations discovered by step
//     4 above.  The values are those from the lattice in the previous bullet.
//
//   * A transfer function that uses the data flow paths from step 5 above to create a new state from an existing state.
//
//   * A merge function so that an input state can be computed for a CFG vertex when that vertex has multiple incoming control
//     flow paths.
//
//   * An initial CFG vertex and its initial incoming state.
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>

#include "AsmUnparser.h"
#include "BinaryTaintedFlow.h"
#include "BinaryFunctionCall.h"
#include "BinarySmtSolver.h"
#include "CommandLine.h"
#include "Diagnostics.h"
#include "DispatcherX86.h"
#include "SymbolicSemantics2.h"
#include "WorkLists.h"

#include <boost/algorithm/string/regex.hpp>
#include <boost/foreach.hpp>
#include <list>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

// Diagnostic output for this tool; initialized in main()
Sawyer::Message::Facility mlog;

// Template function to build a control flow graph whose vertices are either instructions or basic blocks
template<class SageNode>
void buildControlFlowGraph(SgNode *ast, Sawyer::Container::Graph<SageNode*> &cfg /*out*/);

template<>
void buildControlFlowGraph<SgAsmInstruction>(SgNode *ast, Sawyer::Container::Graph<SgAsmInstruction*> &cfg /*out*/) {
    ControlFlow().build_insn_cfg_from_ast(ast, cfg);
}

template<>
void buildControlFlowGraph<SgAsmBlock>(SgNode *ast, Sawyer::Container::Graph<SgAsmBlock*> &cfg /*out*/) {
    ControlFlow().build_block_cfg_from_ast(ast, cfg);
}

// Perform tainted flow analysis on one function.  The SageNode type can be SgAsmInstruction or SgAsmBlock to analyze using
// control flow graphs whose vertices are instructions or basic blocks.
template<class SageNode>
static void analyze(SgAsmFunction *specimen, TaintedFlow::Approximation approximation) {
    using namespace InstructionSemantics2;

    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(specimen);
    ASSERT_always_not_null(interp);
    const RegisterDictionary *regdict = interp->get_registers();

    // List the function
    std::cout <<"\n";
    ::mlog[WHERE] <<"Taint analysis for function \"" <<specimen->get_name() <<"\""
                  <<" at " <<StringUtility::addrToString(specimen->get_entry_va()) <<"\n";
    AsmUnparser().unparse(std::cout, specimen);

    // Control flow graph.  Any graph implementing the Boost Graph Library API is permissible. We use Sawyer's implementation
    // because it has a nicer interface for the rest of this file to use.  We could use basic blocks or instructions as the
    // vertices.  The edge value is irrelevant -- we don't use them.
    ::mlog[TRACE] <<"Obtaining control flow graph...\n";
    typedef Sawyer::Container::Graph<SageNode*> CFG;
    CFG cfg;
    buildControlFlowGraph(specimen, cfg);

    // Find the entry point for the CFG.  There are a couple ways to do this, but the easiest is to assume that since we
    // constructed the CFG from a SgAsmFunction node, and since we didn't filter out any vertices, that the function's entry
    // point is vertex #0.
    size_t cfgStartVertex = 0;

    // CPU instructions often have multiple independent data flows, and we need to discover them somehow.  For instance, an x86
    // "POP EAX" instruction has three dataflows:
    //    1. Incrementing of the instruction pointer registers by the size of the instruction (1 byte)
    //    2. Reading four bytes from memory (top of stack) into the EAX register.
    //    3. Incrementing the stack pointer register by the size of the data just popped (4 bytes)
    //
    // The ROSE libarary doesn't actually enumerate these data flows directly for each instruction, but rather relies on
    // instruction semantics to discern the flows by "executing" each instruction and looking at the side effects. We can use
    // any semantic domain we like (concrete, symbolic, interval, sign, etc), but using the symbolic domain will give us the
    // most accurate information about memory addresses without having to explicitly initialize every register and memory
    // location -- symbolic values are instantiated as variables when needed.
    //
    // So we create a symbolic domain and link it into an instruction dispatcher that knows about Intel x86 instructions.  The
    // Rose::BinaryAnalysis::DataFlow object provides the API for discovering intra-function or intra-block data flows.
    BaseSemantics::RiscOperatorsPtr symbolicOps = SymbolicSemantics::RiscOperators::instance(regdict);
    ASSERT_always_not_null(symbolicOps);
    DispatcherX86Ptr cpu = DispatcherX86::instance(symbolicOps, 32); // assuming the specimen is x86-based
#if 0
    // The initial state can be modified if you like.  Here we use a constant for the stack pointer.
    symbolicOps->writeRegister(cpu->REG_ESP, symbolicOps->number_(32, 0x02800000)); // arbitrary
#endif
    BaseSemantics::SValuePtr esp_0 = symbolicOps->readRegister(cpu->REG_ESP); // initial value of stack pointer

    // The Rose::BinaryAnalysis::TaintedFlow class encapsulates all the methods we need to perform tainted flow analysis.
    TaintedFlow taintAnalysis(cpu);
    taintAnalysis.approximation(approximation);
    if (SmtSolverPtr solver = SmtSolver::bestAvailable()) {
        taintAnalysis.smtSolver(solver);
    } else {
        ::mlog[WARN] <<"not using an SMT solver (none available)\n";
    }

    // Analyze the specimen in order to discover what variables are referenced and the control flow between those variables for
    // each basic block.
    ::mlog[TRACE] <<"Finding data flows between variables...\n";
    taintAnalysis.computeFlowGraphs(cfg, cfgStartVertex);

    // Print some debugging information.
    if (::mlog[DEBUG]) {
        ::mlog[DEBUG] <<"Data flow per basic block:\n";
        BOOST_FOREACH (const DataFlow::VertexFlowGraphs::Node &indexNode, taintAnalysis.vertexFlowGraphs().nodes()) {
            SageNode *node = cfg.findVertex(indexNode.key())->value();
            std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(node);
            ::mlog[DEBUG] <<"  Instruction" <<(1==insns.size()?"":"s") <<":\n";
            BOOST_FOREACH (SgAsmInstruction *insn, insns)
                ::mlog[DEBUG] <<"    " <<unparseInstructionWithAddress(insn) <<"\n";
            ::mlog[DEBUG] <<"  Data flow edges:\n";
            BOOST_FOREACH (const DataFlow::Graph::Edge &edge, indexNode.value().edges()) {
                ::mlog[DEBUG] <<"    data flow #" <<edge.value().sequence
                              <<" from " <<edge.source()->value()
                              <<(DataFlow::Graph::EdgeValue::CLOBBER==edge.value().edgeType ? " clobbers " : " augments ")
                              <<edge.target()->value()
                              <<"\n";
            }
        }
        ::mlog[DEBUG] <<"\nVariables:\n";
        BOOST_FOREACH (const DataFlow::Variable &variable, taintAnalysis.variables())
            ::mlog[DEBUG] <<"  " <<variable <<"\n";
    }

    // Arbitrarily mark variable at address esp_0 + 4 as tainted. This corresponds to the least significant byte of the first
    // function argument. We must use the same initial value for the ESP register as we used when discovering the variables.
    // Mark constants (a variable with no location) as not tainted.
    TaintedFlow::StatePtr initialState = taintAnalysis.stateInstance(TaintedFlow::BOTTOM); // provide a default taintedness
    DataFlow::Variable stack4(symbolicOps->add(esp_0, symbolicOps->number_(esp_0->get_width(), 4)));
    if (initialState->setIfExists(stack4, TaintedFlow::TAINTED)) { // taint source
        ::mlog[TRACE] <<"Taint source: " <<stack4 <<"\n";
    } else {
        ::mlog[TRACE] <<"Taint source: NOTHING! (no function arguments referenced?)\n";
    }
    initialState->setIfExists(DataFlow::Variable(), TaintedFlow::NOT_TAINTED); // constants are not tainted

    // Run the taint analysis until it reaches a fixed point.  Be sure to use the same CFG and starting vertex as above!
    ::mlog[TRACE] <<"Running data flow analysis...\n";
    taintAnalysis.runToFixedPoint(cfg, cfgStartVertex, initialState);

    // Print the final data flow state for each CFG vertex that has no successors.
    ::mlog[TRACE] <<"Printing results...\n";
    BOOST_FOREACH (const typename CFG::Vertex &vertex, cfg.vertices()) {
        if (vertex.nOutEdges()==0) {
            rose_addr_t lastInsnAddr = SageInterface::querySubTree<SgAsmInstruction>(vertex.value()).back()->get_address();
            std::cout <<"\nTaint for each variable at " <<StringUtility::addrToString(lastInsnAddr) <<":\n";
            if (TaintedFlow::StatePtr state = taintAnalysis.getFinalState(vertex.id())) {
                std::cout <<*state;
            } else {
                std::cout <<"  block not reached by data flow\n";
            }
        }
    }
    std::cout <<"\n\n";
}

int main(int argc, char *argv[])
{
    // Configure diagnostic output
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&::mlog, "taintedFlow");
    Rose::Diagnostics::mfacilities.control("taintedFlow(>=where)"); // the default

    // Describe the command-line
    using namespace Sawyer::CommandLine;
    SwitchGroup generic = Rose::CommandLine::genericSwitches();
    SwitchGroup switches("Tainted flow switches");
    std::vector<rose_addr_t> functionAddresses;
    switches.insert(Switch("address")
                    .argument("addr", nonNegativeIntegerParser(functionAddresses))
                    .whichValue(SAVE_ALL)
                    .doc("Specifies function entry addresses for those functions that should be analyzed.  This switch "
                         "can appear multiple times.  Any function whose entry address is a specified address is analyzed."));
    TaintedFlow::Approximation approx = TaintedFlow::UNDER_APPROXIMATE;
    switches.insert(Switch("approx", 'a')
                    .argument("mode", enumParser<TaintedFlow::Approximation>(approx)
                              ->with("under", TaintedFlow::UNDER_APPROXIMATE)
                              ->with("over", TaintedFlow::OVER_APPROXIMATE))
                    .doc("Controls whether the analysis under or over approximates tainted flow.  An under approximation "
                         "uses a must-alias constraint when flowing taint from one abstract location to another, while an "
                         "over approximation uses may-alias.  The effect of over approximating is that the analyzer is "
                         "less able to determine when global variables and local variables are distinct because it "
                         "reasons that a stack-offset expression could be equal to some arbitrary constant.  The @v{mode} "
                         "can be the word \"under\" (the default) or \"over\"."));
    bool useInstructions = false;
    switches.insert(Switch("instructions")
                    .intrinsicValue(true, useInstructions)
                    .whichValue(SAVE_ONE)
                    .doc("Use control flow graphs whose vertices are instructions rather than basic blocks.  The default "
                         "is to use basic blocks; the @s{blocks} switch does the opposite.  The output should be the same "
                         "in either case."));
    switches.insert(Switch("blocks")
                    .key("instructions")
                    .whichValue(SAVE_ONE)               // blocks and instructions are mutually exclusive
                    .intrinsicValue(false, useInstructions)
                    .hidden(true));
    std::vector<std::string> functionNameRegexList;
    switches.insert(Switch("names")
                    .whichValue(SAVE_ALL)
                    .argument("regex", anyParser(functionNameRegexList))
                    .doc("Specifies a regular expression for function names that will be analyzed.  If more than one regular "
                         "expression is given then a function is analyzed if it's name matches any of the expressions."));

    Sawyer::CommandLine::Parser cmdline_parser;
    cmdline_parser.errorStream(::mlog[FATAL]);
    cmdline_parser.purpose("binary tainted flow analysis");
    cmdline_parser.version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE);
    cmdline_parser.chapter(1, "ROSE Command-line Tools");
    cmdline_parser.doc("Synopsis",
                       "@prop{programName} [@v{tool_switches}] [-- @v{rose_switches}] @v{specimen}\n\n");
    cmdline_parser.doc("Description",
                       "Runs a tainted flow analysis on each function in the specified binary @v{specimen}. The output "
                       "is the function listing followed by a list of variables and their taintedness.  The taint source "
                       "for each function is ESP+4, the least significant byte of the first function argument if it "
                       "exists.");
    cmdline_parser.doc("Options",
                       "These are the switches recognized by the @prop{programName} tool itself. Documentation for switches "
                       "that can be passed to ROSE's frontend() function is available by using \"-rose:help\" in the "
                       "@v{rose_switches} part of the command-line (the \"--\" and a specimen name are required because "
                       "frontend tries to parse the program before it spits out the help message, so do it this way: "
                       "\"@prop{programName} -- -rose:help @v{specimen}\").");

    // Parse the command line.
    ParserResult cmdline = cmdline_parser.with(generic).with(switches).parse(argc, argv).apply();

    // Parse the binary container (ELF, PE, etc) and disassemble instructions using the default disassembler.  Organize
    // (partition) the instructions into basic blocks and functions using the default partitioner.  The disassembler and
    // partitioner can be controled to some extent with ROSE command-line switches.  Since librose doesn't describe its
    // command-line with Sawyer::CommandLine, we're somewhat restricted in how we operate.  All tool switches must appear
    // before all frontend() switches, and we must assume that all tool switches are spelled correctly (because we have no way
    // to distinguish between a valid frontend() switch and a misspelled tool switch).  FIXME[Robb P. Matzke 2014-06-06]
    std::vector<std::string> frontendArgs = cmdline.unreachedArgs();
    frontendArgs.insert(frontendArgs.begin(), argv+0, argv+1); // frontend() needs this even though it just throws it away
    SgProject *project = frontend(frontendArgs);

    // Find the most interesting interpretation (the PE interpretation for a PE executable is the last one)
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    std::cout <<"found " <<StringUtility::plural(interps.size(), "binary interpretations") <<"\n";
    ASSERT_forbid(interps.empty());
    SgAsmInterpretation *interp = interps.back();

    // Performan a tainted flow analysis for each function of the interpretation whose name matches the list of
    // regular expressions or whose address matches the list of function addresses.  If both lists are empty then
    // analyze all functions.
    BOOST_FOREACH (SgAsmFunction *func, SageInterface::querySubTree<SgAsmFunction>(interp)) {
        bool doAnalysis = functionNameRegexList.empty() && functionAddresses.empty();

        if (!doAnalysis) {
            std::string funcName = func->get_name();
            BOOST_FOREACH (const std::string &regexString, functionNameRegexList) {
                boost::regex re(regexString);
                if (boost::find_regex(funcName, re)) {
                    doAnalysis = true;
                    break;
                }
            }
        }

        if (!doAnalysis) {
            BOOST_FOREACH (rose_addr_t addr, functionAddresses) {
                if (addr == func->get_entry_va()) {
                    doAnalysis = true;
                    break;
                }
            }
        }

        if (doAnalysis) {
            if (useInstructions) {
                analyze<SgAsmInstruction>(func, approx);
            } else {
                analyze<SgAsmBlock>(func, approx);
            }
        }
    }
}

// FIXME[Robb P. Matzke 2014-06-02]: These are only here to make sure that various kinds of control flow and function call
// graphs can be generated using both Boost Graph Library implementations and Sawyer implementations (via BGL API). These
// should be moved to a different test eventually.
void compileTests(SgAsmInterpretation *interp) {
    typedef Sawyer::Container::Graph<SgAsmBlock*, int> CFG1;
    CFG1 cfg1 = ControlFlow().build_block_cfg_from_ast<CFG1>(interp);
    typedef Sawyer::Container::Graph<SgAsmInstruction*, int> CFG2;
    CFG2 cfg2 = ControlFlow().build_insn_cfg_from_ast<CFG2>(interp);
    typedef ControlFlow::BlockGraph CFG3;
    CFG3 cfg3 = ControlFlow().build_block_cfg_from_ast<CFG3>(interp);
    typedef ControlFlow::InsnGraph CFG4;
    CFG4 cfg4 = ControlFlow().build_insn_cfg_from_ast<CFG4>(interp);
    ControlFlow().fixup_fcall_fret(cfg2, false);
    ControlFlow().fixup_fcall_fret(cfg4, false);
    ControlFlow().build_cg_from_ast<CFG1>(interp);
    ControlFlow().build_cg_from_ast<CFG3>(interp);
    ControlFlow().copy(cfg1);
    ControlFlow().copy(cfg2);
    ControlFlow().copy(cfg3);
    ControlFlow().copy(cfg4);

    typedef Sawyer::Container::Graph<SgAsmFunction*, int> CG1;
    CG1 cg1 = FunctionCall().build_cg_from_ast<CG1>(interp);
    FunctionCall().build_cg_from_cfg<CG1>(cfg1);
    FunctionCall().build_cg_from_cfg<CG1>(cfg2);
    FunctionCall().build_cg_from_cfg<CG1>(cfg3);
    FunctionCall().build_cg_from_cfg<CG1>(cfg4);
    FunctionCall().copy(cg1);

    typedef FunctionCall::Graph CG2;
    CG2 cg2 = FunctionCall().build_cg_from_ast<CG2>(interp);
    FunctionCall().build_cg_from_cfg<CG2>(cfg1);
    FunctionCall().build_cg_from_cfg<CG2>(cfg2);
    FunctionCall().build_cg_from_cfg<CG2>(cfg3);
    FunctionCall().build_cg_from_cfg<CG2>(cfg4);
    FunctionCall().copy(cg2);
}

#endif
