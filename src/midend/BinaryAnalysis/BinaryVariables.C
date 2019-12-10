#include <sage3basic.h>
#include <BinaryVariables.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <Partitioner2/Function.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/Attribute.h>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace S2 = Rose::BinaryAnalysis::InstructionSemantics2;

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
    std::string sign;
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
        return "0x" + h + "<" + sign + boost::lexical_cast<std::string>(n) + ">";
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
print(const StackVariables &lvars, const P2::Partitioner &partitioner,
      std::ostream &out, const std::string &prefix) {
    BOOST_FOREACH (const StackVariable &lvar, lvars.values()) {
        out <<prefix <<lvar <<"\n";
        BOOST_FOREACH (rose_addr_t va, lvar.definingInstructionVas().values())
            out <<prefix <<"  at " <<partitioner.instructionProvider()[va]->toString() <<"\n";
    }
}

void
print(const GlobalVariables &gvars, const P2::Partitioner &partitioner,
      std::ostream &out, const std::string &prefix) {
    BOOST_FOREACH (const GlobalVariable &gvar, gvars.values()) {
        out <<prefix <<gvar <<"\n";
        BOOST_FOREACH (rose_addr_t va, gvar.definingInstructionVas().values())
            out <<prefix <<"  detected at " <<partitioner.instructionProvider()[va]->toString() <<"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseVariable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
BaseVariable::maxSizeBytes(rose_addr_t size) {
    ASSERT_require(size > 0);
    maxSizeBytes_ = size;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StackVariable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StackVariable::StackVariable()
    : frameOffset_(0) {}

StackVariable::StackVariable(const P2::FunctionPtr &function, int64_t frameOffset, rose_addr_t maxSizeBytes,
                             const AddressSet &definingInstructionVas, const std::string &name)
    : BaseVariable(maxSizeBytes, definingInstructionVas, name), function_(function), frameOffset_(frameOffset) {}

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

    if (frameOffset() < 0)
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
    return OffsetInterval::baseSize(frameOffset(), maxSizeBytes());
}

void
StackVariable::print(std::ostream &out) const {
    out <<"local-variable";
    if (!name().empty())
        out <<" \"" <<StringUtility::cEscape(name()) <<"\"";
    out <<" @" <<offsetStr(frameOffset()) <<"+" <<sizeStr(maxSizeBytes());
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

GlobalVariable::GlobalVariable()
    : address_(0) {}

GlobalVariable::GlobalVariable(rose_addr_t startingAddress, rose_addr_t maxSizeBytes,
                               const AddressSet &definingInstructionVas, const std::string &name)
    : BaseVariable(maxSizeBytes, definingInstructionVas, name), address_(startingAddress) {}

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
    out <<"@" <<StringUtility::addrToString(address()) <<"+" <<sizeStr(maxSizeBytes());
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantics for finding local variables. This works by intercepting each memory I/O and examining the address to see whether
// it's an offset from the frame pointer.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace VarSearchSemantics {

typedef P2::Semantics::SValue SValue;
typedef P2::Semantics::SValuePtr SValuePtr;
typedef P2::Semantics::RegisterState RegisterState;
typedef P2::Semantics::RegisterStatePtr RegisterStatePtr;
typedef P2::Semantics::MemoryMapState MemoryState;
typedef P2::Semantics::MemoryMapStatePtr MemoryStatePtr;
typedef P2::Semantics::State State;
typedef P2::Semantics::StatePtr StatePtr;

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators: public P2::Semantics::RiscOperators {
    SymbolicExpr::Ptr base_;                            // value to subtract from each address to find stack variable offsets
    OffsetToAddresses &offsets_;                        // the offsets we found and the instructions where they happen

public:
    typedef P2::Semantics::RiscOperators Super;

protected:
    explicit RiscOperators(const S2::BaseSemantics::SValuePtr &protoval, OffsetToAddresses &offsets /*in,out*/)
        : Super(protoval, SmtSolverPtr()), offsets_(offsets) {
        name("VarSearchSemantics");
        (void) SValue::promote(protoval);
    }

    explicit RiscOperators(const S2::BaseSemantics::StatePtr &state, OffsetToAddresses &offsets /*in,out*/)
        : Super(state, SmtSolverPtr()), offsets_(offsets) {
        name("VarSearchSemantics");
        (void) SValue::promote(state->protoval());
    }

public:
    static RiscOperatorsPtr instance(const P2::Partitioner &partitioner, RegisterDescriptor frameRegister,
                                     OffsetToAddresses &offsets /*in,out*/) {
        const RegisterDictionary *regdict = partitioner.instructionProvider().registerDictionary();
        S2::BaseSemantics::SValuePtr protoval = SValue::instance();
        S2::BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        S2::BaseSemantics::MemoryStatePtr memory = P2::Semantics::MemoryMapState::instance(protoval, protoval);
        S2::BaseSemantics::StatePtr state = State::instance(registers, memory);
        RiscOperatorsPtr retval = RiscOperatorsPtr(new RiscOperators(state, offsets));

        S2::BaseSemantics::SValuePtr dflt = protoval->undefined_(frameRegister.nBits());
        retval->base_ = SValue::promote(state->readRegister(frameRegister, dflt, retval.get()))->get_expression();
        return retval;
    }

public:
    virtual S2::BaseSemantics::RiscOperatorsPtr
    create(const S2::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-16]");
    }

    virtual S2::BaseSemantics::RiscOperatorsPtr
    create(const S2::BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-16]");
    }

public:
    static RiscOperatorsPtr
    promote(const S2::BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual S2::BaseSemantics::SValuePtr
    readMemory(RegisterDescriptor segreg, const S2::BaseSemantics::SValuePtr &addr, const S2::BaseSemantics::SValuePtr &dflt,
               const S2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        lookForVariable(addr);
        return Super::readMemory(segreg, addr, dflt, cond);
    }

    virtual void
    writeMemory(RegisterDescriptor segreg, const S2::BaseSemantics::SValuePtr &addr, const S2::BaseSemantics::SValuePtr &data,
                const S2::BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        lookForVariable(addr);
        return Super::writeMemory(segreg, addr, data, cond);
    }

private:
    void lookForVariable(const S2::BaseSemantics::SValuePtr &addrSVal) {
        ASSERT_not_null(base_);
        SymbolicExpr::Ptr addr = SValue::promote(addrSVal)->get_expression();
        Sawyer::Message::Stream debug(mlog[DEBUG]);

        // The address must reference the stack or frame pointer
        struct Finder: SymbolicExpr::Visitor {
            SymbolicExpr::Ptr needle;                   // thing to find
            SymbolicExpr::Ptr found;                    // first matching expression in haystack
            std::set<SymbolicExpr::Hash> seen;          // subexpressions we've already checked

            Finder(const SymbolicExpr::Ptr needle)
                : needle(needle) {}

            SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) ROSE_OVERRIDE {
                if (seen.insert(node->hash()).second && node->isEquivalentTo(needle)) {
                    found = node;
                    return SymbolicExpr::TERMINATE;
                } else {
                    return SymbolicExpr::CONTINUE;
                }
            }

            SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) ROSE_OVERRIDE {
                return SymbolicExpr::CONTINUE;
            }
        } finder(base_);
        addr->depthFirstTraversal(finder);
        if (!finder.found)
            return;

        // Address must be an offset from some base address. The constant is the offet since the base address is probably a
        // symbolic stack or frame pointer. There might be more than one constant.
        if (SymbolicExpr::OP_ADD == addr->getOperator()) {
            SymbolicExpr::InteriorPtr inode = addr->isInteriorNode();
            BOOST_FOREACH (SymbolicExpr::Ptr operand, addr->children()) {
                int64_t offset = 0;
                if (operand->toSigned().assignTo(offset)) {
                    SAWYER_MESG(debug) <<"    found offset " <<offsetStr(offset)
                                       <<" at " <<currentInstruction()->toString() <<"\n";
                    OffsetToAddresses::iterator inserted = offsets_.insert(std::make_pair(offset, AddressSet())).first;
                    inserted->second.insert(currentInstruction()->get_address());
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

Sawyer::Optional<uint64_t>
VariableFinder::functionFrameSize(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(function);

    // Use a cached value if possible
    if (function->attributeExists(ATTR_FRAME_SIZE))
        return function->getAttribute<uint64_t>(ATTR_FRAME_SIZE);

    SgAsmInstruction *firstInsn = partitioner.instructionProvider()[function->address()];
    if (isSgAsmPowerpcInstruction(firstInsn) && isSgAsmPowerpcInstruction(firstInsn)->get_kind() == powerpc_stwu) {
        // If this PowerPC function starts with "stwu r1, u32 [r1 - N]" then the frame size is N.
        static RegisterDescriptor REG_R1;
        if (REG_R1.isEmpty())
            REG_R1 = *partitioner.instructionProvider().registerDictionary()->lookup("r1");
        SgAsmDirectRegisterExpression *firstRegister = isSgAsmDirectRegisterExpression(firstInsn->operand(0));
        SgAsmMemoryReferenceExpression *secondArg = isSgAsmMemoryReferenceExpression(firstInsn->operand(1));
        SgAsmBinaryAdd *memAddr = secondArg ? isSgAsmBinaryAdd(secondArg->get_address()) : NULL;
        SgAsmDirectRegisterExpression *secondRegister = memAddr ? isSgAsmDirectRegisterExpression(memAddr->get_lhs()) : NULL;
        SgAsmIntegerValueExpression *constant = memAddr ? isSgAsmIntegerValueExpression(memAddr->get_rhs()) : NULL;
        int64_t n = constant ? constant->get_signedValue() : int64_t(0);
        if (firstRegister && secondRegister && constant && n < 0 &&
            firstRegister->get_descriptor() == REG_R1 && secondRegister->get_descriptor() == REG_R1) {
            uint64_t frameSize = -n;
            function->setAttribute(ATTR_FRAME_SIZE, frameSize);
            return frameSize;
        }
    }

    return Sawyer::Nothing();
}

RegisterDescriptor
VariableFinder::frameOrStackPointer(const P2::Partitioner &partitioner) {
    if (boost::dynamic_pointer_cast<S2::DispatcherPowerpc>(partitioner.instructionProvider().dispatcher())) {
        // Although r1 is the conventional stack pointer for PowerPC, most memory references are via r31 which is initialized
        // to the same value as r1.
        const RegisterDescriptor *regptr = partitioner.instructionProvider().registerDictionary()->lookup("r31");
        ASSERT_not_null(regptr);
        ASSERT_forbid(regptr->isEmpty());
        return *regptr;
    } else {
        RegisterDescriptor reg = partitioner.instructionProvider().stackFrameRegister();
        if (reg.isEmpty())
            reg = partitioner.instructionProvider().stackPointerRegister();
        return reg;
    }
}

std::set<int64_t>
VariableFinder::findStackOffsets(const P2::Partitioner &partitioner, SgAsmInstruction *insn) {
    struct T: AstSimpleProcessing {
        std::set<int64_t> offsets;
        RegisterDescriptor framePointerRegister;

        T(RegisterDescriptor framePointerRegister)
            : framePointerRegister(framePointerRegister) {}

        void visit(SgNode *node) {
            if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(node)) {
                // Look for (add (reg ival))
                SgAsmDirectRegisterExpression *reg = isSgAsmDirectRegisterExpression(add->get_lhs());
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(add->get_rhs());
                if (reg && ival &&
                    (reg->get_descriptor() == framePointerRegister || framePointerRegister.isEmpty()) &&
                    SageInterface::getEnclosingNode<SgAsmMemoryReferenceExpression>(node)) {
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
                    (reg->get_descriptor() == framePointerRegister || framePointerRegister.isEmpty()) &&
                    SageInterface::getEnclosingNode<SgAsmMemoryReferenceExpression>(node)) {
                    offsets.insert(ival->get_signedValue());
                    return;
                }
            }
        }
    } visitor(frameOrStackPointer(partitioner));

    ASSERT_not_null(insn);
    visitor.traverse(insn, preorder);
    return visitor.offsets;
}

StackVariables
VariableFinder::findStackVariables(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    OffsetToAddresses stackOffsets;

    // Return cached local variable information
    if (function->attributeExists(ATTR_LOCAL_VARS))
        return function->getAttribute<StackVariables>(ATTR_LOCAL_VARS);

    // Try to figure out the function's stack frame size. It might not be possible.
    SAWYER_MESG(debug) <<"searching for local vars in " <<function->printableName() <<"\n";
    Sawyer::Optional<uint64_t> frameSize = functionFrameSize(partitioner, function);
    if (frameSize) {
        SAWYER_MESG(debug) <<"  stack frame size is " <<StringUtility::plural(*frameSize, "bytes") <<"\n";
    } else {
        SAWYER_MESG(debug) <<"  stack frame size is unknown\n";
    }
    int64_t maxFrameOffset = frameSize ? *frameSize - 1 : 0;

    // Look for stack offsets syntactically
    SAWYER_MESG(debug) <<"  searching for local variables syntactically...\n";
    BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
        P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bblockVa);
        ASSERT_not_null(bb);
        BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
            std::set<int64_t> offsets = findStackOffsets(partitioner, insn);
            BOOST_FOREACH (int64_t offset, offsets) {
                SAWYER_MESG(debug) <<"    found offset " <<offsetStr(offset) <<" at " <<insn->toString() <<"\n";
                OffsetToAddresses::iterator inserted = stackOffsets.insert(std::make_pair(offset, AddressSet())).first;
                inserted->second.insert(insn->get_address());
            }
        }
    }

    // Look for stack offsets semantically
    SAWYER_MESG(debug) <<"  searching for local variables semantically...\n";
    RegisterDescriptor frameRegister = frameOrStackPointer(partitioner);
    BOOST_FOREACH (rose_addr_t bblockVa, function->basicBlockAddresses()) {
        P2::BasicBlock::Ptr bb = partitioner.basicBlockExists(bblockVa);
        ASSERT_not_null(bb);
        VarSearchSemantics::RiscOperatorsPtr ops =
            VarSearchSemantics::RiscOperators::instance(partitioner, frameRegister, stackOffsets /*in,out*/);
        S2::BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(ops);
        BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions())
            cpu->processInstruction(insn);
    }

    // Build the local variable objects to be returned
    StackVariables lvars;
    for (OffsetToAddresses::const_iterator iter = stackOffsets.begin(); iter != stackOffsets.end(); ++iter) {
        // Avoid crossing into the caller's stack frame
        if (frameSize && iter->first >= maxFrameOffset)
            break;

        // This variable cannot overlap with the next variable or cross into the caller's stack frame
        OffsetToAddresses::const_iterator next = iter; ++next;
        int64_t lastOffset = next == stackOffsets.end() ? IntegerOps::genMask<int64_t>(63) : next->first - 1;
        if (frameSize)
            lastOffset = std::min(maxFrameOffset, lastOffset);

        // For architectures where the stack grows down, local variables have negative offsets, function arguments have
        // non-negative offsets. If there's a return address pushed by the call instruction, then it's usually at offset zero.
        if (iter->first < 0 && lastOffset >= 0)
            lastOffset = -1;

        OffsetInterval where = OffsetInterval::hull(iter->first, lastOffset);
        StackVariable lvar(function, iter->first, (lastOffset - iter->first)+1, iter->second);
        if (lvar.maxSizeBytes() > 0) {
            lvar.setDefaultName();
            lvars.insert(where, lvar);
        }
    }

    if (debug) {
        debug <<"  local variables for " <<function->printableName() <<":\n";
        print(lvars, partitioner, debug, "    ");
    }
    function->setAttribute(ATTR_LOCAL_VARS, lvars);
    return lvars;
}

S2::BaseSemantics::SValuePtr
VariableFinder::symbolicAddress(const P2::Partitioner &partitioner, const StackVariable &var,
                                const S2::BaseSemantics::RiscOperatorsPtr &ops) {
    ASSERT_not_null(ops);
    RegisterDescriptor baseReg = frameOrStackPointer(partitioner);
    S2::BaseSemantics::SValuePtr base = ops->peekRegister(baseReg, ops->undefined_(baseReg.nBits()));
    ASSERT_require(sizeof(var.frameOffset()) == sizeof(uint64_t));
    S2::BaseSemantics::SValuePtr offset = ops->number_(baseReg.nBits(), var.frameOffset());
    return ops->add(base, offset);
}

P2::Function::Ptr
VariableFinder::functionForInstruction(const P2::Partitioner &partitioner, SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    P2::BasicBlock::Ptr bb = partitioner.basicBlockContainingInstruction(insn->get_address());
    if (!bb)
        return P2::Function::Ptr();
    std::vector<P2::Function::Ptr> functions = partitioner.functionsOwningBasicBlock(bb);
    if (functions.empty())
        return P2::Function::Ptr();
    return functions[0];                                // arbitrarily choose the first one
}

StackVariables
VariableFinder::findStackVariables(const P2::Partitioner &partitioner, SgAsmInstruction *insn) {
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
VariableFinder::evict(const P2::Partitioner &partitioner) {
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions())
        evict(function);
}

bool
VariableFinder::isCached(const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    return function->attributeExists(ATTR_LOCAL_VARS);
}

std::set<SymbolicExpr::Ptr>
VariableFinder::getMemoryAddresses(const S2::BaseSemantics::MemoryCellStatePtr &mem) {
    struct: S2::BaseSemantics::MemoryCell::Visitor {
        std::set<SymbolicExpr::Ptr> addresses;

        void operator()(S2::BaseSemantics::MemoryCellPtr &cell) {
            SymbolicExpr::Ptr addr = S2::SymbolicSemantics::SValue::promote(cell->get_address())->get_expression();
            addresses.insert(addr);
        }
    } visitor;

    ASSERT_not_null(mem);
    mem->traverse(visitor);
    return visitor.addresses;
}

std::set<rose_addr_t>
VariableFinder::findConstants(const SymbolicExpr::Ptr &expr) {
    struct: SymbolicExpr::Visitor {
        std::set<rose_addr_t> constants;
        std::vector<SymbolicExpr::Ptr> path;

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            SymbolicExpr::InteriorPtr parent;
            if (!path.empty()) {
                parent = path.back()->isInteriorNode();
                ASSERT_not_null(parent);
            }
            path.push_back(node);

            // Some constants are never addresses
            if (parent && parent->getOperator() == SymbolicExpr::OP_EXTRACT &&
                (parent->child(0) == node || parent->child(1) == node))
                return SymbolicExpr::CONTINUE;          // first two args of extract are never addresses

            // If we found a constant, perhaps treat it like an address
            if (Sawyer::Optional<uint64_t> n = node->toUnsigned())
                constants.insert(*n);

            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            ASSERT_forbid(path.empty());
            path.pop_back();
            return SymbolicExpr::CONTINUE;
        }
    } visitor;

    ASSERT_not_null(expr);
    expr->depthFirstTraversal(visitor);
    return visitor.constants;
}

std::set<rose_addr_t>
VariableFinder::findConstants(SgAsmInstruction *insn) {
    struct: AstSimpleProcessing {
        std::set<rose_addr_t> constants;

        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node))
                constants.insert(ival->get_absoluteValue());
        }
    } visitor;

    ASSERT_not_null(insn);
    visitor.traverse(insn, preorder);
    return visitor.constants;
}

std::set<rose_addr_t>
VariableFinder::findAddressConstants(const S2::BaseSemantics::MemoryCellStatePtr &mem) {
    std::set<SymbolicExpr::Ptr> addresses = getMemoryAddresses(mem);
    std::set<rose_addr_t> retval;
    BOOST_FOREACH (const SymbolicExpr::Ptr &address, addresses) {
        std::set<rose_addr_t> constants = findConstants(address);
        retval.insert(constants.begin(), constants.end());
    }
    return retval;
}

AddressToAddresses
VariableFinder::findGlobalVariableVas(const P2::Partitioner &partitioner) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"Finding global variable addresses";
    Sawyer::Stopwatch timer;

    S2::SymbolicSemantics::RiscOperatorsPtr ops =
        S2::SymbolicSemantics::RiscOperators::instance(partitioner.instructionProvider().registerDictionary());
    S2::BaseSemantics::DispatcherPtr cpu = partitioner.newDispatcher(ops);
    ASSERT_not_null(cpu);
    AddressToAddresses retval;

    // FIXME[Robb Matzke 2019-12-06]: This could be parallel
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, partitioner.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            BOOST_FOREACH (SgAsmInstruction *insn, vertex.value().bblock()->instructions()) {
                SAWYER_MESG(debug) <<"  " <<insn->toString() <<"\n";
#if 1 // This method uses instruction semantics and then looks at the resulting memory state
                ops->currentState()->clear();
                try {
                    cpu->processInstruction(insn);
                } catch (...) {
                    SAWYER_MESG(mlog[WARN]) <<"semantics failed for " <<insn->toString() <<"\n";
                }

                // Find all constants that appear in memory address expressions.
                S2::BaseSemantics::MemoryCellStatePtr mem =
                    S2::BaseSemantics::MemoryCellState::promote(cpu->currentState()->memoryState());
                std::set<rose_addr_t> constants = findAddressConstants(mem);
#else // This method just looks for constants within the instructions themselves (i.e., immediates)
                std::set<rose_addr_t> constants = findConstants(insn);
#endif

                // Compute the contiguous regions formed by those constants. E.g., an instruction that reads four bytes of
                // memory might have four consecutive constants.
                AddressIntervalSet regions;
                BOOST_FOREACH (rose_addr_t c, constants) {
                    if (partitioner.instructionExists(c))
                        continue;                       // not a variable pointer if it points to an instruction
                    if (!partitioner.memoryMap()->at(c).exists())
                        continue;                       // global variables always have storage

                    regions.insert(c);
                }

                // Save only the lowest constant in each contiguous region.
                BOOST_FOREACH (const AddressInterval &interval, regions.intervals()) {
                    SAWYER_MESG(debug) <<"      " <<StringUtility::addrToString(interval.least()) <<"\n";
                    retval[interval.least()].insert(insn->get_address());
                }
            }
        }
    }
    info <<"; took " <<timer <<" seconds\n";
    return retval;
}

GlobalVariables
VariableFinder::findGlobalVariables(const P2::Partitioner &partitioner) {
    // Return cached global variable information
    if (partitioner.attributeExists(ATTR_GLOBAL_VARS))
        return partitioner.getAttribute<GlobalVariables>(ATTR_GLOBAL_VARS);

    // First, find all the global variables assume they're as large as possible but no global variables overlap.
    typedef Sawyer::Container::IntervalMap<AddressInterval /*occupiedVas*/, rose_addr_t /*startingVa*/> GVars;
    GVars gvars;
    AddressToAddresses globalVariableVas = findGlobalVariableVas(partitioner);
    static size_t wordSize = partitioner.instructionProvider().stackPointerRegister().nBits();
    for (AddressToAddresses::const_iterator iter = globalVariableVas.begin(); iter != globalVariableVas.end(); ++iter) {

        // This variable cannot overlap with the next variable
        AddressToAddresses::const_iterator next = iter; ++next;
        rose_addr_t lastVa = next == globalVariableVas.end() ? IntegerOps::genMask<rose_addr_t>(wordSize) : next->first - 1;

        // The variable will not extend beyond one segment of the memory map
        MemoryMap::ConstNodeIterator node = partitioner.memoryMap()->at(iter->first).findNode();
        ASSERT_require(node != partitioner.memoryMap()->nodes().end());
        lastVa = std::min(lastVa, node->key().greatest());

        gvars.insert(AddressInterval::hull(iter->first, lastVa), iter->first);
    }

    // Then remove code areas from the global variable location map
    BOOST_FOREACH (const P2::ControlFlowGraph::Vertex &vertex, partitioner.cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            BOOST_FOREACH (SgAsmInstruction *insn, vertex.value().bblock()->instructions())
                gvars.erase(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
        }
    }

    // Finally build the return value
    GlobalVariables retval;
    BOOST_FOREACH (const GVars::Node &node, gvars.nodes()) {
        if (node.key().least() == node.value())
            retval.insert(node.key(), GlobalVariable(node.key().least(), node.key().size(), globalVariableVas[node.value()]));
    }

    const_cast<P2::Partitioner&>(partitioner).setAttribute(ATTR_GLOBAL_VARS, retval);
    return retval;
}

} // namespace
} // namespace
} // namespace
