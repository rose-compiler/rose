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
#include <Rose/BinaryAnalysis/ModelChecker/Path.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/SymbolicExpr.h>
#include <boost/algorithm/string/predicate.hpp>
#include <chrono>
#include <Combinatorics.h>
#include <Rose/CommandLine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

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
                        ->with("must", TestMode::MUST))
              .doc("Method by which to test for null pointer dereferences during memory read operations. Memory addresses "
                   "are tested during read operations to determine if the address falls within the \"null page\", which "
                   "consists of the lowest memory addresses as defined by @s{max-null-page}. The choices are:"

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
                        ->with("must", TestMode::MUST))
              .doc("Method by which to test for null pointer dereferences during memory write operations. Memory addresses "
                   "are tested during write operations to determine if the address falls within the \"null page\", which "
                   "consists of the lowest memory addresses as defined by @s{max-null-page}. The choices are:"

                   "@named{off}{Do not check for null pointer dereferences during write operations." +
                   std::string(TestMode::OFF == settings.nullWrite ? " This is the default." : "") + "}"

                   "@named{may}{Find writes that might access the null page, but do not necessarily do so." +
                   std::string(TestMode::MAY == settings.nullWrite ? " This is the default." : "") + "}"

                   "@named{must}{Find writes that must necessarily access the null page." +
                   std::string(TestMode::MUST == settings.nullWrite ? " This is the default." : "") + "}"));

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

    insertBooleanSwitch(sg, "solver-memoization", settings.solverMemoization,
                        "Causes the SMT solvers (per thread) to memoize their results. In other words, they remember the "
                        "sets of assertions and if the same set appears a second time it will return the same answer as the "
                        "first time without actually calling the SMT solver.  This can sometimes reduce the amount of time "
                        "spent solving, but uses more memory.");

    return sg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics domain
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const Settings &settings, const P2::Partitioner &partitioner,
                             ModelChecker::SemanticCallbacks *semantics, const BS::SValuePtr &protoval,
                             const SmtSolver::Ptr &solver)
    : Super(protoval, solver), settings_(settings), partitioner_(partitioner),
      semantics_(dynamic_cast<P2Model::SemanticCallbacks*>(semantics)) {
    ASSERT_not_null(semantics_);
    name("P2Model");
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instance(const Settings &settings, const P2::Partitioner &partitioner, ModelChecker::SemanticCallbacks *semantics,
                        const BS::SValuePtr &protoval, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(protoval);
    return Ptr(new RiscOperators(settings, partitioner, semantics, protoval, solver));
}

BS::RiscOperatorsPtr
RiscOperators::create(const BS::SValuePtr&, const SmtSolver::Ptr&) const {
    ASSERT_not_implemented("[Robb Matzke 2021-04-01]"); // needs to be overridden, but need not be implemented
}

BS::RiscOperatorsPtr
RiscOperators::create(const BS::StatePtr&, const SmtSolver::Ptr&) const {
    ASSERT_not_implemented("[Robb Matzke 2021-04-01]"); // needs to be overridden, but need not be implemented
}

RiscOperators::Ptr
RiscOperators::promote(const BS::RiscOperatorsPtr &x) {
    Ptr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
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
RiscOperators::isNull(const BS::SValuePtr &addr_, TestMode testMode, IoMode ioMode) {
    // Null-dereferences are only tested when we're actually executing an instruciton. Other incidental operations such as
    // initializing the first state are not checked.
    if (!currentInstruction())
        return false;

    ASSERT_not_null(addr_);
    ASSERT_not_null(modelCheckerSolver_);               // should have all the path assertions already
    SymbolicExpr::Ptr addr = IS::SymbolicSemantics::SValue::promote(addr_)->get_expression();
    const char *direction = IoMode::READ == ioMode ? "read" : "write";


    bool retval = false;
    switch (testMode) {
        case TestMode::OFF:
            break;

        case TestMode::MAY: {
            // May be null if addr <= max is satisfied
            SymbolicExpr::Ptr maxNull = SymbolicExpr::makeIntegerConstant(addr->nBits(), settings_.maxNullAddress);
            SymbolicExpr::Ptr isNull = SymbolicExpr::makeLe(addr, maxNull);
            SmtSolver::Transaction tx(modelCheckerSolver_);
            modelCheckerSolver_->insert(isNull);
            retval = modelCheckerSolver_->check() == SmtSolver::SAT_YES;
            (settings_.debugNull || retval) && SAWYER_MESG(mlog[DEBUG])
                <<"      " <<direction <<" address may be in [0," <<settings_.maxNullAddress <<"]? " <<(retval?"yes":"no")
                <<"; address = " <<*addr <<"\n";
            break;
        }

        case TestMode::MUST: {
            // Must be null if addr > max cannot be satisfied
            SymbolicExpr::Ptr maxNull = SymbolicExpr::makeIntegerConstant(addr->nBits(), settings_.maxNullAddress);
            SymbolicExpr::Ptr isNull = SymbolicExpr::makeGt(addr, maxNull);
            SmtSolver::Transaction tx(modelCheckerSolver_);
            modelCheckerSolver_->insert(isNull);
            retval = modelCheckerSolver_->check() == SmtSolver::SAT_NO;
            (settings_.debugNull || retval) && SAWYER_MESG(mlog[DEBUG])
                <<"      " <<direction <<" address must be in [0," <<settings_.maxNullAddress <<"]? " <<(retval?"yes":"no")
                <<"; address = " <<*addr <<"\n";
            break;
        }
    }

    if (retval && !semantics_->filterNullDeref(addr_, testMode, ioMode)) {
        SAWYER_MESG(mlog[DEBUG]) <<"      nullptr dereference rejected by user; this one is ignored\n";
        retval = false;
    }
    return retval;
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
RiscOperators::finishInstruction(SgAsmInstruction*) {
    ++nInstructions_;
}

BS::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BS::SValuePtr &addr, const BS::SValuePtr &dflt_,
                          const BS::SValuePtr &cond) {
    ASSERT_not_null(addr);
    ASSERT_not_null(dflt_);
    ASSERT_not_null(cond);

    BS::SValuePtr dflt = dflt_;
    if (cond->isFalse())
        return dflt;

    // Offset the address by the value of the segment register
    BS::SValuePtr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = addr;
    } else {
        BS::SValuePtr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(addr, signExtend(segregValue, addr->nBits()));
    }

    // Check for null pointer dereference
    if (isNull(adjustedVa, settings_.nullRead, IoMode::READ)) {
        currentState(BS::StatePtr());               // indicates that execution failed
        throw ThrownTag{NullDerefTag::instance(nInstructions(), settings_.nullRead, IoMode::READ, currentInstruction(),
                                               adjustedVa)};
    }

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
    BS::SValuePtr retval = Super::readMemory(segreg, addr, dflt, cond);
    return retval;
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BS::SValuePtr &addr, const BS::SValuePtr &value,
                           const BS::SValuePtr &cond) {
    if (cond->isFalse())
        return;

    // Offset the address by the value of the segment register
    BS::SValuePtr adjustedVa;
    if (segreg.isEmpty()) {
        adjustedVa = addr;
    } else {
        BS::SValuePtr segregValue = readRegister(segreg, undefined_(segreg.nBits()));
        adjustedVa = add(addr, signExtend(segregValue, addr->nBits()));
    }

    // Check for null pointer dereference
    if (isNull(adjustedVa, settings_.nullWrite, IoMode::WRITE)) {
        currentState(BS::StatePtr());               // indicates that execution failed
        throw ThrownTag{NullDerefTag::instance(nInstructions(), settings_.nullWrite, IoMode::WRITE,
                                               currentInstruction(), adjustedVa)};
    }

    Super::writeMemory(segreg, addr, value, cond);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// High-level semantic operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SemanticCallbacks::SemanticCallbacks(const ModelChecker::Settings::Ptr &mcSettings, const Settings &settings,
                                     const P2::Partitioner &partitioner)
    : ModelChecker::SemanticCallbacks(mcSettings), settings_(settings), partitioner_(partitioner) {
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

BS::RegisterStatePtr
SemanticCallbacks::createInitialRegisters() {
    return BS::RegisterStateGeneric::instance(protoval(), partitioner_.instructionProvider().registerDictionary());
}

BS::MemoryStatePtr
SemanticCallbacks::createInitialMemory() {
    BS::MemoryStatePtr mem;
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

BS::RiscOperatorsPtr
SemanticCallbacks::createRiscOperators() {
    auto ops = RiscOperators::instance(settings_, partitioner_, this, protoval(), SmtSolver::Ptr());
    ops->initialState(nullptr);
    ops->currentState(nullptr);
    return ops;
}

void
SemanticCallbacks::initializeState(const BS::RiscOperatorsPtr &ops) {
    ASSERT_not_null(ops);
    const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
    if (settings_.initialStackVa) {
        BS::SValuePtr sp = ops->number_(SP.nBits(), *settings_.initialStackVa);
        ops->writeRegister(SP, sp);
    }
}

BS::DispatcherPtr
SemanticCallbacks::createDispatcher(const BS::RiscOperatorsPtr &ops) {
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
SemanticCallbacks::attachModelCheckerSolver(const BS::RiscOperatorsPtr &ops, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(ops);
    RiscOperators::promote(ops)->modelCheckerSolver(solver);
}

std::vector<Tag::Ptr>
SemanticCallbacks::preExecute(const ExecutionUnit::Ptr &unit, const BS::RiscOperatorsPtr &ops) {
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

BS::SValuePtr
SemanticCallbacks::instructionPointer(const BS::RiscOperatorsPtr &ops) {
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
                    BS::SValuePtr actualIp = ops->peekRegister(IP);
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
SemanticCallbacks::nextUnits(const Path::Ptr &path, const BS::RiscOperatorsPtr &ops, const SmtSolver::Ptr &solver) {
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
                    BS::SValuePtr addr = ops->number_(partitioner_.instructionProvider().wordSize(), va);
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
SemanticCallbacks::filterNullDeref(const BS::SValuePtr &addr, TestMode testMode, IoMode ioMode) {
    return true;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
