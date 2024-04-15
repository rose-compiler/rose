#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Variables.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/FunctionCallGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/CommandLine/Parser.h>
#include <Rose/StringUtility/Escape.h>

#include <integerOps.h>                                 // rose
#include <stringify.h>                                  // rose

#include <SgAsmAarch32Instruction.h>
#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryMultiply.h>
#include <SgAsmBinaryPreupdate.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmM68kInstruction.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmPowerpcInstruction.h>
#include <SgAsmX86Instruction.h>

#include <Cxx_GrammarDowncast.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/GraphAlgorithm.h>
#include <Sawyer/ThreadWorkers.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/integer_traits.hpp>

#include <regex>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions and variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Message::Facility mlog;

static Sawyer::Attribute::Id ATTR_FRAME_SIZE(-1);       // Key for storing uint64_t frame sizes in P2::Function objects.
static Sawyer::Attribute::Id ATTR_LOCAL_VARS(-1);       // Key for storing StackVariables in a P2::Function.
static Sawyer::Attribute::Id ATTR_GLOBAL_VARS(-1);      // Key for storing GlobalVariables in a P2::Partitioner.

// Called by Rose::Diagnostics::initialize before anything else in this namespace is initialized.
void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Variables");
        mlog.comment("local and global variable detection");
    }
}

// Initialize our own global variables
static void
initNamespaceHelper() {
    ATTR_FRAME_SIZE = Sawyer::Attribute::declare("function frame size");
    ATTR_LOCAL_VARS = Sawyer::Attribute::declare("local variables");
    ATTR_GLOBAL_VARS = Sawyer::Attribute::declare("global variables");
}

static void
initNamespace() {
#if SAWYER_MULTI_THREADED
    static boost::once_flag initFlag = BOOST_ONCE_INIT;
    boost::call_once(initFlag, initNamespaceHelper);
#else
    static bool initialized = false;
    if (!initialized) {
        initNamespaceHelper();
        initialized = true;
    }
#endif
}

std::string
offsetStr(int64_t n) {
    std::string sign = "+";
    if ((uint64_t)n == IntegerOps::shl1<uint64_t>(63)) {
        return "-0x8000000000000000<-9223372036854775808>";
    } else if (n < 0) {
        sign = "-";
        n = -n;
    }

    if (n <= 9) {
        return sign + boost::lexical_cast<std::string>(n);
    } else {
        std::string h = StringUtility::addrToString(n).substr(2);
        boost::trim_left_if(h, boost::is_any_of("0_"));
        ASSERT_forbid(h.empty());
        return sign + "0x" + h + "<" + sign + boost::lexical_cast<std::string>(n) + ">";
    }
}

std::string
sizeStr(uint64_t n) {
    if (n <= 9) {
        return boost::lexical_cast<std::string>(n);
    } else {
        std::string h = StringUtility::addrToString(n).substr(2);
        boost::trim_left_if(h, boost::is_any_of("0_"));
        ASSERT_forbid(h.empty());
        return "0x" + h + "<" + boost::lexical_cast<std::string>(n) + ">";
    }
}

void
print(const StackVariables &lvars, const P2::Partitioner::ConstPtr &partitioner,
      std::ostream &out, const std::string &prefix) {
    ASSERT_not_null(partitioner);
    for (const StackVariable &lvar: lvars.values()) {
        out <<prefix <<lvar <<"\n";
        for (rose_addr_t va: lvar.definingInstructionVas().values())
            out <<prefix <<"  detected at " <<partitioner->instructionProvider()[va]->toString() <<"\n";
    }
}

void
print(const GlobalVariables &gvars, const P2::Partitioner::ConstPtr &partitioner,
      std::ostream &out, const std::string &prefix) {
    ASSERT_not_null(partitioner);
    for (const GlobalVariable &gvar: gvars.values()) {
        out <<prefix <<gvar <<"\n";
        for (rose_addr_t va: gvar.definingInstructionVas().values())
            out <<prefix <<"  detected at " <<partitioner->instructionProvider()[va]->toString() <<"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseVariable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseVariable::BaseVariable() {}

BaseVariable::BaseVariable(size_t maxSizeBytes, const AddressSet &definingInstructionVas, const std::string &name)
    // following arithmetic is to work around lack of SSIZE_MAX on windows. The maxSizeBytes should not be more than the
    // maximum value of the signed type with the same conversion rank.
    : maxSizeBytes_(std::min(maxSizeBytes, ((size_t)(1) << (8*sizeof(size_t)-1))-1)),
      insnVas_(definingInstructionVas), name_(name) {}

BaseVariable::BaseVariable(const BaseVariable &other) = default;
BaseVariable::~BaseVariable() {}

rose_addr_t
BaseVariable::maxSizeBytes() const {
    return maxSizeBytes_;
}

void
BaseVariable::maxSizeBytes(rose_addr_t size) {
    ASSERT_require(size > 0);
    maxSizeBytes_ = size;
}

const AddressSet&
BaseVariable::definingInstructionVas() const {
    return insnVas_;
}

AddressSet&
BaseVariable::definingInstructionVas() {
    return insnVas_;
}

void
BaseVariable::definingInstructionVas(const AddressSet &vas) {
    insnVas_ = vas;
}

const InstructionSemantics::BaseSemantics::InputOutputPropertySet&
BaseVariable::ioProperties() const {
    return ioProperties_;
}

InstructionSemantics::BaseSemantics::InputOutputPropertySet&
BaseVariable::ioProperties() {
    return ioProperties_;
}

void
BaseVariable::ioProperties(const InstructionSemantics::BaseSemantics::InputOutputPropertySet &set) {
    ioProperties_ = set;
}

const std::string&
BaseVariable::name() const {
    return name_;
}

void
BaseVariable::name(const std::string &s) {
    name_ = s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StackVariable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StackVariable::StackVariable() {}

StackVariable::StackVariable(const P2::FunctionPtr &function, int64_t frameOffset, rose_addr_t maxSizeBytes,
                             Purpose purpose, const AddressSet &definingInstructionVas, const std::string &name)
    : BaseVariable(maxSizeBytes, definingInstructionVas, name), function_(function), frameOffset_(frameOffset),
      purpose_(purpose) {}

StackVariable::StackVariable(const StackVariable&) = default;
StackVariable::~StackVariable() {}

P2::Function::Ptr
StackVariable::function() const {
    return function_;
}

void
StackVariable::function(const P2::Function::Ptr &f) {
    ASSERT_not_null(f);
    function_ = f;
}

int64_t
StackVariable::frameOffset() const {
    return frameOffset_;
}

void
StackVariable::frameOffset(int64_t offset) {
    frameOffset_ = offset;
}

StackVariable::Purpose
StackVariable::purpose() const {
    return purpose_;
}

void
StackVariable::purpose(Purpose p) {
    purpose_ = p;
}

const std::string&
StackVariable::setDefaultName() {
    int64_t offset = frameOffset();
    std::string s;

    // Depending on the architecture, local variables could be at negative or positive frame offsets, and arguments stored
    // on the stack are probably have the other sign. We'd like to be able to distinguish the two with different names.
    if (offset < 0)
        offset = -offset;

    static const char* const consonants = "bhjkclmndpqrfstvgwxz";
    static const char* const vowels = "aeiou";
    static int64_t nConsonants = strlen(consonants);
    static int64_t nVowels = strlen(vowels);

    for (size_t i=0; offset > 0; ++i) {
        if (i % 2 == 0) {
            s += consonants[offset % nConsonants];
            offset /= nConsonants;
        } else {
            s += vowels[offset % nVowels];
            offset /= nVowels;
        }
    }

    // Negative frame offsets are more common, so add "y" to the uncommon cases.
    if (frameOffset() >= 0)
        s += "y";

    name(s);
    return name();
}

bool
StackVariable::operator==(const StackVariable &other) const {
    if (!function_ || !other.function_) {
        return function_ == other.function_;
    } else {
        return function_->address() == other.function_->address() &&
            frameOffset() == other.frameOffset() &&
            maxSizeBytes() == other.maxSizeBytes();
    }
}

bool
StackVariable::operator!=(const StackVariable &other) const {
    return !(*this == other);
}

OffsetInterval
StackVariable::interval() const {
    // We need to watch for overflows.  The return type, OffsetInterval, has int64_t least and greatest values. The frame
    // offset is also int64_t. The maximum size in bytes however is uint64_t (i.e., rose_addr_t).  We may need to reduce the
    // maximum size in order to fit it into the interval return value.
    int64_t least = frameOffset_;
    int64_t maxSizeSigned = maxSizeBytes() > boost::numeric_cast<uint64_t>(boost::integer_traits<int64_t>::const_max)
                            ? boost::integer_traits<int64_t>::const_max
                            : boost::numeric_cast<int64_t>(maxSizeBytes());
    if (least >= 0) {
        int64_t headroom = boost::integer_traits<int64_t>::const_max - maxSizeSigned;
        if (least > headroom) {
            // overflow would occur, so we must reduce the maxSizeS appropriately
            return OffsetInterval::hull(least, boost::integer_traits<int64_t>::max());
        } else {
            return OffsetInterval::baseSize(least, maxSizeSigned);
        }
    } else {
        return OffsetInterval::baseSize(least, maxSizeSigned);
    }
}

// class method
StackVariable::Boundary&
StackVariable::insertBoundary(Boundaries &boundaries /*in,out*/, int64_t frameOffset, rose_addr_t insnVa) {
    for (size_t i = 0; i < boundaries.size(); ++i) {
        if (boundaries[i].frameOffset == frameOffset) {
            boundaries[i].definingInsns.insert(insnVa);
            return boundaries[i];
        }
    }

    boundaries.push_back(Boundary());
    boundaries.back().frameOffset = frameOffset;
    boundaries.back().definingInsns.insert(insnVa);
    return boundaries.back();
}

void
StackVariable::print(std::ostream &out) const {
    out <<"local-variable";
    if (!name().empty())
        out <<" \"" <<StringUtility::cEscape(name()) <<"\"";
    out <<" (loc=fp" <<offsetStr(frameOffset()) <<", size=" <<sizeStr(maxSizeBytes())
        <<", type=" <<stringify::Rose::BinaryAnalysis::Variables::StackVariable::Purpose((int64_t)purpose_)
        <<")";
}

std::string
StackVariable::toString() const {
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

std::ostream&
operator<<(std::ostream &out, const Rose::BinaryAnalysis::Variables::StackVariable &x) {
    x.print(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variable descriptors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GlobalVariable::GlobalVariable() {}

GlobalVariable::GlobalVariable(rose_addr_t startingAddress, rose_addr_t maxSizeBytes,
                               const AddressSet &definingInstructionVas, const std::string &name)
    : BaseVariable(maxSizeBytes, definingInstructionVas, name), address_(startingAddress) {}

GlobalVariable::~GlobalVariable() {}

rose_addr_t
GlobalVariable::address() const {
    return address_;
}

void
GlobalVariable::address(rose_addr_t va) {
    address_ = va;
}

const std::string&
GlobalVariable::setDefaultName() {
    ASSERT_not_implemented("[Robb Matzke 2019-12-06]");
}

bool
GlobalVariable::operator==(const GlobalVariable &other) const {
    return address() == other.address() && maxSizeBytes() == other.maxSizeBytes();
}

bool
GlobalVariable::operator!=(const GlobalVariable &other) const {
    return !(*this == other);
}

AddressInterval
GlobalVariable::interval() const {
    return AddressInterval::baseSize(address(), maxSizeBytes());
}

void
GlobalVariable::print(std::ostream &out) const {
    out <<"global-variable";
    if (!name().empty())
        out <<" \"" <<StringUtility::cEscape(name()) <<"\"";
    out <<"(va=" <<StringUtility::addrToString(address()) <<", size=" <<sizeStr(maxSizeBytes()) <<")";
}

std::string
GlobalVariable::toString() const {
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

std::ostream&
operator<<(std::ostream &out, const Rose::BinaryAnalysis::Variables::GlobalVariable &x) {
    x.print(out);
    return out;
}

void
erase(GlobalVariables &gvars, const AddressInterval &where) {
    if (where) {
        ASSERT_forbid(isInconsistent(gvars, mlog[ERROR]));
        gvars.erase(where);

        // If we erased the beginning of a variable, then adjust the variable's starting address and maximum size.
        auto next = gvars.upperBound(where.greatest());
        if (next != gvars.nodes().end()
            && next->key().least() == where.greatest() + 1
            && where.contains(next->value().address())) {
            const uint64_t less = where.greatest() + 1 - next->value().address();
            next->value().address(where.greatest() + 1);
            next->value().maxSizeBytes(next->value().maxSizeBytes() - less);
        }

        // If we erased the middle of a variable, then we should leave only the part that is before `where`.  For now, we'll just
        // erase the following node from the `gvars` map without changing the variable. Next we're going to find the prior entries
        // and adjust the variable's location.
        if (next != gvars.nodes().end()
            && next->key().least() == where.greatest() + 1
            && next->value().address() < where.least()) {
            gvars.erase(next->key());
        }

        // If there's a variable immediately before what we erased and it's size extends into the part we erased, then adjust its
        // size so it doesn't go into the erased area.
        auto prior = gvars.findPrior(where.least());
        if (prior != gvars.nodes().end()
            && prior->key().greatest() + 1 == where.least()
            && prior->value().address() + prior->value().maxSizeBytes() > where.least()) {
            prior->value().maxSizeBytes(where.least() - prior->value().address());
        }
        ASSERT_forbid(isInconsistent(gvars, mlog[ERROR]));
    }
}

AddressInterval
isInconsistent(const GlobalVariables &gvars, Sawyer::Message::Stream &out) {
    for (const auto &node: gvars.nodes()) {
        if (node.key() != AddressInterval::baseSize(node.value().address(), node.value().maxSizeBytes())) {
            if (out) {
                for (const auto &n: gvars.nodes()) {
                    const auto i = AddressInterval::baseSize(node.value().address(), node.value().maxSizeBytes());
                    out <<(n.key() == i ? "    " : "  * ")
                        <<"key = " <<StringUtility::addrToString(n.key())
                        <<" (" <<StringUtility::plural(n.key().size(), "bytes") <<")"
                        <<"; var_location = " <<StringUtility::addrToString(i)
                        <<" (" <<StringUtility::plural(i.size(), "bytes") <<")"
                        <<", variable = " <<n.value() <<"\n";
                }
            }
            return node.key();
        }
    }
    return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantics for finding local variables. This works by intercepting each memory I/O and examining the address to see whether
// it's an offset from the frame pointer.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace VarSearchSemantics {

typedef P2::Semantics::SValue SValue;
typedef P2::Semantics::RegisterState RegisterState;
typedef P2::Semantics::MemoryMapState MemoryState;
typedef P2::Semantics::State State;

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators: public P2::Semantics::RiscOperators {
    SymbolicExpression::Ptr base_;                      // value to subtract from each address to find stack variable offsets
    StackVariable::Boundaries &boundaries_;             // variable boundaries in the stack frame

public:
    using Super = P2::Semantics::RiscOperators;
    using Ptr = RiscOperatorsPtr;

protected:
    explicit RiscOperators(const S2::BaseSemantics::SValue::Ptr &protoval, StackVariable::Boundaries &boundaries /*in,out*/)
        : Super(protoval, SmtSolverPtr()), boundaries_(boundaries) {
        name("VarSearchSemantics");
        (void) SValue::promote(protoval);
    }

    explicit RiscOperators(const S2::BaseSemantics::State::Ptr &state, StackVariable::Boundaries &boundaries /*in,out*/)
        : Super(state, SmtSolverPtr()), boundaries_(boundaries) {
        name("VarSearchSemantics");
        (void) SValue::promote(state->protoval());
    }

public:
    static RiscOperators::Ptr instance(const StackFrame &frame, const P2::Partitioner::ConstPtr &partitioner,
                                     StackVariable::Boundaries &boundaries /*in,out*/) {
        ASSERT_not_null(partitioner);
        RegisterDictionary::Ptr regdict = partitioner->instructionProvider().registerDictionary();
        S2::BaseSemantics::SValue::Ptr protoval = SValue::instance();
        S2::BaseSemantics::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
        S2::BaseSemantics::MemoryState::Ptr memory = P2::Semantics::MemoryMapState::instance(protoval, protoval);
        S2::BaseSemantics::State::Ptr state = State::instance(registers, memory);
        RiscOperators::Ptr retval = RiscOperators::Ptr(new RiscOperators(state, boundaries));

        S2::BaseSemantics::SValue::Ptr dflt = protoval->undefined_(frame.framePointerRegister.nBits());
        retval->base_ = SValue::promote(state->readRegister(frame.framePointerRegister, dflt, retval.get()))->get_expression();
        return retval;
    }

public:
    virtual S2::BaseSemantics::RiscOperators::Ptr
    create(const S2::BaseSemantics::SValue::Ptr &/*protoval*/, const SmtSolverPtr& = SmtSolverPtr()) const override {
        ASSERT_not_implemented("[Robb Matzke 2019-09-16]");
    }

    virtual S2::BaseSemantics::RiscOperators::Ptr
    create(const S2::BaseSemantics::State::Ptr&, const SmtSolverPtr& = SmtSolverPtr()) const override {
        ASSERT_not_implemented("[Robb Matzke 2019-09-16]");
    }

public:
    static RiscOperators::Ptr
    promote(const S2::BaseSemantics::RiscOperators::Ptr &x) {
        RiscOperators::Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual S2::BaseSemantics::SValue::Ptr
    readMemory(RegisterDescriptor segreg, const S2::BaseSemantics::SValue::Ptr &addr, const S2::BaseSemantics::SValue::Ptr &dflt,
               const S2::BaseSemantics::SValue::Ptr &cond) override {
        if (SgAsmInstruction *insn = currentInstruction()) {
            SAWYER_MESG(mlog[DEBUG]) <<"    insn " <<StringUtility::addrToString(insn->get_address())
                                     <<" reads from address: " <<*addr <<"\n";

            lookForVariable(addr);
        }
        return Super::readMemory(segreg, addr, dflt, cond);
    }

    virtual void
    writeMemory(RegisterDescriptor segreg, const S2::BaseSemantics::SValue::Ptr &addr, const S2::BaseSemantics::SValue::Ptr &data,
                const S2::BaseSemantics::SValue::Ptr &cond) override {
        if (SgAsmInstruction *insn = currentInstruction()) {
            SAWYER_MESG(mlog[DEBUG]) <<"    insn " <<StringUtility::addrToString(insn->get_address())
                                     <<" writes to address: " <<*addr <<"\n";
            lookForVariable(addr);
        }
        return Super::writeMemory(segreg, addr, data, cond);
    }

private:
    void lookForVariable(const S2::BaseSemantics::SValue::Ptr &addrSVal) {
        ASSERT_not_null(base_);
        SymbolicExpression::Ptr addr = SValue::promote(addrSVal)->get_expression();
        Sawyer::Message::Stream debug(mlog[DEBUG]);

        // The address must reference the stack or frame pointer
        struct Finder: SymbolicExpression::Visitor {
            SymbolicExpression::Ptr needle;                   // thing to find
            SymbolicExpression::Ptr found;                    // first matching expression in haystack
            std::set<SymbolicExpression::Hash> seen;          // subexpressions we've already checked

            Finder(const SymbolicExpression::Ptr needle)
                : needle(needle) {}

            SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) override {
                if (seen.insert(node->hash()).second && node->isEquivalentTo(needle)) {
                    found = node;
                    return SymbolicExpression::TERMINATE;
                } else {
                    return SymbolicExpression::CONTINUE;
                }
            }

            SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) override {
                return SymbolicExpression::CONTINUE;
            }
        } finder(base_);
        addr->depthFirstTraversal(finder);
        if (!finder.found)
            return;

        // Address must be an offset from some base address. The constant is the offet since the base address is probably a
        // symbolic stack or frame pointer. There might be more than one constant.
        if (SymbolicExpression::OP_ADD == addr->getOperator()) {
            for (SymbolicExpression::Ptr operand: addr->children()) {
                int64_t offset = 0;
                if (operand->toSigned().assignTo(offset)) {
                    SAWYER_MESG(debug) <<"    found offset " <<offsetStr(offset)
                                       <<" at " <<currentInstruction()->toString() <<"\n";
                    StackVariable::insertBoundary(boundaries_, offset, currentInstruction()->get_address());
                }
            }
        }
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VariableFinder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VariableFinder::VariableFinder(const Settings &settings)
    : settings_(settings) {
    initNamespace();
}

VariableFinder::~VariableFinder() {}

VariableFinder::Ptr
VariableFinder::instance(const Settings &settings) {
    return Ptr(new VariableFinder(settings));
}

StackFrame
VariableFinder::detectFrameAttributes(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);
    SgAsmInstruction *firstInsn = partitioner->instructionProvider()[function->address()];
    StackFrame frame;
    frame.framePointerRegister = partitioner->instructionProvider().stackFrameRegister();

    if (isSgAsmX86Instruction(firstInsn)) {
        // See initializeFrameBoundaries for the visual representation of the stack frame
        frame.growthDirection = StackFrame::GROWS_DOWN;
        frame.maxOffset = 2 * partitioner->instructionProvider().wordSize() - 1;
        frame.rule = "x86: general";

    } else if (isSgAsmPowerpcInstruction(firstInsn)) {
        frame.growthDirection = StackFrame::GROWS_DOWN;

        if (isSgAsmPowerpcInstruction(firstInsn)->get_kind() == powerpc_stwu) {
            // If this PowerPC function starts with "stwu r1, u32 [r1 - N]" then the frame size is N.
            static RegisterDescriptor REG_R1;
            if (REG_R1.isEmpty())
                REG_R1 = partitioner->instructionProvider().registerDictionary()->findOrThrow("r1");
            SgAsmDirectRegisterExpression *firstRegister = isSgAsmDirectRegisterExpression(firstInsn->operand(0));
            SgAsmMemoryReferenceExpression *secondArg = isSgAsmMemoryReferenceExpression(firstInsn->operand(1));
            SgAsmBinaryAdd *memAddr = secondArg ? isSgAsmBinaryAdd(secondArg->get_address()) : NULL;
            SgAsmDirectRegisterExpression *secondRegister = memAddr ? isSgAsmDirectRegisterExpression(memAddr->get_lhs()) : NULL;
            SgAsmIntegerValueExpression *constant = memAddr ? isSgAsmIntegerValueExpression(memAddr->get_rhs()) : NULL;
            int64_t n = constant ? constant->get_signedValue() : int64_t(0);
            if (firstRegister && secondRegister && constant && n < 0 &&
                firstRegister->get_descriptor() == REG_R1 && secondRegister->get_descriptor() == REG_R1) {
                frame.size = -n;

                // The frame pointer will point to the bottom (least address) of the frame.
                frame.minOffset = 0;
                frame.maxOffset = *frame.size - 1;
                frame.rule = "ppc: stwu r1, u32 [r1 - N]";
            }

        } else {
            frame.size = 8;
            frame.minOffset = 0;
            frame.maxOffset = 7;
            frame.rule = "ppc: general";
        }

    } else if (auto m68k = isSgAsmM68kInstruction(firstInsn)) {
        frame.growthDirection = StackFrame::GROWS_DOWN;

        const RegisterDescriptor REG_FP = partitioner->instructionProvider().stackFrameRegister();

        // If this m68k function starts with "link.w fp, -N" then the frame size is 4 + 4 + N since the caller has pushed the
        // return address and this function pushes the old frame pointer register r6 and then reserves N additional bytes in
        // the frame.
        if (REG_FP &&
            m68k->get_kind() == m68k_link &&
            m68k->nOperands() == 2 &&
            isSgAsmDirectRegisterExpression(m68k->operand(0)) &&
            isSgAsmDirectRegisterExpression(m68k->operand(0))->get_descriptor() == REG_FP &&
            isSgAsmIntegerValueExpression(m68k->operand(1))) {
            int64_t n = isSgAsmIntegerValueExpression(m68k->operand(1))->get_signedValue();
            ASSERT_require(n <= 0);
            frame.size = 4 /* previously pushed return address */ + 4 /* pushed frame pointer */ + (-n);
            frame.maxOffset = 7;                        // return address and frame pointer
            frame.minOffset = n;                        // negative offset for space reserved by this insn
            frame.rule = "m68k: link a6, N";
        } else {
            frame.size = 4 /* previously pushed return address */;
            frame.maxOffset = 3;
            frame.minOffset = 0;
            frame.rule = "m68k: general";
        }

#ifdef ROSE_ENABLE_ASM_AARCH32
    } else if (isSgAsmAarch32Instruction(firstInsn)) {
        frame.growthDirection = StackFrame::GROWS_DOWN;

        const RegisterDescriptor REG_FP = partitioner->instructionProvider().registerDictionary()->findOrThrow("fp");
        const RegisterDescriptor REG_SP = partitioner->instructionProvider().registerDictionary()->findOrThrow("sp");
        const RegisterDescriptor REG_LR = partitioner->instructionProvider().registerDictionary()->findOrThrow("lr");

        // If the first three instructions are:
        //   push fp, lr
        //   add fp, sp, 4
        //   sub sp, N
        // then the frame size is N + 8 bytes
        SgAsmAarch32Instruction* insns[3] = {nullptr, nullptr, nullptr};
        insns[0] = isSgAsmAarch32Instruction(firstInsn);
        insns[1] = isSgAsmAarch32Instruction(partitioner->instructionProvider()[function->address() + 4]);
        insns[2] = isSgAsmAarch32Instruction(partitioner->instructionProvider()[function->address() + 8]);
        if (// push fp, lr
            insns[0] && insns[0]->get_kind() == Aarch32InstructionKind::ARM_INS_PUSH &&
            insns[0]->nOperands() == 2 &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(0)) &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(0))->get_descriptor() == REG_FP &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(1)) &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(1))->get_descriptor() == REG_LR &&
            // add fp, sp, 4
            insns[1] && insns[1]->get_kind() == Aarch32InstructionKind::ARM_INS_ADD &&
            insns[1]->nOperands() == 3 &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(0)) &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(0))->get_descriptor() == REG_FP &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(1)) &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(1))->get_descriptor() == REG_SP &&
            isSgAsmIntegerValueExpression(insns[1]->operand(2)) &&
            isSgAsmIntegerValueExpression(insns[1]->operand(2))->get_absoluteValue() == 4 &&
            // sub sp, sp, N
            insns[2] && insns[2]->get_kind() == Aarch32InstructionKind::ARM_INS_SUB &&
            insns[2]->nOperands() == 3 &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(0)) &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(0))->get_descriptor() == REG_SP &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(1)) &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(1))->get_descriptor() == REG_SP &&
            isSgAsmIntegerValueExpression(insns[2]->operand(2))) {
            // The frame pointer will point to one past the top (highest address) of the frame
            int64_t n = boost::numeric_cast<int64_t>(isSgAsmIntegerValueExpression(insns[2]->operand(2))->get_absoluteValue());
            frame.size = n + 8;
            frame.maxOffset = 3;                        // lr_0 appears at the frame pointer, plus three bytes above the fp
            frame.minOffset = -(n + 4);                 // entire frame except the lr_0 which appears on the stack at the fp
            // Note that the string "<saved-lr>" is important and used by initializeFrameBoundaries
            frame.rule = "aarch32: push fp, lr <saved-lr>; add fp, sp, 4; sub sp N";

        } else if (// str fp, u32 [sp (after sp -= 4)]
                   insns[0] &&
                   insns[0]->get_kind() == Aarch32InstructionKind::ARM_INS_STR &&
                   insns[0]->nOperands() == 2 &&
                   isSgAsmDirectRegisterExpression(insns[0]->operand(0)) &&
                   isSgAsmDirectRegisterExpression(insns[0]->operand(0))->get_descriptor() == REG_FP &&
                   isSgAsmMemoryReferenceExpression(insns[0]->operand(1)) &&
                   isSgAsmBinaryPreupdate(isSgAsmMemoryReferenceExpression(insns[0]->operand(1))->get_address()) &&
                   isSgAsmDirectRegisterExpression(isSgAsmBinaryPreupdate(isSgAsmMemoryReferenceExpression(insns[0]->operand(1))->get_address())->get_lhs()) &&
                   isSgAsmDirectRegisterExpression(isSgAsmBinaryPreupdate(isSgAsmMemoryReferenceExpression(insns[0]->operand(1))->get_address())->get_lhs())->get_descriptor() == REG_SP &&
                   // add fp, sp, 0
                   insns[1] &&
                   insns[1]->get_kind() == Aarch32InstructionKind::ARM_INS_ADD &&
                   insns[1]->nOperands() == 3 &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(0)) &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(0))->get_descriptor() == REG_FP &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(1)) &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(1))->get_descriptor() == REG_SP &&
                   isSgAsmIntegerValueExpression(insns[1]->operand(2)) &&
                   isSgAsmIntegerValueExpression(insns[1]->operand(2))->get_absoluteValue() == 0 &&
                   // sub sp, sp, N
                   insns[2] &&
                   insns[2]->get_kind() == Aarch32InstructionKind::ARM_INS_SUB &&
                   insns[2]->nOperands() == 3 &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(0)) &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(0))->get_descriptor() == REG_SP &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(1)) &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(1))->get_descriptor() == REG_SP &&
                   isSgAsmIntegerValueExpression(insns[2]->operand(2))) {
            int64_t n = boost::numeric_cast<int64_t>(isSgAsmIntegerValueExpression(insns[2]->operand(2))->get_absoluteValue());
            frame.size = n + 4;
            frame.maxOffset = 3;
            frame.minOffset = -(n + 4);
            frame.rule = "aarch32: str fp, u32 [sp (after sp -= 4)]; add fp, sp, 0; sub sp, sp N";

        } else {
            frame.size = 8;
            frame.maxOffset = 3;
            frame.minOffset = -4;
            frame.rule = "aarch32: general";
        }
#endif
    }

    return frame;
}

void
VariableFinder::initializeFrameBoundaries(const StackFrame &frame, const P2::Partitioner::ConstPtr &partitioner,
                                          const P2::Function::Ptr &function, StackVariable::Boundaries &boundaries /*in,out*/) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);
    SgAsmInstruction *firstInsn = partitioner->instructionProvider()[function->address()];
    const size_t wordNBytes = partitioner->instructionProvider().wordSize() / 8;

    if (isSgAsmX86Instruction(firstInsn)) {
        //
        //                    :                           :
        //                    :   (part of parent frame)  :
        //                    :                           :
        //                (2) | callee's actual arguments |  variable size, 1st arg at lowest address
        //                    +---(current frame----------+
        //                (1) | return address            |  1 word
        // current_frame: (0) | addr of parent frame      |  1 word
        //                    | callee saved registers    |  optional, variable size, multiple of word size
        //                    :                           :
        //                    :                           :
        StackVariable::Boundary &parentPtr = StackVariable::insertBoundary(boundaries, 0, function->address());
        parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;

        StackVariable::Boundary &returnPtr = StackVariable::insertBoundary(boundaries, wordNBytes, function->address());
        returnPtr.purpose = StackVariable::Purpose::RETURN_ADDRESS;

    } else if (isSgAsmPowerpcInstruction(firstInsn)) {
        // For powerpc, the stack is organized like this:
        //                    :                           :
        //                    :   (part of parent frame)  :
        //                    :                           :
        //                (9) | LR saved                  |  4 bytes
        // parent_frame:  (8) | addr of grandparent frame |  4 bytes
        //                    +---(current frame)---------+
        //                (7) | saved FP register area    |  optional, variable size
        //                (6) | saved GP register area    |  optional, multiple of 4 bytes
        //                (5) | CR saved                  |  0 or 4 bytes
        //                (4) | Local variables           |  optional, variable size
        //                (3) | Function parameter area   |  optional, variable size for callee args not fitting in registers
        //                (2) | Padding                   |  0 to 7, although I'm not sure when this is used
        //                (1) | LR saved by callees       |  4 bytes
        // current_frame: (0) | addr of parent frame      |  4 bytes
        //                    +---------------------------+
        //
        // We'd like to define boundaries (offsets) above and below lines (3) and (4) in order to segregate them from each
        // other and from the non-variables around them. However, we don't know where these boundaries are because everything
        // is variable size. The best we can do is insert a boundary above line (1).
        if (frame.size) {
            StackVariable::Boundary &parentPtr = StackVariable::insertBoundary(boundaries, 0, function->address());
            parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;

            StackVariable::Boundary &returnPtr = StackVariable::insertBoundary(boundaries, wordNBytes, function->address());
            returnPtr.purpose = StackVariable::Purpose::RETURN_ADDRESS;

            // Everything else is above this boundary
            StackVariable::insertBoundary(boundaries, 2*wordNBytes, function->address());
        }

    } else if (isSgAsmM68kInstruction(firstInsn)) {
        // Motorola 68000 family of processors
        //
        //                    :                           :
        //                    :   (part of parent frame)  :
        //                    :                           :
        //                    +---(current frame)---------+
        //                (1) | return address            | 4 bytes
        // current_frame: (0) | addr of parent frame      | 4 bytes
        //                    :                           :
        StackVariable::Boundary &parentPtr = StackVariable::insertBoundary(boundaries, 0, function->address());
        parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;

        StackVariable::Boundary &returnPtr = StackVariable::insertBoundary(boundaries, 4, function->address());
        returnPtr.purpose = StackVariable::Purpose::RETURN_ADDRESS;

        if (frame.minOffset && *frame.minOffset < 0) {
            StackVariable::Boundary &bottom = StackVariable::insertBoundary(boundaries, *frame.minOffset, function->address());
            bottom.purpose = StackVariable::Purpose::UNKNOWN;
        }

#ifdef ROSE_ENABLE_ASM_AARCH32
    } else if (isSgAsmAarch32Instruction(firstInsn)) {
        // AArch32 stack frames are organized like this:
        //
        //                    :                           :
        //                    :   (part of parent frame)  :
        //                    :                           :
        //                    +---(current frame)---------+
        // current_frame: (0) | addr of parent frame      | 4 bytes
        //               (-1) | saved link register       | optional, 4 bytes
        //               (-2) | local variables           | variable size
        //                    +---------------------------+
        if (boost::contains(frame.rule, "<saved-lr>")) {
            // The "push fp, lr" pushes 8 bytes calculated from the stack pointer, so we don't want this eight
            // bytes to cross a frame variable boundary.
            StackVariable::Boundary &returnPtr = StackVariable::insertBoundary(boundaries, -4, function->address());
            returnPtr.purpose = StackVariable::Purpose::RETURN_ADDRESS;
        } else {
            StackVariable::Boundary &parentPtr = StackVariable::insertBoundary(boundaries, 0, function->address());
            parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;
        }
#endif
    }
}

#if 0 // [Robb Matzke 2021-10-27]
OffsetInterval
VariableFinder::referencedFrameArea(const Partitioner2::Partitioner &partitioner, const S2::BaseSemantics::RiscOperators::Ptr &ops,
                                    const SymbolicExpression::Ptr &address, size_t nBytes) {
    // Return an empty interval if any information is missing
    ASSERT_not_null(ops);
    ASSERT_not_null(address);
    static const OffsetInterval nothing;
    if (0 == nBytes)
        return nothing;
    const RegisterDescriptor FRAME_PTR = frameOrStackPointer(partitioner);
    if (FRAME_PTR.isEmpty())
        return nothing;

    // Calculate the address as an offset from the frame pointer.
    S2::BaseSemantics::SValue::Ptr framePtrSval = ops->peekRegister(FRAME_PTR, ops->undefined_(FRAME_PTR.nBits()));
    SymbolicExpression::Ptr framePtr = S2::SymbolicSemantics::SValue::promote(framePtrSval)->get_expression();
    SymbolicExpression::Ptr diff = SymbolicExpression::makeAdd(SymbolicExpression::makeNegate(framePtr), address);

    // The returned interval is in terms of the frame pointer offset and the size of the I/O operation.
    Variables::OffsetInterval where;
    if (Sawyer::Optional<int64_t> offset = diff->toSigned()) {
        where = Variables::OffsetInterval::baseSize(*offset, nBytes);
    } else if (diff->getOperator() == SymbolicExpression::OP_ADD) {
        for (SymbolicExpression::Ptr child: diff->children()) {
            if ((offset = child->toSigned())) {
                where = Variables::OffsetInterval::baseSize(*offset, nBytes);
                break;
            }
        }
    }
    return where;
}
#endif

std::set<int64_t>
VariableFinder::findFrameOffsets(const StackFrame &frame, const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) {
    ASSERT_not_null(partitioner);

#ifdef ROSE_ENABLE_ASM_AARCH32
    // Look for ARM AArch32 "sub DEST_REG, fp, N" where DEST_REG is not the stack pointer register
    const RegisterDescriptor REG_SP = partitioner->instructionProvider().stackPointerRegister();
    if (isSgAsmAarch32Instruction(insn) &&
        isSgAsmAarch32Instruction(insn)->get_kind() == Aarch32InstructionKind::ARM_INS_SUB &&
        insn->nOperands() == 3 &&
        isSgAsmDirectRegisterExpression(insn->operand(0)) &&
        isSgAsmDirectRegisterExpression(insn->operand(0))->get_descriptor() != REG_SP &&
        isSgAsmDirectRegisterExpression(insn->operand(1)) &&
        isSgAsmDirectRegisterExpression(insn->operand(1))->get_descriptor() == frame.framePointerRegister &&
        isSgAsmIntegerValueExpression(insn->operand(2))) {
        std::set<int64_t> offsets;
        offsets.insert(-isSgAsmIntegerValueExpression(insn->operand(2))->get_signedValue());
        return offsets;
    }
#endif

    std::set<int64_t> offsets;
    AST::Traversal::forwardPre<SgAsmBinaryAdd>(insn, [&frame, &offsets](SgAsmBinaryAdd *add) {
        // Look for (add reg ival)
        SgAsmDirectRegisterExpression *reg = isSgAsmDirectRegisterExpression(add->get_lhs());
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(add->get_rhs());
        if (reg && ival &&
            (reg->get_descriptor() == frame.framePointerRegister || frame.framePointerRegister.isEmpty()) &&
            AST::Traversal::findParentTyped<SgAsmMemoryReferenceExpression>(add)) {
            offsets.insert(ival->get_signedValue());
            return;
        }

        // Look for (add (add2 reg (mult reg2 ival2)) ival)
        SgAsmBinaryAdd *add2 = isSgAsmBinaryAdd(add->get_lhs());
        reg = add2 ? isSgAsmDirectRegisterExpression(add2->get_lhs()) : NULL;
        SgAsmBinaryMultiply *mult = add2 ? isSgAsmBinaryMultiply(add2->get_rhs()) : NULL;
        SgAsmDirectRegisterExpression *reg2 = mult ? isSgAsmDirectRegisterExpression(mult->get_lhs()) : NULL;
        SgAsmIntegerValueExpression *ival2 = mult ? isSgAsmIntegerValueExpression(mult->get_rhs()) : NULL;
        if (reg && ival && reg2 && ival2 &&
            (reg->get_descriptor() == frame.framePointerRegister || frame.framePointerRegister.isEmpty()) &&
            AST::Traversal::findParentTyped<SgAsmMemoryReferenceExpression>(add)) {
            offsets.insert(ival->get_signedValue());
            return;
        }
    });
    return offsets;
}

static bool
isSortedByOffset(const StackVariable::Boundary &a, const StackVariable::Boundary &b) {
    return a.frameOffset < b.frameOffset;
}

void
VariableFinder::removeOutliers(const StackFrame &frame, const P2::Partitioner::ConstPtr&, const P2::Function::Ptr&,
                               StackVariable::Boundaries &boundaries /*in,out,sorted*/) {

#ifndef NDEBUG
    for (size_t i = 1; i < boundaries.size(); ++i)
        ASSERT_require(isSortedByOffset(boundaries[i-1], boundaries[i]));
#endif

    if (frame.minOffset) {
        // Remove all boundaries that are below the bottom of the frame, but don't yet remove the greatest one that's below the
        // frame.
        size_t i = 0;
        while (i+1 < boundaries.size() && boundaries[i+1].frameOffset < *frame.minOffset)
            ++i;
        boundaries.erase(boundaries.begin(), boundaries.begin() + i);

        // Adjust the lowest bounded area so it starts at the start of the frame unless there's already a boundary there (in
        // which case, remove it instead of moving it).
        if (!boundaries.empty() && boundaries[0].frameOffset < *frame.minOffset) {
            if (boundaries.size() == 1 || boundaries[1].frameOffset > *frame.minOffset) {
                boundaries[0].frameOffset = *frame.minOffset;
            } else {
                boundaries.erase(boundaries.begin());
            }
        }
    }

    if (frame.maxOffset) {
        // Remove all boundaries that start above the top of the frame.
        while (!boundaries.empty() && boundaries.back().frameOffset > *frame.maxOffset)
            boundaries.pop_back();
    }
}

StackVariables
VariableFinder::findStackVariables(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    StackVariable::Boundaries boundaries;

    // Return cached local variable information
    if (function->attributeExists(ATTR_LOCAL_VARS))
        return function->getAttribute<StackVariables>(ATTR_LOCAL_VARS);

    // Get basic information about the frame and check/print it.
    SAWYER_MESG(debug) <<"searching for local vars in " <<function->printableName() <<"\n";
    const StackFrame frame = detectFrameAttributes(partitioner, function);
    SAWYER_MESG(debug) <<"  stack grows " <<(StackFrame::GROWS_DOWN == frame.growthDirection ? "down" : "up") <<"\n";
    if (frame.minOffset) {
        SAWYER_MESG(debug) <<"  stack frame minimum offset is " <<*frame.minOffset <<"\n";
        ASSERT_require2(*frame.minOffset <= 0, "frame pointer must point inside frame");
    }
    if (frame.maxOffset) {
        SAWYER_MESG(debug) <<"  stack frame maximum offset is " <<*frame.maxOffset <<"\n";
        ASSERT_require2(*frame.maxOffset >= 0, "frame pointer must point inside frame");
    }
    if (frame.size) {
        SAWYER_MESG(debug) <<"  stack frame size is " <<StringUtility::plural(*frame.size, "bytes") <<"\n";
    } else {
        SAWYER_MESG(debug) <<"  stack frame size is unknown\n";
    }
    if (frame.minOffset && frame.maxOffset) {
        ASSERT_require(*frame.minOffset <= *frame.maxOffset);
        if (frame.size)
            ASSERT_require((long)*frame.size == (*frame.maxOffset - *frame.minOffset) + 1);
    }
    if (!frame.rule.empty())
        SAWYER_MESG(debug) <<"  stack frame rule is \"" <<StringUtility::cEscape(frame.rule) <<"\"\n";

    // Sometimes the calling convention tells us what to expect on the frame.
    initializeFrameBoundaries(frame, partitioner, function, boundaries /*in,out*/);

    // Look for stack offsets syntactically by looking for instructions that access memory w.r.t. the frame pointer.
    SAWYER_MESG(debug) <<"  searching for local variables syntactically...\n";
    for (rose_addr_t bblockVa: function->basicBlockAddresses()) {
        P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bblockVa);
        ASSERT_not_null(bb);
        for (SgAsmInstruction *insn: bb->instructions()) {
            std::set<int64_t> offsets = findFrameOffsets(frame, partitioner, insn);
            for (int64_t offset: offsets) {
                SAWYER_MESG(debug) <<"    found offset " <<offsetStr(offset) <<" at " <<insn->toString() <<"\n";
                StackVariable::insertBoundary(boundaries, offset, insn->get_address());
            }
        }
    }

    // Look for stack offsets semantically. We could do a complete data-flow analysis, but it turns out that processing each
    // basic block independently is usually just as good, and much faster.
    SAWYER_MESG(debug) <<"  searching for local variables semantically...\n";
    for (rose_addr_t bblockVa: function->basicBlockAddresses()) {
        P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bblockVa);
        ASSERT_not_null(bb);
        VarSearchSemantics::RiscOperators::Ptr ops =
            VarSearchSemantics::RiscOperators::instance(frame, partitioner, boundaries /*in,out*/);
        S2::BaseSemantics::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
        for (SgAsmInstruction *insn: bb->instructions()) {
            try {
                cpu->processInstruction(insn);
            } catch (const S2::BaseSemantics::Exception &e) {
                debug <<"    semantic failure for " <<insn->toString() <<": " <<e.what() <<"\n";
                break;
            } catch (...) {
                debug <<"    semantic failure for " <<insn->toString() <<"\n";
            }
        }
    }

    // Sort and prune the boundaries so we have just those that are in the frame.
    std::sort(boundaries.begin(), boundaries.end(), isSortedByOffset);
    removeOutliers(frame, partitioner, function, boundaries);
    if (debug) {
        debug <<"  final stack boundaries:\n";
        for (const StackVariable::Boundary &boundary: boundaries) {
            debug <<"    fp" <<offsetStr(boundary.frameOffset) <<" "
                  <<stringify::Rose::BinaryAnalysis::Variables::StackVariable::Purpose((int64_t)boundary.purpose) <<"\n";
        }
    }

    // Now that we know the boundaries between parts of the frame, create the variables that live between those boundaries.
    StackVariables lvars;
    for (size_t i = 0; i < boundaries.size(); ++i) {
        const StackVariable::Boundary &boundary = boundaries[i];

        // We know the low offset of this variable, but what is the high offset.
        int64_t maxOffset = boundary.frameOffset;
        if (i + 1 < boundaries.size()) {
            maxOffset = boundaries[i+1].frameOffset - 1;
        } else if (frame.maxOffset) {
            maxOffset = *frame.maxOffset;
        } else {
            maxOffset = (int64_t)BitOps::lowMask<uint64_t>(8*partitioner->instructionProvider().wordSize() - 1);
        }

        // Create the variable
        ASSERT_require2(maxOffset >= boundary.frameOffset,
                        "maxOffset=" + boost::lexical_cast<std::string>(maxOffset) +
                        ", boundary.frameOffset=" + boost::lexical_cast<std::string>(boundary.frameOffset));
        const OffsetInterval where = OffsetInterval::hull(boundary.frameOffset, maxOffset);
        rose_addr_t varMaxSize = (uint64_t)maxOffset + 1u - (uint64_t)boundary.frameOffset;
        StackVariable lvar(function, boundary.frameOffset, varMaxSize, boundary.purpose, boundary.definingInsns);
        lvar.setDefaultName();
        lvars.insert(where, lvar);
    }

    if (debug) {
        debug <<"  local variables for " <<function->printableName() <<":\n";
        print(lvars, partitioner, debug, "    ");
    }
    function->setAttribute(ATTR_LOCAL_VARS, lvars);
    return lvars;
}

#if 0 // [Robb Matzke 2021-10-27]
S2::BaseSemantics::SValue::Ptr
VariableFinder::symbolicAddress(const P2::Partitioner &partitioner, const StackVariable &var,
                                const S2::BaseSemantics::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    RegisterDescriptor baseReg = frameOrStackPointer(partitioner);
    S2::BaseSemantics::SValue::Ptr base = ops->peekRegister(baseReg, ops->undefined_(baseReg.nBits()));
    ASSERT_require(sizeof(var.frameOffset()) == sizeof(uint64_t));
    S2::BaseSemantics::SValue::Ptr offset = ops->number_(baseReg.nBits(), var.frameOffset());
    return ops->add(base, offset);
}
#endif

P2::Function::Ptr
VariableFinder::functionForInstruction(const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(insn);
    P2::BasicBlock::Ptr bb = partitioner->basicBlockContainingInstruction(insn->get_address());
    if (!bb)
        return P2::Function::Ptr();
    std::vector<P2::Function::Ptr> functions = partitioner->functionsOwningBasicBlock(bb);
    if (functions.empty())
        return P2::Function::Ptr();
    return functions[0];                                // arbitrarily choose the first one
}

StackVariables
VariableFinder::findStackVariables(const P2::Partitioner::ConstPtr &partitioner, SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    if (P2::Function::Ptr function = functionForInstruction(partitioner, insn)) {
        return findStackVariables(partitioner, function);
    } else {
        return StackVariables();
    }
}

void
VariableFinder::evict(const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    function->eraseAttribute(ATTR_FRAME_SIZE);
    function->eraseAttribute(ATTR_LOCAL_VARS);
}

void
VariableFinder::evict(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    for (const P2::Function::Ptr &function: partitioner->functions())
        evict(function);
}

bool
VariableFinder::isCached(const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    return function->attributeExists(ATTR_LOCAL_VARS);
}

std::set<SymbolicExpression::Ptr>
VariableFinder::getMemoryAddresses(const S2::BaseSemantics::MemoryCellState::Ptr &mem) {
    struct: S2::BaseSemantics::MemoryCell::Visitor {
        std::set<SymbolicExpression::Ptr> addresses;

        void operator()(S2::BaseSemantics::MemoryCell::Ptr &cell) {
            SymbolicExpression::Ptr addr = S2::SymbolicSemantics::SValue::promote(cell->address())->get_expression();
            addresses.insert(addr);
        }
    } visitor;

    ASSERT_not_null(mem);
    mem->traverse(visitor);
    return visitor.addresses;
}

std::set<rose_addr_t>
VariableFinder::findConstants(const SymbolicExpression::Ptr &expr) {
    struct: SymbolicExpression::Visitor {
        std::set<rose_addr_t> constants;
        std::vector<SymbolicExpression::Ptr> path;

        SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
            const SymbolicExpression::Interior *parent = nullptr;
            if (!path.empty()) {
                parent = path.back()->isInteriorNodeRaw();
                ASSERT_not_null(parent);
            }
            path.push_back(node);

            // Some constants are never addresses
            if (parent && parent->getOperator() == SymbolicExpression::OP_EXTRACT &&
                (parent->child(0) == node || parent->child(1) == node))
                return SymbolicExpression::CONTINUE;          // first two args of extract are never addresses

            // If we found a constant, perhaps treat it like an address
            if (Sawyer::Optional<uint64_t> n = node->toUnsigned())
                constants.insert(*n);

            return SymbolicExpression::CONTINUE;
        }

        SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
            ASSERT_forbid(path.empty());
            path.pop_back();
            return SymbolicExpression::CONTINUE;
        }
    } visitor;

    ASSERT_not_null(expr);
    expr->depthFirstTraversal(visitor);
    return visitor.constants;
}

std::set<rose_addr_t>
VariableFinder::findConstants(SgAsmInstruction *insn) {
    std::set<rose_addr_t> constants;
    AST::Traversal::forwardPre<SgAsmIntegerValueExpression>(insn, [&constants](SgAsmIntegerValueExpression *ival) {
        constants.insert(ival->get_absoluteValue());
    });
    return constants;
}

std::set<rose_addr_t>
VariableFinder::findAddressConstants(const S2::BaseSemantics::MemoryCellState::Ptr &mem) {
    std::set<SymbolicExpression::Ptr> addresses = getMemoryAddresses(mem);
    std::set<rose_addr_t> retval;
    for (const SymbolicExpression::Ptr &address: addresses) {
        std::set<rose_addr_t> constants = findConstants(address);
        retval.insert(constants.begin(), constants.end());
    }
    return retval;
}

class FollowNamedEdges: public P2::DataFlow::InterproceduralPredicate {
    std::regex functionNameRe;

public:
    explicit FollowNamedEdges(const std::string &re)
        : functionNameRe(re) {}

public:
    virtual bool operator()(const P2::ControlFlowGraph&, const P2::ControlFlowGraph::ConstEdgeIterator &edge, size_t /*depth*/) {
        const P2::FunctionSet &functions = edge->target()->value().owningFunctions();
        for (const P2::Function::Ptr &function: functions.values()) {
            const std::string name = function->name();
            if (std::regex_match(name, functionNameRe))
                return true;
        }
        return false;
    }
};

bool
VariableFinder::regionContainsInstructions(const P2::Partitioner::ConstPtr &partitioner, const AddressInterval &region) {
    ASSERT_not_null(partitioner);
    return !partitioner->instructionsOverlapping(region).empty();
}

bool
VariableFinder::regionIsFullyMapped(const P2::Partitioner::ConstPtr &partitioner, const AddressInterval &region) {
    ASSERT_not_null(partitioner);
    if (MemoryMap::Ptr map = partitioner->memoryMap()) {
        return !map->findFreeSpace(1, 1, region);
    } else {
        return false;
    }
}

bool
VariableFinder::regionIsFullyReadWrite(const P2::Partitioner::ConstPtr &partitioner, const AddressInterval &region) {
    ASSERT_not_null(partitioner);
    if (MemoryMap::Ptr map = partitioner->memoryMap()) {
        AddressInterval where = map->at(region).require(MemoryMap::READ_WRITE).available();
        return (where & region) == region;
    } else {
        return false;
    }
}

// For x86 ELF executables, global variables live in a particular section, such as .bss. The compiler typically generates code that
// computes the global variable address as an offset from the function's own address. It does this by calling __x86.get_pc_thunk.ax
// (or similar) which returns the address of the instruction following the call, and then adding a constant to the returned
// value. This computes the starting address of the section (base address), to which additional offsets are added to get to the
// various global variables.  Since the base address is computed in one basic block and used in subsequent basic blocks, we use a
// data flow analysis to propagate the base address to the other blocks.
struct FindGlobalVariableVasMethod1Worker {
    P2::Partitioner::ConstPtr partitioner;
    Sawyer::ProgressBar<size_t> &progress;
    VariableFinder &self;

    // The following data members are synchronized with this mutex
    SAWYER_THREAD_TRAITS::Mutex mutex_;
    AddressToAddresses result;

    explicit FindGlobalVariableVasMethod1Worker(const P2::Partitioner::ConstPtr &partitioner, Sawyer::ProgressBar<size_t> &progress,
                                                VariableFinder &self)
        : partitioner(partitioner), progress(progress), self(self) {
        ASSERT_not_null(partitioner);
    }

    void operator()(size_t /*workId*/, const P2::Function::Ptr &function) {
        ++progress;
        using Df = P2::DataFlow::Engine;
        using Cfg = Df::Cfg;

        Sawyer::Message::Stream debug(mlog[DEBUG]);
        SAWYER_MESG(debug) <<"  analyzing " <<function->printableName() <<"\n";

        // Create the data-flow CFG
        P2::ControlFlowGraph::ConstVertexIterator cfgRoot = partitioner->findPlaceholder(function->address());
        ASSERT_require(cfgRoot != partitioner->cfg().vertices().end());
        FollowNamedEdges predicate(".+\\.get_pc_thunk\\..+");
        Cfg cfg = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), cfgRoot, predicate);

        // Create the data-flow transfer and merge functors
        BS::Dispatcher::Ptr cpu = partitioner->newDispatcher(partitioner->newOperators());
        Df::TransferFunction xfer(cpu);
        xfer.ignoringSemanticFailures(true);
        Df::MergeFunction merger(cpu);

        // Create the initial state, and cause it to use a simple merge function that doesn't account for aliasing
        BS::State::Ptr initialState = xfer.initialState();
        initialState->memoryState()->merger(BS::Merger::instance());
        initialState->memoryState()->merger()->memoryAddressesMayAlias(false);

        // Run the dataflow with a low number of iterations
        Df df(cfg, xfer, merger);
        df.insertStartingVertex(0, initialState);

        Sawyer::Stopwatch timer;
        const size_t maxSteps = cfg.nVertices() * 3 + 1; // arbitrary, but positive
        for (size_t i = 0; i < maxSteps; ++i) {
            if (!df.runOneIteration())
                break;
            if (timer.report() > self.settings().gvarMethod1MaxTimePerFunction.count()) {
                mlog[WARN] <<"max time exceeded for gvar method 1 for " <<function->printableName()
                           <<(boost::format(" (%d of %d steps; %1.0f%%)") % i % maxSteps % (100.0 * i / maxSteps)) <<"\n";
                break;
            }
        }

        // Find all constants that appear in memory address expressions and organize them according to what instruction(s) wrote to
        // that address. We could organize by memory reads also, but alas, the memory state does not track readers--only writers.
        using Map = Sawyer::Container::IntervalSetMap<AddressInterval, Sawyer::Container::Set<rose_addr_t>>;
        struct Accumulator: BS::MemoryCell::Visitor {
            VariableFinder &variableFinder;
            Sawyer::Message::Stream &debug;
            Map map;
            Accumulator(VariableFinder &variableFinder, Sawyer::Message::Stream &debug)
                : variableFinder(variableFinder), debug(debug) {}
            void operator()(BS::MemoryCell::Ptr &cell) override {
                SymbolicExpression::Ptr address = S2::SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                if (debug) {
                    debug <<"      memory address: " <<*address <<"\n"
                          <<"        writers = {";
                    for (rose_addr_t va: cell->getWriters().values())
                        debug <<" " <<StringUtility::addrToString(va);
                    debug <<" }\n";
                }
                std::set<rose_addr_t> constants = variableFinder.findConstants(address);
                for (rose_addr_t c: constants) {
                    SAWYER_MESG(debug) <<"        constant = " <<StringUtility::addrToString(c) <<"\n";
                    map.insert(c, cell->getWriters());
                }
            }
        };

        Accumulator accumulator(self, debug);
        for (size_t i = 0; i < cfg.nVertices(); ++i) {
            SAWYER_MESG(debug) <<"    final state for vertex " <<i <<": " <<cfg.findVertex(i)->value().toString() <<"\n";
            if (auto state = df.getFinalState(i)) {
                SAWYER_MESG(debug) <<(*state+"      ") <<"\n";
                auto mem = BS::MemoryCellState::promote(state->memoryState());
                mem->traverse(accumulator);
            }
        }

        // Compute the result for this single function
        AddressToAddresses functionResult;
        SAWYER_MESG(debug) <<"  summary for " <<function->printableName() <<":\n";
        for (const auto &node: accumulator.map.nodes()) {
            if (debug) {
                debug <<"    variable va: " <<StringUtility::addrToString(node.key()) <<"; insns:";
                for (rose_addr_t va: node.value().values())
                    debug <<" " <<StringUtility::addrToString(va);
                debug <<"\n";
            }

            if (self.regionContainsInstructions(partitioner, node.key())) {
                SAWYER_MESG(debug) <<"      skipped because region overlaps with instructions\n";
                continue;
            }
            if (!self.regionIsFullyMapped(partitioner, node.key())) {
                SAWYER_MESG(debug) <<"      skipped because sub-region is not fully mapped\n";
                continue;
            }
            if (!self.regionIsFullyReadWrite(partitioner, node.key())) {
                SAWYER_MESG(debug) <<"      skipped because sub-region is not fully read+write\n";
                continue;
            }

            functionResult[node.key().least()].insert(node.value());
        }

        // Merge this function's result into the global result
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        for (const auto &node: functionResult)
            result[node.first] |= node.second;
    }
};

AddressToAddresses
VariableFinder::findGlobalVariableVasMethod1(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    AddressToAddresses retval;
    SAWYER_MESG(mlog[DEBUG]) <<"looking for global variable addresses (method 1: data-flow)\n";

    // It doesn't matter what order we process these functions because the worker is intraprocedural. Therefore, we don't need to
    // add any edges to this graph. However, we'll sort the vertices from small to large, and since the workInParallel runs them
    // in the opposite order, this results in the best overall parallel performance.
    Sawyer::Container::Graph<P2::Function::Ptr> g;
    std::vector<P2::Function::Ptr> functions = partitioner->functions();
    std::sort(functions.begin(), functions.end(), [](const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->nBasicBlocks() < b->nBasicBlocks();
    });
    for (const P2::Function::Ptr &function: functions)
        g.insertVertex(function);

    // Process each function and accumulate the results in the worker.result data member
    Sawyer::ProgressBar<size_t> progress(g.nVertices(), mlog[MARCH], "gvars method 1");
    progress.suffix(" functions");
    const size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    FindGlobalVariableVasMethod1Worker worker(partitioner, progress, *this);
    Sawyer::workInParallel(g, nThreads, std::ref(worker));
    return worker.result;
}

struct FindGlobalVariableVasMethod2Worker {
    P2::Partitioner::ConstPtr partitioner;
    Sawyer::ProgressBar<size_t> &progress;
    VariableFinder &self;

    // The following data members are synchronized with this mutex
    SAWYER_THREAD_TRAITS::Mutex mutex_;
    AddressToAddresses result;

    FindGlobalVariableVasMethod2Worker(const P2::Partitioner::ConstPtr &partitioner, Sawyer::ProgressBar<size_t> &progress,
                                       VariableFinder &self)
        : partitioner(partitioner), progress(progress), self(self) {}

    void operator()(size_t /*workId*/, const P2::BasicBlock::Ptr &bb) {
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        ++progress;
        AddressToAddresses partialResult;

        S2::SymbolicSemantics::RiscOperators::Ptr ops =
            S2::SymbolicSemantics::RiscOperators::instanceFromRegisters(partitioner->instructionProvider().registerDictionary());
        S2::BaseSemantics::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
        ASSERT_not_null(cpu);

        for (SgAsmInstruction *insn: bb->instructions()) {
            SAWYER_MESG(debug) <<"  " <<insn->toString() <<"\n";
            ops->currentState()->clear();
            try {
                cpu->processInstruction(insn);
            } catch (...) {
                SAWYER_MESG(mlog[WARN]) <<"semantics failed for " <<insn->toString() <<"\n";
            }

            // Find all constants that appear in memory address expressions.
            S2::BaseSemantics::MemoryCellState::Ptr mem =
                S2::BaseSemantics::MemoryCellState::promote(cpu->currentState()->memoryState());
            std::set<rose_addr_t> constants = self.findAddressConstants(mem);

            // Compute the contiguous regions formed by those constants. E.g., an instruction that reads four bytes of memory
            // might have four consecutive constants.
            AddressIntervalSet regions;
            for (rose_addr_t c: constants) {
                if (self.regionContainsInstructions(partitioner, c))
                    continue;                           // not a variable pointer if it points to an instruction
                if (!self.regionIsFullyMapped(partitioner, c))
                    continue;                           // global variables always have storage
                if (!self.regionIsFullyReadWrite(partitioner, c))
                    continue;
                regions.insert(c);
            }

            // Save only the lowest constant in each contiguous region.
            for (const AddressInterval &interval: regions.intervals()) {
                SAWYER_MESG(debug) <<"      " <<StringUtility::addrToString(interval.least()) <<"\n";
                partialResult[interval.least()].insert(insn->get_address());
            }
        }

        // Save the partial results in the worker
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        for (const auto &node: partialResult)
            result[node.first] |= node.second;
    }
};

AddressToAddresses
VariableFinder::findGlobalVariableVasMethod2(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    AddressToAddresses retval;
    SAWYER_MESG(mlog[DEBUG]) <<"looking for global variable addresses (method 2: insn memory address constants)\n";

    // Get all basic blocks and sort them by increasing number of instructions. This results in best parallelism since
    // workInParallel will run them in the opposite order.
    std::vector<P2::BasicBlock::Ptr> bblocks = partitioner->basicBlocks();
    std::sort(bblocks.begin(), bblocks.end(), [](const P2::BasicBlock::Ptr &a, const P2::BasicBlock::Ptr &b) {
        return a->nInstructions() < b->nInstructions();
    });

    // Basic blocks can be processed in any order, so create a work graph with no edges.
    Sawyer::Container::Graph<P2::BasicBlock::Ptr> g;
    for (const P2::BasicBlock::Ptr &b: bblocks)
        g.insertVertex(b);

    // Analyze basic blocks in parallel
    Sawyer::ProgressBar<size_t> progress(partitioner->cfg().nVertices(), mlog[MARCH], "gvars method 3");
    progress.suffix(" bblocks");
    FindGlobalVariableVasMethod2Worker worker(partitioner, progress, *this);
    const size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    Sawyer::workInParallel(g, nThreads, std::ref(worker));
    return worker.result;
}

struct FindGlobalVariableVasMethod3Worker {
    P2::Partitioner::ConstPtr partitioner;
    Sawyer::ProgressBar<size_t> &progress;
    VariableFinder &self;

    // The following data members are synchronized with this mutex
    SAWYER_THREAD_TRAITS::Mutex mutex_;
    AddressToAddresses result;

    FindGlobalVariableVasMethod3Worker(const P2::Partitioner::ConstPtr &partitioner, Sawyer::ProgressBar<size_t> &progress,
                                       VariableFinder &self)
        : partitioner(partitioner), progress(progress), self(self) {}

    void operator()(size_t /*workId*/, const P2::BasicBlock::Ptr &bb) {
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        ++progress;
        AddressToAddresses partialResult;

        for (SgAsmInstruction *insn: bb->instructions()) {
            SAWYER_MESG(debug) <<"  " <<insn->toString() <<"\n";

            // Find constants in the instruction AST
            std::set<rose_addr_t> constants = self.findConstants(insn);
            AddressIntervalSet regions;
            for (rose_addr_t c: constants) {
                if (self.regionContainsInstructions(partitioner, c))
                    continue;                           // not a variable pointer if it points to an instruction
                if (!self.regionIsFullyMapped(partitioner, c))
                    continue;                           // global variables always have storage
                if (!self.regionIsFullyReadWrite(partitioner, c))
                    continue;
                regions.insert(c);
            }

            // Save only the lowest constant in each contiguous region.
            for (const AddressInterval &interval: regions.intervals()) {
                SAWYER_MESG(debug) <<"      " <<StringUtility::addrToString(interval.least()) <<"\n";
                partialResult[interval.least()].insert(insn->get_address());
            }
        }

        // Save the partial results in the worker
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        for (const auto &node: partialResult)
            result[node.first] |= node.second;
    }
};

AddressToAddresses
VariableFinder::findGlobalVariableVasMethod3(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    AddressToAddresses retval;
    SAWYER_MESG(mlog[DEBUG]) <<"looking for global variable addresses (method 2: insn immediate values)\n";

    // Get all basic blocks and sort them by increasing number of instructions. This results in best parallelism since the
    // workInParallel will run them in the opposite order.
    std::vector<P2::BasicBlock::Ptr> bblocks = partitioner->basicBlocks();
    std::sort(bblocks.begin(), bblocks.end(), [](const P2::BasicBlock::Ptr &a, const P2::BasicBlock::Ptr &b) {
        return a->nInstructions() < b->nInstructions();
    });

    // Basic blocks can be processed in any order, so create a work graph with no edges.
    Sawyer::Container::Graph<P2::BasicBlock::Ptr> g;
    for (const P2::BasicBlock::Ptr &b: bblocks)
        g.insertVertex(b);

    // Analyze basic blocks in parallel
    Sawyer::ProgressBar<size_t> progress(partitioner->cfg().nVertices(), mlog[MARCH], "gvars method 3");
    progress.suffix(" bblocks");
    FindGlobalVariableVasMethod3Worker worker(partitioner, progress, *this);
    const size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    Sawyer::workInParallel(g, nThreads, std::ref(worker));
    return worker.result;
}

void
VariableFinder::merge(AddressToAddresses &result, const AddressToAddresses &src) {
    for (const auto &node: src)
        result[node.first].insert(node.second);
}

AddressToAddresses
VariableFinder::findGlobalVariableVas(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"Finding global variable addresses";
    Sawyer::Stopwatch timer;

    AddressToAddresses retval;
    merge(retval, findGlobalVariableVasMethod1(partitioner));
    merge(retval, findGlobalVariableVasMethod2(partitioner));
    merge(retval, findGlobalVariableVasMethod3(partitioner));

    info <<"; took " <<timer <<"\n";
    return retval;
}

GlobalVariables
VariableFinder::findGlobalVariables(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);

    // Return cached global variable information
    if (partitioner->attributeExists(ATTR_GLOBAL_VARS))
        return partitioner->getAttribute<GlobalVariables>(ATTR_GLOBAL_VARS);

    // First, find all the global variables assume they're as large as possible but no global variables overlap.
    typedef Sawyer::Container::IntervalMap<AddressInterval /*occupiedVas*/, rose_addr_t /*startingVa*/> GVars;
    GVars gvars;
    AddressToAddresses globalVariableVas = findGlobalVariableVas(partitioner);
    static size_t wordSize = partitioner->instructionProvider().stackPointerRegister().nBits();
    for (AddressToAddresses::const_iterator iter = globalVariableVas.begin(); iter != globalVariableVas.end(); ++iter) {

        // This variable cannot overlap with the next variable
        AddressToAddresses::const_iterator next = iter; ++next;
        rose_addr_t lastVa = next == globalVariableVas.end() ? IntegerOps::genMask<rose_addr_t>(wordSize) : next->first - 1;

        // The variable will not extend beyond one segment of the memory map
        MemoryMap::ConstNodeIterator node = partitioner->memoryMap()->at(iter->first).findNode();
        ASSERT_require2(node != partitioner->memoryMap()->nodes().end(), StringUtility::addrToString(iter->first));
        lastVa = std::min(lastVa, node->key().greatest());

        gvars.insert(AddressInterval::hull(iter->first, lastVa), iter->first);
    }

    // Then remove code areas from the global variable location map
    for (const P2::ControlFlowGraph::Vertex &vertex: partitioner->cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            for (SgAsmInstruction *insn: vertex.value().bblock()->instructions())
                gvars.erase(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
        }
    }

    // Finally build the return value
    GlobalVariables retval;
    for (const GVars::Node &node: gvars.nodes()) {
        if (node.key().least() == node.value())
            retval.insert(node.key(), GlobalVariable(node.key().least(), node.key().size(), globalVariableVas[node.value()]));
    }

    const_cast<P2::Partitioner*>(partitioner.getRawPointer())->setAttribute(ATTR_GLOBAL_VARS, retval);
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
