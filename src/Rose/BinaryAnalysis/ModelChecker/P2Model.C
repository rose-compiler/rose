#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/P2Model.h>

#include <Rose/BinaryAnalysis/ModelChecker/BasicBlockUnit.h>
#include <Rose/BinaryAnalysis/ModelChecker/ErrorTag.h>
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

    if (isNull && !semantics_->filterNullDeref(addrSVal, testMode, ioMode)) {
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
                AddressInterval accessedRegion = AddressInterval::baseSizeSat(*va, nBytes);
                if (!referencedRegion.isContaining(accessedRegion)) {
                    currentState(BS::State::Ptr());         // indicates that execution failed
                    throw ThrownTag{OobTag::instance(nInstructions(), testMode, ioMode, currentInstruction(), addrSVal)};
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
        const rose_addr_t sp = spSValue->toUnsigned().get(); // must be concrete
        FunctionCallStack &callStack = State::promote(currentState())->callStack();

        while (!callStack.isEmpty() && callStack.top().initialStackPointer() < sp) {
            SAWYER_MESG(mlog[DEBUG]) <<"      returned from " <<callStack.top().function()->printableName() <<"\n";
            callStack.pop();
            ++nPopped;
        }
    }
    return nPopped;
}

void
RiscOperators::pushCallStack(const P2::Function::Ptr &callee, rose_addr_t initialSp) {
    if (computeMemoryRegions_) {
        FunctionCallStack &callStack = State::promote(currentState())->callStack();

        SAWYER_MESG(mlog[DEBUG]) <<"      called " <<callee->printableName() <<"\n";
        SAWYER_MESG(mlog[DEBUG]) <<"        initial stack pointer = " <<StringUtility::addrToString(initialSp) <<"\n";

        // VariableFinder API is not thread safe, so we need to protect it
        Variables::StackVariables lvars;
        {
            SAWYER_THREAD_TRAITS::LockGuard lock(variableFinderMutex_);
            lvars = variableFinder_unsync->findStackVariables(partitioner_, callee);
        }
        callStack.push(FunctionCall(callee, initialSp, lvars));

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

// Add the signed delta to the unsigned base and return an unsigned saturated value. We have to be careful here because
// signed integer overflows are undefined behavior in C++11 and earlier -- the compiler can optimize them however it likes,
// which might not be the 2's complement we're expecting.
std::pair<uint64_t /*sum*/, bool /*saturated?*/>
RiscOperators::saturatedAdd(uint64_t base, int64_t delta) {
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

AddressInterval
RiscOperators::shiftAddresses(const AddressInterval &base, int64_t delta, const AddressInterval &limit) {
    if (base.isEmpty()) {
        return AddressInterval();
    } else {
        auto loSat = saturatedAdd(base.least(), delta);
        auto hiSat = saturatedAdd(base.greatest(), delta);
        if (loSat.second && hiSat.second) {
            return AddressInterval();
        } else {
            return AddressInterval::hull(loSat.first, hiSat.first) & limit;
        }
    }
}

AddressInterval
RiscOperators::shiftAddresses(uint64_t base, const Variables::OffsetInterval &delta, const AddressInterval &limit) {
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

BS::SValue::Ptr
RiscOperators::assignRegion(const BS::SValue::Ptr &result_) {
    if (computeMemoryRegions_) {
        auto result = SValue::promote(result_);
        if (!result->region()) {
            if (auto va = result->toUnsigned()) {
                FunctionCallStack &callStack = State::promote(currentState())->callStack();
                AddressInterval found;
                for (size_t i = 0; i < callStack.size(); ++i) {
                    const FunctionCall &fcall = callStack[i];
                    for (const Variables::StackVariable &var: fcall.stackVariables().values()) {
                        if (AddressInterval varAddrs = shiftAddresses(fcall.initialStackPointer() + fcall.framePointerDelta(),
                                                                      var.interval(), stackLimits_)) {
                            if (varAddrs.isContaining(*va)) {
                                if (!found || varAddrs.size() < found.size())
                                    found = varAddrs;
                            }
                        }
                    }
                }
                if (found)
                    result->region(found);
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
                mlog[WARN] <<"no function containing instruction at " <<StringUtility::addrToString(va) <<"\n";
            } else if (functions.size() == 1) {
                function = functions[0];
            } else {
                mlog[WARN] <<"multiple functions containing instruction at " <<StringUtility::addrToString(va) <<"\n";
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
                    const rose_addr_t sp = spSValue->toUnsigned().get();      // must be concrete
                    pushCallStack(callee, sp);
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
    ops->initialState(nullptr);
    ops->currentState(nullptr);
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
    auto solver = SmtSolver::instance("best");
    solver->memoization(settings_.solverMemoization);
    if (!rose_isnan(mcSettings()->solverTimeout))
        solver->timeout(boost::chrono::duration<double>(mcSettings()->solverTimeout));
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
    Combinatorics::HasherSha256Builtin hasher;          // we could use a faster one as long as we don't get false matches
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

ExecutionUnit::Ptr
SemanticCallbacks::findUnit(rose_addr_t va) {
    // We use a separate mutex for the unit_ cache so that only one thread computes this at a time without blocking
    // threads trying to make progress on other things.  An alternative would be to lock the main mutex, but only when
    // accessing the cache, and allow the computations to be duplicated with only the first thread saving the result.
    SAWYER_THREAD_TRAITS::LockGuard lock(unitsMutex_);

    ExecutionUnit::Ptr unit;
    if ((unit = units_.getOrDefault(va))) {
        // preexisting
    } else {
        P2::Function::Ptr func = partitioner_.functionExists(va);
        if (func && boost::ends_with(func->name(), "@plt")) {
            unit = ExternalFunctionUnit::instance(func, partitioner_.sourceLocations().get(va));
        } else if (P2::BasicBlock::Ptr bb = partitioner_.basicBlockExists(va)) {
            // Depending on partitioner settings, sometimes a basic block could have an internal loop. For instance, some x86
            // instructions have an optional repeat prefix. Since execution units need to know how many steps they are (so we
            // can inforce the K limit), and since the content of the execution unit needs to be immutable, and since we can't
            // really re-compute path successors with new data (we would have already thrown away the outgoing state), we have
            // to make a decision here and now.
            //
            // So we evaluate each instruction semantically on a clean state and check that the instruction pointer follows the
            // instrucitons in the basic block.  If not, we switch to one-instruction-at-a-time mode for this basic block,
            // which is about half as fast and takes more memory.  By caching our results, we only do this expensive calcultion
            // once per basic block, not each time a path reaches this block.
            if (bb->nInstructions() > 0) {
                auto ops = partitioner_.newOperators(P2::MAP_BASED_MEMORY);
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
            if (!unit)
                unit = BasicBlockUnit::instance(partitioner_, bb);
        } else if (SgAsmInstruction *insn = partitioner_.instructionProvider()[va]) {
            SAWYER_MESG(mlog[DEBUG]) <<"    no basic block at " <<StringUtility::addrToString(va) <<"; switched to insn\n";
            unit = InstructionUnit::instance(insn, partitioner_.sourceLocations().get(va));
        }
    }

    if (unit)
        units_.insert(va, unit);
    return unit;
}

std::vector<SemanticCallbacks::NextUnit>
SemanticCallbacks::nextUnits(const Path::Ptr &path, const BS::RiscOperators::Ptr &ops, const SmtSolver::Ptr &solver) {
    std::vector<SemanticCallbacks::NextUnit> units;

    // If we've seen this state before, then there's nothing new for us to do.
    if (seenState(ops)) {
        SAWYER_MESG(mlog[DEBUG]) <<"  not extending path since state was previously seen\n";
        return {};
    }

    // Find next concrete addresses from instruction pointer symbolic expression.
    CodeAddresses next = nextCodeAddresses(ops);
    auto ip = IS::SymbolicSemantics::SValue::promote(next.ip)->get_expression();
    if (!next.isComplete) {
        auto tag = ErrorTag::instance(0, "abstract jump", "symbolic address not handled yet", nullptr, ip);
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

        SmtSolver::Satisfiable satisfied = SmtSolver::SAT_UNKNOWN;
        try {
            satisfied = solver->check();
        } catch (const SmtSolver::Exception &e) {
            satisfied = SmtSolver::SAT_UNKNOWN;
        }

        switch (satisfied) {
            case SmtSolver::SAT_YES:
                // Create the next execution unit
                if (ExecutionUnit::Ptr unit = findUnit(va)) {
                    units.push_back({unit, assertion});
                } else if (settings_.nullRead != TestMode::OFF && va <= settings_.maxNullAddress) {
                    SourceLocation sloc = partitioner_.sourceLocations().get(va);
                    BS::SValue::Ptr addr = ops->number_(partitioner_.instructionProvider().wordSize(), va);
                    auto tag = NullDerefTag::instance(0, TestMode::MUST, IoMode::READ, nullptr, addr);
                    auto fail = FailureUnit::instance(va, sloc, "invalid instruction address", tag);
                    units.push_back({fail, assertion});
                } else {
                    SourceLocation sloc = partitioner_.sourceLocations().get(va);
                    auto tag = ErrorTag::instance(0, "invalid instruction pointer", "no instruction at address", nullptr, va);
                    units.push_back({FailureUnit::instance(va, sloc, "invalid instruction address", tag), assertion});
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
SemanticCallbacks::filterNullDeref(const BS::SValue::Ptr &addr, TestMode testMode, IoMode ioMode) {
    return true;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
