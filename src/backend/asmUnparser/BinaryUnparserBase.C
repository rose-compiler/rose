#include <sage3basic.h>
#include <BinaryUnparserBase.h>
#include <Diagnostics.h>
#include <Partitioner2/Partitioner.h>
#include <stringify.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics2;

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

State::State(const P2::Partitioner &p, const Settings &settings, const Base &frontUnparser)
    : partitioner_(p), registerNames_(p.instructionProvider().registerDictionary()), frontUnparser_(frontUnparser) {
    if (settings.function.cg.showing)
        cg_ = p.functionCallGraph(false);
}

State::~State() {}

const P2::Partitioner&
State::partitioner() const {
    return partitioner_;
}

const P2::FunctionCallGraph&
State::cg() const {
    return cg_;
}

P2::Function::Ptr
State::currentFunction() const {
    return currentFunction_;
}

void
State::currentFunction(const P2::Function::Ptr &f) {
    currentFunction_ = f;
}

Partitioner2::BasicBlock::Ptr
State::currentBasicBlock() const {
    return currentBasicBlock_;
}

void
State::currentBasicBlock(const Partitioner2::BasicBlockPtr &bb) {
    currentBasicBlock_ = bb;
}

const std::string&
State::nextInsnLabel() const {
    return nextInsnLabel_;
}

void
State::nextInsnLabel(const std::string &label) {
    nextInsnLabel_ = label;
}

const RegisterNames&
State::registerNames() const {
    return registerNames_;
}

void
State::registerNames(const RegisterNames &r) {
    registerNames_ = r;
}

const State::AddrString&
State::basicBlockLabels() const {
    return basicBlockLabels_;
}

State::AddrString&
State::basicBlockLabels() {
    return basicBlockLabels_;
}

const Base&
State::frontUnparser() const {
    return frontUnparser_;
}

class FunctionGuard {
    State &state;
    Partitioner2::FunctionPtr prev;
public:
    FunctionGuard(State &state, const Partitioner2::FunctionPtr &f)
        : state(state) {
        prev = state.currentFunction();
        state.currentFunction(f);
    }

    ~FunctionGuard() {
        state.currentFunction(prev);
    }
};

class BasicBlockGuard {
    State &state;
    Partitioner2::BasicBlockPtr prev;
public:
    BasicBlockGuard(State &state, const Partitioner2::BasicBlockPtr &bb)
        : state(state) {
        prev = state.currentBasicBlock();
        state.currentBasicBlock(bb);
    }
    ~BasicBlockGuard() {
        state.currentBasicBlock(prev);
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Constructors, etc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Base::Base() {}

Base::Base(const Ptr &nextUnparser)
    : nextUnparser_(nextUnparser) {}

Base::~Base() {}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// class method
std::string
Base::leftJustify(const std::string &s, size_t width) {
    if (s.size() > width)
        return s;
    return s + std::string(width-s.size(), ' ');
}

// class method
std::string
Base::juxtaposeColumns(const std::vector<std::string> &parts, const std::vector<size_t> &minWidths,
                       const std::string &columnSeparator) {
    std::string retval;

    // Split each part into separate lines
    size_t nlines = 0;
    std::vector<std::vector<std::string> > partLines(parts.size());
    for (size_t i=0; i<parts.size(); ++i) {
        partLines[i] = StringUtility::split('\n', parts[i]);
        nlines = std::max(nlines, partLines[i].size());
    }

    // Emit one line at a time across all columns
    const std::string empty;
    for (size_t i=0; i<nlines; ++i) {
        if (i != 0)
            retval += "\n";
        std::ostringstream ss;
        for (size_t j=0; j<partLines.size(); ++j) {
            const std::string &s = i < partLines[j].size() ? partLines[j][i] : empty;
            if (j > 0)
                ss <<columnSeparator;
            ss <<leftJustify(s, minWidths[j]);
        }
        retval += boost::trim_right_copy(ss.str());
    }
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Settings::Settings() {
    function.showingReasons = true;
    function.showingDemangled = true;
    function.cg.showing = true;
    function.stackDelta.showing = false;                // very slow for some reason
    function.stackDelta.concrete = true;
    function.callconv.showing = true;
    function.noop.showing = true;
    function.mayReturn.showing = true;

    bblock.cfg.showingPredecessors = true;
    bblock.cfg.showingSuccessors = true;
    bblock.cfg.showingSharing = true;

    insn.address.showing = true;
    insn.address.fieldWidth = 11;                       // "0x" + 8 hex digits + ":"
    insn.bytes.showing = true;
    insn.bytes.perLine = 8;
    insn.bytes.fieldWidth = 25;
    insn.stackDelta.showing = true;
    insn.stackDelta.fieldWidth = 2;
    insn.mnemonic.fieldWidth = 8;
    insn.operands.separator = ", ";
    insn.operands.fieldWidth = 40;
    insn.comment.showing = true;
    insn.comment.usingDescription = true;
    insn.comment.pre = "; ";
    insn.comment.fieldWidth = 1;
}

// class method
Settings
Settings::full() {
    return Settings();
}

// class method
Settings
Settings::minimal() {
    Settings s = full();
    s.function.showingReasons = false;
    s.function.showingDemangled = false;
    s.function.cg.showing = false;
    s.function.stackDelta.showing = false;
    s.function.callconv.showing = false;
    s.function.noop.showing = false;
    s.function.mayReturn.showing = false;

    s.bblock.cfg.showingPredecessors = false;
    s.bblock.cfg.showingSuccessors = false;
    s.bblock.cfg.showingSharing = false;

    s.insn.address.showing = false;
    s.insn.address.fieldWidth = 8;
    s.insn.bytes.showing = false;
    s.insn.stackDelta.showing = false;
    s.insn.mnemonic.fieldWidth = 8;
    s.insn.operands.fieldWidth = 40;
    s.insn.comment.showing = false;
    s.insn.comment.usingDescription = true;             // but hidden by s.insn.comment.showing being false

    return s;
}

Sawyer::CommandLine::SwitchGroup
commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    using namespace CommandlineProcessing;

    SwitchGroup sg("Unparsing switches");
    sg.name("out");
    sg.doc("These switches control the formats used when converting the internal representation of instructions, basic "
           "blocks, data blocks, and functions to a textual representation.");

    //-----  Functions -----

    insertBooleanSwitch(sg, "function-reasons", settings.function.showingReasons,
                        "Show the list of reasons why a function was created.");

    insertBooleanSwitch(sg, "demangle-names", settings.function.showingDemangled,
                        "Show demangled names in preference to mangled names when a function has both.");

    insertBooleanSwitch(sg, "function-cg", settings.function.cg.showing,
                        "Show the function call graph. That is, for each function show the functions that call the said "
                        "function, and those functions that the said function calls.");

    insertBooleanSwitch(sg, "function-stack-delta", settings.function.stackDelta.showing,
                        "Show function stack deltas. The stack delta is the net effect that the function has on the "
                        "stack pointer. The output is only produced if a stack delta analysis has been performed; this "
                        "switch itself is not sufficient to cause that analysis to happen. See also, "
                        "@s{function-stack-delta-domain}.");

    sg.insert(Switch("function-stack-delta-domain")
              .argument("domain", enumParser(settings.function.stackDelta.concrete)
                        ->with("concrete", true)
                        ->with("symbolic", false))
              .doc("Whether to show stack deltas in a concrete domain or a symbolic domain.  This output is produced only "
                   "if stack deltas are enabled; see @s{function-stack-delta}. The default is " +
                   std::string(settings.function.stackDelta.concrete ? "concrete" : "symbolic") + "."));

    insertBooleanSwitch(sg, "function-callconv", settings.function.callconv.showing,
                        "Show function calling convention.  This output is only produced if a calling convention analysis "
                        "has been performed; this switch itself is not sufficient to cause that analysis to happen.");

    insertBooleanSwitch(sg, "function-noop", settings.function.noop.showing,
                        "Add a comment to indicate that a function has no effect on the machine state.  This output is only "
                        "produced if a no-op analysis has been performed; this switch itself is not sufficient to cause "
                        "that analysis to happen.");

    insertBooleanSwitch(sg, "function-may-return", settings.function.mayReturn.showing,
                        "Add a comment to indicate when a function cannot return to its caller. This output is only "
                        "produced if a may-return analysis has been performed; this switch itself is not sufficient to "
                        "cause that analysis to happen.");

    //----- Basic blocks -----

    insertBooleanSwitch(sg, "bb-cfg-predecessors", settings.bblock.cfg.showingPredecessors,
                        "For each basic block, show its control flow graph predecessors.");

    insertBooleanSwitch(sg, "bb-cfg-successors", settings.bblock.cfg.showingSuccessors,
                        "For each basic block, show its control flow graph successors.");

    insertBooleanSwitch(sg, "bb-cfg-sharing", settings.bblock.cfg.showingSharing,
                        "For each basic block, emit the list of functions that own the block in addition to the function "
                        "in which the block is listed.");

    //----- Data blocks -----

    //----- Instructions -----

    insertBooleanSwitch(sg, "insn-address", settings.insn.address.showing,
                        "Show the address of each instruction.");

    sg.insert(Switch("insn-address-width")
              .argument("nchars", positiveIntegerParser(settings.insn.address.fieldWidth))
              .doc("Minimum size of the address field in characters if addresses are enabled with @s{insn-address}. The "
                   "default is " + boost::lexical_cast<std::string>(settings.insn.address.fieldWidth) + "."));

    insertBooleanSwitch(sg, "insn-bytes", settings.insn.bytes.showing,
                        "Show the raw bytes that make up each instruction. See also, @s{insn-bytes-per-line}.");

    sg.insert(Switch("insn-bytes-per-line")
              .argument("n", positiveIntegerParser(settings.insn.bytes.perLine))
              .doc("Maximum number of bytes to show per line if raw bytes are enabled with @s{insn-bytes}. Additional bytes "
                   "will be shown on subsequent lines. The default is " +
                   boost::lexical_cast<std::string>(settings.insn.bytes.perLine) + "."));

    sg.insert(Switch("insn-bytes-width")
              .argument("nchars", positiveIntegerParser(settings.insn.bytes.fieldWidth))
              .doc("Minimum size of the raw bytes field in characters if raw bytes are enabled with @s{insn-bytes}. The "
                   "default is " + boost::lexical_cast<std::string>(settings.insn.bytes.fieldWidth) + "."));

    insertBooleanSwitch(sg, "insn-stack-delta", settings.insn.stackDelta.showing,
                        "For each instruction, show the stack delta. This is the value of the stack pointer relative to "
                        "the stack pointer at the beginning of the function. The value shown is the delta before the "
                        "said instruction executes.");

    sg.insert(Switch("insn-stack-delta-width")
              .argument("nchars", positiveIntegerParser(settings.insn.stackDelta.fieldWidth))
              .doc("Minimum size of the stack delta field in characters if stack deltas are enabled with @s{insn-stack-delta}. "
                   "The default is " + boost::lexical_cast<std::string>(settings.insn.stackDelta.fieldWidth) + "."));

    sg.insert(Switch("insn-mnemonic-width")
              .argument("nchars", positiveIntegerParser(settings.insn.mnemonic.fieldWidth))
              .doc("Minimum size of the instruction mnemonic field in characters. The default is " +
                   boost::lexical_cast<std::string>(settings.insn.mnemonic.fieldWidth) + "."));

    sg.insert(Switch("insn-operand-separator")
              .argument("string", anyParser(settings.insn.operands.separator))
              .doc("The string that will separate one instruction operand from another. The default is \"" +
                   StringUtility::cEscape(settings.insn.operands.separator) + "\"."));

    sg.insert(Switch("insn-operand-width")
              .argument("nchars", positiveIntegerParser(settings.insn.operands.fieldWidth))
              .doc("Minimum size of the instruction operands field in characters. The default is " +
                   boost::lexical_cast<std::string>(settings.insn.operands.fieldWidth) + "."));

    insertBooleanSwitch(sg, "insn-comment", settings.insn.comment.showing,
                        "Show comments for instructions that have them. The comments are shown to the right of each "
                        "instruction.");

    insertBooleanSwitch(sg, "insn-description", settings.insn.comment.usingDescription,
                        "If comments are being shown and an instruction has an empty comment, then use the instruction "
                        "description instead.  This is especially useful for users that aren't familiar with the "
                        "instruction mnemonics for this architecture.");

    return sg;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Top-level
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
Base::unparse(const P2::Partitioner &p) const {
    std::ostringstream ss;
    unparse(ss, p);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner &p, SgAsmInstruction *insn) const {
    std::ostringstream ss;
    unparse(ss, p, insn);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner &p, const Partitioner2::BasicBlock::Ptr &bb) const {
    std::ostringstream ss;
    unparse(ss, p, bb);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner &p, const Partitioner2::DataBlock::Ptr &db) const {
    std::ostringstream ss;
    unparse(ss, p, db);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner &p, const Partitioner2::Function::Ptr &f) const {
    std::ostringstream ss;
    unparse(ss, p, f);
    return ss.str();
}

void
Base::unparse(std::ostream &out, const Partitioner2::Partitioner &p) const {
    State state(p, settings(), *this);
    BOOST_FOREACH (P2::Function::Ptr f, p.functions())
        emitFunction(out, f, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, SgAsmInstruction *insn) const {
    State state(p, settings(), *this);
    emitInstruction(out, insn, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, const P2::BasicBlock::Ptr &bb) const {
    State state(p, settings(), *this);
    emitBasicBlock(out, bb, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, const P2::DataBlock::Ptr &db) const {
    State state(p, settings(), *this);
    emitDataBlock(out, db, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, const P2::Function::Ptr &f) const {
    State state(p, settings(), *this);
    emitFunction(out, f, state);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::emitFunction(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    FunctionGuard push(state, function);
    if (nextUnparser()) {
        nextUnparser()->emitFunction(out, function, state);
    } else {
        state.frontUnparser().emitFunctionPrologue(out, function, state);
        state.frontUnparser().emitFunctionBody(out, function, state);
        state.frontUnparser().emitFunctionEpilogue(out, function, state);
    }
}

void
Base::emitFunctionPrologue(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionPrologue(out, function, state);
    } else {
        out <<std::string(120, ';') <<"\n";
        if (settings().function.showingDemangled) {
            out <<";;; " <<function->printableName() <<"\n";
            if (function->demangledName() != function->name())
                out <<";;; mangled name is \"" <<StringUtility::cEscape(function->name()) <<"\"\n";
        } else {
            out <<";;; function " <<StringUtility::addrToString(function->address());
            if (!function->name().empty())
                out <<" \"" <<StringUtility::cEscape(function->name()) <<"\"";
            out <<"\n";
        }
        state.frontUnparser().emitFunctionComment(out, function, state);
        if (settings().function.showingReasons)
            state.frontUnparser().emitFunctionReasons(out, function, state);
        if (settings().function.cg.showing) {
            state.frontUnparser().emitFunctionCallers(out, function, state);
            state.frontUnparser().emitFunctionCallees(out, function, state);
        }
        if (settings().function.stackDelta.showing)
            state.frontUnparser().emitFunctionStackDelta(out, function, state);
        if (settings().function.callconv.showing)
            state.frontUnparser().emitFunctionCallingConvention(out, function, state);
        if (settings().function.mayReturn.showing)
            state.frontUnparser().emitFunctionMayReturn(out, function, state);
        if (settings().function.noop.showing)
            state.frontUnparser().emitFunctionNoopAnalysis(out, function, state);
    }
}

void
Base::emitFunctionBody(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionBody(out, function, state);
    } else {
        // If we're not emitting instruction addresses then we need some other way to identify basic blocks for branch targets.
        if (!settings().insn.address.showing) {
            state.basicBlockLabels().clear();
            BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
                std::string label = "L" + boost::lexical_cast<std::string>(state.basicBlockLabels().size()+1);
                state.basicBlockLabels().insertMaybe(bbVa, label);
            }
        }

        rose_addr_t nextBlockVa = 0;
        BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
            out <<"\n";
            if (P2::BasicBlock::Ptr bb = state.partitioner().basicBlockExists(bbVa)) {
                if (bbVa != *function->basicBlockAddresses().begin()) {
                    if (bbVa > nextBlockVa) {
                        out <<";;; skip forward " <<StringUtility::plural(bbVa - nextBlockVa, "bytes") <<"\n";
                    } else if (bbVa < nextBlockVa) {
                        out <<";;; skip backward " <<StringUtility::plural(nextBlockVa - bbVa, "bytes") <<"\n";
                    }
                }
                state.frontUnparser().emitBasicBlock(out, bb, state);
                if (bb->nInstructions() > 0) {
                    nextBlockVa = bb->fallthroughVa();
                } else {
                    nextBlockVa = bb->address();
                }
            }
        }
        BOOST_FOREACH (P2::DataBlock::Ptr db, function->dataBlocks()) {
            out <<"\n";
            state.frontUnparser().emitDataBlock(out, db, state);
        }
    }
}

void
Base::emitFunctionEpilogue(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionEpilogue(out, function, state);
    } else {
        out <<"\n\n";
    }
}

static void
addFunctionReason(std::vector<std::string> &strings /*in,out*/, unsigned &flags /*in,out*/,
                  unsigned bit, const std::string &why) {
    if (0 != (flags & bit)) {
        strings.push_back(why);
        flags &= ~bit;
    }
}

void
Base::emitFunctionReasons(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionReasons(out, function, state);
    } else {
        unsigned flags = function->reasons();
        std::vector<std::string> strings;
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_ENTRY_POINT,  "program entry point");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_IMPORT,       "import");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_THUNK,        "thunk");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_EXPORT,       "export");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_CALL_TARGET,  "function call target");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_CALL_INSN,    "possible function call target");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_EXCEPTION_HANDLER, "exception handler");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_EH_FRAME,     "referenced by ELF .eh_frame");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_SYMBOL,       "referenced by symbol table");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_PATTERN,      "pattern recognition");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_GRAPH,        "CFG traversal");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_PADDING,      "padding");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_DISCONT,      "discontiguous");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_INSNHEAD,     "possibly unreached (head)");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_LEFTOVERS,    "provisional");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_INTRABLOCK,   "possibly unreached (intra)");
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_USERDEF,      "user defined");
        if (flags != 0) {
            char buf[64];
            sprintf(buf, "0x%08x", flags);
            strings.push_back("other (" + std::string(buf));
        }
        if (!strings.empty())
            out <<";;; reasons for function: " <<boost::join(strings, ", ") <<"\n";
    }
}

void
Base::emitFunctionCallers(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionCallers(out, function, state);
    } else {
        P2::FunctionCallGraph::Graph::ConstVertexIterator vertex = state.cg().findFunction(function);
        if (state.cg().graph().isValidVertex(vertex)) {
            BOOST_FOREACH (const P2::FunctionCallGraph::Graph::Edge &edge, vertex->inEdges()) {
                out <<";;; ";
                switch (edge.value().type()) {
                    case P2::E_FUNCTION_CALL:
                        out <<"called from ";
                        break;
                    case P2::E_FUNCTION_XFER:
                        out <<"transfered from ";
                        break;
                    default:
                        out <<"predecessor ";
                        break;
                }
                out <<edge.source()->value()->printableName();
                if (edge.value().count() > 1)
                    out <<" " <<edge.value().count() <<" times";
                out <<"\n";
            }
        }
    }
}

void
Base::emitFunctionCallees(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionCallees(out, function, state);
    } else {
        P2::FunctionCallGraph::Graph::ConstVertexIterator vertex = state.cg().findFunction(function);
        if (state.cg().graph().isValidVertex(vertex)) {
            BOOST_FOREACH (const P2::FunctionCallGraph::Graph::Edge &edge, vertex->outEdges()) {
                out <<";;; ";
                switch (edge.value().type()) {
                    case P2::E_FUNCTION_CALL:
                        out <<"calls ";
                        break;
                    case P2::E_FUNCTION_XFER:
                        out <<"transfers to ";
                        break;
                    default:
                        out <<"successor ";
                        break;
                }
                out <<edge.target()->value()->printableName();
                if (edge.value().count() > 1)
                    out <<" " <<edge.value().count() <<" times";
                out <<"\n";
            }
        }
    }
}

void
Base::emitFunctionComment(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionComment(out, function, state);
    } else {
        std::string s = boost::trim_copy(function->comment());
        if (!s.empty()) {
            out <<";;;\n";
            state.frontUnparser().emitCommentBlock(out, s, state, ";;; ");
            out <<";;;\n";
        }
    }
}

void
Base::emitFunctionStackDelta(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionStackDelta(out, function, state);
    } else {
        if (settings().function.stackDelta.concrete) {
            int64_t delta = function->stackDeltaConcrete();
            if (delta != SgAsmInstruction::INVALID_STACK_DELTA)
                out <<";;; function stack delta is " <<StringUtility::toHex2(delta, 64) <<"\n";
        } else if (S2::BaseSemantics::SValuePtr delta = function->stackDelta()) {
            out <<";;; function stack delta is " <<*delta <<"\n";
        }
    }
}

void
Base::emitFunctionCallingConvention(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionCallingConvention(out, function, state);
    } else {
        const CallingConvention::Analysis &analyzer = function->callingConventionAnalysis();
        if (analyzer.hasResults()) {
            if (analyzer.didConverge()) {
                // Calling convention analysis
                std::ostringstream ss;
                analyzer.print(ss, true /*multi-line*/);
                state.frontUnparser().emitCommentBlock(out, "calling convention analysis:", state);
                state.frontUnparser().emitCommentBlock(out, ss.str(), state, ";;;   ");

                // Calling convention dictionary matches
                CallingConvention::Dictionary matches = state.partitioner().functionCallingConventionDefinitions(function);
                std::string s = "calling convention definitions:";
                if (!matches.empty()) {
                    BOOST_FOREACH (const CallingConvention::Definition::Ptr &ccdef, matches)
                        s += " " + ccdef->name();
                } else {
                    s += " unknown";
                }
                state.frontUnparser().emitCommentBlock(out, s, state);
            } else {
                out <<";;; calling convention analysis did not converge\n";
            }
        }
    }
}

void
Base::emitFunctionNoopAnalysis(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionNoopAnalysis(out, function, state);
    } else {
        if (function->isNoop().getOptional().orElse(false))
            out <<";;; this function is a no-op\n";
    }
}

void
Base::emitFunctionMayReturn(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionMayReturn(out, function, state);
    } else {
        if (!state.partitioner().functionOptionalMayReturn(function).orElse(true))
            out <<";;; this function does not return to its caller\n";
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Basic Blocks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::emitBasicBlock(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlock(out, bb, state);
    } else {
        BasicBlockGuard push(state, bb);
        state.frontUnparser().emitBasicBlockPrologue(out, bb, state);
        state.frontUnparser().emitBasicBlockBody(out, bb, state);
        state.frontUnparser().emitBasicBlockEpilogue(out, bb, state);
    }
}

void
Base::emitBasicBlockPrologue(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockPrologue(out, bb, state);
    } else {
        state.frontUnparser().emitBasicBlockComment(out, bb, state);
        if (settings().bblock.cfg.showingSharing)
            state.frontUnparser().emitBasicBlockSharing(out, bb, state);
        if (settings().bblock.cfg.showingPredecessors)
            state.frontUnparser().emitBasicBlockPredecessors(out, bb, state);

        // Comment warning about block not being the function entry point.
        if (state.currentFunction() && bb->address() == *state.currentFunction()->basicBlockAddresses().begin() &&
            bb->address() != state.currentFunction()->address()) {
            out <<"\t;; this is not the function entry point; entry point is ";
            if (settings().insn.address.showing) {
                out <<StringUtility::addrToString(state.currentFunction()->address()) <<"\n";
            } else {
                out <<state.basicBlockLabels().getOrElse(state.currentFunction()->address(),
                                                         StringUtility::addrToString(state.currentFunction()->address())) <<"\n";
            }
        }
    }
}

void
Base::emitBasicBlockBody(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    ASSERT_not_null(bb);
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockBody(out, bb, state);
    } else {
        if (0 == bb->nInstructions()) {
            out <<"no instructions";
        } else {
            state.nextInsnLabel(state.basicBlockLabels().getOrElse(bb->address(), ""));
            BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                state.frontUnparser().emitInstruction(out, insn, state);
                out <<"\n";
                state.nextInsnLabel("");
            }
        }
    }
}

void
Base::emitBasicBlockEpilogue(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockEpilogue(out, bb, state);
    } else {
        BOOST_FOREACH (P2::DataBlock::Ptr db, bb->dataBlocks())
            state.frontUnparser().emitDataBlock(out, db, state);
        if (settings().bblock.cfg.showingSuccessors)
            state.frontUnparser().emitBasicBlockSuccessors(out, bb, state);
    }
}

void
Base::emitBasicBlockComment(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockComment(out, bb, state);
    } else {
        std::string s = boost::trim_copy(bb->comment());
        if (!s.empty())
            state.frontUnparser().emitCommentBlock(out, s, state, "\t;; ");
    }
}

void
Base::emitBasicBlockSharing(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    ASSERT_not_null(bb);
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockSharing(out, bb, state);
    } else {
        std::vector<P2::Function::Ptr> functions = state.partitioner().functionsOwningBasicBlock(bb);
        std::vector<P2::Function::Ptr>::iterator current =
            std::find(functions.begin(), functions.end(), state.currentFunction());
        if (current != functions.end())
            functions.erase(current);
        BOOST_FOREACH (P2::Function::Ptr function, functions)
            out <<"\t;; block also owned by " <<function->printableName() <<"\n";
    }
}

static std::string
edgeTypeName(const P2::EdgeType &edgeType) {
    std::string retval = stringifyBinaryAnalysisPartitioner2EdgeType(edgeType, "E_");
    BOOST_FOREACH (char &ch, retval)
        ch = '_' == ch ? ' ' : tolower(ch);
    return retval;
}

void
Base::emitBasicBlockPredecessors(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    ASSERT_not_null(bb);
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockPredecessors(out, bb, state);
    } else {
        P2::ControlFlowGraph::ConstVertexIterator vertex = state.partitioner().findPlaceholder(bb->address());
        ASSERT_require(state.partitioner().cfg().isValidVertex(vertex));

        Sawyer::Container::Map<rose_addr_t, std::string> preds;
        BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->inEdges()) {
            P2::ControlFlowGraph::ConstVertexIterator pred = edge.source();
            switch (pred->value().type()) {
                case P2::V_BASIC_BLOCK:
                    if (pred->value().bblock()->nInstructions() > 1) {
                        rose_addr_t insnVa = pred->value().bblock()->instructions().back()->get_address();
                        std::string s = "instruction " + StringUtility::addrToString(insnVa) +
                                        " from " + pred->value().bblock()->printableName();
                        preds.insert(insnVa, edgeTypeName(edge.value().type()) + " edge from " + s);
                    } else {
                        std::ostringstream ss;
                        state.frontUnparser().emitAddress(ss, pred->value().address(), state);
                        preds.insert(pred->value().address(), edgeTypeName(edge.value().type()) + " edge from " + ss.str());
                    }
                    break;
                case P2::V_USER_DEFINED: {
                    std::ostringstream ss;
                    state.frontUnparser().emitAddress(ss, pred->value().address(), state);
                    preds.insert(pred->value().address(), edgeTypeName(edge.value().type()) + " edge from " + ss.str());
                    break;
                }
                case P2::V_NONEXISTING:
                case P2::V_UNDISCOVERED:
                case P2::V_INDETERMINATE:
                    ASSERT_not_reachable("vertex type should have no successors");
                default:
                    ASSERT_not_implemented("cfg vertex type");
            }
        }

        BOOST_FOREACH (const std::string &s, preds.values())
            out <<"\t;; predecessor: " <<s <<"\n";
    }
}

void
Base::emitBasicBlockSuccessors(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockSuccessors(out, bb, state);
    } else {
        const P2::BasicBlock::Successors &succs = state.partitioner().basicBlockSuccessors(bb);
        std::vector<std::string> strings;

        // Real successors
        BOOST_FOREACH (const P2::BasicBlock::Successor &succ, succs) {
            std::string s = edgeTypeName(succ.type()) + " edge to ";
            ASSERT_not_null(succ.expr());
            SymbolicExpr::Ptr expr = succ.expr()->get_expression();
            if (expr->isNumber() && expr->nBits() <= 64) {
                std::ostringstream ss;
                state.frontUnparser().emitAddress(ss, expr->toInt(), state);
                s += ss.str();
            } else if (expr->isLeafNode()) {
                s += "unknown";
            } else {
                std::ostringstream ss;
                ss <<*expr;
                s += ss.str();
            }

            if (std::find(strings.begin(), strings.end(), s) == strings.end())
                strings.push_back(s);
        }

        // Ghost successors due to opaque predicates
        if (bb->ghostSuccessors().isCached()) {
            BOOST_FOREACH (rose_addr_t va, bb->ghostSuccessors().get()) {
                std::ostringstream ss;
                state.frontUnparser().emitAddress(ss, va, state);
                if (std::find(strings.begin(), strings.end(), ss.str()) == strings.end() &&
                    std::find(strings.begin(), strings.end(), ss.str() + " (ghost)") == strings.end())
                    strings.push_back(ss.str() + " (ghost)");
            }
        }

        // Output
        if (strings.empty()) {
            out <<"\t;; successor: none\n";
        } else {
            BOOST_FOREACH (const std::string &s, strings)
                out <<"\t;; successor: " <<s <<"\n";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Data Blocks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::emitDataBlock(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlock(out, db, state);
    } else {
        state.frontUnparser().emitDataBlockPrologue(out, db, state);
        state.frontUnparser().emitDataBlockBody(out, db, state);
        state.frontUnparser().emitDataBlockEpilogue(out, db, state);
    }
}

void
Base::emitDataBlockPrologue(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlockPrologue(out, db, state);
    } else {
    }
}

void
Base::emitDataBlockBody(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlockBody(out, db, state);
    } else {
        out <<"data blocks not implemented yet";
    }
}

void
Base::emitDataBlockEpilogue(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlockEpilogue(out, db, state);
    } else {
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Instructions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::emitInstruction(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitInstruction(out, insn, state);
    } else {
        state.frontUnparser().emitInstructionPrologue(out, insn, state);
        state.frontUnparser().emitInstructionBody(out, insn, state);
        state.frontUnparser().emitInstructionEpilogue(out, insn, state);
    }
}

void
Base::emitInstructionPrologue(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitInstructionPrologue(out, insn, state);
    } else {
    }
}

void
Base::emitInstructionBody(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitInstructionBody(out, insn, state);
    } else {
        std::vector<std::string> parts;
        std::vector<size_t> fieldWidths;

        // Address or label
        if (settings().insn.address.showing) {
            // Use the instruction address instead of any label
            if (insn) {
                std::ostringstream ss;
                state.frontUnparser().emitInstructionAddress(ss, insn, state);
                parts.push_back(ss.str());
            } else {
                parts.push_back("");
            }
        } else if (!state.nextInsnLabel().empty()) {
            // Use the label that has been specified in the state
            parts.push_back(state.nextInsnLabel() + ":");
        } else {
            // No address or label
            parts.push_back("");
        }
        fieldWidths.push_back(settings().insn.address.fieldWidth);
        state.nextInsnLabel("");

        // Raw bytes
        if (settings().insn.bytes.showing) {
            if (insn) {
                std::ostringstream ss;
                state.frontUnparser().emitInstructionBytes(ss, insn, state);
                parts.push_back(ss.str());
            } else {
                parts.push_back("");
            }
            fieldWidths.push_back(settings().insn.bytes.fieldWidth);
        }

        // Stack delta
        if (settings().insn.stackDelta.showing) {
            if (insn) {
                std::ostringstream ss;
                state.frontUnparser().emitInstructionStackDelta(ss, insn, state);
                parts.push_back(ss.str());
            } else {
                parts.push_back("");
            }
            fieldWidths.push_back(settings().insn.stackDelta.fieldWidth);
        }

        // Mnemonic
        if (insn) {
            std::ostringstream ss;
            state.frontUnparser().emitInstructionMnemonic(ss, insn, state);
            parts.push_back(ss.str());
        } else {
            parts.push_back("none");
        }
        fieldWidths.push_back(settings().insn.mnemonic.fieldWidth);


        // Operands
        if (insn) {
            std::ostringstream ss;
            state.frontUnparser().emitInstructionOperands(ss, insn, state);
            parts.push_back(ss.str());
        } else {
            parts.push_back("");
        }
        fieldWidths.push_back(settings().insn.operands.fieldWidth);

        // Comment
        if (settings().insn.comment.showing) {
            std::ostringstream ss;
            state.frontUnparser().emitInstructionComment(ss, insn, state);
            parts.push_back(ss.str());
            fieldWidths.push_back(settings().insn.comment.fieldWidth);
        }

        out <<juxtaposeColumns(parts, fieldWidths);
    }
}

void
Base::emitInstructionEpilogue(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitInstructionEpilogue(out, insn, state);
    } else {
    }
}

void
Base::emitInstructionAddress(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionAddress(out, insn, state);
    } else {
        out <<StringUtility::addrToString(insn->get_address()) <<":";
    }
}

void
Base::emitInstructionBytes(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionBytes(out, insn, state);
    } else {
        const SgUnsignedCharList &bytes = insn->get_raw_bytes();
        for (size_t i = 0; i < bytes.size(); ++i) {
            if (0 == i) {
                // void
            } else if (0 == i % 8) {
                out <<"\n";
            } else {
                out <<" ";
            }
            Diagnostics::mfprintf(out)("%02x", bytes[i]);
        }
    }
}

void
Base::emitInstructionStackDelta(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionStackDelta(out, insn, state);
    } else {
        // Although SgAsmInstruction has stackDeltaIn and stackDelta out data members, they're not initialized or used when the
        // instruction only exists inside a Partitioner2 object -- they're only initialized when a complete AST is created from
        // the Partitioner2. Instead, we need to look at the enclosing function's stack delta analysis.  Since basic blocks can
        // be shared among functions, this method prints the stack deltas from the perspective of the function we're emitting.
        if (P2::Function::Ptr function = state.currentFunction()) {
            const StackDelta::Analysis &sdAnalysis = function->stackDeltaAnalysis();
            if (sdAnalysis.hasResults()) {
                int64_t delta = sdAnalysis.toInt(sdAnalysis.instructionInputStackDeltaWrtFunction(insn));
                if (SgAsmInstruction::INVALID_STACK_DELTA == delta) {
                    out <<" ??";
                } else if (delta == 0) {
                    out <<" 00";
                } else if (delta > 0) {
                    Diagnostics::mfprintf(out)("+%02x", (unsigned)delta);
                } else {
                    Diagnostics::mfprintf(out)("-%02x", (unsigned)(-delta));
                }
                return;
            }
        }
        out <<"??";
    }
}

void
Base::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionMnemonic(out, insn, state);
    } else {
        out <<insn->get_mnemonic();
    }
}

void
Base::emitInstructionOperands(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionOperands(out, insn, state);
    } else {
        const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        for (size_t i=0; i<operands.size(); ++i) {
            if (i > 0)
                out <<settings().insn.operands.separator;
            state.frontUnparser().emitOperand(out, operands[i], state);
        }
    }
}

void
Base::emitInstructionComment(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionComment(out, insn, state);
    } else {
        std::string comment = insn->get_comment();
        boost::trim(comment);
        if (comment.empty() && settings().insn.comment.usingDescription)
            comment = insn->description();
        if (!comment.empty())
            out <<"; " <<StringUtility::cEscape(comment);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Operand expressions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::emitOperand(std::ostream &out, SgAsmExpression *expr, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitOperand(out, expr, state);
    } else {
        state.frontUnparser().emitOperandPrologue(out, expr, state);
        state.frontUnparser().emitOperandBody(out, expr, state);
        state.frontUnparser().emitOperandEpilogue(out, expr, state);
    }
}

void
Base::emitOperandPrologue(std::ostream &out, SgAsmExpression *expr, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitOperandPrologue(out, expr, state);
    } else {
    }
}

void
Base::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitOperandBody(out, expr, state);
    } else {
    }
}

void
Base::emitOperandEpilogue(std::ostream &out, SgAsmExpression *expr, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitOperandEpilogue(out, expr, state);
    } else {
    }
}

bool
Base::emitAddress(std::ostream &out, rose_addr_t va, State &state, bool always) const {
    if (nextUnparser()) {
        return nextUnparser()->emitAddress(out, va, state, always);
    } else {
        std::string label;
        if (state.basicBlockLabels().getOptional(va).assignTo(label)) {
            out <<"basic block " <<label;
            return true;
        }
        if (P2::Function::Ptr f = state.partitioner().functionExists(va)) {
            out <<f->printableName();
            return true;
        }
        if (P2::BasicBlock::Ptr bb = state.partitioner().basicBlockExists(va)) {
            out <<bb->printableName();
            return true;
        }
        if (always) {
            out <<StringUtility::addrToString(va);
            return false;
        }
        return false;
    }
}

bool
Base::emitAddress(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state, bool always) const {
    if (nextUnparser()) {
        return nextUnparser()->emitAddress(out, bv, state, always);
    } else {
        if (bv.size() <= 64) {
            return state.frontUnparser().emitAddress(out, bv.toInteger(), state, always);
        } else if (always) {
            out <<"0x" <<bv.toHex();
            return false;
        } else {
            return false;
        }
    }
}

std::vector<std::string>
Base::emitInteger(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state, bool isSigned) const {
    if (nextUnparser()) {
        return nextUnparser()->emitInteger(out, bv, state, isSigned);
    } else {
        std::vector<std::string> comments;
        std::string label;

        if (bv.size() == state.partitioner().instructionProvider().instructionPointerRegister().get_nbits() &&
            state.frontUnparser().emitAddress(out, bv, state, false)) {
            // address with a label, existing basic block, or existing function.
        } else if (bv.isEqualToZero()) {
            out <<"0";
        } else if (bv.size() <= 64 && bv.toInteger() < 16) {
            out <<bv.toInteger();
        } else if (bv.size() == 32 && bv.toInteger() >= 0xffff && bv.toInteger() < 0xffff0000) {
            state.frontUnparser().emitAddress(out, bv, state);
        } else if (bv.size() == 64 && bv.toInteger() >= 0xffff && bv.toInteger() < 0xffffffffffff0000ull) {
            state.frontUnparser().emitAddress(out, bv, state);
        } else if (bv.size() > 64) {
            out <<"0x" <<bv.toHex();                        // too wide for decimal representation
        } else {
            out <<"0x" <<bv.toHex();
            uint64_t ui = bv.toInteger();
            comments.push_back(boost::lexical_cast<std::string>(ui));
            if (isSigned) {
                int64_t si = (int64_t)IntegerOps::signExtend2(ui, bv.size(), 64);
                if (si < 0)
                    comments.push_back(boost::lexical_cast<std::string>(si));
            }
        }
        return comments;
    }
}

std::vector<std::string>
Base::emitSignedInteger(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state) const {
    if (nextUnparser()) {
        return nextUnparser()->emitSignedInteger(out, bv, state);
    } else {
        return state.frontUnparser().emitInteger(out, bv, state, true /*signed*/);
    }
}

std::vector<std::string>
Base::emitUnsignedInteger(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state) const {
    if (nextUnparser()) {
        return nextUnparser()->emitUnsignedInteger(out, bv, state);
    } else {
        return state.frontUnparser().emitInteger(out, bv, state, false /*unsigned*/);
    }
}

void
Base::emitRegister(std::ostream &out, RegisterDescriptor reg, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitRegister(out, reg, state);
    } else {
        out <<state.registerNames()(reg);
    }
}

void
Base::emitCommentBlock(std::ostream &out, const std::string &comment, State &state, const std::string &prefix) const {
    if (nextUnparser()) {
        nextUnparser()->emitCommentBlock(out, comment, state, prefix);
    } else {
        std::vector<std::string> lines = StringUtility::split('\n', comment);
        BOOST_FOREACH (const std::string &line, lines)
            out <<prefix <<line <<"\n";
    }
}

void
Base::emitTypeName(std::ostream &out, SgAsmType *type, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitTypeName(out, type, state);
    } else {
        if (NULL == type) {
            out <<"notype";

        } else if (SgAsmIntegerType *it = isSgAsmIntegerType(type)) {
            if (it->get_isSigned()) {
                out <<"i";
            } else {
                out <<"u";
            }
            out <<it->get_nBits();

        } else if (SgAsmFloatType *ft = isSgAsmFloatType(type)) {
            out <<"f" <<ft->get_nBits();

        } else {
            ASSERT_not_implemented(type->toString());
        }
    }
}


} // namespace
} // namespace
} // namespace
