#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>

#include <BaseSemantics2.h>
#include <BinaryReachability.h>
#include <BinaryUnparserBase.h>
#include <CommandLine.h>
#include <Diagnostics.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/ProgressBar.h>
#include <SourceLocation.h>
#include <stringify.h>
#include <TraceSemantics2.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant.hpp>
#include <ctype.h>
#include <sstream>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics2;

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

Sawyer::Message::Facility mlog;

// class method
void initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Unparser");
        mlog.comment("generating assembly listings (vers 2)");
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

State::State(const P2::Partitioner &p, const Settings &settings, const Base &frontUnparser)
    : partitioner_(p), registerNames_(p.instructionProvider().registerDictionary()), frontUnparser_(frontUnparser) {
    if (settings.function.cg.showing)
        cg_ = p.functionCallGraph(P2::AllowParallelEdges::NO);
    intraFunctionCfgArrows_.arrows.arrowStyle(settings.arrow.style, EdgeArrows::LEFT);
    intraFunctionBlockArrows_.arrows.arrowStyle(settings.arrow.style, EdgeArrows::LEFT);
    globalBlockArrows_.arrows.arrowStyle(settings.arrow.style, EdgeArrows::LEFT);
    globalBlockArrows_.flags.set(ArrowMargin::ALWAYS_RENDER);
    cfgArrowsPointToInsns_ = !settings.bblock.cfg.showingPredecessors || !settings.bblock.cfg.showingSuccessors;
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

const std::vector<Reachability::ReasonFlags>
State::cfgVertexReachability() const {
    return cfgVertexReachability_;
}

void
State::cfgVertexReachability(const std::vector<Reachability::ReasonFlags> &reachability) {
    cfgVertexReachability_ = reachability;
}

Reachability::ReasonFlags
State::isCfgVertexReachable(size_t vertexId) const {
    return vertexId < cfgVertexReachability_.size() ? cfgVertexReachability_[vertexId] : Reachability::ASSUMED;
}

void
State::reachabilityName(Reachability::Reason value, const std::string &name) {
    if (name.empty()) {
        reachabilityNames_.erase(value);
    } else {
        reachabilityNames_.insert(value, name);
    }
}

std::string
State::reachabilityName(Reachability::ReasonFlags value) const {
    std::string s;
    if (reachabilityNames_.getOptional(value.vector()).assignTo(s))
        return s;

    std::vector<std::string> names;
    for (size_t i = 0; i < 8*sizeof(Reachability::ReasonFlags::Vector); ++i) {
        Reachability::ReasonFlags bit(1U << i);
        if (value.isAllSet(bit)) {
            if (reachabilityNames_.getOptional(bit.vector()).assignTo(s)) {
                // void
            } else if (bit.vector() >= Reachability::USER_DEFINED_0) {
                s = "user-defined";
                for (size_t j=0; j < 8*sizeof(Reachability::ReasonFlags::Vector); j++) {
                    if (bit.vector() >> j == Reachability::USER_DEFINED_0) {
                        s += "-" + StringUtility::numberToString(j);
                        break;
                    }
                }
            } else if (const char *cs = stringify::Rose::BinaryAnalysis::Reachability::Reason(bit.vector())) {
                s = cs;
                BOOST_FOREACH (char &ch, s)
                    ch = tolower(ch);
            } else {
                s = StringUtility::toHex2(bit.vector(), 8*sizeof(Reachability::ReasonFlags::Vector), false, false);
            }
            names.push_back(s);
        }
    }

    return StringUtility::join(", ", names);
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

void
State::thisIsBasicBlockFirstInstruction() {
    if (cfgArrowsPointToInsns())
        intraFunctionCfgArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_START);
    intraFunctionBlockArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_START);
    globalBlockArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_START);
}

void
State::thisIsBasicBlockLastInstruction() {
    if (cfgArrowsPointToInsns())
        intraFunctionCfgArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_END);
    intraFunctionBlockArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_END);
    globalBlockArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_END);
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
// State::Margin
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
ArrowMargin::render(Sawyer::Optional<EdgeArrows::VertexId> currentEntity) {
    // Arrow output only starts after we've seen a START or END and we have our first pointable entity ID
    if (currentEntity && (flags.isSet(POINTABLE_ENTITY_START) || flags.isSet(POINTABLE_ENTITY_END)))
        latestEntity = *currentEntity;

    // Generate output if we've seen any pointable entities, even if we're not currently in a pointable entity.
    if (latestEntity) {
        if (flags.isSet(POINTABLE_ENTITY_START) && flags.isSet(POINTABLE_ENTITY_END)) {
            // The caller wants to show the arrow sources and targets both on the same line. We can't render that, so
            // do just one and delay the other until the next line.
            flags.clear(POINTABLE_ENTITY_START);
            if (arrows.nTargets(*latestEntity) > 0) {
                flags.set(POINTABLE_ENTITY_INSIDE);
                return arrows.render(*latestEntity, EdgeArrows::FIRST_LINE);
            } else {
                flags.clear(POINTABLE_ENTITY_END);
                flags.clear(POINTABLE_ENTITY_INSIDE);
                return arrows.render(*latestEntity, EdgeArrows::LAST_LINE);
            }

        } else if (flags.testAndClear(POINTABLE_ENTITY_START)) {
            flags.set(POINTABLE_ENTITY_INSIDE);
            return arrows.render(*latestEntity, EdgeArrows::FIRST_LINE);

        } else if (flags.testAndClear(POINTABLE_ENTITY_END)) {
            flags.clear(POINTABLE_ENTITY_INSIDE);
            return arrows.render(*latestEntity, EdgeArrows::LAST_LINE);

        } else if (flags.isSet(POINTABLE_ENTITY_INSIDE)) {
            return arrows.render(*latestEntity, EdgeArrows::MIDDLE_LINE);

        } else {
            return arrows.render(*latestEntity, EdgeArrows::INTER_LINE);
        }
    } else if (flags.isSet(ALWAYS_RENDER)) {
        return arrows.renderBlank();
    }
    return "";
}

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
    function.showingSourceLocation = true;
    function.showingReasons = true;
    function.showingDemangled = true;
    function.cg.showing = true;
    function.stackDelta.showing = false;                // very slow for some reason
    function.stackDelta.concrete = true;
    function.callconv.showing = true;
    function.noop.showing = true;
    function.mayReturn.showing = true;

    bblock.showingSourceLocation = true;
    bblock.cfg.showingPredecessors = true;
    bblock.cfg.showingSuccessors = true;
    bblock.cfg.showingSharing = true;
    bblock.cfg.showingArrows = false;
    bblock.reach.showingReachability = true;

    dblock.showingSourceLocation = true;

    insn.address.showing = true;
    insn.address.fieldWidth = 11;                       // "0x" + 8 hex digits + ":"
    insn.bytes.showing = true;
    insn.bytes.perLine = 8;
    insn.bytes.fieldWidth = 25;
    insn.stackDelta.showing = true;
    insn.stackDelta.fieldWidth = 2;
    insn.mnemonic.fieldWidth = 8;
    insn.mnemonic.semanticFailureMarker = "[!]";
    insn.operands.separator = ", ";
    insn.operands.fieldWidth = 40;
    insn.operands.showingWidth = false;
    insn.comment.showing = true;
    insn.comment.usingDescription = true;
    insn.comment.pre = "; ";
    insn.comment.fieldWidth = 1;
    insn.semantics.showing = false;                     // not usually desired, and somewhat slow
    insn.semantics.tracing = false;                     // not usually desired even for full output
    insn.semantics.formatter.set_show_latest_writers(false);
    insn.semantics.formatter.set_line_prefix("        ;; state: ");

    arrow.style = EdgeArrows::UNICODE_2;
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
    s.function.showingSourceLocation = false;
    s.function.showingReasons = false;
    s.function.showingDemangled = false;
    s.function.cg.showing = false;
    s.function.stackDelta.showing = false;
    s.function.callconv.showing = false;
    s.function.noop.showing = false;
    s.function.mayReturn.showing = false;

    s.bblock.showingSourceLocation = false;
    s.bblock.cfg.showingPredecessors = false;
    s.bblock.cfg.showingSuccessors = false;
    s.bblock.cfg.showingSharing = false;
    s.bblock.cfg.showingArrows = false;
    s.bblock.reach.showingReachability = false;

    s.dblock.showingSourceLocation = false;

    s.insn.address.showing = false;
    s.insn.address.fieldWidth = 8;
    s.insn.bytes.showing = false;
    s.insn.stackDelta.showing = false;
    s.insn.mnemonic.fieldWidth = 8;
    s.insn.mnemonic.semanticFailureMarker = "[!]";
    s.insn.operands.fieldWidth = 40;
    s.insn.operands.showingWidth = false;
    s.insn.comment.showing = false;
    s.insn.comment.usingDescription = true;             // but hidden by s.insn.comment.showing being false
    s.insn.semantics.showing = false;
    s.insn.semantics.tracing = false;

    s.arrow.style = EdgeArrows::UNICODE_2;

    return s;
}

Sawyer::CommandLine::SwitchGroup
commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    using namespace CommandlineProcessing;
    using namespace Rose::CommandLine;

    SwitchGroup sg("Unparsing switches");
    sg.name("unparse");
    sg.doc("These switches control the formats used when converting the internal representation of instructions, basic "
           "blocks, data blocks, and functions to a textual representation.");

    //-----  Functions -----

    insertBooleanSwitch(sg, "function-source-location", settings.function.showingSourceLocation,
                        "Show the file name, line number, and column number of the function's definition in source code "
                        "if that information is available.");

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

    insertBooleanSwitch(sg, "bb-source-location", settings.bblock.showingSourceLocation,
                        "Show the file name, line number, and column number of the basic block's definition in source code "
                        "if that information is available.");

    insertBooleanSwitch(sg, "bb-cfg-predecessors", settings.bblock.cfg.showingPredecessors,
                        "For each basic block, show its control flow graph predecessors.");

    insertBooleanSwitch(sg, "bb-cfg-successors", settings.bblock.cfg.showingSuccessors,
                        "For each basic block, show its control flow graph successors.");

    insertBooleanSwitch(sg, "bb-cfg-arrows", settings.bblock.cfg.showingArrows,
                        "Show control flow arrows. Only arrows corresponding to CFG edges where both endpoints are within "
                        "the current function are shown; arrows for things like function calls (unless the call is to "
                        "the current function) are not shownn because they would end up needing a very wide margin for "
                        "most real-life programs.");

    insertBooleanSwitch(sg, "bb-cfg-sharing", settings.bblock.cfg.showingSharing,
                        "For each basic block, emit the list of functions that own the block in addition to the function "
                        "in which the block is listed.");

    insertBooleanSwitch(sg, "bb-reachability", settings.bblock.reach.showingReachability,
                        "For each basic block, emit information about whether the block is reachable according to the "
                        "reachability analysis. If the unparser wasn't given any reachability analysis results then "
                        "nothing is shown.");

    //----- Data blocks -----

    insertBooleanSwitch(sg, "db-source-location", settings.dblock.showingSourceLocation,
                        "Show the file name, line number, and column number of the data block's definition in source code "
                        "if that information is available.");

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

    insertBooleanSwitch(sg, "insn-operand-size", settings.insn.operands.showingWidth,
                        "Show the width in bits of each term in an operand expression. The width is shown in square brackets "
                        "after the term, similar to how it's shown for symbolic expressions. Although memory dereferences are "
                        "also shown in square brackets, they are formatted different.");

    insertBooleanSwitch(sg, "insn-comment", settings.insn.comment.showing,
                        "Show comments for instructions that have them. The comments are shown to the right of each "
                        "instruction.");

    insertBooleanSwitch(sg, "insn-description", settings.insn.comment.usingDescription,
                        "If comments are being shown and an instruction has an empty comment, then use the instruction "
                        "description instead.  This is especially useful for users that aren't familiar with the "
                        "instruction mnemonics for this architecture.");

    insertBooleanSwitch(sg, "insn-semantics", settings.insn.semantics.showing,
                        "Run each instruction on a clean semantic state and show the results. This is useful if you want "
                        "to see the effect of each instruction.");

    insertBooleanSwitch(sg, "insn-semantics-trace", settings.insn.semantics.tracing,
                        "Show a trace of the individual semantic operations when showing semantics rather than just showing the "
                        "net effect.");

    sg.insert(Switch("insn-semantic-failure")
              .argument("string", anyParser(settings.insn.mnemonic.semanticFailureMarker))
              .doc("String to append to instruction mnemonic when the instruction is the cause of a semantic failure. The default "
                   "is \"" + StringUtility::cEscape(settings.insn.mnemonic.semanticFailureMarker) + "\"."));

    //----- Arrows -----
    sg.insert(Switch("arrow-style")
              .argument("preset", enumParser<EdgeArrows::ArrowStylePreset>(settings.arrow.style)
                        ->with("unicode-1", EdgeArrows::UNICODE_1)
                        ->with("unicode-2", EdgeArrows::UNICODE_2)
                        ->with("ascii-1", EdgeArrows::ASCII_1)
                        ->with("ascii-2", EdgeArrows::ASCII_2)
                        ->with("ascii-3", EdgeArrows::ASCII_3))
              .doc("Style of the left-margin arrows used for things like control flow, call graphs, model checking "
                   "paths, etc.  The choices are:"

                   "@named{unicode-1}{Arrows use single-column Unicode characters." +
                   std::string(EdgeArrows::UNICODE_1==settings.arrow.style ? " This is the default.":"") + "}"

                   "@named{unicode-2}{Arrows use double-column Unicode characters." +
                   std::string(EdgeArrows::UNICODE_2==settings.arrow.style ? " This is the default.":"") + "}"

                   "@named{ascii-1}{Arrows use single-column ASCII-art." +
                   std::string(EdgeArrows::ASCII_1==settings.arrow.style ? " This is the default.":"") + "}"

                   "@named{ascii-2}{Arrows use double-column ASCII-art." +
                   std::string(EdgeArrows::ASCII_2==settings.arrow.style ? " This is the default.":"") + "}"

                   "@named{ascii-3}{Arrows use triple-column ASCII-art." +
                   std::string(EdgeArrows::ASCII_3==settings.arrow.style ? " This is the default.":"") + "}"));

    return sg;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Top-level
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
Base::unparse(const P2::Partitioner &p, const Progress::Ptr &progress) const {
    std::ostringstream ss;
    unparse(ss, p, progress);
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
Base::unparse(std::ostream &out, const Partitioner2::Partitioner &p, const Progress::Ptr &progress) const {
    Sawyer::ProgressBar<size_t> progressBar(p.nFunctions(), mlog[MARCH], "unparse");
    progressBar.suffix(" functions");
    State state(p, settings(), *this);
    initializeState(state);
    BOOST_FOREACH (P2::Function::Ptr f, p.functions()) {
        ++progressBar;
        if (progress)
            progress->update(Progress::Report("unparse", progressBar.ratio()));
        emitFunction(out, f, state);
    }
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, SgAsmInstruction *insn) const {
    State state(p, settings(), *this);
    initializeState(state);
    emitInstruction(out, insn, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, const P2::BasicBlock::Ptr &bb) const {
    State state(p, settings(), *this);
    initializeState(state);
    emitBasicBlock(out, bb, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, const P2::DataBlock::Ptr &db) const {
    State state(p, settings(), *this);
    initializeState(state);
    emitDataBlock(out, db, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner &p, const P2::Function::Ptr &f) const {
    State state(p, settings(), *this);
    initializeState(state);
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

        // Update intra-function CFG arrow object for this function.
        if (settings().bblock.cfg.showingArrows) {
            state.intraFunctionCfgArrows().reset();
            if (state.cfgArrowsPointToInsns()) {
                state.intraFunctionCfgArrows().arrows.computeCfgBlockLayout(state.partitioner(), function);
            } else {
                state.intraFunctionCfgArrows().arrows.computeCfgEdgeLayout(state.partitioner(), function);
            }
        }

        // Update user-defined intra-function arrows for this function.
        state.intraFunctionBlockArrows().reset();
        state.frontUnparser().updateIntraFunctionArrows(state);

        state.frontUnparser().emitFunctionBody(out, function, state);

        state.intraFunctionCfgArrows().reset();
        state.intraFunctionBlockArrows().reset();

        state.frontUnparser().emitFunctionEpilogue(out, function, state);
    }
}

void
Base::emitFunctionPrologue(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionPrologue(out, function, state);
    } else {
        state.frontUnparser().emitLinePrefix(out, state);
        out <<std::string(120, ';') <<"\n";

        if (settings().function.showingDemangled) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; " <<function->printableName() <<"\n";
            if (function->demangledName() != function->name()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<";;; mangled name is \"" <<StringUtility::cEscape(function->name()) <<"\"\n";
            }
        } else {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; function " <<StringUtility::addrToString(function->address());
            if (!function->name().empty()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<" \"" <<StringUtility::cEscape(function->name()) <<"\"";
            }
            out <<"\n";
        }
        state.frontUnparser().emitFunctionComment(out, function, state);
        if (settings().function.showingSourceLocation)
            state.frontUnparser().emitFunctionSourceLocation(out, function, state);
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

typedef boost::variant<P2::BasicBlock::Ptr, P2::DataBlock::Ptr> InsnsOrData;

struct AddressVisitor: public boost::static_visitor<rose_addr_t> {
    template<class T>
    rose_addr_t operator()(const T& a) const {
        return a->address();
    }
};

static bool
increasingAddress(const InsnsOrData &a, const InsnsOrData &b) {
    rose_addr_t aVa = boost::apply_visitor(AddressVisitor(), a);
    rose_addr_t bVa = boost::apply_visitor(AddressVisitor(), b);
    return aVa < bVa;
}

struct EmitBlockVisitor: public boost::static_visitor<> {
    std::ostream &out;
    P2::Function::Ptr function;
    rose_addr_t &nextBlockVa;
    State &state;

    EmitBlockVisitor(std::ostream &out, const P2::Function::Ptr &function, rose_addr_t &nextBlockVa, State &state)
        : out(out), function(function), nextBlockVa(nextBlockVa), state(state) {}

    void operator()(const P2::BasicBlock::Ptr &bb) const {
        state.frontUnparser().emitLinePrefix(out, state);
        out <<"\n";
        if (bb->address() != *function->basicBlockAddresses().begin()) {
            if (bb->address() > nextBlockVa) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<";;; skip forward " <<StringUtility::plural(bb->address() - nextBlockVa, "bytes") <<"\n";
            } else if (bb->address() < nextBlockVa) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<";;; skip backward " <<StringUtility::plural(nextBlockVa - bb->address(), "bytes") <<"\n";
            }
        }
        state.frontUnparser().emitBasicBlock(out, bb, state);
        if (bb->nInstructions() > 0) {
            nextBlockVa = bb->fallthroughVa();
        } else {
            nextBlockVa = bb->address();
        }
    }

    void operator()(const P2::DataBlock::Ptr &db) const {
        state.frontUnparser().emitLinePrefix(out, state);
        out <<"\n";
        if (db->address() != *function->basicBlockAddresses().begin()) {
            if (db->address() > nextBlockVa) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<";;; skip forward " <<StringUtility::plural(db->address() - nextBlockVa, "bytes") <<"\n";
            } else if (db->address() < nextBlockVa) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<";;; skip backward " <<StringUtility::plural(nextBlockVa - db->address(), "bytes") <<"\n";
            }
        }
        state.frontUnparser().emitDataBlock(out, db, state);
        nextBlockVa = db->address() + db->size();
    }
};


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

        // Get all the basic blocks and data blocks and sort them by starting address. Be careful because data blocks might
        // appear more than once since they can be owned by both the function and by multiple basic blocks.
        std::set<P2::DataBlock::Ptr> dblocks;
        std::vector<InsnsOrData> blocks;
        blocks.reserve(function->nBasicBlocks() + function->nDataBlocks());
        BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = state.partitioner().basicBlockExists(bbVa)) {
                blocks.push_back(bb);
                BOOST_FOREACH (const P2::DataBlock::Ptr db, bb->dataBlocks())
                    dblocks.insert(db);
            }
        }
        BOOST_FOREACH (const P2::DataBlock::Ptr &db, function->dataBlocks())
            dblocks.insert(db);
        blocks.insert(blocks.end(), dblocks.begin(), dblocks.end());
        std::sort(blocks.begin(), blocks.end(), increasingAddress);

        // Emit each basic- or data-block
        rose_addr_t nextBlockVa = function->address();
        BOOST_FOREACH (const InsnsOrData &block, blocks)
            boost::apply_visitor(EmitBlockVisitor(out, function, nextBlockVa, state), block);
    }
}

void
Base::emitFunctionSourceLocation(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionSourceLocation(out, function, state);
    } else {
        if (!function->sourceLocation().isEmpty()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; defined at " <<function->sourceLocation() <<"\n";
        }
    }
}

void
Base::emitFunctionEpilogue(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionEpilogue(out, function, state);
    } else {
        state.frontUnparser().emitLinePrefix(out, state);
        out <<"\n";
        state.frontUnparser().emitLinePrefix(out, state);
        out <<"\n";
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
        addFunctionReason(strings, flags, SgAsmFunction::FUNC_THUNK_TARGET, "thunk target");
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

        if (flags & 0xff) {
            switch (flags & 0xff) {
                case SgAsmFunction::FUNC_INTERPADFUNC:
                    strings.push_back("interpadfunc");
                    break;
                case SgAsmFunction::FUNC_PESCRAMBLER_DISPATCH:
                    strings.push_back("pescrambler dispatch");
                    break;
                case SgAsmFunction::FUNC_CONFIGURED:
                    strings.push_back("configuration");
                    break;
                case SgAsmFunction::FUNC_CMDLINE:
                    strings.push_back("command-line");
                    break;
                case SgAsmFunction::FUNC_SCAN_RO_DATA:
                    strings.push_back("scanned read-only ptr");
                    break;
                case SgAsmFunction::FUNC_INSN_RO_DATA:
                    strings.push_back("referenced read-only ptr");
                    break;
                default:
                    strings.push_back("miscellaneous(" + StringUtility::numberToString(flags & 0xff) + ")");
                    break;
            }
            flags &= ~0xff;
        }

        if (flags != 0) {
            char buf[64];
            sprintf(buf, "0x%08x", flags);
            strings.push_back("other (" + std::string(buf));
        }
        if (!strings.empty()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; reasons for function: " <<boost::join(strings, ", ") <<"\n";
        }
        if (!function->reasonComment().empty()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; reason comment: " <<function->reasonComment() <<"\n";
        }
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
                state.frontUnparser().emitLinePrefix(out, state);
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
                state.frontUnparser().emitLinePrefix(out, state);
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
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;;\n";
            state.frontUnparser().emitCommentBlock(out, s, state, ";;; ");
            state.frontUnparser().emitLinePrefix(out, state);
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
            if (delta != SgAsmInstruction::INVALID_STACK_DELTA) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<";;; function stack delta is " <<StringUtility::toHex2(delta, 64) <<"\n";
            }
        } else if (S2::BaseSemantics::SValuePtr delta = function->stackDelta()) {
            state.frontUnparser().emitLinePrefix(out, state);
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
                state.frontUnparser().emitLinePrefix(out, state);
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
        if (function->isNoop().getOptional().orElse(false)) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; this function is a no-op\n";
        }
    }
}

void
Base::emitFunctionMayReturn(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionMayReturn(out, function, state);
    } else {
        if (!state.partitioner().functionOptionalMayReturn(function).orElse(true)) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; this function does not return to its caller\n";
        }
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
        if (settings().bblock.showingSourceLocation)
            state.frontUnparser().emitBasicBlockSourceLocation(out, bb, state);
        if (settings().bblock.cfg.showingSharing)
            state.frontUnparser().emitBasicBlockSharing(out, bb, state);
        if (settings().bblock.cfg.showingPredecessors)
            state.frontUnparser().emitBasicBlockPredecessors(out, bb, state);
        if (settings().bblock.reach.showingReachability)
            state.frontUnparser().emitBasicBlockReachability(out, bb, state);

        // Comment warning about block not being the function entry point.
        if (state.currentFunction() && bb->address() == *state.currentFunction()->basicBlockAddresses().begin() &&
            bb->address() != state.currentFunction()->address()) {
            state.frontUnparser().emitLinePrefix(out, state);
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
            state.thisIsBasicBlockFirstInstruction();
            state.thisIsBasicBlockLastInstruction();
            state.frontUnparser().emitLinePrefix(out, state);
            out <<StringUtility::addrToString(bb->address()) <<": no instructions\n";
        } else {
            state.thisIsBasicBlockFirstInstruction();
            state.nextInsnLabel(state.basicBlockLabels().getOrElse(bb->address(), ""));
            BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
                if (insn == bb->instructions().back())
                    state.thisIsBasicBlockLastInstruction();
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
        BOOST_FOREACH (P2::DataBlock::Ptr db, bb->dataBlocks()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<"\t;; related " <<db->printableName() <<", " <<StringUtility::plural(db->size(), "bytes") <<"\n";
        }
        if (settings().bblock.cfg.showingSuccessors)
            state.frontUnparser().emitBasicBlockSuccessors(out, bb, state);
    }
}

void
Base::emitBasicBlockSourceLocation(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockSourceLocation(out, bb, state);
    } else {
        if (!bb->sourceLocation().isEmpty()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<"\t;; defined at " <<bb->sourceLocation() <<"\n";
        }
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
        BOOST_FOREACH (P2::Function::Ptr function, functions) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<"\t;; block also owned by " <<function->printableName() <<"\n";
        }
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
        std::vector<P2::ControlFlowGraph::ConstEdgeIterator> edges = orderedBlockPredecessors(state.partitioner(), bb);
        BOOST_FOREACH (P2::ControlFlowGraph::ConstEdgeIterator edge, edges) {

            if (!state.cfgArrowsPointToInsns()) {
                // The line were about to emit is the sharp end of the arrow--the arrow's target, and we're emitting arrows
                // that point to/from the "predecessors:" and "successors:" lines. Arrows that point instead to the
                // instructions of a basic block are handled elsewhere.
                state.currentPredSuccId(EdgeArrows::cfgEdgeTargetEndpoint(edge->id()));
                state.intraFunctionCfgArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_START); // point end of arrow
            }

            // Emit the "predecessor:" line
            std::string s = edgeTypeName(edge->value().type()) + " edge";
            P2::ControlFlowGraph::ConstVertexIterator pred = edge->source();
            switch (pred->value().type()) {
                case P2::V_BASIC_BLOCK: {
                    std::ostringstream fromAddrSs;
                    state.frontUnparser().emitAddress(fromAddrSs, *pred->value().optionalLastAddress(), state);
                    if (pred->value().bblock()->nInstructions() > 1) {
                        s = " from instruction " + fromAddrSs.str() + " from " + pred->value().bblock()->printableName();
                    } else {
                        s = " from " + fromAddrSs.str();
                    }
                    break;
                }

                case P2::V_USER_DEFINED: {
                    std::ostringstream fromAddrSs;
                    state.frontUnparser().emitAddress(fromAddrSs, *pred->value().optionalLastAddress(), state);
                    s = " from " + fromAddrSs.str();
                    break;
                }
                case P2::V_NONEXISTING:
                case P2::V_UNDISCOVERED:
                case P2::V_INDETERMINATE:
                    ASSERT_not_reachable("vertex type should have no successors");
                default:
                    ASSERT_not_implemented("cfg vertex type");
            }

            state.frontUnparser().emitLinePrefix(out, state);
            out <<"\t;; predecessor: " <<s <<"\n";
        }
    }
}

void
Base::emitBasicBlockSuccessors(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockSuccessors(out, bb, state);
    } else {
        // Get the CFG edges in the order they should be displayed even though we actually emit the basicBlockSuccessors
        std::vector<P2::ControlFlowGraph::ConstEdgeIterator> edges = orderedBlockSuccessors(state.partitioner(), bb);
        P2::BasicBlock::Successors successors = state.partitioner().basicBlockSuccessors(bb);
        BOOST_FOREACH (P2::ControlFlowGraph::ConstEdgeIterator edge, edges) {
            Sawyer::Optional<rose_addr_t> targetVa = edge->target()->value().optionalAddress();

            if (!state.cfgArrowsPointToInsns()) {
                // The line we're about to emit is the nock end of the arrow--the arrow's origin, and we're emitting
                // arrows that point to/from the "predecessors:" and "successors:" lines. Arrows that point instead to
                // the instructions of a basic block are handled elsewhere.
                state.currentPredSuccId(EdgeArrows::cfgEdgeSourceEndpoint(edge->id()));
                state.intraFunctionCfgArrows().flags.set(ArrowMargin::POINTABLE_ENTITY_END);// nock end of arrow
            }

            // Find a matching successor that we haven't emitted yet
            bool emitted = false;
            for (size_t i=0; i<successors.size(); ++i) {
                ASSERT_not_null(successors[i].expr());
                SymbolicExpr::Ptr expr = successors[i].expr()->get_expression();

                if (targetVa && expr->toUnsigned().isEqual(targetVa)) {
                    // Edge to concrete node
                    state.frontUnparser().emitLinePrefix(out, state);
                    out <<"\t;; successor: " <<edgeTypeName(successors[i].type()) <<" edge to ";
                    state.frontUnparser().emitAddress(out, *targetVa, state);
                    out <<"\n";
                    successors.erase(successors.begin()+i);
                    emitted = true;
                    break;

                } else if (!targetVa && !expr->isIntegerConstant()) {
                    // Edge to computed address
                    state.frontUnparser().emitLinePrefix(out, state);
                    out <<"\t;; successor: " <<edgeTypeName(successors[i].type()) <<" edge to " <<*expr <<"\n";
                    successors.erase(successors.begin()+i);
                    emitted = true;
                    break;
                }
            }

            // When this CFG edge has no matching successor. Not sure whether this can happen. [Robb Matzke 2018-12-05]
            if (!emitted) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t;; successor: (only in CFG) " <<edgeTypeName(edge->value().type()) <<" edge to ";
                if (targetVa) {
                    state.frontUnparser().emitAddress(out, *targetVa, state);
                    out <<"\n";
                } else {
                    out <<"unknown\n";
                }
            }
        }

        // Emit the basic block successors that don't correspond to any CFG edge. Can this happen? [Robb Matzke 2018-12-05]
        BOOST_FOREACH (const P2::BasicBlock::Successor &successor, successors) {
            ASSERT_not_null(successor.expr());
            SymbolicExpr::Ptr expr = successor.expr()->get_expression();
            if (expr->isIntegerConstant() && expr->nBits() <= 64) {
                // Edge to concrete node
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t;; successor: (not in CFG) " <<edgeTypeName(successor.type()) <<" edge to ";
                state.frontUnparser().emitAddress(out, expr->toUnsigned().get(), state);
                out <<"\n";
            } else if (expr->isLeafNode()) {
                // What?
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t;; successor: (not in CFG) " <<edgeTypeName(successor.type()) <<" edge to unknown\n";
            } else {
                // Edge to computed address
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t;; successor: (not in CFG) " <<edgeTypeName(successor.type()) <<" edge to " <<*expr <<"\n";
            }
        }

        // Ghost successors due to opaque predicates
        if (bb->ghostSuccessors().isCached()) {
            BOOST_FOREACH (rose_addr_t va, bb->ghostSuccessors().get()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t;; successor: (ghost) ";
                state.frontUnparser().emitAddress(out, va, state);
                out <<"\n";
            }
        }
    }
}

void
Base::emitBasicBlockReachability(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockReachability(out, bb, state);
    } else if (!state.cfgVertexReachability().empty()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = state.partitioner().findPlaceholder(bb->address());
        if (vertex != state.partitioner().cfg().vertices().end()) {
            Reachability::ReasonFlags reachable = state.isCfgVertexReachable(vertex->id());
            if (reachable.isAnySet()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t;; reachable from: " <<state.reachabilityName(reachable) <<"\n";
            } else {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t;; not reachable\n";
            }
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
        if (settings().dblock.showingSourceLocation)
            state.frontUnparser().emitDataBlockSourceLocation(out, db, state);
        if (!db->comment().empty())
            state.frontUnparser().emitCommentBlock(out, db->comment(), state, "\t;; ");

        state.frontUnparser().emitLinePrefix(out, state);
        out <<"\t;; " <<db->printableName() <<", " <<StringUtility::plural(db->size(), "bytes") <<"\n";
        if (P2::Function::Ptr function = state.currentFunction()) {
            BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
                if (P2::BasicBlock::Ptr bb = state.partitioner().basicBlockExists(bbVa)) {
                    if (bb->dataBlockExists(db)) {
                        state.frontUnparser().emitLinePrefix(out, state);
                        out <<"\t;; referenced by " <<bb->printableName() <<"\n";
                    }
                }
            }
        }

        state.frontUnparser().emitLinePrefix(out, state);
        out <<"\t;; block type is " <<db->type()->toString() <<"\n";
    }
}

void
Base::emitDataBlockBody(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlockBody(out, db, state);
    } else if (AddressInterval where = db->extent()) {
        if (MemoryMap::Ptr map = state.partitioner().memoryMap()) {
            // hexdump format
            std::ostringstream prefix;
            state.frontUnparser().emitLinePrefix(prefix, state);
            HexdumpFormat fmt;
            fmt.prefix = prefix.str();
            fmt.multiline = true;

            // Read the data in chunks and produce a hexdump
            while (where) {
                uint8_t buf[8192];                      // multiple of 16
                size_t maxSize = std::min(where.size(), (rose_addr_t)(sizeof buf));
                AddressInterval read = state.partitioner().memoryMap()->atOrAfter(where.least()).limit(maxSize).read(buf);
                SgAsmExecutableFileFormat::hexdump(out, read.least(), buf, read.size(), fmt);
                if (read.greatest() == where.greatest())
                    break;                              // avoid possible overflow
                where = AddressInterval::hull(read.greatest()+1, where.greatest());
            }
        } else {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<";;; no memory map from which to obtain the static block data\n";
        }
    }
}

void
Base::emitDataBlockEpilogue(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlockEpilogue(out, db, state);
    } else {
    }
}

void
Base::emitDataBlockSourceLocation(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlockSourceLocation(out, db, state);
    } else {
        if (!db->sourceLocation().isEmpty()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<"\t;; defined at " <<db->sourceLocation() <<"\n";
        }
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

        std::string full = juxtaposeColumns(parts, fieldWidths);
        std::vector<std::string> lines = StringUtility::split('\n', full);
        BOOST_FOREACH (const std::string &line, lines) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<line;
        }
    }
}

void
Base::emitInstructionEpilogue(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitInstructionEpilogue(out, insn, state);
    } else {
        state.frontUnparser().emitInstructionSemantics(out, insn, state);
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
        if (insn->semanticFailure() > 0)
            out <<settings().insn.mnemonic.semanticFailureMarker;
    }
}

void
Base::emitInstructionOperands(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionOperands(out, insn, state);
    } else if (insn->get_operandList()) {
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

void
Base::emitInstructionSemantics(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (settings().insn.semantics.showing) {
        S2::BaseSemantics::RiscOperatorsPtr ops = state.partitioner().newOperators();
        if (settings().insn.semantics.tracing)
            ops = S2::TraceSemantics::RiscOperators::instance(ops);

        if (S2::BaseSemantics::DispatcherPtr cpu = state.partitioner().newDispatcher(ops)) {
            try {
                cpu->processInstruction(insn);
                S2::BaseSemantics::Formatter fmt = settings().insn.semantics.formatter;
                std::ostringstream ss;
                ss <<"\n" <<(*cpu->currentState()->registerState() + fmt) <<(*cpu->currentState()->memoryState() + fmt);
                out <<StringUtility::trim(ss.str(), "\n", false, true);
            } catch (...) {
            }
        }
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

        if (bv.size() == state.partitioner().instructionProvider().instructionPointerRegister().nBits() &&
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
        if (settings().insn.operands.showingWidth)
            out <<"[" <<bv.size() <<"]";
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
        if (settings().insn.operands.showingWidth)
            out <<"[" <<reg.nBits() <<"]";
    }
}

void
Base::emitCommentBlock(std::ostream &out, const std::string &comment, State &state, const std::string &prefix) const {
    if (nextUnparser()) {
        nextUnparser()->emitCommentBlock(out, comment, state, prefix);
    } else {
        std::vector<std::string> lines = StringUtility::split('\n', comment);
        BOOST_FOREACH (const std::string &line, lines) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<prefix <<line <<"\n";
        }
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

        } else if (SgAsmVectorType *vt = isSgAsmVectorType(type)) {
            out <<"vector(" <<vt->get_nElmts() <<" * ";
            if (SgAsmType *subtype = vt->get_elmtType()) {
                emitTypeName(out, subtype, state);
            } else {
                out <<"unknown";
            }
            out <<")";

        } else {
            ASSERT_not_implemented(type->toString());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Line control
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::emitLinePrefix(std::ostream &out, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitLinePrefix(out, state);
    } else {
        // Generate intra-function arrows that point to basic blocks. I.e., basic blocks are the pointable entities,
        // and we're only drawing arrows that both originate and terminate within the current function.
        Sawyer::Optional<EdgeArrows::VertexId> arrowVertexId;
        if (state.currentBasicBlock())
            arrowVertexId = state.currentBasicBlock()->address();
        out <<state.globalBlockArrows().render(arrowVertexId);
        out <<state.intraFunctionBlockArrows().render(arrowVertexId);

        // CFG arrows point to either the basic bocks or to the "predecessor:" and "successor:" lines.
        if (settings().bblock.cfg.showingArrows) {
            if (state.cfgArrowsPointToInsns()) {
                out <<state.intraFunctionCfgArrows().render(arrowVertexId);
            } else {
                out <<state.intraFunctionCfgArrows().render(state.currentPredSuccId());
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Non-emitting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::initializeState(State &state) const {
    if (nextUnparser())
        nextUnparser()->initializeState(state);
}

void
Base::updateIntraFunctionArrows(State &state) const {
    if (nextUnparser())
        nextUnparser()->updateIntraFunctionArrows(state);
}


// class method
bool
Base::ascendingSourceAddress(P2::ControlFlowGraph::ConstEdgeIterator a, P2::ControlFlowGraph::ConstEdgeIterator b) {
    Sawyer::Optional<rose_addr_t> aVa = a->source()->value().optionalLastAddress();
    Sawyer::Optional<rose_addr_t> bVa = b->source()->value().optionalLastAddress();

    if (aVa && bVa) {
        return *aVa < *bVa;
    } else if (aVa) {
        return true;                                    // addresses before non-addresses
    } else {
        return false;
    }
}

// class method
bool
Base::ascendingTargetAddress(P2::ControlFlowGraph::ConstEdgeIterator a, P2::ControlFlowGraph::ConstEdgeIterator b) {
    Sawyer::Optional<rose_addr_t> aVa = a->source()->value().optionalAddress();
    Sawyer::Optional<rose_addr_t> bVa = b->source()->value().optionalAddress();

    if (aVa && bVa) {
        return *aVa < *bVa;
    } else if (aVa) {
        return true;                                    // addresses before non-addresses
    } else {
        return false;
    }
}

// class method
std::vector<P2::ControlFlowGraph::ConstEdgeIterator>
Base::orderedBlockPredecessors(const P2::Partitioner &partitioner, const P2::BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(bb->address());
    std::vector<P2::ControlFlowGraph::ConstEdgeIterator> retval;
    retval.reserve(vertex->nInEdges());
    for (P2::ControlFlowGraph::ConstEdgeIterator edge = vertex->inEdges().begin(); edge != vertex->inEdges().end(); ++edge)
        retval.push_back(edge);
    std::sort(retval.begin(), retval.end(), ascendingSourceAddress);
    return retval;
}

// class method
std::vector<P2::ControlFlowGraph::ConstEdgeIterator>
Base::orderedBlockSuccessors(const P2::Partitioner &partitioner, const P2::BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner.findPlaceholder(bb->address());
    std::vector<P2::ControlFlowGraph::ConstEdgeIterator> retval;
    retval.reserve(vertex->nOutEdges());
    for (P2::ControlFlowGraph::ConstEdgeIterator edge = vertex->outEdges().begin(); edge != vertex->outEdges().end(); ++edge)
        retval.push_back(edge);
    std::sort(retval.begin(), retval.end(), ascendingTargetAddress);
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
