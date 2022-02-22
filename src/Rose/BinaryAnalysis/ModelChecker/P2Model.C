#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/P2Model.h>

#include <Rose/BinaryAnalysis/ModelChecker/BasicBlockUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/ErrorTag.h>
#include <Rose/BinaryAnalysis/ModelChecker/Exception.h>
#include <Rose/BinaryAnalysis/ModelChecker/ExternalFunctionUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/FailureUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/InstructionUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/NullDerefTag.h>
#include <Rose/BinaryAnalysis/ModelChecker/OobTag.h>
#include <Rose/BinaryAnalysis/ModelChecker/Path.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>

#include <Rose/BinaryAnalysis/InstructionSemantics2/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>
#include <Rose/BitOps.h>
#include <Rose/CommandLine.h>
#include <Combinatorics.h>                              // ROSE

#include <boost/algorithm/string/predicate.hpp>
#include <chrono>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {
namespace P2Model {

Sawyer::CommandLine::SwitchGroup
commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;
    using Rose::CommandLine::insertBooleanSwitch;
    SwitchGroup sg("Model settings");

    sg.insert(Switch("initial-stack")
              .argument("value", nonNegativeIntegerParser(settings.initialStackVa))
              .doc("Specifies an initial value for the stack pointer register for each path. Setting the initial stack "
                   "pointer to a concrete value causes most stack reads and writes to have concrete addresses that are non-zero. "
                   "Having a concrete address makes analysis easier, but a value should be chosen that won't interfere with "
                   "memory used for other purposes. The default is that the model checker will automatically choose an address "
                   "that's usually appropriate based on the memory layout of the specimen."));

    sg.insert(Switch("detect-null-reads")
              .argument("method", enumParser<TestMode>(settings.nullRead)
                        ->with("off", TestMode::OFF)
                        ->with("may", TestMode::MAY)
                        ->with("must", TestMode::MUST),
                        "must")
              .doc("Method by which to test for null pointer dereferences during memory read operations. Memory addresses "
                   "are tested during read operations to determine if the address falls within the \"null page\", which "
                   "consists of the lowest memory addresses as defined by @s{max-null-page}. If no switch argument is "
                   "supplied, then \"must\" is assumed. The arguments are:"

                   "@named{off}{Do not check for null pointer dereferences during read operations." +
                   std::string(TestMode::OFF == settings.nullRead ? " This is the default." : "") + "}"

                   "@named{may}{Find reads that might access the null page, but do not necessarily do so." +
                   std::string(TestMode::MAY == settings.nullRead ? " This is the default." : "") + "}"

                   "@named{must}{Find reads that must necessarily access the null page." +
                   std::string(TestMode::MUST == settings.nullRead ? " This is the default." : "") + "}"));

    sg.insert(Switch("detect-null-writes")
              .argument("method", enumParser<TestMode>(settings.nullWrite)
                        ->with("off", TestMode::OFF)
                        ->with("may", TestMode::MAY)
                        ->with("must", TestMode::MUST),
                        "must")
              .doc("Method by which to test for null pointer dereferences during memory write operations. Memory addresses "
                   "are tested during write operations to determine if the address falls within the \"null page\", which "
                   "consists of the lowest memory addresses as defined by @s{max-null-page}. If no switch argument is "
                   "supplied, then \"must\" is assumed. The arguments are:"

                   "@named{off}{Do not check for null pointer dereferences during write operations." +
                   std::string(TestMode::OFF == settings.nullWrite ? " This is the default." : "") + "}"

                   "@named{may}{Find writes that might access the null page, but do not necessarily do so." +
                   std::string(TestMode::MAY == settings.nullWrite ? " This is the default." : "") + "}"

                   "@named{must}{Find writes that must necessarily access the null page." +
                   std::string(TestMode::MUST == settings.nullWrite ? " This is the default." : "") + "}"));

    sg.insert(Switch("detect-oob-reads")
              .argument("method", enumParser<TestMode>(settings.oobRead)
                        ->with("off", TestMode::OFF)
                        ->with("may", TestMode::MAY)
                        ->with("must", TestMode::MUST),
                        "must")
              .doc("Method by which to test for out-of-bounds variable acceess during memory read operations. Memory addresses "
                   "are tested during read operations to determine if the address falls outside the variable(s) associated with "
                   "that instruction. If no switch argument is supplied, then \"must\" is assumed. The arguments are:"

                   "@named{off}{Do not check for out-of-bounds variable accesses during read operations." +
                   std::string(TestMode::OFF == settings.oobRead ? " This is the default." : "") + "}"

                   "@named{may}{Find out-of-bounds variable reads that may occur, but possibly not necessarily occur." +
                   std::string(TestMode::MAY == settings.oobRead ? " This is the default." : "") + "}"

                   "@named{must}{Find out-of-bounds variable reads that must necessarily occur." +
                   std::string(TestMode::MUST == settings.oobRead ? " This is the default." : "") + "}"));

    sg.insert(Switch("detect-oob-writes")
              .argument("method", enumParser<TestMode>(settings.oobWrite)
                        ->with("off", TestMode::OFF)
                        ->with("may", TestMode::MAY)
                        ->with("must", TestMode::MUST),
                        "must")
              .doc("Method by which to test for out-of-bounds variable acceess during memory write operations. Memory addresses "
                   "are tested during write operations to determine if the address falls outside the variable(s) associated with "
                   "that instruction. If no switch argument is supplied, then \"must\" is assumed. The arguments are:"

                   "@named{off}{Do not check for out-of-bounds variable accesses during write operations." +
                   std::string(TestMode::OFF == settings.oobWrite ? " This is the default." : "") + "}"

                   "@named{may}{Find out-of-bounds variable writes that may occur, but possibly not necessarily occur." +
                   std::string(TestMode::MAY == settings.oobWrite ? " This is the default." : "") + "}"

                   "@named{must}{Find out-of-bounds variable writes that must necessarily occur." +
                   std::string(TestMode::MUST == settings.oobWrite ? " This is the default." : "") + "}"));

    sg.insert(Switch("max-null-page")
              .argument("address", nonNegativeIntegerParser(settings.maxNullAddress))
              .doc("When checking for null pointer dereferences, any read or write to an address within the \"null page\" "
                   "is considered to be a null pointer dereference. The null page consists of the lowest memory addresses "
                   "from zero up to, and including, the value specified by this switch. The default is " +
                   StringUtility::addrToString(settings.maxNullAddress) + "."));

    sg.insert(Switch("semantic-memory")
              .argument("type", enumParser<Settings::MemoryType>(settings.memoryType)
                        ->with("list", Settings::MemoryType::LIST)
                        ->with("map", Settings::MemoryType::MAP))
              .doc("Type of memory state representation. The choices are:"

                   "@named{list}{This represents memory using a reverse chronological list of memory address and value pairs. "
                   "This representation is more accurate for answering questions about aliasing, but is slower and uses more "
                   "memory." +
                   std::string(Settings::MemoryType::LIST == settings.memoryType ? " This is the default." : "") + "}"

                   "@named{map}{This represents memory using a mapping from the hash of the symbolic address expression to the "
                   "byte value stored at that address. Although this is faster and uses less memory, it is generally unable "
                   "to answer questions about aliasing." +
                   std::string(Settings::MemoryType::MAP == settings.memoryType ? " This is the default." : "") + "}"));

    insertBooleanSwitch(sg, "debug-null-deref", settings.debugNull,
                        "If this feature is enabled, then the diagnostic output (if it is also enabled) "
                        "will include information about null pointer dereference checking, such as the "
                        "symbolic address being checked.");

    insertBooleanSwitch(sg, "debug-semantic-operations", settings.traceSemantics,
                        "If enabled, then each low-level instruction semantic operation is emitted to the diagnostic output "
                        "(if it is also enabled).");

    insertBooleanSwitch(sg, "solver-memoization", settings.solverMemoization,
                        "Causes the SMT solvers (per thread) to memoize their results. In other words, they remember the "
                        "sets of assertions and if the same set appears a second time it will return the same answer as the "
                        "first time without actually calling the SMT solver.  This can sometimes reduce the amount of time "
                        "spent solving, but uses more memory.");

    return sg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Saturating addition.
//
// Add the signed value to the unsigned base, returning an unsigned result. If the result would overflow then return either
// zero or the maximum unsigned value, depending on the direction of overflow. The return value is a pair that contains the
// saturated result and a Boolean indicating whether saturation occured. We have to be careful here because signed integer
// overflows are undefined behavior in C++11 and earlier -- the compiler can optimize them however it likes, which might not be
// the 2's complement we're expecting.
static std::pair<uint64_t /*sum*/, bool /*saturated?*/>
saturatedAdd(uint64_t base, int64_t delta) {
    if (0 == delta) {
        return {base, false};
    } else if (delta > 0) {
        uint64_t addend = (uint64_t)delta;
        if (base + addend < base) {
            return {~(uint64_t)0, true};
        } else {
            return {base + addend, false};
        }
    } else {
        uint64_t subtrahend = (uint64_t)(-delta);
        if (subtrahend > base) {
            return {0, true};
        } else {
            return {base - subtrahend, false};
        }
    }
}

// Offset an interval by a signed amount.
//
// The unsigned address interval is shifted lower or higher according to the signed @p delta value. The endpoints of the
// returned interval saturate to zero or maximum address if the @p delta causes the interval to partially overflow. If the
// interval completely overflows (both its least and greatest values overflow) then the empty interval is
// returned. Shifting an empty address interval any amount also returns the emtpy interval. The return value is clipped
// by intersecting it with the specified @p limit.
static AddressInterval shiftAddresses(uint64_t base, const Variables::OffsetInterval &delta, const AddressInterval &limit) {
    if (delta.isEmpty()) {
        return AddressInterval();
    } else {
        auto loSat = saturatedAdd(base, delta.least());
        auto hiSat = saturatedAdd(base, delta.greatest());
        if (loSat.second && hiSat.second) {
            return AddressInterval();
        } else {
            return AddressInterval::hull(loSat.first, hiSat.first) & limit;
        }
    }
}

// Scan through the call stack and try to find a local variable that overlaps all the specified addresses. If more than one
// such variable exists, return the smallest one, arbitrarily breaking ties. Stack variables are described in terms of their
// offsets in a call frame w.r.t. a frame pointer, but we search for them using concrete addresses. We also return the concrete
// addresses where the variable lives on the stack.
static std::pair<AddressInterval, Variables::StackVariable>
findStackVariable(const FunctionCallStack &callStack, const AddressInterval &location, const AddressInterval &stackLimits) {
    AddressInterval foundInterval;
    Variables::StackVariable foundVariable;

    for (size_t i = 0; i < callStack.size(); ++i) {
        const FunctionCall &fcall = callStack[i];
        for (const Variables::StackVariable &var: fcall.stackVariables().values()) {
            if (AddressInterval varAddrs = shiftAddresses(fcall.initialStackPointer() + fcall.framePointerDelta(),
                                                          var.interval(), stackLimits)) {
                if (varAddrs.isContaining(location)) {
                    if (!foundInterval || varAddrs.size() < foundInterval.size()) {
                        foundInterval = varAddrs;
                        foundVariable = var;
                    }
                }
            }
        }
    }
    return {foundInterval, foundVariable};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function call stack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionCall::FunctionCall(const Partitioner2::FunctionPtr &function, rose_addr_t initialStackPointer,
                           const Variables::StackVariables &vars)
    : function_(function), initialStackPointer_(initialStackPointer), stackVariables_(vars) {}

FunctionCall::~FunctionCall() {}

rose_addr_t
FunctionCall::initialStackPointer() const {
    return initialStackPointer_;
}

P2::Function::Ptr
FunctionCall::function() const {
    return function_;
}

const Variables::StackVariables&
FunctionCall::stackVariables() const {
    return stackVariables_;
}

rose_addr_t
FunctionCall::framePointerDelta() const {
    return framePointerDelta_;
}

void
FunctionCall::framePointerDelta(rose_addr_t delta) {
    framePointerDelta_ = delta;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::SValue() {}

SValue::~SValue() {}

SValue::SValue(size_t nBits, uint64_t number)
    : Super(nBits, number) {}

SValue::SValue(const SymbolicExpr::Ptr &expr)
    : Super(expr) {}

SValue::Ptr
SValue::instance() {
    return Ptr(new SValue);
}

SValue::Ptr
SValue::instanceBottom(size_t nBits) {
    return Ptr(new SValue(SymbolicExpr::makeIntegerVariable(nBits, "", SymbolicExpr::Node::BOTTOM)));
}

SValue::Ptr
SValue::instanceUndefined(size_t nBits) {
    return Ptr(new SValue(SymbolicExpr::makeIntegerVariable(nBits)));
}

SValue::Ptr
SValue::instanceUnspecified(size_t nBits) {
    return Ptr(new SValue(SymbolicExpr::makeIntegerVariable(nBits, "", SymbolicExpr::Node::UNSPECIFIED)));
}

SValue::Ptr
SValue::instanceInteger(size_t nBits, uint64_t value) {
    return Ptr(new SValue(SymbolicExpr::makeIntegerConstant(nBits, value)));
}

SValue::Ptr
SValue::instanceSymbolic(const SymbolicExpr::Ptr &value) {
    ASSERT_not_null(value);
    return Ptr(new SValue(value));
}

BS::SValue::Ptr
SValue::copy(size_t newWidth) const {
    Ptr retval(new SValue(*this));
    if (newWidth != 0 && newWidth != retval->nBits())
        retval->set_width(newWidth);
    return retval;
}

Sawyer::Optional<BS::SValue::Ptr>
SValue::createOptionalMerge(const BS::SValue::Ptr &other, const BS::Merger::Ptr &merger,
                    const SmtSolver::Ptr &solver) const {
    ASSERT_not_implemented("[Robb Matzke 2021-07-07]");
}

AddressInterval
SValue::region() const {
    return region_;
}

void
SValue::region(const AddressInterval &where) {
    region_ = where;
}

void
SValue::print(std::ostream &out, BS::Formatter &fmt) const {
    if (region_)
        out <<"<" <<region_.size() <<"-byte region at " <<StringUtility::addrToString(region_) <<"> ";

    Super::print(out, fmt);
}

void
SValue::hash(Combinatorics::Hasher &hasher) const {
    Super::hash(hasher);
    if (region_) {
        hasher.insert(region_.least());
        hasher.insert(region_.greatest());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

State::State() {}

State::State(const BS::RegisterState::Ptr &registers, const BS::MemoryState::Ptr &memory)
    : Super(registers, memory) {}

State::State(const State &other)
    : Super(other), callStack_(other.callStack_) {}

State::Ptr
State::instance(const BS::RegisterState::Ptr &registers, const BS::MemoryState::Ptr &memory) {
    return Ptr(new State(registers, memory));
}

State::Ptr
State::instance(const Ptr &other) {
    ASSERT_not_null(other);
    return Ptr(new State(*other));
}

BS::State::Ptr
State::clone() const {
    return Ptr(new State(*this));
}

State::Ptr
State::promote(const BS::State::Ptr &x) {
    Ptr retval = boost::dynamic_pointer_cast<State>(x);
    ASSERT_not_null(retval);
    return retval;
}

const FunctionCallStack&
State::callStack() const {
    return callStack_;
}

FunctionCallStack&
State::callStack() {
    return callStack_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const Settings &settings, const P2::Partitioner &partitioner,
                             ModelChecker::SemanticCallbacks *semantics, const BS::SValue::Ptr &protoval,
                             const SmtSolver::Ptr &solver, const Variables::VariableFinder::Ptr &varFinder)
    : Super(protoval, solver), settings_(settings), partitioner_(partitioner),
      semantics_(dynamic_cast<P2Model::SemanticCallbacks*>(semantics)), variableFinder_unsync(varFinder) {
    ASSERT_not_null(semantics_);
    ASSERT_not_null(variableFinder_unsync);
    (void)SValue::promote(protoval);
    name("P2Model");

    // Calculate the stack limits.  Start by assuming the stack can occupy all of memory. Then make that smaller based on
    // other information.
    const size_t nBits = partitioner.instructionProvider().instructionPointerRegister().nBits();
    stackLimits_ = AddressInterval::hull(0, BitOps::lowMask<rose_addr_t>(nBits));
    if (settings_.initialStackVa) {
        // The stack should extend down to the top of the next lower mapped address.
        ASSERT_forbid(partitioner.memoryMap()->at(*settings_.initialStackVa).exists());
        if (auto prev = partitioner.memoryMap()->atOrBefore(*settings_.initialStackVa).next(Sawyer::Container::MATCH_BACKWARD))
            stackLimits_ = stackLimits_ & AddressInterval::hull(*prev + 1, stackLimits_.greatest());

        // The stack should extend up to the bottom of the next higher mapped address.
        if (auto next = partitioner.memoryMap()->atOrAfter(*settings_.initialStackVa).next())
            stackLimits_ = stackLimits_ & AddressInterval::hull(stackLimits_.least(), *next - 1);

        // The stack should extend only a few bytes above it's initial location.
        stackLimits_ = stackLimits_ & AddressInterval::hull(stackLimits_.least(),
                                                            saturatedAdd(*settings_.initialStackVa, 256 /*arbitrary*/).first);
    }
    SAWYER_MESG(mlog[DEBUG]) <<"stack limited to " <<StringUtility::addrToString(stackLimits_) <<"\n";
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instance(const Settings &settings, const P2::Partitioner &partitioner, ModelChecker::SemanticCallbacks *semantics,
                        const BS::SValue::Ptr &protoval, const SmtSolver::Ptr &solver,
                        const Variables::VariableFinder::Ptr &varFinder) {
    ASSERT_not_null(protoval);
    return Ptr(new RiscOperators(settings, partitioner, semantics, protoval, solver, varFinder));
}

BS::RiscOperators::Ptr
RiscOperators::create(const BS::SValue::Ptr&, const SmtSolver::Ptr&) const {
    ASSERT_not_implemented("[Robb Matzke 2021-04-01]"); // needs to be overridden, but need not be implemented
}

BS::RiscOperators::Ptr
RiscOperators::create(const BS::State::Ptr&, const SmtSolver::Ptr&) const {
    ASSERT_not_implemented("[Robb Matzke 2021-04-01]"); // needs to be overridden, but need not be implemented
}

RiscOperators::Ptr
RiscOperators::promote(const BS::RiscOperators::Ptr &x) {
    Ptr retval;
    if (auto traceOps = boost::dynamic_pointer_cast<IS::TraceSemantics::RiscOperators>(x)) {
        retval = boost::dynamic_pointer_cast<RiscOperators>(traceOps->subdomain());
    } else {
        retval = boost::dynamic_pointer_cast<RiscOperators>(x);
    }
    ASSERT_not_null(retval);
    return retval;
}

const Partitioner2::Partitioner&
RiscOperators::partitioner() const {
    return partitioner_;
}

SmtSolver::Ptr
RiscOperators::modelCheckerSolver() const {
    return modelCheckerSolver_;
}

void
RiscOperators::modelCheckerSolver(const SmtSolver::Ptr &solver) {
    modelCheckerSolver_ = solver;
}

bool
RiscOperators::computeMemoryRegions() const {
    return computeMemoryRegions_;
}

void
RiscOperators::computeMemoryRegions(bool b) {
    computeMemoryRegions_ = b;
}

void
RiscOperators::checkNullAccess(const BS::SValue::Ptr &addrSVal, TestMode testMode, IoMode ioMode) {
    // Null-dereferences are only tested when we're actually executing an instruciton. Other incidental operations such as
    // initializing the first state are not checked.
    if (!currentInstruction())
        return;
    if (TestMode::OFF == testMode)
        return;

    ASSERT_not_null(addrSVal);
    ASSERT_not_null(modelCheckerSolver_);               // should have all the path assertions already
    SymbolicExpr::Ptr addr = IS::SymbolicSemantics::SValue::promote(addrSVal)->get_expression();
    const char *direction = IoMode::READ == ioMode ? "read" : "write";
    ProgressTask task(modelCheckerSolver_->progress(), "nullptr");

    bool isNull = false;
    switch (testMode) {
        case TestMode::OFF:
            break;

        case TestMode::MAY: {
            // May be null if addr <= max is satisfied
            SymbolicExpr::Ptr maxNullExpr = SymbolicExpr::makeIntegerConstant(addr->nBits(), settings_.maxNullAddress);
            SymbolicExpr::Ptr isNullExpr = SymbolicExpr::makeLe(addr, maxNullExpr);
            SmtSolver::Transaction tx(modelCheckerSolver_);
            modelCheckerSolver_->insert(isNullExpr);
            isNull = modelCheckerSolver_->check() == SmtSolver::SAT_YES;
            (settings_.debugNull || isNull) && SAWYER_MESG(mlog[DEBUG])
                <<"      " <<direction <<" address may be in [0," <<settings_.maxNullAddress <<"]? " <<(isNull?"yes":"no")
                <<"; address = " <<*addr <<"\n";
            break;
        }

        case TestMode::MUST: {
            // Must be null if addr > max cannot be satisfied
            SymbolicExpr::Ptr maxNullExpr = SymbolicExpr::makeIntegerConstant(addr->nBits(), settings_.maxNullAddress);
            SymbolicExpr::Ptr isNullExpr = SymbolicExpr::makeGt(addr, maxNullExpr);
            SmtSolver::Transaction tx(modelCheckerSolver_);
            modelCheckerSolver_->insert(isNullExpr);
            isNull = modelCheckerSolver_->check() == SmtSolver::SAT_NO;
            (settings_.debugNull || isNull) && SAWYER_MESG(mlog[DEBUG])
                <<"      " <<direction <<" address must be in [0," <<settings_.maxNullAddress <<"]? " <<(isNull?"yes":"no")
                <<"; address = " <<*addr <<"\n";
            break;
        }
    }

    if (isNull && !semantics_->filterNullDeref(addrSVal, currentInstruction(), testMode, ioMode)) {
        SAWYER_MESG(mlog[DEBUG]) <<"      nullptr dereference rejected by user; this one is ignored\n";
        isNull = false;
    }

    if (isNull) {
        currentState(BS::State::Ptr());                   // indicates that execution failed
        throw ThrownTag{NullDerefTag::instance(nInstructions(), testMode, ioMode, currentInstruction(), addrSVal)};
    }
}

void
RiscOperators::checkOobAccess(const BS::SValue::Ptr &addrSVal_, TestMode testMode, IoMode ioMode, size_t nBytes) {
    if (computeMemoryRegions_) {
        auto addrSVal = SValue::promote(addrSVal_);

        // Out of bounds references are only tested when we're actually executing an instruction. Other incidental operations such
        // as initializing the first state are not checked.
        if (!currentInstruction())
            return;
        if (TestMode::OFF == testMode)
            return;

        // If the address is concrete and refers to a region of memory but is outside that region, then we have an OOB access.
        if (auto va = addrSVal->toUnsigned()) {
            if (AddressInterval referencedRegion = addrSVal->region()) {
                ProgressTask task(modelCheckerSolver_->progress(), "oob");
                AddressInterval accessedRegion = AddressInterval::baseSizeSat(*va, nBytes);
                if (!referencedRegion.isContaining(accessedRegion)) {

                    // Get information about the variable that was intended to be accessed, and the variable (if any) that was
                    // actually accessed.
                    FunctionCallStack &callStack = State::promote(currentState())->callStack();
                    auto whereWhatIntended = findStackVariable(callStack, referencedRegion, stackLimits_);
                    auto whereWhatActual = findStackVariable(callStack, accessedRegion, stackLimits_);
                    if (!whereWhatActual.first) {
                        for (rose_addr_t accessedByteVa: accessedRegion) {
                            whereWhatActual = findStackVariable(callStack, accessedByteVa, stackLimits_);
                            if (whereWhatActual.first)
                                break;
                        }
                    }

                    // Optionally throw the tag that describes the OOB access
                    if (!semantics_->filterOobAccess(addrSVal, referencedRegion, accessedRegion, currentInstruction(),
                                                     testMode, ioMode, whereWhatIntended.second, whereWhatIntended.first,
                                                     whereWhatActual.second, whereWhatActual.first)) {
                        SAWYER_MESG(mlog[DEBUG]) <<"      buffer overflow rejected by user; this one is ignored\n";
                    } else {
                        currentState(BS::State::Ptr());         // indicates that execution failed
                        throw ThrownTag{OobTag::instance(nInstructions(), testMode, ioMode, currentInstruction(), addrSVal,
                                                         whereWhatIntended.second, whereWhatIntended.first,
                                                         whereWhatActual.second, whereWhatActual.first)};
                    }
                }
            }
        }
    }
}

size_t
RiscOperators::nInstructions() const {
    return nInstructions_;
}

void
RiscOperators::nInstructions(size_t n) {
    nInstructions_ = n;
}

size_t
RiscOperators::pruneCallStack() {
    size_t nPopped = 0;
    if (computeMemoryRegions_) {
        const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
        const BS::SValue::Ptr spSValue = peekRegister(SP, undefined_(SP.nBits()));
        if (auto sp = spSValue->toUnsigned()) {
            FunctionCallStack &callStack = State::promote(currentState())->callStack();

            while (!callStack.isEmpty() && callStack.top().initialStackPointer() < *sp) {
                SAWYER_MESG(mlog[DEBUG]) <<"      returned from " <<callStack.top().function()->printableName() <<"\n";
                callStack.pop();
                ++nPopped;
            }
        }
    }
    return nPopped;
}

void
RiscOperators::pushCallStack(const P2::Function::Ptr &callee, rose_addr_t initialSp) {
    if (computeMemoryRegions_) {
        FunctionCallStack &callStack = State::promote(currentState())->callStack();

        while (!callStack.isEmpty() && callStack.top().initialStackPointer() <= initialSp) {
            SAWYER_MESG(mlog[DEBUG]) <<"      returned from " <<callStack.top().function()->printableName() <<"\n";
            callStack.pop();
        }

        SAWYER_MESG(mlog[DEBUG]) <<"      called " <<callee->printableName() <<"\n";
        SAWYER_MESG(mlog[DEBUG]) <<"        initial stack pointer = " <<StringUtility::addrToString(initialSp) <<"\n";

        // VariableFinder API is not thread safe, so we need to protect it
        Variables::StackVariables lvars;
        Sawyer::Optional<uint64_t> frameSize;
        {
            SAWYER_THREAD_TRAITS::LockGuard lock(variableFinderMutex_);
            lvars = variableFinder_unsync->findStackVariables(partitioner_, callee);
            frameSize = variableFinder_unsync->detectFrameAttributes(partitioner_, callee).size;
        }
        callStack.push(FunctionCall(callee, initialSp, lvars));
        if (frameSize)
            callStack[0].framePointerDelta(-*frameSize);

        if (mlog[DEBUG])
            printCallStack(mlog[DEBUG]);
    }
}

void
RiscOperators::printCallStack(std::ostream &out) {
    if (computeMemoryRegions_) {
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        out <<"      function call stack:\n";
        if (callStack.isEmpty()) {
            out <<"        empty\n";
        } else {
            for (size_t i = callStack.size(); i > 0; --i) {
                const FunctionCall &fcall = callStack[i-1];
                out <<"        " <<fcall.function()->printableName()
                    <<" initSp=" <<StringUtility::addrToString(fcall.initialStackPointer()) <<"\n";
                for (const Variables::StackVariable &var: fcall.stackVariables().values()) {
                    AddressInterval where = shiftAddresses(fcall.initialStackPointer() + fcall.framePointerDelta(),
                                                           var.interval(), stackLimits_);
                    out <<"          " <<var <<" at " <<StringUtility::addrToString(where) <<"\n";
                }
            }
        }
    }
}

BS::SValue::Ptr
RiscOperators::assignRegion(const BS::SValue::Ptr &result_) {
    if (computeMemoryRegions_) {
        auto result = SValue::promote(result_);
        if (!result->region()) {
            if (auto va = result->toUnsigned()) {
                FunctionCallStack &callStack = State::promote(currentState())->callStack();
                auto whereWhat = findStackVariable(callStack, *va, stackLimits_);
                if (whereWhat.first) {
                    switch (whereWhat.second.purpose()) {
                        case Variables::StackVariable::Purpose::RETURN_ADDRESS:
                        case Variables::StackVariable::Purpose::FRAME_POINTER:
                        case Variables::StackVariable::Purpose::SPILL_AREA:
                            // These types of stack areas don't really have hard-and-fast boundaries because the compiler often
                            // generates code to compute these addresses as part of accessing other areas of the stack frame.
                            break;

                        case Variables::StackVariable::Purpose::NORMAL:
                            // The variable is known source-code. These are things we definitely want to check!
                            result->region(whereWhat.first);
                            break;

                        case Variables::StackVariable::Purpose::UNKNOWN:
                        case Variables::StackVariable::Purpose::OTHER:
                            // We're not quite sure what's here, so treat it as if it were a source code variable.
                            result->region(whereWhat.first);
                            break;
                    }
                }
            }
        }
    }
    return result_;
}

BS::SValue::Ptr
RiscOperators::assignRegion(const BS::SValue::Ptr &result, const BS::SValue::Ptr &a) {
    if (computeMemoryRegions_) {
        AddressInterval ar = SValue::promote(a)->region();
        if (ar) {
            SValue::promote(result)->region(ar);
        } else {
            assignRegion(result);
        }
    }
    return result;
}

BS::SValue::Ptr
RiscOperators::assignRegion(const BS::SValue::Ptr &result, const BS::SValue::Ptr &a, const BS::SValue::Ptr &b) {
    if (computeMemoryRegions_) {
        AddressInterval ar = SValue::promote(a)->region();
        AddressInterval br = SValue::promote(b)->region();
        if (ar && !br) {
            SValue::promote(result)->region(ar);
        } else if (!ar && br) {
            SValue::promote(result)->region(br);
        } else {
            assignRegion(result);
        }
    }
    return result;
}

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    Super::startInstruction(insn);

    // Remove stale function calls
    if (pruneCallStack() && mlog[DEBUG])
        printCallStack(mlog[DEBUG]);

    // If the call stack is empty, then push a record for the current function.
    if (computeMemoryRegions_) {
        const rose_addr_t va = insn->get_address();
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        if (callStack.isEmpty()) {
            std::vector<P2::Function::Ptr> functions = partitioner_.functionsSpanning(va);
            P2::Function::Ptr function;
            if (functions.empty()) {
                SAWYER_MESG(mlog[WARN]) <<"no function containing instruction at " <<StringUtility::addrToString(va) <<"\n";
            } else if (functions.size() == 1) {
                function = functions[0];
            } else {
                SAWYER_MESG(mlog[WARN]) <<"multiple functions containing instruction at " <<StringUtility::addrToString(va) <<"\n";
                function = functions[0];                    // arbitrary
            }

            if (function) {
                const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
                const BS::SValue::Ptr spSValue = peekRegister(SP, undefined_(SP.nBits()));
                const rose_addr_t sp = spSValue->toUnsigned().get();      // must be concrete
                pushCallStack(function, sp);
            }
        }
    }
}

void
RiscOperators::finishInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    ++nInstructions_;

    // If this was a function call, then push a new entry onto the call stack and insert the memory regions for local variables
    // and function stack arguments.
    if (computeMemoryRegions_) {
        bool isFunctionCall = false;
        if (P2::BasicBlock::Ptr bb = partitioner_.basicBlockContainingInstruction(insn->get_address())) {
            isFunctionCall = partitioner_.basicBlockIsFunctionCall(bb);
        } else {
            isFunctionCall = insn->isFunctionCallFast(std::vector<SgAsmInstruction*>{insn}, nullptr, nullptr);
        }
        if (isFunctionCall) {
            const RegisterDescriptor IP = partitioner_.instructionProvider().instructionPointerRegister();
            BS::SValue::Ptr ipSValue = peekRegister(IP, undefined_(IP.nBits()));
            if (auto ip = ipSValue->toUnsigned()) {
                if (P2::Function::Ptr callee = partitioner_.functionExists(*ip)) {
                    // We are calling a function, so push a record onto the call stack.
                    const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
                    const BS::SValue::Ptr spSValue = peekRegister(SP, undefined_(SP.nBits()));
                    if (auto sp = spSValue->toUnsigned())
                        pushCallStack(callee, *sp);
                }
            }
        }
    }

    Super::finishInstruction(insn);
}

BS::SValue::Ptr
RiscOperators::number_(size_t nBits, uint64_t value) {
    return assignRegion(Super::number_(nBits, value));
}

BS::SValue::Ptr
RiscOperators::extract(const BS::SValue::Ptr &a, size_t begin, size_t end) {
    return assignRegion(Super::extract(a, begin, end), a);
}

BS::SValue::Ptr
RiscOperators::concat(const BS::SValue::Ptr &lowBits, const BS::SValue::Ptr &highBits) {
    if (computeMemoryRegions_) {
        auto result = SValue::promote(Super::concat(lowBits, highBits));
        auto a = SValue::promote(lowBits);
        auto b = SValue::promote(highBits);
        if (a->region() && a->region() == b->region()) {
            result->region(a->region());
        } else {
            assignRegion(result, a, b);
        }
        return result;
    } else {
        return Super::concat(lowBits, highBits);
    }
}

BS::SValue::Ptr
RiscOperators::shiftLeft(const BS::SValue::Ptr &a, const BS::SValue::Ptr &nBits) {
    return assignRegion(Super::shiftLeft(a, nBits), a);
}

BS::SValue::Ptr
RiscOperators::shiftRight(const BS::SValue::Ptr &a, const BS::SValue::Ptr &nBits) {
    return assignRegion(Super::shiftRight(a, nBits), a);
}

BS::SValue::Ptr
RiscOperators::shiftRightArithmetic(const BS::SValue::Ptr &a, const BS::SValue::Ptr &nBits) {
    return assignRegion(Super::shiftRightArithmetic(a, nBits), a);
}

BS::SValue::Ptr
RiscOperators::unsignedExtend(const BS::SValue::Ptr &a, size_t newWidth) {
    return assignRegion(Super::unsignedExtend(a, newWidth), a);
}

BS::SValue::Ptr
RiscOperators::signExtend(const BS::SValue::Ptr &a, size_t newWidth) {
    return assignRegion(Super::signExtend(a, newWidth), a);
}

BS::SValue::Ptr
RiscOperators::add(const BS::SValue::Ptr &a, const BS::SValue::Ptr &b) {
    return assignRegion(Super::add(a, b), a, b);
}

BS::SValue::Ptr
RiscOperators::addCarry(const BS::SValue::Ptr &a, const BS::SValue::Ptr &b,
                        BS::SValue::Ptr &carryOut /*out*/, BS::SValue::Ptr &overflowed /*out*/) {
    return assignRegion(Super::addCarry(a, b, carryOut, overflowed), a, b);
}

BS::SValue::Ptr
RiscOperators::subtract(const BS::SValue::Ptr &a, const BS::SValue::Ptr &b) {
    return assignRegion(Super::subtract(a, b), a, b);
}

BS::SValue::Ptr
RiscOperators::subtractCarry(const BS::SValue::Ptr &a, const BS::SValue::Ptr &b,
                             BS::SValue::Ptr &carryOut /*out*/, BS::SValue::Ptr &overflowed /*out*/) {
    return assignRegion(Super::subtractCarry(a, b, carryOut, overflowed), a, b);
}

BS::SValue::Ptr
RiscOperators::addWithCarries(const BS::SValue::Ptr &a, const BS::SValue::Ptr &b,
                              const BS::SValue::Ptr &c, BS::SValue::Ptr &carryOut /*out*/) {
    return assignRegion(Super::addWithCarries(a, b, c, carryOut), a, b);
}

BS::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const BS::SValue::Ptr &dflt) {
    if (computeMemoryRegions_) {
        auto retval = SValue::promote(Super::readRegister(reg, dflt));

        // The stack pointer and frame pointers are typically used by adding or subtracting something from them to access a
        // *different* local variable than they point to directly. The whole point of adding and subtracting is to access a
        // different variable rather than being an out-of-bounds access for the directly pointed variable.
        if (partitioner_.instructionProvider().stackPointerRegister() == reg ||
            partitioner_.instructionProvider().stackFrameRegister() == reg)
            retval->region(AddressInterval());
        return retval;
    } else {
        return Super::readRegister(reg, dflt);
    }
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BS::SValue::Ptr &value) {
    if (computeMemoryRegions_) {
        // If we're writing to the frame pointer register, then update the top function of the call stack to indicate how the
        // frame pointer is related to the initial stack pointer.
        const RegisterDescriptor FP = partitioner_.instructionProvider().stackFrameRegister();
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        if (FP == reg && !callStack.isEmpty()) {
            if (auto fp = value->toUnsigned()) {
                rose_addr_t delta = *fp - callStack.top().initialStackPointer();
                callStack.top().framePointerDelta(delta);
                SAWYER_MESG(mlog[DEBUG]) <<"    adjusted frame pointer within " <<callStack.top().function()->printableName()
                                         <<" delta = " <<StringUtility::signedToHex2(delta, FP.nBits()) <<"\n";
            }
        }
    }

    Super::writeRegister(reg, value);
}

BS::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &dflt_,
                          const BS::SValue::Ptr &cond) {
    ASSERT_not_null(addr);
    ASSERT_not_null(dflt_);
    ASSERT_not_null(cond);

    BS::SValue::Ptr dflt = dflt_;
    if (cond->isFalse())
        return dflt;

    // Offset the address by the value of the segment register
    BS::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = addr;
    } else {
        BS::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(addr, signExtend(segregValue, addr->nBits()));
    }

    // Check the model and throw exception if violated.
    checkNullAccess(adjustedVa, settings_.nullRead, IoMode::READ);
    checkOobAccess(adjustedVa, settings_.oobRead, IoMode::READ, dflt->nBits() / 8);

    // Read from memory map. If we know the address and that memory exists, then read the memory to obtain the default value
    // which we'll use to update the symbolic state in a moment.
    ASSERT_require(0 == dflt->nBits() % 8);
    const size_t nBytes = dflt->nBits() / 8;
    uint8_t buf[8];
    if (adjustedVa->toUnsigned() && nBytes <= sizeof(buf) &&
        nBytes == partitioner_.memoryMap()->at(adjustedVa->toUnsigned().get()).limit(nBytes).read(buf).size()) {
        switch (partitioner_.memoryMap()->byteOrder()) {
            case ByteOrder::ORDER_UNSPECIFIED:
            case ByteOrder::ORDER_LSB: {
                uint64_t value = 0;
                for (size_t i=0; i<nBytes; ++i)
                    value |= (uint64_t)buf[i] << (8*i);
                dflt = number_(dflt->nBits(), value);
                break;
            }

            case ByteOrder::ORDER_MSB: {
                uint64_t value = 0;
                for (size_t i=0; i<nBytes; ++i)
                    value = (value << 8) | (uint64_t)buf[i];
                dflt = number_(dflt->nBits(), value);
                break;
            }
        }
    }

    // Read from the symbolic state, and update the state with the default from real memory if known.
    return assignRegion(Super::readMemory(segreg, addr, dflt, cond));
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &value,
                           const BS::SValue::Ptr &cond) {
    if (cond->isFalse())
        return;

    // Offset the address by the value of the segment register
    BS::SValue::Ptr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = addr;
    } else {
        BS::SValue::Ptr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(addr, signExtend(segregValue, addr->nBits()));
    }

    // Check the model and throw exception if violated.
    checkNullAccess(adjustedVa, settings_.nullWrite, IoMode::WRITE);
    checkOobAccess(adjustedVa, settings_.oobWrite, IoMode::WRITE, value->nBits() / 8);

    Super::writeMemory(segreg, addr, value, cond);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// High-level semantic operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SemanticCallbacks::SemanticCallbacks(const ModelChecker::Settings::Ptr &mcSettings, const Settings &settings,
                                     const P2::Partitioner &partitioner)
    : ModelChecker::SemanticCallbacks(mcSettings), settings_(settings), partitioner_(partitioner) {

    variableFinder_ = Variables::VariableFinder::instance();

    if (!settings_.initialStackVa) {
        // Choose an initial stack pointer that's unlikely to interfere with instructions or data.
        static const size_t RESERVE_BELOW  = 15*1024*1024;          // memory to reserve below the initial stack inter
        static const size_t RESERVE_ABOVE   =  1*1024*1024;         // memory reserved at and above the initial stack pointer
        static const size_t STACK_ALIGNMENT = 16;                   // alignment in bytes
        auto where = AddressInterval::hull(0x80000000, 0xffffffff); // where to look in the address space

        Sawyer::Optional<rose_addr_t> va =
            partitioner.memoryMap()->findFreeSpace(RESERVE_BELOW + RESERVE_ABOVE, STACK_ALIGNMENT, where,
                                                   Sawyer::Container::MATCH_BACKWARD);
        if (!va) {
            mlog[ERROR] <<"no room for a stack anywhere in " <<StringUtility::addrToString(where) <<"\n"
                        <<"falling back to abstract stack address\n";
        } else {
            settings_.initialStackVa = *va + RESERVE_BELOW;
            SAWYER_MESG(mlog[INFO]) <<"initial stack pointer = " <<StringUtility::addrToString(*settings_.initialStackVa) <<"\n";
        }
    }
}

SemanticCallbacks::~SemanticCallbacks() {}

SemanticCallbacks::Ptr
SemanticCallbacks::instance(const ModelChecker::Settings::Ptr &mcSettings, const Settings &settings,
                            const P2::Partitioner &partitioner) {
    return Ptr(new SemanticCallbacks(mcSettings, settings, partitioner));
}

SmtSolver::Memoizer::Ptr
SemanticCallbacks::smtMemoizer() const {
    return smtMemoizer_;
}

void
SemanticCallbacks::smtMemoizer(const SmtSolver::Memoizer::Ptr &memoizer) {
    smtMemoizer_ = memoizer;
}

void
SemanticCallbacks::followOnePath(const std::list<ExecutionUnitPtr> &units) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    onePath_ = units;
    followingOnePath_ = true;
}

bool
SemanticCallbacks::followingOnePath() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return followingOnePath_;
}

void
SemanticCallbacks::followingOnePath(bool b) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (!b)
        onePath_.clear();
    followingOnePath_ = b;
}

void
SemanticCallbacks::reset() {
    seenStates_.clear();
    nDuplicateStates_ = 0;
    nSolverFailures_ = 0;
    unitsReached_.clear();
    units_.clear();                                     // perhaps not necessary
}

size_t
SemanticCallbacks::nDuplicateStates() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return nDuplicateStates_;
}

size_t
SemanticCallbacks::nSolverFailures() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return nSolverFailures_;
}

const P2::Partitioner&
SemanticCallbacks::partitioner() const {
    return partitioner_;
}

BS::SValue::Ptr
SemanticCallbacks::protoval() {
    return SValue::instance();
}

BS::RegisterState::Ptr
SemanticCallbacks::createInitialRegisters() {
    return BS::RegisterStateGeneric::instance(protoval(), partitioner_.instructionProvider().registerDictionary());
}

BS::MemoryState::Ptr
SemanticCallbacks::createInitialMemory() {
    BS::MemoryState::Ptr mem;
    switch (settings_.memoryType) {
        case Settings::MemoryType::LIST:
            mem = IS::SymbolicSemantics::MemoryListState::instance(protoval(), protoval());
            break;
        case Settings::MemoryType::MAP:
            mem = IS::SymbolicSemantics::MemoryMapState::instance(protoval(), protoval());
            break;
    }
    mem->set_byteOrder(partitioner_.instructionProvider().defaultByteOrder());
    return mem;
}

BS::State::Ptr
SemanticCallbacks::createInitialState() {
    BS::RegisterState::Ptr registers = createInitialRegisters();
    BS::MemoryState::Ptr memory = createInitialMemory();
    return State::instance(registers, memory);
}

BS::RiscOperators::Ptr
SemanticCallbacks::createRiscOperators() {
    auto ops = RiscOperators::instance(settings_, partitioner_, this, protoval(), SmtSolver::Ptr(), variableFinder_);
    ops->trimThreshold(mcSettings()->maxSymbolicSize);      // zero means no limit
    ops->initialState(nullptr);
    ops->currentState(nullptr);
    ops->solver(SmtSolver::Ptr());
    ops->computeMemoryRegions(settings_.oobRead != TestMode::OFF || settings_.oobWrite != TestMode::OFF);

    if (settings_.traceSemantics) {
        auto trace = IS::TraceSemantics::RiscOperators::instance(ops);
        trace->stream(mlog[DEBUG]);
        trace->indentation("        ");
        return trace;
    } else {
        return ops;
    }
}

void
SemanticCallbacks::initializeState(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
    if (settings_.initialStackVa) {
        BS::SValue::Ptr sp = ops->number_(SP.nBits(), *settings_.initialStackVa);
        ops->writeRegister(SP, sp);
    }
}

BS::Dispatcher::Ptr
SemanticCallbacks::createDispatcher(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    ASSERT_not_null2(partitioner_.instructionProvider().dispatcher(), "no semantics for this ISA");
    return partitioner_.instructionProvider().dispatcher()->create(ops);
}

SmtSolver::Ptr
SemanticCallbacks::createSolver() {
    std::string solverName = Rose::CommandLine::genericSwitchArgs.smtSolver;
    if (solverName.empty() || "none" == solverName)
        solverName = "best";
    auto solver = SmtSolver::instance(solverName);

    if (settings_.solverMemoization) {
        if (!smtMemoizer_)
            smtMemoizer_ = SmtSolver::Memoizer::instance();
        solver->memoizer(smtMemoizer_);
    }

    if (mcSettings()->solverTimeout)
        solver->timeout(boost::chrono::seconds(*mcSettings()->solverTimeout));

    return solver;
}

void
SemanticCallbacks::attachModelCheckerSolver(const BS::RiscOperators::Ptr &ops, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(ops);
    RiscOperators::promote(ops)->modelCheckerSolver(solver);
}

std::vector<Tag::Ptr>
SemanticCallbacks::preExecute(const ExecutionUnit::Ptr &unit, const BS::RiscOperators::Ptr &ops) {
    RiscOperators::promote(ops)->nInstructions(0);

    // Track which basic blocks (or instructions) were reached
    if (auto va = unit->address()) {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        ++unitsReached_.insertMaybe(*va, 0);
    }

    // Debugging
    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"  function call stack:\n";
        const FunctionCallStack &callStack = State::promote(ops->currentState())->callStack();
        const size_t nBits = partitioner_.instructionProvider().wordSize();
        const rose_addr_t mask = BitOps::lowMask<rose_addr_t>(nBits);
        for (size_t i = callStack.size(); i > 0; --i) { // print from earliest to most recent record, i.e., bottom up
            const FunctionCall &call = callStack[i-1];
            mlog[DEBUG] <<"    " <<call.function()->printableName() <<"\n";
            mlog[DEBUG] <<"      initial stack pointer = " <<StringUtility::addrToString(call.initialStackPointer()) <<"\n";
            mlog[DEBUG] <<"      frame pointer delta   = " <<StringUtility::toHex(call.framePointerDelta()) <<"\n";
            const rose_addr_t framePointer = (call.initialStackPointer() + call.framePointerDelta()) & mask;
            mlog[DEBUG] <<"      frame pointer         = " <<StringUtility::addrToString(framePointer) <<"\n";
            for (const Variables::StackVariable &var: call.stackVariables().values()) {
                const rose_addr_t varVa = (framePointer + (rose_addr_t)var.interval().least()) & mask;
                ASSERT_require(var.maxSizeBytes() > 0);
                const rose_addr_t varSize = std::min(mask-varVa, var.maxSizeBytes()-1) + 1; // careful for overflow
                const AddressInterval varLoc = AddressInterval::baseSize(varVa, varSize);
                mlog[DEBUG] <<"        va " <<StringUtility::addrToString(varLoc) <<" is " <<var.toString() <<"\n";
            }
        }
        if (callStack.isEmpty())
            mlog[DEBUG] <<"    empty\n";
    }

    return {};
}

size_t
SemanticCallbacks::nUnitsReached() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return unitsReached_.size();
}

SemanticCallbacks::UnitCounts
SemanticCallbacks::unitsReached() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return unitsReached_;
}

BS::SValue::Ptr
SemanticCallbacks::instructionPointer(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    const RegisterDescriptor IP = partitioner_.instructionProvider().instructionPointerRegister();
    return ops->peekRegister(IP);
}

bool
SemanticCallbacks::seenState(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    ASSERT_not_null(ops->currentState());
    if (mcSettings()->exploreDuplicateStates) {
        return false;
    } else {
        Combinatorics::HasherSha256Builtin hasher;      // we could use a faster one as long as we don't get false matches
        ops->hash(hasher);

        // Some hashers (including SHA256) have digests that are wider than 64 bits, but we don't really want to throw anything
        // away. So we'll just fold all the bits into the 64 bit value with XOR.
        uint64_t hash = 0;
        for (uint8_t byte: hasher.digest())
            hash = BitOps::rotateLeft(hash, 8) ^ uint64_t{byte};
        SAWYER_MESG(mlog[DEBUG]) <<"  state hash = " <<StringUtility::addrToString(hash) <<"\n";

        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        bool seen = !seenStates_.insert(hash).second;
        if (seen)
            ++nDuplicateStates_;
        return seen;
    }
}

ExecutionUnit::Ptr
SemanticCallbacks::findUnit(rose_addr_t va, const Progress::Ptr &progress) {
    ExecutionUnit::Ptr unit;

    // If we're following one path, then the execution unit is always the next one on the path.
    {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        if (followingOnePath_) {
            if (!onePath_.empty()) {
                unit = onePath_.front();
                onePath_.pop_front();
            }
        }
    }

    if (unit) {
        // We took it from the one path we're following (see above)
        if (*unit->address() != va) {
            SAWYER_MESG(mlog[DEBUG]) <<"next follow-one-path execution unit address mismatch:"
                                     <<" expected " <<StringUtility::addrToString(va)
                                     <<" but has " <<StringUtility::addrToString(*unit->address()) <<"\n";
            return ExecutionUnit::Ptr();
        }
        return unit;
    }


    {
        // We use a separate mutex for the unit_ cache so that only one thread computes this at a time without blocking
        // threads trying to make progress on other things.  An alternative would be to lock the main mutex, but only when
        // accessing the cache, and allow the computations to be duplicated with only the first thread saving the result.
        SAWYER_THREAD_TRAITS::LockGuard lock(unitsMutex_);
        unit = units_.getOrDefault(va);
    }

    if (unit)
        return unit;                                    // preexisting

    // Compute the next unit
    P2::Function::Ptr func = partitioner_.functionExists(va);
    if (func && boost::ends_with(func->name(), "@plt")) {
        unit = ExternalFunctionUnit::instance(func, partitioner_.sourceLocations().get(va));
    } else if (P2::BasicBlock::Ptr bb = partitioner_.basicBlockExists(va)) {
        // Depending on partitioner settings, sometimes a basic block could have an internal loop. For instance, some x86
        // instructions have an optional repeat prefix. Since execution units need to know how many steps they are (so we can
        // inforce the K limit), and since the content of the execution unit needs to be immutable, and since we can't really
        // re-compute path successors with new data (we would have already thrown away the outgoing state), we have to make a
        // decision here and now.
        //
        // So we evaluate each instruction semantically on a clean state and check that the instruction pointer follows the
        // instrucitons in the basic block.  If not, we switch to one-instruction-at-a-time mode for this basic block, which is
        // about half as fast and takes more memory.  By caching our results, we only do this expensive calcultion once per
        // basic block, not each time a path reaches this block.
        if (bb->nInstructions() > 0) {
            ProgressTask task(progress, "findNext");

            // We intentionally don't use an SMT solver here because it's not usually needed--we're only processing a single
            // basic block. If the Z3 solver is used here, we find that Z3 eventually hangs during the z3check call on some
            // threads even when we tell it to time out after a few seconds.
            auto ops = partitioner_.newOperators(P2::MAP_BASED_MEMORY);
            IS::SymbolicSemantics::RiscOperators::promote(ops)->trimThreshold(mcSettings()->maxSymbolicSize);
            auto cpu = partitioner_.newDispatcher(ops);
            const RegisterDescriptor IP = partitioner_.instructionProvider().instructionPointerRegister();
            for (size_t i = 0; i < bb->nInstructions() - 1; ++i) {
                SgAsmInstruction *insn = bb->instructions()[i];
                try {
                    cpu->processInstruction(insn);
                } catch (...) {
                    SAWYER_MESG(mlog[DEBUG]) <<"    " <<bb->printableName() <<" at " <<insn->toString() <<"; switched to insn\n";
                    unit = BasicBlockUnit::instance(partitioner_, bb);
                    break;
                }
                BS::SValue::Ptr actualIp = ops->peekRegister(IP);
                rose_addr_t expectedIp = bb->instructions()[i+1]->get_address();
                if (actualIp->toUnsigned().orElse(expectedIp+1) != expectedIp) {
                    SAWYER_MESG(mlog[DEBUG]) <<"    " <<bb->printableName()
                                             <<" sequence error after " <<insn->toString() <<"; switched to insn\n";
                    unit = InstructionUnit::instance(bb->instructions()[0], partitioner_.sourceLocations().get(va));
                    break;
                }
            }
        }
        if (!unit && bb->nInstructions() > 0)
            unit = BasicBlockUnit::instance(partitioner_, bb);
    } else if (SgAsmInstruction *insn = partitioner_.instructionProvider()[va]) {
        SAWYER_MESG(mlog[DEBUG]) <<"    no basic block at " <<StringUtility::addrToString(va) <<"; switched to insn\n";
        unit = InstructionUnit::instance(insn, partitioner_.sourceLocations().get(va));
    }

    // Save the result, but if some other thread beat us to this point, use their result instead.
    if (unit) {
        SAWYER_THREAD_TRAITS::LockGuard lock(unitsMutex_);
        unit = units_.insertMaybe(va, unit);
    }

    return unit;
}

SemanticCallbacks::CodeAddresses
SemanticCallbacks::nextCodeAddresses(const BS::RiscOperators::Ptr &ops) {
    ExecutionUnit::Ptr nextUnit;
    {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        if (followingOnePath_) {
            if (onePath_.empty()) {
                SAWYER_MESG(mlog[DEBUG]) <<"reached end of predetermined path\n";
                return CodeAddresses();
            }
            nextUnit = onePath_.front();
        }
    }

    if (nextUnit) {
        CodeAddresses retval;
        rose_addr_t va = *onePath_.front()->address();
        retval.ip = instructionPointer(ops);
        retval.addresses.insert(va);
        retval.isComplete = true;
        return retval;

    } else {
        return ModelChecker::SemanticCallbacks::nextCodeAddresses(ops); // delegate to parent class
    }
}

std::vector<SemanticCallbacks::NextUnit>
SemanticCallbacks::nextUnits(const Path::Ptr &path, const BS::RiscOperators::Ptr &ops, const SmtSolver::Ptr &solver) {
    std::vector<SemanticCallbacks::NextUnit> units;
    ProgressTask task(solver->progress(), "nextUnits");

    // If we've seen this state before, then there's nothing new for us to do.
    if (seenState(ops)) {
        SAWYER_MESG(mlog[DEBUG]) <<"  not extending path since state was previously seen\n";
        return {};
    }

    // Find next concrete addresses from instruction pointer symbolic expression.
    CodeAddresses next = nextCodeAddresses(ops);
    if (!next.ip)
        return {};
    auto ip = IS::SymbolicSemantics::SValue::promote(next.ip)->get_expression();
    if (!next.isComplete) {
        auto tag = ErrorTag::instance(0, "abstract jump", "symbolic address not handled yet", nullptr, ip);
        tag->importance(WARN);
        auto fail = FailureUnit::instance(Sawyer::Nothing(), SourceLocation(), "no concrete instruction pointer", tag);
        units.push_back({fail, SymbolicExpr::makeBooleanConstant(true)});
        return units;
    }

    // Create execution units for the next concrete addresses
    for (rose_addr_t va: next.addresses) {
        // Test whether next address is feasible with the given previous path assertions
        SmtSolver::Transaction tx(solver);
        auto assertion = SymbolicExpr::makeEq(ip, SymbolicExpr::makeIntegerConstant(ip->nBits(), va));
        solver->insert(assertion);

        switch (solver->check()) {
            case SmtSolver::SAT_YES:
                // Create the next execution unit
                if (ExecutionUnit::Ptr unit = findUnit(va, solver->progress())) {
                    units.push_back({unit, assertion, solver->evidenceByName()});
                } else if (settings_.nullRead != TestMode::OFF && va <= settings_.maxNullAddress) {
                    SourceLocation sloc = partitioner_.sourceLocations().get(va);
                    BS::SValue::Ptr addr = ops->number_(partitioner_.instructionProvider().wordSize(), va);
                    auto tag = NullDerefTag::instance(0, TestMode::MUST, IoMode::READ, nullptr, addr);
                    auto fail = FailureUnit::instance(va, sloc, "invalid instruction address", tag);
                    units.push_back({fail, assertion, SmtSolver::Evidence()});
                } else {
                    SourceLocation sloc = partitioner_.sourceLocations().get(va);
                    auto tag = ErrorTag::instance(0, "invalid instruction pointer", "no instruction at address", nullptr, va);
                    units.push_back({FailureUnit::instance(va, sloc, "invalid instruction address", tag), assertion,
                                     SmtSolver::Evidence()});
                }
                break;
            case SmtSolver::SAT_NO:
                // Do not extend the current path in this direction since it is infeasible.
                break;
            case SmtSolver::SAT_UNKNOWN: {
                // The SMT solver failed. This is could be a timeout or some other failure.
                SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
                ++nSolverFailures_;
                break;
            }
        }
    }
    return units;
}

bool
SemanticCallbacks::filterNullDeref(const BS::SValue::Ptr &addr, SgAsmInstruction *insn, TestMode testMode, IoMode ioMode) {
    return true;
}

bool
SemanticCallbacks::filterOobAccess(const BS::SValue::Ptr &addr, const AddressInterval &referencedRegion,
                                   const AddressInterval &accessedRegion, SgAsmInstruction *insn, TestMode testMode,
                                   IoMode ioMode, const Variables::StackVariable &intendedVariable,
                                   const AddressInterval &intendedVariableLocation,
                                   const Variables::StackVariable &accessedVariable,
                                   const AddressInterval &accessedVariableLocation) {
    return true;
}

#ifdef ROSE_HAVE_LIBYAML
std::list<ExecutionUnit::Ptr>
SemanticCallbacks::parsePath(const YAML::Node &root, const std::string &sourceName) {
    std::list<ExecutionUnit::Ptr> retval;

    if (!root.IsSequence() || root.size() == 0)
        throw ParseError(sourceName, "a path must be a non-empty sequence of path nodes");

    for (size_t i = 0; i < root.size(); ++i) {
        const std::string where = "path vertex #" + boost::lexical_cast<std::string>(i) + " ";
        if (!root[i].IsMap() || !root[i]["vertex-type"])
            throw ParseError(sourceName, where + "is not an object with a \"vertex-type\" field");

        std::string vertexType = root[i]["vertex-type"].as<std::string>();
        if ("basic-block" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (P2::BasicBlock::Ptr bb = partitioner().basicBlockExists(va)) {
                retval.push_back(BasicBlockUnit::instance(partitioner(), bb));
            } else {
                throw ParseError(sourceName, where + "no such basic block at " + StringUtility::addrToString(va));
            }

        } else if ("instruction" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (SgAsmInstruction *insn = partitioner().instructionProvider()[va]) {
                retval.push_back(InstructionUnit::instance(insn, partitioner().sourceLocations().get(va)));
            } else {
                throw ParseError(sourceName, where + "no instruction at " + StringUtility::addrToString(va));
            }

        } else if ("extern-function" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (P2::Function::Ptr function = partitioner().functionExists(va)) {
                retval.push_back(ExternalFunctionUnit::instance(function, partitioner().sourceLocations().get(va)));
            } else {
                throw ParseError(sourceName, where + "no function at " + StringUtility::addrToString(va));
            }

        } else {
            throw ParseError(sourceName, where + "has unrecognized type \"" + StringUtility::cEscape(vertexType) + "\"");
        }
    }
    return retval;
}
#endif

} // namespace
} // namespace
} // namespace
} // namespace

#endif
