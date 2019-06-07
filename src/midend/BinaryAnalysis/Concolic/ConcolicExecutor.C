#include <sage3basic.h>

#include <BinaryConcolic.h>
#include <ConcreteSemantics2.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/FileSystem.h>
#include <SqlDatabase.h>

#ifdef __linux__
#include <sys/syscall.h>
#endif

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
namespace ISBase = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

typedef ISBase::Dispatcher Cpu;
typedef ISBase::DispatcherPtr CpuPtr;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantic domain for concrete execution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef IS::ConcreteSemantics::SValue ConcreteValue;
typedef IS::ConcreteSemantics::SValuePtr ConcreteValuePtr;
typedef IS::ConcreteSemantics::RegisterState ConcreteRegisters;
typedef IS::ConcreteSemantics::RegisterStatePtr ConcreteRegistersPtr;
typedef IS::ConcreteSemantics::MemoryState ConcreteMemory;
typedef IS::ConcreteSemantics::MemoryStatePtr ConcreteMemoryPtr;
typedef IS::ConcreteSemantics::State ConcreteState;
typedef IS::ConcreteSemantics::StatePtr ConcreteStatePtr;

typedef boost::shared_ptr<class ConcreteOperators> ConcreteOperatorsPtr;
class ConcreteOperators: public IS::ConcreteSemantics::RiscOperators {
public:
    typedef IS::ConcreteSemantics::RiscOperators Super;

protected: // Real constructors
    ConcreteOperators(const ISBase::SValuePtr &protoval, const SmtSolverPtr &solver)
        : Super(protoval, solver) {
        name("Concolic-concrete");
        (void) ConcreteValue::promote(protoval);
    }

    ConcreteOperators(const ISBase::StatePtr &state, const SmtSolverPtr &solver)
        : Super(state, solver) {
        name("Concolic-concrete");
        (void) ConcreteValue::promote(state->protoval());
    }

public: // Static allocating constructors
    static ConcreteOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr()) {
        ISBase::SValuePtr protoval = ConcreteValue::instance();
        ISBase::RegisterStatePtr registers = ConcreteRegisters::instance(protoval, regdict);
        ISBase::MemoryStatePtr memory = ConcreteMemory::instance(protoval, protoval);
        ISBase::StatePtr state = ConcreteState::instance(registers, memory);
        return ConcreteOperatorsPtr(new ConcreteOperators(state, solver));
    }

    static ConcreteOperatorsPtr instance(const ISBase::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return ConcreteOperatorsPtr(new ConcreteOperators(protoval, solver));
    }

    static ConcreteOperatorsPtr instance(const ISBase::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return ConcreteOperatorsPtr(new ConcreteOperators(state, solver));
    }

public: // Virtual constructors
    virtual ISBase::RiscOperatorsPtr create(const ISBase::SValuePtr &protoval,
                                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual ISBase::RiscOperatorsPtr create(const ISBase::StatePtr &state,
                                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(state, solver);
    }
    
public: // Dynamic pointer casts
    static ConcreteOperatorsPtr promote(const ISBase::RiscOperatorsPtr &x) {
        ConcreteOperatorsPtr retval = boost::dynamic_pointer_cast<ConcreteOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE {
        if (x86_exception_int == majr && 0x80 == minr) {
            systemCall();
        } else {
            Super::interrupt(majr, minr);
        }
    }

private:
    // Handles a Linux system call of the INT 0x80 variety.
    void systemCall() {
        // Result of an "int 0x80" system call instruction
        const RegisterDictionary *regdict = currentState()->registerState()->get_register_dictionary();
        const RegisterDescriptor REG_AX = *regdict->lookup("rax");
        ISBase::SValuePtr ax = peekRegister(REG_AX);
#ifdef __linux__
        if (SYS_exit_group == ax->get_number() || SYS_exit == ax->get_number()) {
            const RegisterDescriptor REG_DI = *regdict->lookup("rdi");
            ISBase::SValuePtr di = readRegister(REG_DI);
            int64_t exitValue = di->get_number();

            // FIXME[Robb Matzke 2019-06-06]: need better way to handle this
            mlog[ERROR] <<"specimen exiting with value " <<exitValue <<"\n";
            exit(0);
        }
#endif
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantic domain for symbolic execution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef IS::SymbolicSemantics::SValue SymbolicValue;
typedef IS::SymbolicSemantics::SValuePtr SymbolicValuePtr;
typedef IS::SymbolicSemantics::RegisterState SymbolicRegisters;
typedef IS::SymbolicSemantics::RegisterStatePtr SymbolicRegistersPtr;
typedef IS::SymbolicSemantics::MemoryState SymbolicMemory;
typedef IS::SymbolicSemantics::MemoryStatePtr SymbolicMemoryPtr;
typedef IS::SymbolicSemantics::State SymbolicState;
typedef IS::SymbolicSemantics::StatePtr SymbolicStatePtr;

typedef boost::shared_ptr<class SymbolicOperators> SymbolicOperatorsPtr;
class SymbolicOperators: public IS::SymbolicSemantics::RiscOperators {
public:
    typedef IS::SymbolicSemantics::RiscOperators Super;

protected: // Real constructors
    SymbolicOperators(const ISBase::SValuePtr &protoval, const SmtSolverPtr &solver)
        : Super(protoval, solver) {
        name("Concolic-symbolic");
        (void) SymbolicValue::promote(protoval);
    }

    SymbolicOperators(const ISBase::StatePtr &state, const SmtSolverPtr &solver)
        : Super(state, solver) {
        name("Concolic-symbolic");
        (void) SymbolicValue::promote(state->protoval());
    }

public: // Static allocating constructors
    static SymbolicOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr()) {
        ISBase::SValuePtr protoval = SymbolicValue::instance();
        ISBase::RegisterStatePtr registers = SymbolicRegisters::instance(protoval, regdict);
        ISBase::MemoryStatePtr memory = SymbolicMemory::instance(protoval, protoval);
        ISBase::StatePtr state = SymbolicState::instance(registers, memory);
        return SymbolicOperatorsPtr(new SymbolicOperators(state, solver));
    }

    static SymbolicOperatorsPtr instance(const ISBase::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return SymbolicOperatorsPtr(new SymbolicOperators(protoval, solver));
    }

    static SymbolicOperatorsPtr instance(const ISBase::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return SymbolicOperatorsPtr(new SymbolicOperators(state, solver));
    }

public: // Virtual constructors
    virtual ISBase::RiscOperatorsPtr create(const ISBase::SValuePtr &protoval,
                                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual ISBase::RiscOperatorsPtr create(const ISBase::StatePtr &state,
                                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(state, solver);
    }
    
public: // Dynamic pointer casts
    static SymbolicOperatorsPtr promote(const ISBase::RiscOperatorsPtr &x) {
        SymbolicOperatorsPtr retval = boost::dynamic_pointer_cast<SymbolicOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Combined concrete and symbolic semantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Semantics {
private:
    static const rose_addr_t initialStackPointer = 0xc0000000;
    const P2::Partitioner &partitioner_;
    const RegisterDictionary *regdict_;
    CpuPtr symbolicCpu_;
    CpuPtr concreteCpu_;

public:
    Semantics(const P2::Partitioner &partitioner)
        : partitioner_(partitioner),
          regdict_(partitioner_.instructionProvider().registerDictionary()) {

        // Symbolic semantics
        SymbolicOperatorsPtr symbolicOps = SymbolicOperators::instance(regdict_);
        SymbolicRegistersPtr symbolicRegisters = SymbolicRegisters::promote(symbolicOps->currentState()->registerState());
        SymbolicMemoryPtr symbolicMemory = SymbolicMemory::promote(symbolicOps->currentState()->memoryState());
        symbolicCpu_ = partitioner_.newDispatcher(symbolicOps);
        ASSERT_not_null(symbolicCpu_);

        // Concrete semantics
        ConcreteOperatorsPtr concreteOps = ConcreteOperators::instance(regdict_);
        ConcreteRegistersPtr concreteRegisters = ConcreteRegisters::promote(concreteOps->currentState()->registerState());
        ConcreteMemoryPtr concreteMemory = ConcreteMemory::promote(concreteOps->currentState()->memoryState());
        MemoryMap::Ptr concreteMap = partitioner_.memoryMap()->shallowCopy();
        concreteMap->shrinkUnshare(); // make it a deep copy
        concreteMemory->memoryMap(concreteMap);
        concreteCpu_ = partitioner_.newDispatcher(concreteOps);
        ASSERT_not_null(concreteCpu_);

        // Give the stack pointer a concrete value for both domains
        const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
        concreteOperators()->writeRegister(SP, concreteOperators()->number_(SP.nBits(), initialStackPointer));
        symbolicOperators()->writeRegister(SP, symbolicOperators()->number_(SP.nBits(), initialStackPointer));
    }

    SymbolicOperatorsPtr symbolicOperators() const {
        return SymbolicOperators::promote(symbolicCpu_->get_operators());
    }

    ConcreteOperatorsPtr concreteOperators() const {
        return ConcreteOperators::promote(concreteCpu_->get_operators());
    }

    SymbolicRegistersPtr symbolicRegisters() const {
        return SymbolicRegisters::promote(symbolicCpu_->currentState()->registerState());
    }

    ConcreteRegistersPtr concreteRegisters() const {
        return ConcreteRegisters::promote(concreteCpu_->currentState()->registerState());
    }

    SymbolicMemoryPtr symbolicMemory() const {
        return SymbolicMemory::promote(symbolicCpu_->currentState()->memoryState());
    }

    ConcreteMemoryPtr concreteMemory() const {
        return ConcreteMemory::promote(concreteCpu_->currentState()->memoryState());
    }
    
    // Process the specified instruction and transition concrete and symbolic states based on the instruction's semantics.
    void processInstruction(SgAsmInstruction *insn) {
        ASSERT_not_null(insn);

        // FIXME[Robb Matzke 2019-06-07]: These should be executed in tandom even if the first throws an exception
        symbolicCpu_->processInstruction(insn);
        concreteCpu_->processInstruction(insn);
    }

    // Returns the instruction pointer register's concrete value from the concrete domain.
    rose_addr_t concreteInstructionPointer() const {
        return concreteOperators()
            ->peekRegister(partitioner_.instructionProvider().instructionPointerRegister())
            ->get_number();
    }

    // Print the concrete and symbolic states for debugging
    void print(std::ostream &out) const {
        IS::SymbolicSemantics::Formatter symbolicFmt;
        symbolicFmt.set_line_prefix("  ");
        IS::ConcreteSemantics::Formatter concreteFmt;
        concreteFmt.set_line_prefix("  ");

        out <<"symbolic registers:\n";
        out <<(*symbolicRegisters() + symbolicFmt);
        out <<"concrete registers:\n";
        out <<(*concreteRegisters() + concreteFmt);
        out <<"symbolic memory:\n";
        out <<(*symbolicMemory() + symbolicFmt);
        out <<"concrete memory:\n";
        out <<(*concreteMemory() + concreteFmt);
    }
};

std::ostream&
operator<<(std::ostream &out, const Semantics &semantics) {
    semantics.print(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConcolicExecutor::Ptr
ConcolicExecutor::instance() {
    return Ptr(new ConcolicExecutor);
}

    
std::vector<TestCase::Ptr>
ConcolicExecutor::execute(const Database::Ptr &db, const TestCase::Ptr &testCase) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);
    P2::Partitioner partitioner = partition(db, testCase->specimen());
    ASSERT_not_implemented("more to do");
}

P2::Partitioner
ConcolicExecutor::partition(const Database::Ptr &db, const Specimen::Ptr &specimen) {
    ASSERT_not_null(db);
    ASSERT_not_null(specimen);
    Database::SpecimenId specimenId = db->id(specimen, Update::NO);
    ASSERT_require2(specimenId, "specimen must be in the database");

    P2::Engine engine;
    engine.settings().engine = settings_.partitionerEngine;
    engine.settings().loader = settings_.loader;
    engine.settings().disassembler = settings_.disassembler;
    engine.settings().partitioner = settings_.partitioner;

    P2::Partitioner partitioner;
    if (!db->rbaExists(specimenId)) {
        // Extract the specimen into a temporary file in order to parse it
        Sawyer::FileSystem::TemporaryDirectory tempDir;
        boost::filesystem::path specimenFileName = tempDir.name() / "specimen";
        std::ofstream specimenFile(specimenFileName.native().c_str());
        ASSERT_forbid(specimen->content().empty()); // &std::vector<T>::operator[](0) is UB if empty; "data" is C++11
        specimenFile.write((const char*)&specimen->content()[0], specimen->content().size());
        specimenFile.close();
        partitioner = engine.partition(specimenFileName.native());

        // Cache the results in the database.
        boost::filesystem::path rbaFileName = tempDir.name() / "specimen.rba";
        engine.savePartitioner(partitioner, rbaFileName);
        db->saveRbaFile(rbaFileName, specimenId);
    } else {
        Sawyer::FileSystem::TemporaryFile rbaFile;
        db->extractRbaFile(rbaFile.name(), specimenId);
        partitioner = engine.loadPartitioner(rbaFile.name());
    }

    return partitioner;
}

void
ConcolicExecutor::run(const P2::Partitioner &partitioner) {
    BOOST_FOREACH (const P2::Function::Ptr &function, partitioner.functions()) {
        if ((function->reasons() & SgAsmFunction::FUNC_ENTRY_POINT) != 0) {
            run(partitioner, function->address());
            break;
        }
    }
}

void
ConcolicExecutor::run(const P2::Partitioner &partitioner, rose_addr_t startVa) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Semantics semantics(partitioner);

    // Process instructions in execution order starting at startVa
    rose_addr_t va = startVa;
    while (true) {
        SgAsmInstruction *insn = partitioner.instructionProvider()[va];
        ASSERT_not_null2(insn, StringUtility::addrToString(va) + " is not a valid execution address");
        SAWYER_MESG(debug) <<"executing " <<insn->toString() <<"\n";
        semantics.processInstruction(insn);
        va = semantics.concreteInstructionPointer();
        SAWYER_MESG(debug) <<semantics;
    }
}

} // namespace
} // namespace
} // namespace
