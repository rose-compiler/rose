#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Unparser/Base.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Reachability.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/SourceLocation.h>
#include <integerOps.h>                                 // rose
#include <stringify.h>                                  // rose

#include <SgAsmAarch32Coprocessor.h>
#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryAsr.h>
#include <SgAsmBinaryConcat.h>
#include <SgAsmBinaryLsl.h>
#include <SgAsmBinaryLsr.h>
#include <SgAsmBinaryMsl.h>
#include <SgAsmBinaryPostupdate.h>
#include <SgAsmBinaryPreupdate.h>
#include <SgAsmBinaryRor.h>
#include <SgAsmBinarySubtract.h>
#include <SgAsmByteOrder.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmFloatType.h>
#include <SgAsmFloatValueExpression.h>
#include <SgAsmFunction.h>
#include <SgAsmInstruction.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmRegisterNames.h>
#include <SgAsmUnarySignedExtend.h>
#include <SgAsmUnaryTruncate.h>
#include <SgAsmUnaryUnsignedExtend.h>
#include <SgAsmVectorType.h>

#include <Cxx_GrammarDowncast.h>

#include <Sawyer/ProgressBar.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/variant.hpp>

#include <ctype.h>
#include <sstream>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics;

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

std::string
invalidRegister(SgAsmInstruction *insn, RegisterDescriptor reg, const RegisterDictionary::Ptr &regdict) {
    using namespace StringUtility;
    Stream warn(mlog[WARN]);

    std::string regstr = numberToString(reg.majorNumber()) + "." + numberToString(reg.minorNumber()) + "." +
                         numberToString(reg.offset()) + "." + numberToString(reg.nBits());
    if (insn) {
        warn <<"invalid register reference " <<regstr <<" at va " <<addrToString(insn->get_address()) <<"\n";
    } else {
        warn <<"invalid register reference " <<regstr <<"\n";
    }

    static bool wasDescribed = false;
    if (!wasDescribed) {
        warn <<"  This warning is caused by instructions using registers that don't have names in the\n"
             <<"  register dictionary.  The register dictionary used during unparsing comes from either\n"
             <<"  the explicitly specified dictionary (see AsmUnparser::set_registers()) or the dictionary\n"
             <<"  associated with the SgAsmInterpretation being unparsed.  The interpretation normally\n"
             <<"  chooses a dictionary based on the architecture specified in the file header. For example,\n"
             <<"  this warning may be caused by a file whose header specifies i386 but the instructions in\n"
             <<"  the file are for the amd64 architecture.  The assembly listing will indicate unnamed\n"
             <<"  registers with the notation \"BAD_REGISTER(a.b.c.d)\" where \"a\" and \"b\" are the major\n"
             <<"  and minor numbers for the register, \"c\" is the bit offset within the underlying machine\n"
             <<"  register, and \"d\" is the number of significant bits.\n";
        if (regdict!=nullptr)
            warn <<"  Dictionary in use at time of warning: " <<regdict->name() <<"\n";
        wasDescribed = true;
    }
    return "BAD_REGISTER(" + regstr + ")";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Styles
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
Style::ansiStyle() const {
    std::string retval;
    if (foreground || background) {
        if (foreground)
            retval += foreground->toAnsi(Color::Layer::FOREGROUND);
        if (background)
            retval += background->toAnsi(Color::Layer::BACKGROUND);
    } else {
        retval = "\033[0m";
    }
    return retval;
}

size_t
StyleStack::push(const Style &style) {
    stack_.push_back(style);
    merge(style);
    return stack_.size() - 1;
}

void
StyleStack::pop() {
    ASSERT_require(!stack_.empty());
    stack_.pop_back();
    mergeAll();
}

void
StyleStack::popTo(size_t n) {
    if (n < stack_.size()) {
        stack_.resize(n);
        mergeAll();
    }
}

void
StyleStack::reset() {
    stack_.clear();
    current_ = Style();
}

size_t
StyleStack::size() const {
    return stack_.size();
}

const Style&
StyleStack::current() const {
    return current_;
}

void
StyleStack::merge(const Style &style) {
    if (style.foreground)
        current_.foreground = Color::terminal(*style.foreground, Rose::CommandLine::genericSwitchArgs.colorization);
    if (style.background)
        current_.background = Color::terminal(*style.background, Rose::CommandLine::genericSwitchArgs.colorization);
}

void
StyleStack::mergeAll() {
    current_ = Style();
    for (const Style &style: boost::adaptors::reverse(stack_)) {
        if (current_.foreground && current_.background)
            break;
        if (!current_.foreground && style.foreground)
            current_.foreground = Color::terminal(*style.foreground, Rose::CommandLine::genericSwitchArgs.colorization);
        if (!current_.background && style.background)
            current_.background = Color::terminal(*style.background, Rose::CommandLine::genericSwitchArgs.colorization);
    }
}

std::string
StyleGuard::render() const {
    return stack_.colorization().isEnabled() ? current_.ansiStyle() : "";
}

std::string
StyleGuard::restore() const {
    return stack_.colorization().isEnabled() ? previous_.ansiStyle() : "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

State::State(const P2::Partitioner::ConstPtr &partitioner, const Architecture::Base::ConstPtr &arch, const Settings &settings,
             const Base &frontUnparser)
    : partitioner_(partitioner), registerNames_(notnull(arch)->registerDictionary()), frontUnparser_(frontUnparser) {
    if (settings.function.cg.showing && partitioner)
        cg_ = partitioner->functionCallGraph(P2::AllowParallelEdges::NO);
    intraFunctionCfgArrows_.arrows.arrowStyle(settings.arrow.style, EdgeArrows::LEFT);
    intraFunctionBlockArrows_.arrows.arrowStyle(settings.arrow.style, EdgeArrows::LEFT);
    globalBlockArrows_.arrows.arrowStyle(settings.arrow.style, EdgeArrows::LEFT);
    globalBlockArrows_.flags.set(ArrowMargin::ALWAYS_RENDER);
    cfgArrowsPointToInsns_ = !settings.bblock.cfg.showingPredecessors || !settings.bblock.cfg.showingSuccessors;
    styleStack_.colorization(settings.colorization.merge(CommandLine::genericSwitchArgs.colorization));
}

State::~State() {}

P2::Partitioner::ConstPtr
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
                for (char &ch: s)
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

bool
State::isPostInstruction() const {
    return isPostInstruction_;
}

void
State::isPostInstruction(bool b) {
    isPostInstruction_ = b;
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

Base::Base(const Architecture::Base::ConstPtr &architecture)
    : architecture_(architecture) {
    ASSERT_not_null(architecture);
}

Base::Base(const Ptr &nextUnparser)
    : architecture_(nextUnparser->architecture()), nextUnparser_(nextUnparser) {}

Base::~Base() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Unparser properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Architecture::Base::ConstPtr
Base::architecture() const {
    ASSERT_not_null(architecture_);
    return architecture_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// class method
std::string
Base::leftJustify(const std::string &s, size_t width) {

    // Assume that ANSI escapes occupy no space and do not move the cursor.
    std::string noEscapes = StringUtility::removeAnsiEscapes(s);

    if (noEscapes.size() > width)
        return s;
    return s + std::string(width-noEscapes.size(), ' ');
}

// class method
std::string
Base::juxtaposeColumns(const std::vector<std::string> &parts, const std::vector<size_t> &minWidths,
                       const std::vector<std::pair<std::string, std::string> > &colorEscapes,
                       const std::string &columnSeparator) {
    ASSERT_require(minWidths.size() == parts.size());
    ASSERT_require(colorEscapes.size() == parts.size());

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
            ss <<colorEscapes[j].first <<leftJustify(s, minWidths[j]) <<colorEscapes[j].second;
        }
        retval += boost::trim_right_copy(ss.str());
    }
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Settings::Settings() {
    // All colors listed here are assumed to be dark foreground colors on a light background.
    comment.line.style.foreground = Color::HSV(0.0, 0.0, 0.6);

    function.separatorStyle.foreground = Color::HSV(0.33, 1.0, 0.3);
    function.titleStyle.foreground = Color::HSV(0.33, 1.0, 0.3);
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
    bblock.cfg.showingArrows = true;
    bblock.reach.showingReachability = true;
    bblock.cfg.arrowStyle.foreground = Color::HSV(0.58, 0.90, 0.3); // blue
    bblock.showingPostBlock = true;

    dblock.showingSourceLocation = true;

    insn.address.showing = true;
    insn.address.useLabels = false;                     // show addresses instead of labels
    insn.address.fieldWidth = 11;                       // "0x" + 8 hex digits + ":"
    insn.address.style.foreground = Color::HSV(0.05, 0.9, 0.3); // orange
    insn.bytes.showing = true;
    insn.bytes.perLine = 8;
    insn.bytes.fieldWidth = 25;
    insn.bytes.style.foreground = Color::HSV(0.58, 0.41, 0.4); // faded blue
    insn.stackDelta.showing = true;
    insn.stackDelta.fieldWidth = 2;
    insn.stackDelta.style.foreground = Color::HSV(0.9, 0.7, 0.4); // magenta
    insn.frameDelta.showing = true;
    insn.frameDelta.fieldWidth = 2;
    insn.frameDelta.style.foreground = Color::HSV(0.8, 0.8, 0.4);
    insn.mnemonic.fieldWidth = 8;
    insn.mnemonic.semanticFailureMarker = "[!]";
    insn.mnemonic.semanticFailureStyle.foreground = Color::HSV(0, 1, 0.4); // red
    insn.mnemonic.style.foreground = Color::HSV(0.17, 1.00, 0.4); // yellow
    insn.operands.separator = ", ";
    insn.operands.fieldWidth = 40;
    insn.operands.showingWidth = false;
    insn.operands.style.foreground = Color::HSV(0.33, 0.31, 0.3); // lightish green
    insn.comment.showing = true;
    insn.comment.usingDescription = true;
    insn.comment.pre = "; ";
    insn.comment.fieldWidth = 1;
    insn.semantics.showing = false;                     // not usually desired, and somewhat slow
    insn.semantics.tracing = false;                     // not usually desired even for full output
    insn.semantics.formatter.set_show_latest_writers(false);
    insn.semantics.formatter.set_line_prefix("        ;; state: ");
    insn.semantics.style.foreground = Color::HSV(0.17, 0.30, 0.3);

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
    s.bblock.showingPostBlock = false;

    s.dblock.showingSourceLocation = false;

    s.insn.address.showing = true;
    s.insn.address.useLabels = true;                    // generate and use labels instead of addresses
    s.insn.address.fieldWidth = 8;
    s.insn.bytes.showing = false;
    s.insn.stackDelta.showing = false;
    s.insn.frameDelta.showing = false;
    s.insn.mnemonic.fieldWidth = 8;
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

    insertBooleanSwitch(sg, "bb-postblock", settings.bblock.showingPostBlock,
                        "Show information following the block's final instruction, such as stack deltas and other things that "
                        "would normally appear with the next instruction if there was one.");

    //----- Data blocks -----

    insertBooleanSwitch(sg, "db-source-location", settings.dblock.showingSourceLocation,
                        "Show the file name, line number, and column number of the data block's definition in source code "
                        "if that information is available.");

    //----- Instructions -----

    // The --insn-address and --no-insn-address only have effects when settings.insn.address.showing is true.
    sg.insert(Switch("insn-address")
              .intrinsicValue(false, settings.insn.address.useLabels)
              .doc("Show the address of each instruction. The @s{no-insn-address} switch suppresses the address and generates "
                   "labels when necessary. The defaut is to use " +
                   std::string(settings.insn.address.useLabels ? "labels" : "addresses") + "."));
    sg.insert(Switch("no-insn-address")
              .key("insn-address")
              .intrinsicValue(true, settings.insn.address.useLabels)
              .hidden(true));

    sg.insert(Switch("insn-address-width")
              .argument("nchars", positiveIntegerParser(settings.insn.address.fieldWidth))
              .doc("Minimum size of the address or label field in characters if addresses are enabled with @s{insn-address}. "
                   "The default is " + boost::lexical_cast<std::string>(settings.insn.address.fieldWidth) + "."));

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

    insertBooleanSwitch(sg, "insn-frame-delta", settings.insn.frameDelta.showing,
                        "For each instruction, show the frame delta. This is the difference between the function call frame "
                        "pointer and the stack pointer before the instruction executes.");

    sg.insert(Switch("insn-frame-delta-width")
              .argument("nchars", positiveIntegerParser(settings.insn.frameDelta.fieldWidth))
              .doc("Minimum size of the frame delta field in characters if frame deltas are enabled with @s {insn-frame-delta}. "
                   "The default is " + boost::lexical_cast<std::string>(settings.insn.frameDelta.fieldWidth) + "."));

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
void
Base::operator()(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner) const {
    unparse(out, partitioner);
}

void
Base::operator()(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) const {
    unparse(out, partitioner, insn);
}

void
Base::operator()(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::BasicBlock::Ptr &bb) const {
    unparse(out, partitioner, bb);
}

void
Base::operator()(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::DataBlock::Ptr &db) const {
    unparse(out, partitioner, db);
}

void
Base::operator()(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &f) const {
    unparse(out, partitioner, f);
}

void
Base::operator()(std::ostream &out, SgAsmInstruction *insn) const {
    unparse(out, insn);
}

std::string
Base::operator()(const P2::Partitioner::ConstPtr &partitioner, const Progress::Ptr &progress) const {
    return unparse(partitioner, progress);
}

std::string
Base::operator()(const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) const {
    return unparse(partitioner, insn);
}

std::string
Base::operator()(const P2::Partitioner::ConstPtr &partitioner, const P2::BasicBlock::Ptr &bb) const {
    return unparse(partitioner, bb);
}

std::string
Base::operator()(const P2::Partitioner::ConstPtr &partitioner, const P2::DataBlock::Ptr &db) const {
        return unparse(partitioner, db);
}

std::string
Base::operator()(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &f) const {
    return unparse(partitioner, f);
}

std::string
Base::operator()(SgAsmInstruction *insn) const {
    return unparse(insn);
}

std::string
Base::unparse(const P2::Partitioner::ConstPtr &partitioner, const Progress::Ptr &progress) const {
    std::ostringstream ss;
    unparse(ss, partitioner, progress);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) const {
    std::ostringstream ss;
    unparse(ss, partitioner, insn);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner::ConstPtr &partitioner, const Partitioner2::BasicBlock::Ptr &bb) const {
    std::ostringstream ss;
    unparse(ss, partitioner, bb);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner::ConstPtr &partitioner, const Partitioner2::DataBlock::Ptr &db) const {
    std::ostringstream ss;
    unparse(ss, partitioner, db);
    return ss.str();
}

std::string
Base::unparse(const P2::Partitioner::ConstPtr &partitioner, const Partitioner2::Function::Ptr &f) const {
    std::ostringstream ss;
    unparse(ss, partitioner, f);
    return ss.str();
}

std::string
Base::unparse(SgAsmInstruction *insn) const {
    std::ostringstream ss;
    unparse(ss, insn);
    return ss.str();
}

void
Base::unparse(std::ostream &out, const Partitioner2::Partitioner::ConstPtr &partitioner, const Progress::Ptr &progress) const {
    ASSERT_not_null(partitioner);
    Sawyer::ProgressBar<size_t> progressBar(partitioner->nFunctions(), mlog[MARCH], "unparse");
    progressBar.suffix(" functions");
    State state(partitioner, partitioner->architecture(), settings(), *this);
    initializeState(state);
    for (P2::Function::Ptr f: partitioner->functions()) {
        ++progressBar;
        if (progress)
            progress->update(Progress::Report("unparse", progressBar.ratio()));
        emitFunction(out, f, state);
    }
}

void
Base::unparse(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) const {
    State state(partitioner, architecture(), settings(), *this);
    initializeState(state);
    emitInstruction(out, insn, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::BasicBlock::Ptr &bb) const {
    State state(partitioner, architecture(), settings(), *this);
    initializeState(state);
    emitBasicBlock(out, bb, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::DataBlock::Ptr &db) const {
    State state(partitioner, architecture(), settings(), *this);
    initializeState(state);
    emitDataBlock(out, db, state);
}

void
Base::unparse(std::ostream &out, const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &f) const {
    State state(partitioner, architecture(), settings(), *this);
    initializeState(state);
    emitFunction(out, f, state);
}

void
Base::unparse(std::ostream &out, SgAsmInstruction *insn) const {
    State state(P2::Partitioner::Ptr(), architecture(), settings(), *this);
    initializeState(state);
    emitInstruction(out, insn, state);
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

        {
            StyleGuard style(state.styleStack(), settings().comment.line.style, settings().function.separatorStyle);
            out <<style.render();
            out <<std::string(120, ';') <<"\n";
            out <<style.restore();
        }

        if (settings().function.showingDemangled) {
            state.frontUnparser().emitLinePrefix(out, state);

            StyleGuard style(state.styleStack(), settings().comment.line.style, settings().function.titleStyle);
            out <<style.render() <<";;; " <<function->printableName() <<"\n";
            if (function->demangledName() != function->name()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<";;; mangled name is \"" <<StringUtility::cEscape(function->name()) <<"\"\n";
            }
            out <<style.restore();
        } else {
            state.frontUnparser().emitLinePrefix(out, state);

            StyleGuard style(state.styleStack(), settings().comment.line.style, settings().function.titleStyle);
            out <<style.render() <<";;; function " <<StringUtility::addrToString(function->address());
            if (!function->name().empty()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<" \"" <<StringUtility::cEscape(function->name()) <<"\"";
            }
            out <<style.restore() <<"\n";
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
                StyleGuard style(state.styleStack(), state.frontUnparser().settings().comment.line.style);
                out <<style.render();
                out <<";;; skip forward " <<StringUtility::plural(bb->address() - nextBlockVa, "bytes") <<"\n";
                out <<style.restore();
            } else if (bb->address() < nextBlockVa) {
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), state.frontUnparser().settings().comment.line.style);
                out <<style.render();
                out <<";;; skip backward " <<StringUtility::plural(nextBlockVa - bb->address(), "bytes") <<"\n";
                out <<style.restore();
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
                StyleGuard style(state.styleStack(), state.frontUnparser().settings().comment.line.style);
                out <<style.render();
                out <<";;; skip forward " <<StringUtility::plural(db->address() - nextBlockVa, "bytes") <<"\n";
                out <<style.restore();
            } else if (db->address() < nextBlockVa) {
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), state.frontUnparser().settings().comment.line.style);
                out <<style.render();
                out <<";;; skip backward " <<StringUtility::plural(nextBlockVa - db->address(), "bytes") <<"\n";
                out <<style.restore();
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
        if (settings().insn.address.showing && settings().insn.address.useLabels) {
            state.basicBlockLabels().clear();
            for (rose_addr_t bbVa: function->basicBlockAddresses()) {
                std::string label = "L" + boost::lexical_cast<std::string>(state.basicBlockLabels().size()+1);
                state.basicBlockLabels().insertMaybe(bbVa, label);
            }
        }

        // Get all the basic blocks and data blocks and sort them by starting address. Be careful because data blocks might
        // appear more than once since they can be owned by both the function and by multiple basic blocks.
        std::set<P2::DataBlock::Ptr> dblocks;
        std::vector<InsnsOrData> blocks;
        blocks.reserve(function->nBasicBlocks() + function->nDataBlocks());
        ASSERT_not_null(state.partitioner());
        for (rose_addr_t bbVa: function->basicBlockAddresses()) {
            if (P2::BasicBlock::Ptr bb = state.partitioner()->basicBlockExists(bbVa)) {
                blocks.push_back(bb);
                for (const P2::DataBlock::Ptr &db: bb->dataBlocks())
                    dblocks.insert(db);
            }
        }
        for (const P2::DataBlock::Ptr &db: function->dataBlocks())
            dblocks.insert(db);
        blocks.insert(blocks.end(), dblocks.begin(), dblocks.end());
        std::sort(blocks.begin(), blocks.end(), increasingAddress);

        // Emit each basic- or data-block
        rose_addr_t nextBlockVa = function->address();
        for (const InsnsOrData &block: blocks)
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<style.render();
            out <<";;; defined at " <<function->sourceLocation() <<"\n";
            out <<style.restore();
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
            snprintf(buf, sizeof(buf), "0x%08x", flags);
            strings.push_back("other (" + std::string(buf));
        }

        StyleGuard style(state.styleStack(), settings().comment.line.style);
        if (!strings.empty()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<style.render() <<";;; reasons for function: " <<boost::join(strings, ", ") <<style.restore() <<"\n";
        }
        if (!function->reasonComment().empty()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<style.render() <<";;; reason comment: " <<function->reasonComment() <<style.restore() <<"\n";
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            for (const P2::FunctionCallGraph::Graph::Edge &edge: vertex->inEdges()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<style.render() <<";;; ";
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
                out <<style.restore() <<"\n";
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            for (const P2::FunctionCallGraph::Graph::Edge &edge: vertex->outEdges()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<style.render() <<";;; ";
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
                out <<style.restore() <<"\n";
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<style.render() <<";;;" <<style.restore() <<"\n";
            state.frontUnparser().emitCommentBlock(out, s, state, ";;; ");
            state.frontUnparser().emitLinePrefix(out, state);
            out <<style.render() <<";;;" <<style.restore() <<"\n";
        }
    }
}

void
Base::emitFunctionStackDelta(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionStackDelta(out, function, state);
    } else {
        StyleGuard style(state.styleStack(), settings().comment.line.style);
        if (settings().function.stackDelta.concrete) {
            int64_t delta = function->stackDeltaConcrete();
            if (delta != SgAsmInstruction::INVALID_STACK_DELTA) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<style.render() <<";;; function stack delta is " <<StringUtility::toHex2(delta, 64)
                    <<style.restore() <<"\n";
            }
        } else if (S2::BaseSemantics::SValue::Ptr delta = function->stackDelta()) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<style.render() <<";;; function stack delta is " <<*delta <<style.restore() <<"\n";
        }
        out <<style.restore();
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
                ASSERT_not_null(state.partitioner());
                CallingConvention::Dictionary matches = state.partitioner()->functionCallingConventionDefinitions(function);
                std::string s = "calling convention definitions:";
                if (!matches.empty()) {
                    for (const CallingConvention::Definition::Ptr &ccdef: matches)
                        s += " " + ccdef->name();
                } else {
                    s += " unknown";
                }
                state.frontUnparser().emitCommentBlock(out, s, state);
            } else {
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), settings().comment.line.style);
                out <<style.render();
                out <<";;; calling convention analysis did not converge\n";
                out <<style.restore();
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<style.render();
            out <<";;; this function is a no-op\n";
            out <<style.restore();
        }
    }
}

void
Base::emitFunctionMayReturn(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitFunctionMayReturn(out, function, state);
    } else if (state.partitioner()) {
        if (!state.partitioner()->functionOptionalMayReturn(function).orElse(true)) {
            state.frontUnparser().emitLinePrefix(out, state);
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<style.render();
            out <<";;; this function does not return to its caller\n";
            out <<style.restore();
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; this is not the function entry point; entry point is ";
            if (settings().insn.address.showing) {
                if (settings().insn.address.useLabels) {
                    out <<state.basicBlockLabels().getOrElse(state.currentFunction()->address(),
                                                             StringUtility::addrToString(state.currentFunction()->address()));
                } else {
                    out <<StringUtility::addrToString(state.currentFunction()->address());
                }
            }
            out <<style.restore() <<"\n";
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
            for (SgAsmInstruction *insn: bb->instructions()) {
                if (insn == bb->instructions().back())
                    state.thisIsBasicBlockLastInstruction();
                state.frontUnparser().emitInstruction(out, insn, state);
                out <<"\n";
                state.nextInsnLabel("");
            }

            // Emit a line after the last instruction if we need to show anything that happens after that instruction.
            if (settings().bblock.showingPostBlock && !bb->instructions().empty() &&
                (settings().insn.stackDelta.showing || settings().insn.frameDelta.showing)) {
                SgAsmInstruction *insn = bb->instructions().back();
                std::ostringstream ss;
                state.isPostInstruction(true);
                emitInstruction(ss, insn, state);
                state.isPostInstruction(false);
                const bool hasNonSpace = [](const std::string &s) {
                    for (char ch: s) {
                        if (!isspace(ch))
                            return true;
                    }
                    return false;
                }(ss.str());
                if (hasNonSpace)
                    out <<ss.str() <<"\n";
            }
        }
    }
}

void
Base::emitBasicBlockEpilogue(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockEpilogue(out, bb, state);
    } else {
        for (P2::DataBlock::Ptr db: bb->dataBlocks()) {
            state.frontUnparser().emitLinePrefix(out, state);
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; related " <<db->printableName() <<", " <<StringUtility::plural(db->size(), "bytes")
                <<style.restore() <<"\n";
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; defined at " <<bb->sourceLocation() <<style.restore() <<"\n";
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
    } else if (state.partitioner()) {
        std::vector<P2::Function::Ptr> functions = state.partitioner()->functionsOwningBasicBlock(bb);
        std::vector<P2::Function::Ptr>::iterator current =
            std::find(functions.begin(), functions.end(), state.currentFunction());
        if (current != functions.end())
            functions.erase(current);
        for (P2::Function::Ptr function: functions) {
            state.frontUnparser().emitLinePrefix(out, state);
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; block also owned by " <<function->printableName() <<style.restore() <<"\n";
        }
    }
}

static std::string
edgeTypeName(const P2::EdgeType &edgeType) {
    std::string retval = stringifyBinaryAnalysisPartitioner2EdgeType(edgeType, "E_");
    for (char &ch: retval)
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
        for (P2::ControlFlowGraph::ConstEdgeIterator edge: edges) {

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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; predecessor: " <<s <<style.restore() <<"\n";
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
        P2::BasicBlock::Successors successors = state.partitioner()->basicBlockSuccessors(bb);
        for (P2::ControlFlowGraph::ConstEdgeIterator edge: edges) {
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
                SymbolicExpression::Ptr expr = successors[i].expr()->get_expression();

                if (targetVa && expr->toUnsigned().isEqual(targetVa)) {
                    // Edge to concrete node
                    state.frontUnparser().emitLinePrefix(out, state);
                    StyleGuard style(state.styleStack(), settings().comment.line.style);
                    out <<"\t" <<style.render() <<";; successor: " <<edgeTypeName(successors[i].type()) <<" edge to ";
                    state.frontUnparser().emitAddress(out, *targetVa, state);
                    out <<style.restore() <<"\n";
                    successors.erase(successors.begin()+i);
                    emitted = true;
                    break;

                } else if (!targetVa && !expr->isIntegerConstant()) {
                    // Edge to computed address
                    state.frontUnparser().emitLinePrefix(out, state);
                    StyleGuard style(state.styleStack(), settings().comment.line.style);
                    out <<"\t" <<style.render() <<";; successor: " <<edgeTypeName(successors[i].type())
                        <<" edge to " <<*expr <<style.restore() <<"\n";
                    successors.erase(successors.begin()+i);
                    emitted = true;
                    break;
                }
            }

            // When this CFG edge has no matching successor. Not sure whether this can happen. [Robb Matzke 2018-12-05]
            if (!emitted) {
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), settings().comment.line.style);
                out <<"\t" <<style.render() <<";; successor: (only in CFG) " <<edgeTypeName(edge->value().type()) <<" edge to ";
                if (targetVa) {
                    state.frontUnparser().emitAddress(out, *targetVa, state);
                } else {
                    out <<"unknown";
                }
                out <<style.restore() <<"\n";
            }
        }

        // Emit the basic block successors that don't correspond to any CFG edge. Can this happen? [Robb Matzke 2018-12-05]
        for (const P2::BasicBlock::Successor &successor: successors) {
            ASSERT_not_null(successor.expr());
            SymbolicExpression::Ptr expr = successor.expr()->get_expression();
            if (expr->isIntegerConstant() && expr->nBits() <= 64) {
                // Edge to concrete node
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), settings().comment.line.style);
                out <<"\t" <<style.render() <<";; successor: (not in CFG) " <<edgeTypeName(successor.type()) <<" edge to ";
                state.frontUnparser().emitAddress(out, expr->toUnsigned().get(), state);
                out <<style.restore() <<"\n";
            } else if (expr->isLeafNode()) {
                // What?
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), settings().comment.line.style);
                out <<"\t" <<style.render() <<";; successor: (not in CFG) " <<edgeTypeName(successor.type()) <<" edge to unknown"
                    <<style.restore() <<"\n";
            } else {
                // Edge to computed address
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), settings().comment.line.style);
                out <<"\t" <<style.render() <<";; successor: (not in CFG) " <<edgeTypeName(successor.type()) <<" edge to " <<*expr
                    <<style.restore() <<"\n";
            }
        }

        // Ghost successors due to opaque predicates
        if (bb->ghostSuccessors().isCached()) {
            for (rose_addr_t va: bb->ghostSuccessors().get()) {
                state.frontUnparser().emitLinePrefix(out, state);
                StyleGuard style(state.styleStack(), settings().comment.line.style);
                out <<"\t" <<style.render() <<";; successor: (ghost) ";
                state.frontUnparser().emitAddress(out, va, state);
                out <<style.restore() <<"\n";
            }
        }
    }
}

void
Base::emitBasicBlockReachability(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitBasicBlockReachability(out, bb, state);
    } else if (!state.cfgVertexReachability().empty()) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = state.partitioner()->findPlaceholder(bb->address());
        if (vertex != state.partitioner()->cfg().vertices().end()) {
            Reachability::ReasonFlags reachable = state.isCfgVertexReachable(vertex->id());
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            if (reachable.isAnySet()) {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t" <<style.render() <<";; reachable from: " <<state.reachabilityName(reachable)
                    <<style.restore() <<"\n";
            } else {
                state.frontUnparser().emitLinePrefix(out, state);
                out <<"\t" <<style.render() <<";; not reachable" <<style.restore() <<"\n";
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
        {
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; " <<db->printableName() <<", " <<StringUtility::plural(db->size(), "bytes")
                <<style.restore() <<"\n";
        }
        if (P2::Function::Ptr function = state.currentFunction()) {
            for (rose_addr_t bbVa: function->basicBlockAddresses()) {
                if (P2::BasicBlock::Ptr bb = state.partitioner()->basicBlockExists(bbVa)) {
                    if (bb->dataBlockExists(db)) {
                        state.frontUnparser().emitLinePrefix(out, state);
                        StyleGuard style(state.styleStack(), settings().comment.line.style);
                        out <<"\t" <<style.render() <<";; referenced by " <<bb->printableName() <<style.restore() <<"\n";
                    }
                }
            }
        }

        state.frontUnparser().emitLinePrefix(out, state);
        {
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; block type is " <<db->type()->toString() <<style.restore() <<"\n";
        }
    }
}

void
Base::emitDataBlockBody(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitDataBlockBody(out, db, state);
    } else if (AddressInterval where = db->extent()) {
        if (MemoryMap::Ptr map = state.partitioner()->memoryMap()) {
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
                AddressInterval read = state.partitioner()->memoryMap()->atOrAfter(where.least()).limit(maxSize).read(buf);
                hexdump(out, read.least(), buf, read.size(), fmt);
                if (read.greatest() == where.greatest())
                    break;                              // avoid possible overflow
                where = AddressInterval::hull(read.greatest()+1, where.greatest());
            }
        } else {
            state.frontUnparser().emitLinePrefix(out, state);
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<style.render();
            out <<";;; no memory map from which to obtain the static block data\n";
            out <<style.restore();
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
            StyleGuard style(state.styleStack(), settings().comment.line.style);
            out <<"\t" <<style.render() <<";; defined at " <<db->sourceLocation() <<style.restore() <<"\n";
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
        std::vector<std::pair<std::string, std::string> > styles;

        // Address or label or nothing.
        if (settings().insn.address.showing) {
            if (state.isPostInstruction()) {
                // Addresses and labels appear only on the first line of the instruction
                parts.push_back("");
            } else if (settings().insn.address.useLabels) {
                if (!state.nextInsnLabel().empty()) {
                    // Use the label that has been specified in the state
                    parts.push_back(state.nextInsnLabel() + ":");
                } else {
                    // We're using labels, but no label is necessary for this instruction. We still need to reserve the space
                    // for this column.
                    parts.push_back("");
                }
            } else {
                if (insn) {
                    std::ostringstream ss;
                    state.frontUnparser().emitInstructionAddress(ss, insn, state);
                    parts.push_back(ss.str());
                } else {
                    parts.push_back("");
                }
            }
            StyleGuard style(state.styleStack(), settings().insn.address.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
            fieldWidths.push_back(settings().insn.address.fieldWidth);
        }
        if (!state.isPostInstruction())
            state.nextInsnLabel("");

        // Raw bytes
        if (settings().insn.bytes.showing) {
            if (state.isPostInstruction()) {
                parts.push_back("");
            } else if (insn) {
                std::ostringstream ss;
                state.frontUnparser().emitInstructionBytes(ss, insn, state);
                parts.push_back(ss.str());
            } else {
                parts.push_back("");
            }
            fieldWidths.push_back(settings().insn.bytes.fieldWidth);
            StyleGuard style(state.styleStack(), settings().insn.bytes.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
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
            StyleGuard style(state.styleStack(), settings().insn.stackDelta.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
        }

        // Frame delta
        if (settings().insn.frameDelta.showing) {
            if (insn) {
                std::ostringstream ss;
                state.frontUnparser().emitInstructionFrameDelta(ss, insn, state);
                parts.push_back(ss.str());
            } else {
                parts.push_back("");
            }
            fieldWidths.push_back(settings().insn.frameDelta.fieldWidth);
            StyleGuard style(state.styleStack(), settings().insn.frameDelta.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
        }

        // Mnemonic
        if (state.isPostInstruction()) {
            parts.push_back("");
            styles.push_back(std::make_pair(std::string(), std::string()));
        } else if (insn) {
            std::ostringstream ss;
            state.frontUnparser().emitInstructionMnemonic(ss, insn, state);
            parts.push_back(ss.str());
            StyleGuard style(state.styleStack(), settings().insn.mnemonic.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
        } else {
            parts.push_back("none");
            StyleGuard style(state.styleStack(), settings().insn.mnemonic.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
        }
        fieldWidths.push_back(settings().insn.mnemonic.fieldWidth);

        // Operands
        if (state.isPostInstruction()) {
            parts.push_back("");
            styles.push_back(std::make_pair(std::string(), std::string()));
        } else if (insn) {
            std::ostringstream ss;
            state.frontUnparser().emitInstructionOperands(ss, insn, state);
            parts.push_back(ss.str());
            StyleGuard style(state.styleStack(), settings().insn.operands.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
        } else {
            parts.push_back("");
            StyleGuard style(state.styleStack(), settings().insn.operands.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
        }
        fieldWidths.push_back(settings().insn.operands.fieldWidth);

        // Comment
        if (settings().insn.comment.showing) {
            if (state.isPostInstruction()) {
                parts.push_back("");
            } else {
                std::ostringstream ss;
                state.frontUnparser().emitInstructionComment(ss, insn, state);
                parts.push_back(ss.str());
            }
            fieldWidths.push_back(settings().insn.comment.fieldWidth);
            StyleGuard style(state.styleStack(), settings().comment.trailing.style);
            styles.push_back(std::make_pair(style.render(), style.restore()));
        }

        std::string full = juxtaposeColumns(parts, fieldWidths, styles);
        std::vector<std::string> lines = StringUtility::split('\n', full);
        for (size_t i = 0; i < lines.size(); ++i) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<lines[i];
            if (i + 1 < lines.size())
                out <<"\n";
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
        const SgUnsignedCharList &bytes = insn->get_rawBytes();
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
                const auto delta = state.isPostInstruction() ?
                                   sdAnalysis.toInt(sdAnalysis.instructionOutputStackDeltaWrtFunction(insn)) :
                                   sdAnalysis.toInt(sdAnalysis.instructionInputStackDeltaWrtFunction(insn));
                if (delta) {
                    if (*delta == 0) {
                        out <<" 00";
                    } else if (*delta > 0) {
                        Diagnostics::mfprintf(out)("+%02x", (unsigned)*delta);
                    } else {
                        Diagnostics::mfprintf(out)("-%02x", (unsigned)(-*delta));
                    }
                } else {
                    out <<" ??";
                }
            }
        } else {
            out <<"??";
        }
    }
}

void
Base::emitInstructionFrameDelta(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionFrameDelta(out, insn, state);
    } else {
        // Although SgAsmInstruction has frameDelta properties, they're not initialized or used when the instruction only exists
        // inside a Partitioner2 object -- they're only initialized when a complete AST is created from the Partitioner2. Instead,
        // we need to look at the enclosing function's stack delta analysis.  Since basic blocks can be shared among functions, this
        // method prints the frame deltas from the perspective of the function we're emitting.
        if (P2::Function::Ptr function = state.currentFunction()) {
            const StackDelta::Analysis &sdAnalysis = function->stackDeltaAnalysis();
            if (sdAnalysis.hasResults()) {
                const auto delta = state.isPostInstruction() ?
                                   sdAnalysis.toInt(sdAnalysis.instructionOutputFrameDelta(insn)) :
                                   sdAnalysis.toInt(sdAnalysis.instructionInputFrameDelta(insn));
                if (delta) {
                    if (*delta == 0) {
                        out <<" 00";
                    } else if (*delta > 0) {
                        Diagnostics::mfprintf(out)("+%02x", (unsigned)*delta);
                    } else {
                        Diagnostics::mfprintf(out)("-%02x", (unsigned)(-*delta));
                    }
                } else {
                    out <<" ??";
                }
            }
        } else {
            out <<"??";
        }
    }
}

void
Base::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (nextUnparser()) {
        nextUnparser()->emitInstructionMnemonic(out, insn, state);
    } else {
        out <<insn->get_mnemonic();
        if (insn->semanticFailure() > 0) {
            StyleGuard style(state.styleStack(), settings().insn.mnemonic.semanticFailureStyle);
            out <<style.render() <<settings().insn.mnemonic.semanticFailureMarker <<style.restore();
        }
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
            comment = architecture()->instructionDescription(insn);
        if (!comment.empty()) {
            StyleGuard style(state.styleStack(), settings().comment.trailing.style);
            out <<style.render() <<"; " <<StringUtility::cEscape(comment) <<style.restore();
        }
    }
}

void
Base::emitInstructionSemantics(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    if (state.partitioner() && settings().insn.semantics.showing) {
        S2::BaseSemantics::RiscOperators::Ptr ops = state.partitioner()->newOperators();
        if (settings().insn.semantics.tracing)
            ops = S2::TraceSemantics::RiscOperators::instance(ops);

        if (S2::BaseSemantics::Dispatcher::Ptr cpu = state.partitioner()->newDispatcher(ops)) {
            try {
                cpu->processInstruction(insn);
                S2::BaseSemantics::Formatter fmt = settings().insn.semantics.formatter;
                std::ostringstream ss;
                ss <<"\n" <<(*cpu->currentState()->registerState() + fmt) <<(*cpu->currentState()->memoryState() + fmt);
                StyleGuard style(state.styleStack(), settings().insn.semantics.style);
                out <<style.render() <<StringUtility::trim(ss.str(), "\n", false, true) <<style.restore();
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
        emitExpression(out, expr, state);
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
        if (state.partitioner()) {
            if (P2::Function::Ptr f = state.partitioner()->functionExists(va)) {
                out <<f->printableName();
                return true;
            }
            if (P2::BasicBlock::Ptr bb = state.partitioner()->basicBlockExists(va)) {
                out <<bb->printableName();
                return true;
            }
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

        if (state.partitioner() && !state.partitioner()->isDefaultConstructed() &&
            bv.size() == state.partitioner()->instructionProvider().instructionPointerRegister().nBits() &&
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
        StyleGuard style(state.styleStack(), settings().comment.line.style);
        out <<style.render();
        std::vector<std::string> lines = StringUtility::split('\n', comment);
        for (const std::string &line: lines) {
            state.frontUnparser().emitLinePrefix(out, state);
            out <<prefix <<line <<"\n";
        }
        out <<style.restore();
    }
}

void
Base::emitTypeName(std::ostream &out, SgAsmType *type, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitTypeName(out, type, state);
    } else {
        if (nullptr == type) {
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

// Parentheses for emitting expressions with inverted precedences
struct Parens {
    std::string left, right;
    Parens() {}
    Parens(const std::string &left, const std::string &right)
        : left(left), right(right) {}
};

static int
operatorPrecedence(SgAsmExpression *expr) {
    ASSERT_not_null(expr);
    if (isSgAsmBinaryPostupdate(expr) ||
        isSgAsmBinaryPreupdate(expr)) {
        return 0;                                       // these look like "X then Y" or "X after Y"

    } else if (isSgAsmBinaryLsr(expr) ||
               isSgAsmBinaryLsl(expr)) {
        return 1;                                       // shift/rotate


    } else if (isSgAsmBinaryAdd(expr) ||
               isSgAsmBinarySubtract(expr)) {
        return 2;

    } else if (isSgAsmBinaryConcat(expr)) {
        return 3;

    } else if (isSgAsmMemoryReferenceExpression(expr) ||
               isSgAsmDirectRegisterExpression(expr) ||
               isSgAsmIntegerValueExpression(expr) ||
               isSgAsmFloatValueExpression(expr) ||
               isSgAsmUnaryUnsignedExtend(expr) ||
               isSgAsmUnarySignedExtend(expr) ||
               isSgAsmUnaryTruncate(expr) ||
               isSgAsmBinaryAsr(expr) ||
               isSgAsmBinaryRor(expr) ||
               isSgAsmBinaryMsl(expr) ||
               isSgAsmAarch32Coprocessor(expr) ||
               isSgAsmByteOrder(expr) ||
               isSgAsmRegisterNames(expr)) {
        return 4;

    } else {
        ASSERT_not_reachable("invalid operator for disassembly");
    }
}

static Parens
parensForPrecedence(int rootPrec, SgAsmExpression *subexpr) {
    ASSERT_not_null(subexpr);
    int subPrec = operatorPrecedence(subexpr);
    if (subPrec < rootPrec) {
        return Parens("(", ")");
    } else {
        return Parens();
    }
}

void
Base::emitExpression(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *op = isSgAsmBinaryAdd(expr)) {
        // Print the "+" and RHS only if RHS is non-zero
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(op->get_rhs());
        if (!ival || !ival->get_bitVector().isAllClear()) {
            int prec = operatorPrecedence(expr);
            Parens parens = parensForPrecedence(prec, op->get_lhs());
            out <<parens.left;
            emitExpression(out, op->get_lhs(), state);
            out <<parens.right;

            out <<" + ";

            parens = parensForPrecedence(prec, op->get_rhs());
            out <<parens.left;
            emitExpression(out, op->get_rhs(), state);
            out <<parens.right;
        } else {
            emitExpression(out, op->get_lhs(), state);
        }

    } else if (SgAsmBinarySubtract *op = isSgAsmBinarySubtract(expr)) {
        // Print the "-" and RHS only if RHS is non-zero
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(op->get_rhs());
        if (!ival || !ival->get_bitVector().isAllClear()) {
            int prec = operatorPrecedence(expr);
            Parens parens = parensForPrecedence(prec, op->get_lhs());
            out <<parens.left;
            emitExpression(out, op->get_lhs(), state);
            out <<parens.right;

            out <<" - ";

            parens = parensForPrecedence(prec, op->get_rhs());
            out <<parens.left;
            emitExpression(out, op->get_rhs(), state);
            out <<parens.right;
        } else {
            emitExpression(out, op->get_lhs(), state);
        }

    } else if (SgAsmBinaryPreupdate *op = isSgAsmBinaryPreupdate(expr)) {
        emitExpression(out, op->get_lhs(), state);
        out <<" (after ";
        emitExpression(out, op->get_lhs(), state);
        out <<" = ";
        emitExpression(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmBinaryPostupdate *op = isSgAsmBinaryPostupdate(expr)) {
        emitExpression(out, op->get_lhs(), state);
        out <<" (then ";
        emitExpression(out, op->get_lhs(), state);
        out <<" = ";
        emitExpression(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmMemoryReferenceExpression *op = isSgAsmMemoryReferenceExpression(expr)) {
        state.frontUnparser().emitTypeName(out, op->get_type(), state);
        out <<" [";
        emitExpression(out, op->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *op = isSgAsmDirectRegisterExpression(expr)) {
        emitRegister(out, op->get_descriptor(), state);

    } else if (SgAsmIntegerValueExpression *op = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, op->get_bitVector(), state);

    } else if (SgAsmFloatValueExpression *op = isSgAsmFloatValueExpression(expr)) {
        out <<op->get_nativeValue();

    } else if (SgAsmUnaryUnsignedExtend *op = isSgAsmUnaryUnsignedExtend(expr)) {
        out <<"uext(";
        emitExpression(out, op->get_operand(), state);
        out <<", " <<op->get_nBits() <<")";

    } else if (SgAsmUnarySignedExtend *op = isSgAsmUnarySignedExtend(expr)) {
        out <<"sext(";
        emitExpression(out, op->get_operand(), state);
        out <<", " <<op->get_nBits() <<")";

    } else if (SgAsmUnaryTruncate *op = isSgAsmUnaryTruncate(expr)) {
        out <<"trunc(";
        emitExpression(out, op->get_operand(), state);
        out <<", " <<op->get_nBits() <<")";

    } else if (SgAsmBinaryAsr *op = isSgAsmBinaryAsr(expr)) {
        out <<"asr(";
        emitExpression(out, op->get_lhs(), state);
        out <<", ";
        emitExpression(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmBinaryRor *op = isSgAsmBinaryRor(expr)) {
        out <<"ror(";
        emitExpression(out, op->get_lhs(), state);
        out <<", ";
        emitExpression(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmBinaryLsr *op = isSgAsmBinaryLsr(expr)) {
        int prec = operatorPrecedence(expr);
        Parens parens = parensForPrecedence(prec, op->get_lhs());
        out <<parens.left;
        emitExpression(out, op->get_lhs(), state);
        out <<parens.right;

        out <<" >> ";

        parens = parensForPrecedence(prec, op->get_rhs());
        out <<parens.left;
        emitExpression(out, op->get_rhs(), state);
        out <<parens.right;

    } else if (SgAsmBinaryLsl *op = isSgAsmBinaryLsl(expr)) {
        int prec = operatorPrecedence(expr);
        Parens parens = parensForPrecedence(prec, op->get_lhs());
        out <<parens.left;
        emitExpression(out, op->get_lhs(), state);
        out <<parens.right;

        out <<" << ";

        parens = parensForPrecedence(prec, op->get_rhs());
        out <<parens.left;
        emitExpression(out, op->get_rhs(), state);
        out <<parens.right;

    } else if (SgAsmBinaryMsl *op = isSgAsmBinaryMsl(expr)) {
        out <<"msl(";
        emitExpression(out, op->get_lhs(), state);
        out <<", ";
        emitExpression(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmAarch32Coprocessor *op = isSgAsmAarch32Coprocessor(expr)) {
        out <<"p" <<op->coprocessor();

    } else if (SgAsmByteOrder *op = isSgAsmByteOrder(expr)) {
        switch (op->byteOrder()) {
            case ByteOrder::ORDER_MSB:
                out <<"be";
                break;
            case ByteOrder::ORDER_LSB:
                out <<"le";
                break;
            default:
                ASSERT_not_reachable("invalid byte order " + boost::lexical_cast<std::string>(op->byteOrder()));
        }

    } else if (SgAsmRegisterNames *op = isSgAsmRegisterNames(expr)) {
        for (size_t i = 0; i < op->get_registers().size(); ++i) {
            out <<(0 == i ? "{" : ", ");
            emitExpression(out, op->get_registers()[i], state);
        }
        out <<"}";

    } else if (SgAsmBinaryConcat *op = isSgAsmBinaryConcat(expr)) {
        int prec = operatorPrecedence(expr);
        Parens parens = parensForPrecedence(prec, op->get_lhs());
        out <<parens.left;
        emitExpression(out, op->get_lhs(), state);
        out <<parens.right;

        out <<":";
        parens = parensForPrecedence(prec, op->get_rhs());
        out <<parens.left;
        emitExpression(out, op->get_rhs(), state);
        out <<parens.right;

    } else {
        ASSERT_not_implemented(expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Line control
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Base::emitLinePrefix(std::ostream &out, State &state) const {
    if (nextUnparser()) {
        nextUnparser()->emitLinePrefix(out, state);
    } else {
        // Indentation prefix
        out <<settings().linePrefix;

        // Generate intra-function arrows that point to basic blocks. I.e., basic blocks are the pointable entities,
        // and we're only drawing arrows that both originate and terminate within the current function.
        StyleGuard style(state.styleStack(), settings().bblock.cfg.arrowStyle);
        out <<style.render();

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

        out <<style.restore();
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
Base::orderedBlockPredecessors(const P2::Partitioner::ConstPtr &partitioner, const P2::BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    std::vector<P2::ControlFlowGraph::ConstEdgeIterator> retval;

    if (partitioner) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner->findPlaceholder(bb->address());
        retval.reserve(vertex->nInEdges());
        for (P2::ControlFlowGraph::ConstEdgeIterator edge = vertex->inEdges().begin(); edge != vertex->inEdges().end(); ++edge)
            retval.push_back(edge);
        std::sort(retval.begin(), retval.end(), ascendingSourceAddress);
    }
    return retval;
}

// class method
std::vector<P2::ControlFlowGraph::ConstEdgeIterator>
Base::orderedBlockSuccessors(const P2::Partitioner::ConstPtr &partitioner, const P2::BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    std::vector<P2::ControlFlowGraph::ConstEdgeIterator> retval;

    if (partitioner) {
        P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner->findPlaceholder(bb->address());
        retval.reserve(vertex->nOutEdges());
        for (P2::ControlFlowGraph::ConstEdgeIterator edge = vertex->outEdges().begin(); edge != vertex->outEdges().end(); ++edge)
            retval.push_back(edge);
        std::sort(retval.begin(), retval.end(), ascendingTargetAddress);
    }
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
