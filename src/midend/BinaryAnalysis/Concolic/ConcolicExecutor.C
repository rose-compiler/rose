#include <sage3basic.h>

#include <BinaryConcolic.h>
#include <CommandLine.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/FileSystem.h>
#include <SqlDatabase.h>

#ifdef __linux__
#include <sys/syscall.h>
#endif

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantic domain for concrete execution.
//
// For now we're emulating instructions with the concrete semantic domain because this is the more general method, but the plan
// is to replace this with native execution (e.g., single stepping in a debugger) where it can be supported.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Emulation {

ConcreteOperatorsPtr
ConcreteOperators::instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver) {
    BaseValuePtr protoval = ConcreteValue::instance();
    BaseRegistersPtr registers = ConcreteRegisters::instance(protoval, regdict);
    BaseMemoryPtr memory = ConcreteMemory::instance(protoval, protoval);
    BaseStatePtr state = ConcreteState::instance(registers, memory);
    return ConcreteOperatorsPtr(new ConcreteOperators(state, solver));
}

ConcreteOperatorsPtr
ConcreteOperators::instance(const BaseValuePtr &protoval, const SmtSolverPtr &solver) {
    return ConcreteOperatorsPtr(new ConcreteOperators(protoval, solver));
}

ConcreteOperatorsPtr
ConcreteOperators::instance(const BaseStatePtr &state, const SmtSolverPtr &solver) {
    return ConcreteOperatorsPtr(new ConcreteOperators(state, solver));
}

BaseOperatorsPtr
ConcreteOperators::create(const BaseValuePtr &protoval, const SmtSolverPtr &solver) const {
    return instance(protoval, solver);
}

BaseOperatorsPtr
ConcreteOperators::create(const BaseStatePtr &state, const SmtSolverPtr &solver) const {
    return instance(state, solver);
}
    
ConcreteOperatorsPtr
ConcreteOperators::promote(const BaseOperatorsPtr &x) {
    ConcreteOperatorsPtr retval = boost::dynamic_pointer_cast<ConcreteOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

void
ConcreteOperators::interrupt(int majr, int minr) {
    if (x86_exception_int == majr && 0x80 == minr) {
        systemCall();
    } else {
        Super::interrupt(majr, minr);
    }
}

void
ConcreteOperators::systemCall() {
    // Result of an "int 0x80" system call instruction
    const RegisterDictionary *regdict = currentState()->registerState()->get_register_dictionary();
    const RegisterDescriptor REG_AX = *regdict->lookup("rax");
    BaseValuePtr ax = peekRegister(REG_AX);
#ifdef __linux__
    if (SYS_exit_group == ax->get_number() || SYS_exit == ax->get_number()) {
        const RegisterDescriptor REG_DI = *regdict->lookup("rdi");
        BaseValuePtr di = readRegister(REG_DI);
        int64_t exitValue = di->get_number();

        // FIXME[Robb Matzke 2019-06-06]: need better way to handle this
        mlog[ERROR] <<"specimen exiting with value " <<exitValue <<"\n";
        exit(0);
    }
#endif
}

} // namespace


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantic domain for symbolic execution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Emulation {

SymbolicOperatorsPtr
SymbolicOperators::instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver) {
    BaseValuePtr protoval = SymbolicValue::instance();
    BaseRegistersPtr registers = SymbolicRegisters::instance(protoval, regdict);
    BaseMemoryPtr memory = SymbolicMemory::instance(protoval, protoval);
    BaseStatePtr state = SymbolicState::instance(registers, memory);
    return SymbolicOperatorsPtr(new SymbolicOperators(state, solver));
}

SymbolicOperatorsPtr
SymbolicOperators::instance(const BaseValuePtr &protoval, const SmtSolverPtr &solver) {
    return SymbolicOperatorsPtr(new SymbolicOperators(protoval, solver));
}

SymbolicOperatorsPtr
SymbolicOperators::instance(const BaseStatePtr &state, const SmtSolverPtr &solver) {
    return SymbolicOperatorsPtr(new SymbolicOperators(state, solver));
}

BaseOperatorsPtr
SymbolicOperators::create(const BaseValuePtr &protoval, const SmtSolverPtr &solver) const {
    return instance(protoval, solver);
}

BaseOperatorsPtr
SymbolicOperators::create(const BaseStatePtr &state, const SmtSolverPtr &solver) const {
    return instance(state, solver);
}
    
SymbolicOperatorsPtr
SymbolicOperators::promote(const BaseOperatorsPtr &x) {
    SymbolicOperatorsPtr retval = boost::dynamic_pointer_cast<SymbolicOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

BaseValuePtr
SymbolicOperators::readRegister(RegisterDescriptor reg, const BaseValuePtr &dflt) {
    BaseValuePtr retval = Super::readRegister(reg, dflt);
    if (SymbolicExpr::LeafPtr variable = SymbolicValue::promote(retval)->get_expression()->isLeafNode()) {
        if (variable->isVariable() && !variables_.exists(variable))
            variables_.insert(variable, VariableProvenance(reg));
    }
    return retval;
}

BaseValuePtr
SymbolicOperators::readMemory(RegisterDescriptor segreg, const BaseValuePtr &addr, const BaseValuePtr &dflt,
                              const BaseValuePtr &cond) {
    BaseValuePtr retval = Super::readMemory(segreg, addr, dflt, cond);
    if (SymbolicExpr::LeafPtr variable = SymbolicValue::promote(retval)->get_expression()->isLeafNode()) {
        if (variable->isVariable() && !variables_.exists(variable))
            variables_.insert(variable, VariableProvenance(addr));
    }
    return retval;
}

} // namespace


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Combined concrete and symbolic semantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Emulation {

void
Semantics::init() {
    // Extend the register set with an additional Boolean register named "path"
    RegisterDictionary *regdict = new RegisterDictionary("Rose::BinaryAnalysis::Concolic");
    regdict->insert(partitioner_.instructionProvider().registerDictionary());
    ASSERT_require(REG_PATH.isEmpty());
    REG_PATH = RegisterDescriptor(regdict->firstUnusedMajor(), 0, 0, 1);
    regdict->insert("path", REG_PATH);
    regdict_ = regdict;

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
    concreteOperators()->writeRegister(SP, concreteOperators()->number_(SP.nBits(), initialStackPointer_));
    symbolicOperators()->writeRegister(SP, symbolicOperators()->number_(SP.nBits(), initialStackPointer_));

    // The PATH pseudo-register's initial value is true
    concreteOperators()->writeRegister(REG_PATH, concreteOperators()->boolean_(true));
    symbolicOperators()->writeRegister(REG_PATH, symbolicOperators()->boolean_(true));
}

SymbolicOperatorsPtr
Semantics::symbolicOperators() const {
    return SymbolicOperators::promote(symbolicCpu_->get_operators());
}

ConcreteOperatorsPtr
Semantics::concreteOperators() const {
    return ConcreteOperators::promote(concreteCpu_->get_operators());
}

SymbolicRegistersPtr
Semantics::symbolicRegisters() const {
    return SymbolicRegisters::promote(symbolicCpu_->currentState()->registerState());
}

ConcreteRegistersPtr
Semantics::concreteRegisters() const {
    return ConcreteRegisters::promote(concreteCpu_->currentState()->registerState());
}

SymbolicMemoryPtr
Semantics::symbolicMemory() const {
    return SymbolicMemory::promote(symbolicCpu_->currentState()->memoryState());
}

ConcreteMemoryPtr
Semantics::concreteMemory() const {
    return ConcreteMemory::promote(concreteCpu_->currentState()->memoryState());
}
    
void
Semantics::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);

    // FIXME[Robb Matzke 2019-06-07]: These should be executed in tandom even if the first throws an exception
    symbolicCpu_->processInstruction(insn);
    concreteCpu_->processInstruction(insn);
}

rose_addr_t
Semantics::concreteInstructionPointer() const {
    return concreteOperators()
        ->peekRegister(partitioner_.instructionProvider().instructionPointerRegister())
        ->get_number();
}

const Variables&
Semantics::symbolicVariables() const {
    return symbolicOperators()->variables();
}
    
void
Semantics::print(std::ostream &out) const {
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

std::ostream&
operator<<(std::ostream &out, const Semantics &semantics) {
    semantics.print(out);
    return out;
}

} // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
ConcolicExecutor::Ptr
ConcolicExecutor::instance() {
    return Ptr(new ConcolicExecutor);
}

// class method
std::vector<Sawyer::CommandLine::SwitchGroup>
ConcolicExecutor::commandLineSwitches(Settings &settings /*in,out*/) {
    std::vector<Sawyer::CommandLine::SwitchGroup> sgroups;
    sgroups.push_back(P2::Engine::loaderSwitches(settings.loader));
    sgroups.push_back(P2::Engine::disassemblerSwitches(settings.disassembler));
    sgroups.push_back(P2::Engine::partitionerSwitches(settings.partitioner));
    return sgroups;
}

std::vector<TestCase::Ptr>
ConcolicExecutor::execute(const Database::Ptr &db, const TestCase::Ptr &testCase) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);
    P2::Partitioner partitioner = partition(db, testCase->specimen());
    run(partitioner);
    std::vector<TestCase::Ptr> newCases;
    return newCases;
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

    return boost::move(partitioner);
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
    Sawyer::Message::Stream trace(mlog[TRACE]);
    Emulation::Semantics semantics(partitioner);

    // Process instructions in execution order starting at startVa
    rose_addr_t va = startVa;
    while (true) {
        SgAsmInstruction *insn = partitioner.instructionProvider()[va];
        ASSERT_not_null2(insn, StringUtility::addrToString(va) + " is not a valid execution address");
        SAWYER_MESG_OR(trace, debug) <<"executing " <<insn->toString() <<"\n";
        semantics.processInstruction(insn);
#if 1 // DEBUGGING [Robb Matzke 2019-08-15]
        printVariables(std::cerr, semantics);
#endif
        va = semantics.concreteInstructionPointer();
        SAWYER_MESG(debug) <<semantics;
    }
}

// Print variable information for debugging
void
ConcolicExecutor::printVariables(std::ostream &out, const Emulation::Semantics &semantics) const {
    RegisterNames registerNames(semantics.partitioner().instructionProvider().registerDictionary());
    BOOST_FOREACH (const Emulation::Variables::Node &variable, semantics.symbolicVariables().nodes()) {
        out <<*variable.key() <<": ";
        switch (variable.value().whence) {
            case Emulation::VariableProvenance::REGISTER:
                out <<"register " <<registerNames(variable.value().reg) <<"\n";
                break;
            case Emulation::VariableProvenance::MEMORY:
                out <<"memory " <<*variable.value().addr <<"\n";
                break;
            case Emulation::VariableProvenance::INVALID:
                out <<"invalid\n";
                break;
        }
    }
}

} // namespace
} // namespace
} // namespace
