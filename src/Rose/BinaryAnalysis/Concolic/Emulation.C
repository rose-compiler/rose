#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Emulation.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

#include <SgAsmInstruction.h>

#include <boost/scope_exit.hpp>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Emulation {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RiscOperators for concolic emulation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const Settings &settings, const Database::Ptr &db, const TestCase::Ptr &testCase,
                             const Partitioner2::Partitioner::ConstPtr &partitioner, const Architecture::Ptr &process,
                             const InstructionSemantics::BaseSemantics::State::Ptr &state, const SmtSolver::Ptr &solver)
    : Super(state, solver), REG_PATH(state->registerState()->registerDictionary()->findOrThrow("path")),
      settings_(settings), db_(db), testCase_(testCase), partitioner_(partitioner), process_(process),
      hadSystemCall_(false) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);
    ASSERT_not_null(process);
    ASSERT_not_null(state);
    ASSERT_not_null(solver);
    name("Concolic-symbolic");
    (void) SValue::promote(state->protoval());
}

RiscOperators::~RiscOperators() {}

RiscOperators::Ptr
RiscOperators::instance(const Settings &settings, const Database::Ptr &db, const TestCase::Ptr &testCase,
                        const P2::Partitioner::ConstPtr &partitioner, const Architecture::Ptr &process,
                        const BS::SValue::Ptr &protoval, const SmtSolver::Ptr &solver) {
    // Extend the register set with an additional Boolean register named "path"
    RegisterDictionary::Ptr regdict = RegisterDictionary::instance("Rose::BinaryAnalysis::Concolic");
    regdict->insert(partitioner->instructionProvider().registerDictionary());
    const RegisterDescriptor path(partitioner->instructionProvider().registerDictionary()->firstUnusedMajor(), 0, 0, 1);
    regdict->insert("path", path);

    // Initialize machine state
    RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
    MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
    memory->set_byteOrder(partitioner->instructionProvider().defaultByteOrder());
    memory->cellCompressor(IS::SymbolicSemantics::MemoryListState::CellCompressorSimple::instance());
    State::Ptr state = State::instance(registers, memory);
    RiscOperators::Ptr ops(new RiscOperators(settings, db, testCase, partitioner, process, state, solver));
    ASSERT_require(ops->REG_PATH == path);
    ops->writeRegister(path, ops->boolean_(true));
    return ops;
}

RiscOperators::Ptr
RiscOperators::promote(const BS::RiscOperators::Ptr &x) {
    RiscOperators::Ptr retval = as<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

BS::RiscOperators::Ptr
RiscOperators::create(const BS::SValue::Ptr&, const SmtSolver::Ptr&) const {
    ASSERT_not_implemented("[Robb Matzke 2019-09-24]");
}

BS::RiscOperatorsPtr
RiscOperators::create(const BS::State::Ptr&, const SmtSolver::Ptr&) const {
    ASSERT_not_implemented("[Robb Matzke 2019-09-24]");
}

const RiscOperators::Settings&
RiscOperators::settings() const {
    return settings_;
}

P2::Partitioner::ConstPtr
RiscOperators::partitioner() const {
    return partitioner_;
}

size_t
RiscOperators::wordSizeBits() const {
    return partitioner_->instructionProvider().instructionPointerRegister().nBits();
}

TestCase::Ptr
RiscOperators::testCase() const {
    return testCase_;
}

Database::Ptr
RiscOperators::database() const {
    return db_;
}

Architecture::Ptr
RiscOperators::process() const {
    return process_;
}

InputVariables::Ptr
RiscOperators::inputVariables() const {
    ASSERT_not_null(process_);
    return process_->inputVariables();
}

bool
RiscOperators::hadSystemCall() const {
    return hadSystemCall_;
}

void
RiscOperators::hadSystemCall(bool b) {
    hadSystemCall_ = b;
}

ExecutionEvent::Ptr
RiscOperators::hadSharedMemoryAccess() const {
    return hadSharedMemoryAccess_;
}

void
RiscOperators::hadSharedMemoryAccess(const ExecutionEventPtr &e) {
    hadSharedMemoryAccess_ = e;
}

bool
RiscOperators::isRecursive() const {
    return isRecursive_;
}

void
RiscOperators::isRecursive(bool b) {
    isRecursive_ = b;
}

RegisterDictionary::Ptr
RiscOperators::registerDictionary() const {
    return partitioner_->instructionProvider().registerDictionary();
}

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    hadSystemCall_ = false;
    hadSharedMemoryAccess_ = ExecutionEvent::Ptr();
    Super::startInstruction(insn);
}

void
RiscOperators::interrupt(int majr, int minr) {
    if (x86_exception_int == majr && 0x80 == minr) {
        hadSystemCall_ = true;
    } else {
        Super::interrupt(majr, minr);
    }
}

void
RiscOperators::doExit(uint64_t status) {
    // This is called during the symbolic phase. The concrete system call hasn't happened yet.
    mlog[INFO] <<"specimen exiting with " <<status <<"\n";
    throw Exit(status);
}

BS::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const BS::SValue::Ptr &dfltUnused) {
    // Read the register's value symbolically, and if we don't have a value then read it concretely and use the concrete value
    // to update the symbolic state.
    SValue::Ptr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
    SymbolicExpression::Ptr concrete = SymbolicExpression::makeIntegerConstant(process_->readRegister(reg));
    dflt->set_expression(concrete);
    return Super::readRegister(reg, dflt);
}

BS::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg) {
    return readRegister(reg, undefined_(reg.nBits()));
}

BS::SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const BS::SValue::Ptr &dfltUnused) {
    // Return the register's symbolic value if it exists, else the concrete value.
    SValue::Ptr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
    SymbolicExpression::Ptr concrete = SymbolicExpression::makeIntegerConstant(process_->readRegister(reg));
    dflt->set_expression(concrete);
    return Super::peekRegister(reg, dflt);
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BS::SValue::Ptr &value) {
    if (currentInstruction() && hadSharedMemoryAccess()) {
        Sawyer::Message::Stream debug(mlog[DEBUG]);

        RegisterDictionary::Ptr regdict = currentState()->registerState()->registerDictionary();

        // This probably writing a previously-read value from shared memory into a register. It's common on RISC
        // architectures to have an instruction that copies a value from memory into a register, in which case the value
        // being written here is just the variable we're using to represent what was read from shared memory. On CISC
        // architectures the value being written might be a more complex function of the value that was read.
        //
        // In cany case, since we can't directly adjust the byte read from concrete memory (not even by pre-writing to that
        // address since it might be read-only or it might not follow normal memory semantics), we do the next best thing: we
        // adjust all the side effects of the instruction that read the byte from memory.
        SymbolicExpression::Ptr valueExpr = Emulation::SValue::promote(value)->get_expression();
        SAWYER_MESG(debug) <<"  register update for shared memory read\n"
                           <<"    register = " <<regdict->quadAndName(reg) <<"\n"
                           <<"    value    = " <<*valueExpr <<"\n";
        auto event = ExecutionEvent::registerWrite(process()->testCase(), process()->nextEventLocation(When::POST),
                                                   currentInstruction()->get_address(), reg, SymbolicExpression::Ptr(),
                                                   SymbolicExpression::Ptr(), valueExpr);
        event->name(hadSharedMemoryAccess()->name() + "_toreg");
        database()->save(event);
        SAWYER_MESG(debug) <<"    created " <<event->printableName(database()) <<"\n";
    }
    Super::writeRegister(reg, value);
}

BS::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg, const BS::SValue::Ptr &addr,
                          const BS::SValue::Ptr &dfltUnused, const BS::SValue::Ptr &cond) {
    if (isRecursive()) {
        // Don't do anything special because we're being called from inside a user callback.
        return Super::readMemory(segreg, addr, dfltUnused, cond);

    } else if (auto va = addr->toUnsigned()) {
        // Read the memory's value symbolically, and if we don't have a value then read it concretely and use the concrete value to
        // update the symbolic state. However, we can only read it concretely if the address is a constant.
        size_t nBytes = dfltUnused->nBits() / 8;
        SymbolicExpression::Ptr concrete = SymbolicExpression::makeIntegerConstant(dfltUnused->nBits(),
                                                                                   process_->readMemoryUnsigned(*va, nBytes));

        // Handle shared memory by invoking user-defined callbacks
        SharedMemoryCallbacks callbacks = process()->sharedMemory().getOrDefault(*va);
        if (!callbacks.isEmpty()) {
            isRecursive(true);
            BOOST_SCOPE_EXIT(this_) {
                this_->isRecursive(false);
            } BOOST_SCOPE_EXIT_END;

            // FIXME[Robb Matzke 2021-09-09]: use structured bindings when ROSE requires C++17 or later
            auto x = process()->sharedMemoryRead(callbacks, partitioner(), RiscOperators::promote(shared_from_this()),
                                                 *va, nBytes);
            ExecutionEvent::Ptr sharedMemoryEvent = x.first;
            SymbolicExpression::Ptr result = x.second;

            if (result) {
                hadSharedMemoryAccess(sharedMemoryEvent);
                return svalueExpr(result);
            }
        }

        SValue::Ptr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
        dflt->set_expression(concrete);
        return Super::readMemory(segreg, addr, dflt, cond);

    } else {
        // Not a concrete address, so do things symbolically
        return Super::readMemory(segreg, addr, dfltUnused, cond);
    }
}

BS::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &dfltUnused) {
    // Read the memory's symbolic value if it exists, else read the concrete value. We can't read concretely if the address is
    // symbolic.
    if (auto va = addr->toUnsigned()) {
        size_t nBytes = dfltUnused->nBits() / 8;
        SymbolicExpression::Ptr concrete = SymbolicExpression::makeIntegerConstant(dfltUnused->nBits(),
                                                                                   process_->readMemoryUnsigned(*va, nBytes));
        SValue::Ptr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
        dflt->set_expression(concrete);
        return Super::peekMemory(segreg, addr, dflt);
    } else {
        return Super::peekMemory(segreg, addr, dfltUnused);
    }
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &value,
                           const BS::SValue::Ptr &cond) {
    if (isRecursive()) {
        // Don't do anything special because we're being called from inside a user callback.
        Super::writeMemory(segreg, addr, value, cond);

    } else if (auto va = addr->toUnsigned()) {
        SharedMemoryCallbacks callbacks = process()->sharedMemory().getOrDefault(*va);
        if (!callbacks.isEmpty()) {
            isRecursive(true);
            BOOST_SCOPE_EXIT(this_) {
                this_->isRecursive(false);
            } BOOST_SCOPE_EXIT_END;

            // FIXME[Robb Matzke 2022-12-16]: use structured bindings when ROSE requires C++17 or later
            const bool processed =
                process()->sharedMemoryWrite(callbacks, partitioner(), RiscOperators::promote(shared_from_this()),
                                             *va, value);
            if (processed) {
#if 0 // [Robb Matzke 2022-12-16]
                hadSharedMemoryAccess(sharedMemoryEvent);
                return svalueExpr(result);
#else
                return;
#endif
            }
        }

        Super::writeMemory(segreg, addr, value, cond);

    } else {
        // Not a concrete address, so do things symbolically
        Super::writeMemory(segreg, addr, value, cond);
    }
}

void
RiscOperators::createInputVariables(const SmtSolver::Ptr &solver) {
    process_->createInputVariables(partitioner(), RiscOperators::promote(shared_from_this()), solver);
}

void
RiscOperators::restoreInputVariables(const SmtSolver::Ptr &solver) {
    process_->restoreInputVariables(partitioner(), RiscOperators::promote(shared_from_this()), solver);
}

void
RiscOperators::printInputVariables(std::ostream &out) const {
    out <<"input variables:\n";
    process_->inputVariables()->print(out, "  ");
}

void
RiscOperators::printAssertions(std::ostream &out) const {
    out <<"SMT solver assertions:\n";
    for (SymbolicExpression::Ptr assertion: solver()->assertions())
        out <<"  " <<*assertion <<"\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Dispatcher::Dispatcher(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr &ops, const Architecture::Ptr &architecture) {
    ASSERT_not_null(ops);
    inner_ = architecture->makeDispatcher(ops);
}

Dispatcher::~Dispatcher() {}

Dispatcher::Ptr
Dispatcher::instance(const InstructionSemantics::BaseSemantics::RiscOperators::Ptr &ops, const Architecture::Ptr &architecture) {
    return Ptr(new Dispatcher(ops, architecture));
}

BS::RiscOperators::Ptr
Dispatcher::operators() const {
    ASSERT_not_null(inner_);
    return inner_->operators();
}

// class method
RiscOperators::Ptr
Dispatcher::unwrapEmulationOperators(const BS::RiscOperators::Ptr &ops) {
    if (IS::TraceSemantics::RiscOperators::Ptr trace = as<IS::TraceSemantics::RiscOperators>(ops))
        return RiscOperators::promote(trace->subdomain());
    return RiscOperators::promote(ops);
}

RiscOperators::Ptr
Dispatcher::emulationOperators() const {
    return unwrapEmulationOperators(operators());
}

rose_addr_t
Dispatcher::concreteInstructionPointer() const {
    return emulationOperators()->process()->ip();
}

void
Dispatcher::processConcreteInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    emulationOperators()->process()->executeInstruction(operators(), insn);
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    ASSERT_not_null(inner_);

    // We do things this way so as to not interfere with debugging exceptions. We don't want to catch and re-throw.
    BOOST_SCOPE_EXIT(this_, insn) {
        this_->processConcreteInstruction(insn);
    } BOOST_SCOPE_EXIT_END;

    // Symbolic execution happens before the concrete execution (code above), but may throw an exception.
    Emulation::RiscOperators::Ptr ops = emulationOperators();
    SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"executing insn #" <<ops->process()->currentLocation().primary()
                                             <<" " <<ops->partitioner()->unparse(insn) <<"\n";
    inner_->processInstruction(insn);
}

bool
Dispatcher::isTerminated() const {
    return emulationOperators()->process()->isTerminated();
}

RegisterDictionary::Ptr
Dispatcher::registerDictionary() const {
    ASSERT_not_null(inner_);
    return inner_->registerDictionary();
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
