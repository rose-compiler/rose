#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/PartitionerModel.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/ModelChecker/BasicBlockUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/ErrorTag.h>
#include <Rose/BinaryAnalysis/ModelChecker/Exception.h>
#include <Rose/BinaryAnalysis/ModelChecker/ExternalFunctionUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/FailureUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/InstructionUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/NullDereferenceTag.h>
#include <Rose/BinaryAnalysis/ModelChecker/OutOfBoundsTag.h>
#include <Rose/BinaryAnalysis/ModelChecker/Path.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/ModelChecker/UninitializedVariableTag.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BitOps.h>
#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Escape.h>
#include <Combinatorics.h>                              // ROSE

#include <SgAsmInstruction.h>

#include <boost/algorithm/string/predicate.hpp>
#include <chrono>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {
namespace PartitionerModel {

void
commandLineDebugSwitches(Sawyer::CommandLine::SwitchGroup &sg, Settings &settings) {
    using Rose::CommandLine::insertBooleanSwitch;
    insertBooleanSwitch(sg, "debug-null-deref", settings.debugNull,
                        "If this feature is enabled, then the diagnostic output (if it is also enabled) "
                        "will include information about null pointer dereference checking, such as the "
                        "symbolic address being checked.");

    insertBooleanSwitch(sg, "debug-semantic-operations", settings.traceSemantics,
                        "If enabled, then each low-level instruction semantic operation is emitted to the diagnostic output "
                        "(if it is also enabled).");
}

Sawyer::CommandLine::SwitchGroup
commandLineModelSwitches(Settings &settings) {
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

    sg.insert(Switch("detect-uninitialized-vars")
              .argument("method", enumParser<TestMode>(settings.uninitVar)
                        ->with("off", TestMode::OFF)
                        ->with("may", TestMode::MAY)
                        ->with("must", TestMode::MUST),
                        "must")
              .doc("Method by which to test for uninitialized variable reads. Memory addresses are tested during write operations "
                   "to determine if the address is a variable that has not been initialized yet. If no switch argument is "
                   "supplied, then \"must\" is assumed. The arguments are:"

                   "@named{off}{Do not check for uninitialized variable reads." +
                   std::string(TestMode::OFF == settings.uninitVar ? " This is the default." : "") + "}"

                   "@named{may}{Find cases where an uninitialized variable might be read." +
                   std::string(TestMode::MAY == settings.uninitVar ? " This is the default." : "") + "}"

                   "@named{must}{Find cases where an uninitialized variable read must necessarily occur." +
                   std::string(TestMode::MUST == settings.uninitVar ? " This is the default." : "") + "}"));

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

    insertBooleanSwitch(sg, "solver-memoization", settings.solverMemoization,
                        "Causes the SMT solvers (per thread) to memoize their results. In other words, they remember the "
                        "sets of assertions and if the same set appears a second time it will return the same answer as the "
                        "first time without actually calling the SMT solver.  This can sometimes reduce the amount of time "
                        "spent solving, but uses more memory.");

    return sg;
}

Sawyer::CommandLine::SwitchGroup
commandLineSwitches(Settings &settings) {
    Sawyer::CommandLine::SwitchGroup sg = commandLineModelSwitches(settings);
    commandLineDebugSwitches(sg, settings);
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
                if (varAddrs.contains(location)) {
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

// Return the global variable at the specified location, but only if the variable contains the entire location.
static Variables::GlobalVariable
findGlobalVariable(const Variables::GlobalVariables &gvars, const AddressInterval &location) {
    if (location) {
        if (Variables::GlobalVariable gvar = gvars.getOrDefault(location.least())) {
            if (gvar.interval().contains(location))
                return gvar;
        }
    }
    return {};
}

// Find a global variable or a local variable.
static FoundVariable
findVariable(const AddressInterval &location, const Variables::GlobalVariables &gvars, const FunctionCallStack &callStack,
             const AddressInterval &stackLimits) {
    if (Variables::GlobalVariable gvar = findGlobalVariable(gvars, location))
        return FoundVariable(gvar);

    auto whereWhat = findStackVariable(callStack, location, stackLimits);
    if (whereWhat.first)
        return FoundVariable(whereWhat.first, whereWhat.second);

    return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function call stack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionCall::FunctionCall(const Partitioner2::Function::Ptr &function, rose_addr_t initialStackPointer,
                           Sawyer::Optional<rose_addr_t> returnAddress, const Variables::StackVariables &vars)
    : function_(function), initialStackPointer_(initialStackPointer), returnAddress_(returnAddress), stackVariables_(vars) {}

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

rose_addr_t
FunctionCall::framePointer(size_t nBits) const {
    rose_addr_t mask = BitOps::lowMask<rose_addr_t>(nBits);
    return (initialStackPointer_ + framePointerDelta_) & mask;
}

Sawyer::Optional<rose_addr_t>
FunctionCall::returnAddress() const {
    return returnAddress_;
}

void
FunctionCall::returnAddress(Sawyer::Optional<rose_addr_t> va) {
    returnAddress_ = va;
}

std::string
FunctionCall::printableName(size_t nBits) const {
    return function()->printableName() +
        " initSp=" + StringUtility::addrToString(initialStackPointer_) +
        " frame=" + StringUtility::addrToString(framePointer(nBits)) +
        " retVa=" + StringUtility::addrToString(returnAddress_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::SValue() {}

SValue::~SValue() {}

SValue::SValue(size_t nBits, uint64_t number)
    : Super(nBits, number) {}

SValue::SValue(const SymbolicExpression::Ptr &expr)
    : Super(expr) {}

SValue::Ptr
SValue::instance() {
    return Ptr(new SValue);
}

SValue::Ptr
SValue::instanceBottom(size_t nBits) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nBits, "", SymbolicExpression::Node::BOTTOM)));
}

SValue::Ptr
SValue::instanceUndefined(size_t nBits) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nBits)));
}

SValue::Ptr
SValue::instanceUnspecified(size_t nBits) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerVariable(nBits, "", SymbolicExpression::Node::UNSPECIFIED)));
}

SValue::Ptr
SValue::instanceInteger(size_t nBits, uint64_t value) {
    return Ptr(new SValue(SymbolicExpression::makeIntegerConstant(nBits, value)));
}

SValue::Ptr
SValue::instanceSymbolic(const SymbolicExpression::Ptr &value) {
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
SValue::createOptionalMerge(const BS::SValue::Ptr &/*other*/, const BS::Merger::Ptr&, const SmtSolver::Ptr&) const {
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

RiscOperators::RiscOperators(const Settings &settings, const P2::Partitioner::ConstPtr &partitioner,
                             ModelChecker::SemanticCallbacks *semantics, const BS::SValue::Ptr &protoval,
                             const SmtSolver::Ptr &solver, const Variables::VariableFinder::Ptr &varFinder,
                             const Variables::GlobalVariables &gvars)
    : Super(protoval, solver), settings_(settings), partitioner_(partitioner),
      semantics_(dynamic_cast<PartitionerModel::SemanticCallbacks*>(semantics)), gvars_(gvars),
      variableFinder_unsync(varFinder) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(semantics_);
    ASSERT_not_null(variableFinder_unsync);
    (void)SValue::promote(protoval);
    name("PartitionerModel");

    // Calculate the stack limits.  Start by assuming the stack can occupy all of memory. Then make that smaller based on
    // other information.
    const size_t nBits = partitioner->instructionProvider().instructionPointerRegister().nBits();
    stackLimits_ = AddressInterval::hull(0, BitOps::lowMask<rose_addr_t>(nBits));
    if (settings_.initialStackVa) {
        // The stack should extend down to the top of the next lower mapped address.
        ASSERT_forbid(partitioner->memoryMap()->at(*settings_.initialStackVa).exists());
        if (auto prev = partitioner->memoryMap()->atOrBefore(*settings_.initialStackVa).next(Sawyer::Container::MATCH_BACKWARD))
            stackLimits_ = stackLimits_ & AddressInterval::hull(*prev + 1, stackLimits_.greatest());

        // The stack should extend up to the bottom of the next higher mapped address.
        if (auto next = partitioner->memoryMap()->atOrAfter(*settings_.initialStackVa).next())
            stackLimits_ = stackLimits_ & AddressInterval::hull(stackLimits_.least(), *next - 1);

        // The stack should extend only a few bytes above it's initial location.
        stackLimits_ = stackLimits_ & AddressInterval::hull(stackLimits_.least(),
                                                            saturatedAdd(*settings_.initialStackVa, 256 /*arbitrary*/).first);
    }
    SAWYER_MESG(mlog[DEBUG]) <<"stack limited to " <<StringUtility::addrToString(stackLimits_) <<"\n";
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instance(const Settings &settings, const P2::Partitioner::ConstPtr &partitioner,
                        ModelChecker::SemanticCallbacks *semantics, const BS::SValue::Ptr &protoval, const SmtSolver::Ptr &solver,
                        const Variables::VariableFinder::Ptr &varFinder, const Variables::GlobalVariables &gvars) {
    ASSERT_not_null(protoval);
    return Ptr(new RiscOperators(settings, partitioner, semantics, protoval, solver, varFinder, gvars));
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

Partitioner2::Partitioner::ConstPtr
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
    SymbolicExpression::Ptr addr = IS::SymbolicSemantics::SValue::promote(addrSVal)->get_expression();
    const char *direction = IoMode::READ == ioMode ? "read" : "write";
    ProgressTask task(modelCheckerSolver_->progress(), "nullptr");

    bool isNull = false;
    switch (testMode) {
        case TestMode::OFF:
            break;

        case TestMode::MAY: {
            // May be null if addr <= max is satisfied
            SymbolicExpression::Ptr maxNullExpr = SymbolicExpression::makeIntegerConstant(addr->nBits(), settings_.maxNullAddress);
            SymbolicExpression::Ptr isNullExpr = SymbolicExpression::makeLe(addr, maxNullExpr);
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
            SymbolicExpression::Ptr maxNullExpr = SymbolicExpression::makeIntegerConstant(addr->nBits(), settings_.maxNullAddress);
            SymbolicExpression::Ptr isNullExpr = SymbolicExpression::makeGt(addr, maxNullExpr);
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
        throw ThrownTag{NullDereferenceTag::instance(nInstructions(), testMode, ioMode, currentInstruction(), addrSVal)};
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

        // The address must be concrete and it must have an attached region that does not fully contain the memory bytes being
        // accessed they the address (based on the address itself and the `nBytes` parameter.
        const auto va = addrSVal->toUnsigned();
        if (!va)
            return;
        const AddressInterval referencedRegion = addrSVal->region();
        if (!referencedRegion)
            return;
        const AddressInterval accessedRegion = AddressInterval::baseSizeSat(*va, nBytes);
        ProgressTask task(modelCheckerSolver_->progress(), "oob");
        if (referencedRegion.contains(accessedRegion))
            return;                                     // in bounds

        // The region attached to the address expression is the region for the variable that was intended to be accessed.  This
        // variable must exist and have been detected earlier by the variable analysis.
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        const FoundVariable intendedVariable = findVariable(referencedRegion, gvars_, callStack, stackLimits_);
        if (!intendedVariable)
            return;

        // The region actually accessed might be the address for a known source code variable that was detected earlier by the
        // variable analysis. If we can't find a whole variable, try the individual addresses. It's okay if we don't find
        // anything, which just means that we had an OOB access for the intended variable that doesn't land in any particular
        // actual variable.
        const FoundVariable accessedVariable = [this, &accessedRegion, &callStack]() {
            if (auto v = findVariable(accessedRegion, gvars_, callStack, stackLimits_))
                return v;
            for (rose_addr_t accessedByteVa: accessedRegion) {
                if (auto v = findVariable(accessedByteVa, gvars_, callStack, stackLimits_))
                    return v;
            }
            return FoundVariable();
        }();

        // We've found an out-of-bounds access, but is it significant from the user's point of view?
        if (!semantics_->filterOobAccess(addrSVal, referencedRegion, accessedRegion, currentInstruction(), testMode, ioMode,
                                         intendedVariable, accessedVariable)) {
            SAWYER_MESG(mlog[DEBUG]) <<"      buffer overflow rejected by user; this one is ignored\n";
            return;
        }

        // We've found a significant out-of-bounds access. Report it.
        currentState(BS::State::Ptr());                 // indicates that execution failed
        throw ThrownTag{OutOfBoundsTag::instance(nInstructions(), testMode, ioMode, currentInstruction(), addrSVal,
                                                 intendedVariable, accessedVariable)};
    }
}

void
RiscOperators::checkUninitVar(const BS::SValue::Ptr &addrSVal_, TestMode testMode, size_t nBytes) {
    if (!computeMemoryRegions_)
        return;

    // Uninitialized reads are only tested when we're actually executing an instruction.
    if (!currentInstruction())
        return;
    if (TestMode::OFF == testMode)
        return;

    // The address must be concrete and it must have an attached region that fully contains the memory bytes being accessed
    // by the address (based on the address itself and the `nBytes` parameter.
    const auto addrSVal = SValue::promote(addrSVal_);
    const auto va = addrSVal->toUnsigned();
    if (!va)
        return;
    const AddressInterval referencedRegion = addrSVal->region();
    if (!referencedRegion)
        return;
    const AddressInterval accessedRegion = AddressInterval::baseSize(*va, nBytes);
    if (!referencedRegion.contains(accessedRegion))
        return;

    // In order for the memory being accessed to be considered uninitialized, it must have no writers.
    auto mem = BS::MemoryCellState::promote(currentState()->memoryState());
    if (mem->getWritersUnion(addrSVal, 8*nBytes, this, this))
        return;

    // The region attached to the address must refer to a source code variable that was detected earlier by the variable
    // analysis.
    const FunctionCallStack &callStack = State::promote(currentState())->callStack();
    const FoundVariable variable = findVariable(referencedRegion, gvars_, callStack, stackLimits_);
    if (!variable)
        return;

    // We've found a read of an uninialized variable, but is it significant from the user's point of view?
    if (!semantics_->filterUninitVar(addrSVal, referencedRegion, accessedRegion, currentInstruction(), testMode, variable)) {
        SAWYER_MESG(mlog[DEBUG]) <<"      uninitialized variable rejected by user; this one is ignored\n";
        return;
    }

    // We've found a significant read of an uninitialized variable. Report it.
    currentState(BS::State::Ptr());                 // indicates that execution failed
    throw ThrownTag{UninitializedVariableTag::instance(nInstructions(), testMode, currentInstruction(), addrSVal, variable)};
}

size_t
RiscOperators::nInstructions() const {
    return nInstructions_;
}

void
RiscOperators::nInstructions(size_t n) {
    nInstructions_ = n;
}

void
RiscOperators::maybeInitCallStack(rose_addr_t insnVa) {
    // If the call stack is empty, then push a record for the current function.
    if (computeMemoryRegions_) {
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        if (callStack.isEmpty()) {
            std::vector<P2::Function::Ptr> functions = partitioner_->functionsSpanning(insnVa);
            P2::Function::Ptr function;
            if (functions.empty()) {
                SAWYER_MESG(mlog[WARN]) <<"no function containing instruction at " <<StringUtility::addrToString(insnVa) <<"\n";
            } else if (functions.size() == 1) {
                function = functions[0];
            } else {
                SAWYER_MESG(mlog[WARN]) <<"multiple functions containing instruction at "
                                        <<StringUtility::addrToString(insnVa) <<"\n";
                function = functions[0];                    // arbitrary
            }

            if (function) {
                const RegisterDescriptor SP = partitioner_->instructionProvider().stackPointerRegister();
                const BS::SValue::Ptr spSValue = peekRegister(SP, undefined_(SP.nBits()));
                const rose_addr_t sp = spSValue->toUnsigned().get();      // must be concrete
                pushCallStack(function, sp, Sawyer::Nothing());
            }
        }
    }
}

void
RiscOperators::pushCallStack(const P2::Function::Ptr &callee, rose_addr_t initialSp, Sawyer::Optional<rose_addr_t> returnVa) {
    if (computeMemoryRegions_) {
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        const size_t nBits = partitioner_->instructionProvider().wordSize();

        while (!callStack.isEmpty() && callStack.top().initialStackPointer() <= initialSp) {
            const FunctionCall &fcall = callStack.top();
            SAWYER_MESG(mlog[DEBUG]) <<"      returned from " <<fcall.printableName(nBits) <<"\n";
            callStack.pop();
        }

        // VariableFinder API is not thread safe, so we need to protect it
        Variables::StackVariables lvars;
        Sawyer::Optional<uint64_t> frameSize;
        {
            SAWYER_THREAD_TRAITS::LockGuard lock(variableFinderMutex_);
            lvars = variableFinder_unsync->findStackVariables(partitioner_, callee);
            frameSize = variableFinder_unsync->detectFrameAttributes(partitioner_, callee).size;
        }
        callStack.push(FunctionCall(callee, initialSp, returnVa, lvars));
        const std::string isa = partitioner_->instructionProvider().disassembler()->name();
        if ("a32" == isa || "t32" == isa || "a64" == isa || "coldfire" == isa) {
            callStack[0].framePointerDelta(-4);
        } else if (frameSize) {
            callStack[0].framePointerDelta(-*frameSize);
        }

        SAWYER_MESG(mlog[DEBUG]) <<"      called " <<callStack[0].printableName(nBits) <<"\n";
    }
}

void
RiscOperators::popCallStack() {
    FunctionCallStack &callStack = State::promote(currentState())->callStack();
    ASSERT_forbid(callStack.isEmpty());
    if (mlog[DEBUG]) {
        const FunctionCall &fcall = callStack.top();
        const size_t nBits = partitioner_->instructionProvider().wordSize();
        mlog[DEBUG] <<"    returned from " <<fcall.printableName(nBits) <<"\n";
    }

    Sawyer::Optional<rose_addr_t> poppedInitialSp = callStack.top().initialStackPointer();
    callStack.pop();

    // Predicate to determine whether a memory cell should be discarded. When popping a call from the call stack, we need to
    // discard the stack frame for the function being popped. Assume that the stack grows down, therefore we need to discard
    // any memory address less than the boundary address.
    class IsPopped: public BS::MemoryCell::Predicate {
        SymbolicExpression::Ptr lowBoundaryInclusive;
        SymbolicExpression::Ptr highBoundaryExclusive;
    public:
        size_t nErased = 0;

    public:
        explicit IsPopped(const SymbolicExpression::Ptr &lowBoundaryInclusive, const SymbolicExpression::Ptr &highBoundaryExclusive)
            : lowBoundaryInclusive(lowBoundaryInclusive), highBoundaryExclusive(highBoundaryExclusive) {}

        bool operator()(const BS::MemoryCell::Ptr &cell) {
            SymbolicExpression::Ptr va = SValue::promote(cell->address())->get_expression();
            SymbolicExpression::Ptr stackGe = SymbolicExpression::makeGe(va, lowBoundaryInclusive);
            SymbolicExpression::Ptr stackLt = SymbolicExpression::makeLt(va, highBoundaryExclusive);
            SymbolicExpression::Ptr isDiscardable = SymbolicExpression::makeAnd(stackGe, stackLt);
            bool retval = isDiscardable->mustEqual(SymbolicExpression::makeBooleanConstant(true));
            if (retval) {
                ++nErased;
                SAWYER_MESG(mlog[DEBUG]) <<"        erasing memory cell " <<*cell <<"\n";
            }
            return retval;
        }
    };

    // Figure out what memory addresses need to be erased. For instance, on x86 the caller's CALL instruction pushed a 4-byte
    // return address onto the stack but this address is removed by the callee's RET instruction. Therefore the initial stack
    // pointer for the callee will be off by four -- we need to remove all the stuff pushed by the callee, plus the four byte
    // return value pushed by the caller.
    if (poppedInitialSp) {
        rose_addr_t stackBoundary = *poppedInitialSp;
        const std::string isaName = partitioner_->instructionProvider().disassembler()->name();
        if ("i386" == isaName) {
            // x86 "call" pushes a 4-byte return address that's popped when the function returns. The stack grows down.
            stackBoundary += 4;
        } else if ("amd64" == isaName) {
            // x86-64 "call" pushes an 8-byte return address that's popped when the function returns. Stack grows down.
            stackBoundary += 8;
        } else if (boost::starts_with(isaName, "ppc32") || boost::starts_with(isaName, "ppc64")) {
            // PowerPC function calls don't push a return value.
        } else if ("a32" == isaName || "t32" == isaName || "a64" == isaName) {
            // ARM AArch32 and AArch64 function calls don't push a return value
        } else if ("coldfire" == isaName) {
            // m68k pops the return value from the stack
            stackBoundary += 4;
        } else {
            ASSERT_not_implemented("isaName = " + isaName);
        }

        const size_t wordSize = partitioner_->instructionProvider().stackPointerRegister().nBits();
        ASSERT_require(wordSize > 8 && wordSize <= 64);
        SymbolicExpression::Ptr highBoundaryExclusive = SymbolicExpression::makeIntegerConstant(wordSize, stackBoundary);
        SymbolicExpression::Ptr lowBoundaryInclusive = SymbolicExpression::makeIntegerConstant(wordSize, stackLimits_.least());
        SAWYER_MESG(mlog[DEBUG]) <<"      initial stack pointer = " <<StringUtility::addrToString(*poppedInitialSp) <<"\n"
                                 <<"      erasing memory between " <<*lowBoundaryInclusive <<" (inclusive) and "
                                 <<*highBoundaryExclusive <<" (exclusive)\n";
        auto mem = BS::MemoryCellState::promote(currentState()->memoryState());
        IsPopped predicate(lowBoundaryInclusive, highBoundaryExclusive);
        mem->eraseMatchingCells(predicate);
        if (mlog[DEBUG] && predicate.nErased > 0) {
            BS::Formatter fmt;
            fmt.set_line_prefix("        ");
            mlog[DEBUG] <<"      memory state after erasures:\n" <<(*mem + fmt);
        }
    }
}

size_t
RiscOperators::pruneCallStack() {
    size_t nPopped = 0;
    if (computeMemoryRegions_) {
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        const size_t nBits = partitioner_->instructionProvider().wordSize();

        // Pop functions whose initial stack pointer is beyond the end of the current stack.
        const RegisterDescriptor SP = partitioner_->instructionProvider().stackPointerRegister();
        auto sp = peekRegister(SP, undefined_(SP.nBits()))->toUnsigned();
        if (sp) {
            while (!callStack.isEmpty() && callStack.top().initialStackPointer() < *sp) {
                const FunctionCall &fcall = callStack.top();
                SAWYER_MESG(mlog[DEBUG]) <<"    popping " <<fcall.printableName(nBits)
                                         <<" because its initial stack pointer is beyond the current SP "
                                         <<StringUtility::addrToString(*sp) <<"\n";
                popCallStack();
                ++nPopped;
            }
        }

        // Pop a function whose initial stack pointer is equal to the current stack pointer and whose return-to address
        // is equal to the current instruction pointer.
        if (!callStack.isEmpty()) {
            const FunctionCall &fcall = callStack.top();
            const RegisterDescriptor IP = partitioner_->instructionProvider().instructionPointerRegister();
            auto ip = peekRegister(IP, undefined_(IP.nBits()))->toUnsigned();
            auto funcRet = fcall.returnAddress();
            if (ip && funcRet && *ip == *funcRet && sp && fcall.initialStackPointer() == *sp) {
                SAWYER_MESG(mlog[DEBUG]) <<"    popping " <<fcall.printableName(nBits)
                                         <<" because its initial stack pointer is equal to the current stack pointer"
                                         <<" and its return address is equal to the current instruction pointer\n";
                popCallStack();
                ++nPopped;
            }
        }

#if 0 // [Robb Matzke 2022-03-10]
        // Architecture specific stuff
        if (!callStack.isEmpty()) {
            const FunctionCall &fcall = callStack.top();
            const std::string isa = partitioner_->instructionProvider().disassembler()->name();
            if ("a32" == isa || "t32" == isa || "a64" == isa) {
                // ARM AArch32 or AArch64 instructions. Returning based on the current value of the link register (LR) is not
                // possible because the LR is not a callee-saved register. The callee (function about to return) pushes the
                // initial LR value onto the stack at the beginning, but instead of popping it back into the LR at the end, it
                // pops it into the instruction pointer register (PC) directly in order to effect the return.
                const RegisterDescriptor IP = partitioner_->instructionProvider().instructionPointerRegister();
                auto ip = peekRegister(IP, undefined_(IP.nBits()))->toUnsigned();
                auto funcRet = fcall.returnAddress();
                if (ip && funcRet && *ip == *funcRet && sp && fcall.initialStackPointer() == *sp) {
                    SAWYER_MESG(mlog[DEBUG]) <<"    popping " <<fcall.printableName(nBits)
                                             <<" because its initial stack pointer is equal to the current stack pointer"
                                             <<" and its return address is equal to the current instruction pointer\n";
                    popCallStack();
                    ++nPopped;
                }

            } else if (boost::starts_with(isa, "ppc32") || boost::starts_with(isa, "ppc64")) {
                // PowerPC uses a link register which is a callee-saved register. The callee (function about to return) saves
                // and restores the link register via the stack if necessary. When returning, it uses a branch instruction that
                // branches to the address stored in the link register. Therfore, if the current top of stack is the same as
                // the initial stack pointer for the top callee, and the current instruction pointer is the same as the link
                // register, then we must have just returned from that function, so pop it.
                if (sp && callStack.top().initialStackPointer() == *sp) {
                    const RegisterDescriptor IP = partitioner_->instructionProvider().instructionPointerRegister();
                    const RegisterDescriptor LR = partitioner_->instructionProvider().callReturnRegister();
                    if (IP && LR) {
                        auto ip = peekRegister(IP, undefined_(IP.nBits()))->toUnsigned();
                        auto lr = peekRegister(LR, undefined_(LR.nBits()))->toUnsigned();
                        if (ip && lr && *ip == *lr) {
                            SAWYER_MESG(mlog[DEBUG]) <<"    popping " <<fcall.printableName(nBits)
                                                     <<" because its initial stack pointer is equal to the current stack pointer"
                                                     <<" and its return address, current instruction pointer, and link register"
                                                     <<" are equal.\n";
                            popCallStack();
                            ++nPopped;
                        }
                    }
                }
            }
        }
#endif
    }

    return nPopped;
}

void
RiscOperators::printCallStack(std::ostream &out, const std::string &prefix) {
    if (computeMemoryRegions_) {
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        out <<prefix <<"function call stack:\n";
        if (callStack.isEmpty()) {
            out <<prefix <<"  empty\n";
        } else {
            const size_t nBits = partitioner_->instructionProvider().wordSize();
            for (size_t i = callStack.size(); i > 0; --i) {
                const FunctionCall &fcall = callStack[i-1];
                out <<prefix <<"  " <<fcall.function()->printableName()
                    <<" initSp=" <<StringUtility::addrToString(fcall.initialStackPointer())
                    <<" fpDelta=" <<StringUtility::toHex(fcall.framePointerDelta())
                    <<" frame=" <<StringUtility::addrToString(fcall.framePointer(nBits))
                    <<" returnVa=" <<StringUtility::addrToString(fcall.returnAddress()) <<"\n";
                for (const Variables::StackVariable &var: fcall.stackVariables().values()) {
                    AddressInterval where = shiftAddresses(fcall.initialStackPointer() + fcall.framePointerDelta(),
                                                           var.interval(), stackLimits_);
                    out <<prefix <<"    " <<var <<" at " <<StringUtility::addrToString(where) <<"\n";
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
                auto state = State::promote(currentState());

                if (Variables::GlobalVariable gvar = findGlobalVariable(gvars_, *va)) {
                    result->region(gvar.interval());

                } else {
                    auto whereWhat = findStackVariable(state->callStack(), *va, stackLimits_);
                    if (whereWhat.first) {
                        switch (whereWhat.second.purpose()) {
                            case Variables::StackVariable::Purpose::RETURN_ADDRESS:
                            case Variables::StackVariable::Purpose::FRAME_POINTER:
                            case Variables::StackVariable::Purpose::SPILL_AREA:
                                // These types of stack areas don't really have hard-and-fast boundaries because the compiler
                                // often generates code to compute these addresses as part of accessing other areas of the
                                // stack frame.
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
RiscOperators::finishInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    ++nInstructions_;

    // If this was a function call, then push a new entry onto the call stack and insert the memory regions for local variables
    // and function stack arguments.
    if (computeMemoryRegions_) {
        bool isFunctionCall = false;
        if (P2::BasicBlock::Ptr bb = partitioner_->basicBlockContainingInstruction(insn->get_address())) {
            isFunctionCall = partitioner_->basicBlockIsFunctionCall(bb);
        } else {
            isFunctionCall = partitioner_->architecture()->isFunctionCallFast(std::vector<SgAsmInstruction*>{insn},
                                                                              nullptr, nullptr);
        }
        if (isFunctionCall) {
            const RegisterDescriptor IP = partitioner_->instructionProvider().instructionPointerRegister();
            BS::SValue::Ptr ipSValue = peekRegister(IP, undefined_(IP.nBits()));
            if (auto ip = ipSValue->toUnsigned()) {
                if (P2::Function::Ptr callee = partitioner_->functionExists(*ip)) {
                    // We are calling a function, so push a record onto the call stack.
                    const RegisterDescriptor SP = partitioner_->instructionProvider().stackPointerRegister();
                    const BS::SValue::Ptr spSValue = peekRegister(SP, undefined_(SP.nBits()));
                    if (auto sp = spSValue->toUnsigned())
                        pushCallStack(callee, *sp, insn->get_address() + insn->get_size());
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
        if (partitioner_->instructionProvider().stackPointerRegister() == reg ||
            partitioner_->instructionProvider().stackFrameRegister() == reg)
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
        const RegisterDescriptor FP = partitioner_->instructionProvider().stackFrameRegister();
        FunctionCallStack &callStack = State::promote(currentState())->callStack();
        if (FP == reg && !callStack.isEmpty()) {
            FunctionCall &fcall = callStack.top();
            if (auto fp = value->toUnsigned()) {
                if (*fp <= fcall.initialStackPointer()) { // probably part of a function call return instruction
                    const size_t nBits = partitioner_->instructionProvider().wordSize();
                    const rose_addr_t oldFp = fcall.framePointer(nBits);
                    const rose_addr_t oldDelta = fcall.framePointerDelta();
                    const rose_addr_t newDelta = *fp - fcall.initialStackPointer();

                    if (newDelta != oldDelta) {
                        fcall.framePointerDelta(newDelta);
                        const rose_addr_t newFp = fcall.framePointer(nBits);
                        SAWYER_MESG(mlog[DEBUG]) <<"    adjusted frame pointer within " <<fcall.function()->printableName()
                                                 <<" from " <<StringUtility::addrToString(oldFp)
                                                 <<" to " <<StringUtility::addrToString(newFp)
                                                 <<", delta from " <<StringUtility::signedToHex2(oldDelta, FP.nBits())
                                                 <<" to " <<StringUtility::signedToHex2(newDelta, FP.nBits()) <<"\n";
                    }
                }
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
    if (!isNoopRead()) {
        checkNullAccess(adjustedVa, settings_.nullRead, IoMode::READ);
        checkOobAccess(adjustedVa, settings_.oobRead, IoMode::READ, dflt->nBits() / 8);
        checkUninitVar(adjustedVa, settings_.uninitVar, dflt->nBits() / 8);
    }

    // Read from memory map. If we know the address and that memory exists, then read the memory to obtain the default value
    // which we'll use to update the symbolic state in a moment.
    ASSERT_require(0 == dflt->nBits() % 8);
    const size_t nBytes = dflt->nBits() / 8;
    uint8_t buf[8];
    if (adjustedVa->toUnsigned() && nBytes <= sizeof(buf) &&
        nBytes == (partitioner_->memoryMap()->at(adjustedVa->toUnsigned().get()).limit(nBytes)
                   .require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).read(buf).size())) {
        switch (partitioner_->memoryMap()->byteOrder()) {
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
                                     const P2::Partitioner::ConstPtr &partitioner)
    : ModelChecker::SemanticCallbacks(mcSettings), settings_(settings), partitioner_(partitioner) {

    ASSERT_not_null(partitioner);

    // Find global variables needed for uninitialized variable and buffer overflow model checkers. Although the variable
    // finder is not thread safe, it's okay to use it here without obtaining a lock because no other thread could possibly
    // be using it already since we just created it.
    variableFinder_ = Variables::VariableFinder::instance();
    if (TestMode::OFF != settings_.oobRead || TestMode::OFF != settings_.oobWrite || TestMode::OFF != settings_.uninitVar) {
        Sawyer::Message::Stream info(mlog[INFO]);
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        SAWYER_MESG_FIRST(info, debug) <<"searching for global variables";
        gvars_ = variableFinder_->findGlobalVariables(partitioner_);

        // Don't allow variables at very low memory addresses because lots of pointer arithmetic uses these constants and
        // might get assigned global variable memory regions in this area. For instance,
        //    mov eax, 0x1000  // not a global variable address
        //    mov ecx, 4       // size, but happens to be a (miscalculated) global variable address
        //    mul ecx, [esp+4] // ecx is now outside the the (miscalculated) global variable area
        //    mov eax, [eax + ecx] // buffer overflow detected
        Variables::erase(gvars_, AddressInterval::baseSize(0, 256));

        // Don't allow global variables at the first byte of any read+write segment of memory. This is because the segment address
        // is sometimes an intermediate calculation before adding an offset to get to the global variable. If a variable exists at
        // the beginning of the segment, then the calculation would become locked to that variable's region and any subsequent
        // offset to get to the intended variable would look like an out-of-bounds access.
        for (const auto &node: partitioner->memoryMap()->require(MemoryMap::READ_WRITE).nodes())
            Variables::erase(gvars_, node.key().least());

        SAWYER_MESG_FIRST(info, debug) <<"; found " <<StringUtility::plural(gvars_.nIntervals(), "global variables") <<"\n";
        for (const Variables::GlobalVariable &gvar: gvars_.values())
            SAWYER_MESG(debug) <<"  found " <<gvar <<"\n";
    }

    if (!settings_.initialStackVa) {
        // Choose an initial stack pointer that's unlikely to interfere with instructions or data.
        static const size_t RESERVE_BELOW  = 15*1024*1024;          // memory to reserve below the initial stack inter
        static const size_t RESERVE_ABOVE   =  1*1024*1024;         // memory reserved at and above the initial stack pointer
        static const size_t STACK_ALIGNMENT = 16;                   // alignment in bytes
        auto where = AddressInterval::hull(0x80000000, 0xffffffff); // where to look in the address space

        Sawyer::Optional<rose_addr_t> va =
            partitioner->memoryMap()->findFreeSpace(RESERVE_BELOW + RESERVE_ABOVE, STACK_ALIGNMENT, where,
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
                            const P2::Partitioner::ConstPtr &partitioner) {
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
    gvars_.clear();
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

P2::Partitioner::ConstPtr
SemanticCallbacks::partitioner() const {
    return partitioner_;
}

BS::SValue::Ptr
SemanticCallbacks::protoval() {
    return SValue::instance();
}

BS::RegisterState::Ptr
SemanticCallbacks::createInitialRegisters() {
    return BS::RegisterStateGeneric::instance(protoval(), partitioner_->instructionProvider().registerDictionary());
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
    mem->set_byteOrder(partitioner_->instructionProvider().defaultByteOrder());
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
    auto ops = RiscOperators::instance(settings_, partitioner_, this, protoval(), SmtSolver::Ptr(), variableFinder_, gvars_);
    ops->trimThreshold(mcSettings()->maxSymbolicSize);      // zero means no limit
    ops->initialState(nullptr);
    ops->currentState(nullptr);
    ops->solver(SmtSolver::Ptr());
    ops->computeMemoryRegions(settings_.oobRead != TestMode::OFF ||
                              settings_.oobWrite != TestMode::OFF ||
                              settings_.uninitVar != TestMode::OFF);

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
    const RegisterDescriptor SP = partitioner_->instructionProvider().stackPointerRegister();
    if (settings_.initialStackVa) {
        BS::SValue::Ptr sp = ops->number_(SP.nBits(), *settings_.initialStackVa);
        ops->writeRegister(SP, sp);
    }
}

BS::Dispatcher::Ptr
SemanticCallbacks::createDispatcher(const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(ops);
    BS::Dispatcher::Ptr dispatcher = partitioner_->architecture()->newInstructionDispatcher(ops);
    ASSERT_not_null2(dispatcher, "no semantics for this ISA");
    return  dispatcher;
}

SmtSolver::Ptr
SemanticCallbacks::createSolver() {
    std::string solverName = Rose::CommandLine::genericSwitchArgs.smtSolver;
    if (solverName.empty() || "none" == solverName)
        solverName = "best";
    auto solver = SmtSolver::instance(solverName);
    ASSERT_always_not_null2(solver, "do you have an SMT solver configured? solverName=" + solverName);

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
SemanticCallbacks::preExecute(const ExecutionUnit::Ptr &unit, const BS::RiscOperators::Ptr &ops_) {
    auto ops = RiscOperators::promote(ops_);
    ops->nInstructions(0);

    // Track which basic blocks (or instructions) were reached
    if (auto va = unit->address()) {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        ++unitsReached_.insertMaybe(*va, 0);
    }

    // Remove stale function calls, but make sure there's at least one function call
    ops->pruneCallStack();
    ops->maybeInitCallStack(unit->address().orElse(0));

    // Debugging
    if (mlog[DEBUG])
        ops->printCallStack(mlog[DEBUG], "  ");

    return {};
}

std::vector<Tag::Ptr>
SemanticCallbacks::postExecute(const ExecutionUnit::Ptr &unit, const BS::RiscOperators::Ptr &ops_) {
    ASSERT_not_null(unit);
    ASSERT_not_null(ops_);

    auto ops = RiscOperators::promote(ops_);

    // Remove stale function calls, but make sure there's at least one function call
    ops->pruneCallStack();
    ops->maybeInitCallStack(unit->address().orElse(0));

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
    const RegisterDescriptor IP = partitioner_->instructionProvider().instructionPointerRegister();
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
    P2::Function::Ptr func = partitioner_->functionExists(va);
    if (func && boost::ends_with(func->name(), "@plt")) {
        unit = ExternalFunctionUnit::instance(func, partitioner_->sourceLocations().get(va));
    } else if (P2::BasicBlock::Ptr bb = partitioner_->basicBlockExists(va)) {
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
            auto ops = partitioner_->newOperators(P2::MAP_BASED_MEMORY);
            IS::SymbolicSemantics::RiscOperators::promote(ops)->trimThreshold(mcSettings()->maxSymbolicSize);
            auto cpu = partitioner_->newDispatcher(ops);
            const RegisterDescriptor IP = partitioner_->instructionProvider().instructionPointerRegister();
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
                    unit = InstructionUnit::instance(bb->instructions()[0], partitioner_->sourceLocations().get(va));
                    break;
                }
            }
        }
        if (!unit && bb->nInstructions() > 0)
            unit = BasicBlockUnit::instance(partitioner_, bb);
    } else if (SgAsmInstruction *insn = partitioner_->instructionProvider()[va]) {
        SAWYER_MESG(mlog[DEBUG]) <<"    no basic block at " <<StringUtility::addrToString(va) <<"; switched to insn\n";
        unit = InstructionUnit::instance(insn, partitioner_->sourceLocations().get(va));
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
SemanticCallbacks::nextUnits(const Path::Ptr&, const BS::RiscOperators::Ptr &ops, const SmtSolver::Ptr &solver) {
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
        units.push_back({fail, SymbolicExpression::makeBooleanConstant(true), SmtSolver::Evidence()});
        return units;
    }

    // Create execution units for the next concrete addresses
    for (rose_addr_t va: next.addresses) {
        // Test whether next address is feasible with the given previous path assertions
        SmtSolver::Transaction tx(solver);
        auto assertion = SymbolicExpression::makeEq(ip, SymbolicExpression::makeIntegerConstant(ip->nBits(), va));
        solver->insert(assertion);

        switch (solver->check()) {
            case SmtSolver::SAT_YES:
                // Create the next execution unit
                if (ExecutionUnit::Ptr unit = findUnit(va, solver->progress())) {
                    units.push_back({unit, assertion, solver->evidenceByName()});
                } else if (settings_.nullRead != TestMode::OFF && va <= settings_.maxNullAddress) {
                    SourceLocation sloc = partitioner_->sourceLocations().get(va);
                    BS::SValue::Ptr addr = ops->number_(partitioner_->instructionProvider().wordSize(), va);
                    auto tag = NullDereferenceTag::instance(0, TestMode::MUST, IoMode::READ, nullptr, addr);
                    auto fail = FailureUnit::instance(va, sloc, "invalid instruction address", tag);
                    units.push_back({fail, assertion, SmtSolver::Evidence()});
                } else {
                    SourceLocation sloc = partitioner_->sourceLocations().get(va);
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
SemanticCallbacks::filterNullDeref(const BS::SValue::Ptr &/*addr*/, SgAsmInstruction*, TestMode, IoMode) {
    return true;
}

bool
SemanticCallbacks::filterOobAccess(const BS::SValue::Ptr &/*addr*/, const AddressInterval &/*referencedRegion*/,
                                   const AddressInterval &/*accessedRegion*/, SgAsmInstruction*, TestMode, IoMode,
                                   const FoundVariable &/*intendedVariable*/, const FoundVariable &/*accessedVariable*/) {
    return true;
}

bool
SemanticCallbacks::filterUninitVar(const BS::SValue::Ptr &/*addr*/, const AddressInterval &/*referencedRegion*/,
                                   const AddressInterval &/*accessedRegion*/, SgAsmInstruction*, TestMode,
                                   const FoundVariable &/*variable*/) {
    return true;
}

#ifdef ROSE_HAVE_YAMLCPP
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
            if (P2::BasicBlock::Ptr bb = partitioner()->basicBlockExists(va)) {
                retval.push_back(BasicBlockUnit::instance(partitioner(), bb));
            } else {
                throw ParseError(sourceName, where + "no such basic block at " + StringUtility::addrToString(va));
            }

        } else if ("instruction" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (SgAsmInstruction *insn = partitioner()->instructionProvider()[va]) {
                retval.push_back(InstructionUnit::instance(insn, partitioner()->sourceLocations().get(va)));
            } else {
                throw ParseError(sourceName, where + "no instruction at " + StringUtility::addrToString(va));
            }

        } else if ("extern-function" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (P2::Function::Ptr function = partitioner()->functionExists(va)) {
                retval.push_back(ExternalFunctionUnit::instance(function, partitioner()->sourceLocations().get(va)));
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

std::list<ExecutionUnit::Ptr>
SemanticCallbacks::parsePath(const Yaml::Node &root, const std::string &sourceName) {
    std::list<ExecutionUnit::Ptr> retval;

    if (!root.isSequence() || root.size() == 0)
        throw ParseError(sourceName, "a path must be a non-empty sequence of path nodes");

    for (size_t i = 0; i <root.size(); ++i) {
        const std::string where = "path vertex #" + boost::lexical_cast<std::string>(i) + " ";
        if (!root[i].isMap() || !root[i]["vertex-type"])
            throw ParseError(sourceName, where + "is not an object with a \"vertex-type\" field");

        std::string vertexType = root[i]["vertex-type"].as<std::string>();
        if ("basic-block" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (P2::BasicBlock::Ptr bb = partitioner()->basicBlockExists(va)) {
                retval.push_back(BasicBlockUnit::instance(partitioner(), bb));
            } else {
                throw ParseError(sourceName, where + "no such basic block at " + StringUtility::addrToString(va));
            }

        } else if ("instruction" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (SgAsmInstruction *insn = partitioner()->instructionProvider()[va]) {
                retval.push_back(InstructionUnit::instance(insn, partitioner()->sourceLocations().get(va)));
            } else {
                throw ParseError(sourceName, where + "no instruction at " + StringUtility::addrToString(va));
            }

        } else if ("extern-function" == vertexType) {
            if (!root[i]["vertex-address"])
                throw ParseError(sourceName, where + "must have a \"vertex-address\" field");
            rose_addr_t va = root[i]["vertex-address"].as<rose_addr_t>();
            if (P2::Function::Ptr function = partitioner()->functionExists(va)) {
                retval.push_back(ExternalFunctionUnit::instance(function, partitioner()->sourceLocations().get(va)));
            } else {
                throw ParseError(sourceName, where + "no function at " + StringUtility::addrToString(va));
            }

        } else {
            throw ParseError(sourceName, where + "has unrecognized type \"" + StringUtility::cEscape(vertexType) + "\"");
        }
    }
    return retval;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
