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
// RiscOperators for concolic emulation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Emulation {

RiscOperatorsPtr
RiscOperators::instance(const P2::Partitioner &partitioner, const Debugger::Ptr &process,
                        const IS::BaseSemantics::SValuePtr &protoval, const SmtSolver::Ptr &solver) {
    // Extend the register set with an additional Boolean register named "path"
    RegisterDictionary *regdict = new RegisterDictionary("Rose::BinaryAnalysis::Concolic");
    regdict->insert(process->registerDictionary());
    const RegisterDescriptor path(process->registerDictionary()->firstUnusedMajor(), 0, 0, 1);
    regdict->insert("path", path);

    // Initialize machine state
    RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
    MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
    memory->set_byteOrder(ByteOrder::ORDER_LSB);
    StatePtr state = State::instance(registers, memory);
    RiscOperatorsPtr ops(new RiscOperators(partitioner, process, state, solver));
    ASSERT_require(ops->REG_PATH == path);
    ops->writeRegister(path, ops->boolean_(true));

    // Mark program inputs
    ops->markProgramArguments();

    return ops;
}

RiscOperatorsPtr
RiscOperators::promote(const IS::BaseSemantics::RiscOperatorsPtr &x) {
    RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

size_t
RiscOperators::wordSizeBits() const {
    return partitioner_.instructionProvider().instructionPointerRegister().nBits();
}

const RegisterDictionary*
RiscOperators::registerDictionary() const {
    return partitioner_.instructionProvider().registerDictionary();
}

void
RiscOperators::interrupt(int majr, int minr) {
    if (x86_exception_int == majr && 0x80 == minr) {
        systemCall();
    } else {
        Super::interrupt(majr, minr);
    }
}

void
RiscOperators::systemCall() {
    // Result of an "int 0x80" system call instruction
    const RegisterDictionary *regdict = currentState()->registerState()->get_register_dictionary();
    const RegisterDescriptor REG_AX = *regdict->lookup("rax");
    IS::BaseSemantics::SValuePtr ax = peekRegister(REG_AX, undefined_(REG_AX.nBits()));
#ifdef __linux__
    if (SYS_exit_group == ax->get_number() || SYS_exit == ax->get_number()) {
        const RegisterDescriptor REG_DI = *regdict->lookup("rdi");
        IS::BaseSemantics::SValuePtr di = readRegister(REG_DI, undefined_(REG_DI.nBits()));
        int64_t exitValue = di->get_number();

        // FIXME[Robb Matzke 2019-06-06]: need better way to handle this
        mlog[ERROR] <<"specimen exiting with value " <<exitValue <<"\n";
        exit(0);
    }
#endif
}

IS::BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const IS::BaseSemantics::SValuePtr &dflt) {
    IS::BaseSemantics::SValuePtr retval = Super::readRegister(reg, dflt);
    if (SymbolicExpr::LeafPtr variable = SValue::promote(retval)->get_expression()->isLeafNode()) {
        if (variable->isVariable2() && !variables_.exists(variable))
            variables_.insert(variable, VariableProvenance(reg));
    }
    return retval;
}

IS::BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const IS::BaseSemantics::SValuePtr &addr,
                          const IS::BaseSemantics::SValuePtr &dflt, const IS::BaseSemantics::SValuePtr &cond) {
    SValuePtr retval = SValue::promote(Super::readMemory(segreg, addr, dflt, cond));
    if (SymbolicExpr::LeafPtr variable = retval->get_expression()->isLeafNode()) {
        if (variable->isVariable2() && !variables_.exists(variable))
            variables_.insert(variable, VariableProvenance(addr));
    }
    return retval;
}

void
RiscOperators::markProgramArguments() {
    // For Linux ELF x86 and amd64, the argc and argv values are on the stack, not in registers.
    const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
    size_t wordSizeBytes = SP.nBits() / 8;
    rose_addr_t sp = process_->readRegister(SP).toInteger();
    rose_addr_t argcVa = sp;                            // specimen's &argc
    rose_addr_t argvVa = sp + wordSizeBytes;            // specimen's argv+0
    size_t argc = process_->readMemory(sp, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();
#if 1 // DEBUGGING [Robb Matzke 2019-09-23]
    std::cerr <<"ROBB: argc = " <<argc <<"\n";
#endif

    // Create symbolic values for argc
    SValuePtr argcSem = SValue::promote(undefined_(SP.nBits()));
    argcSem->set_comment("argc");
    writeMemory(RegisterDescriptor(), number_(SP.nBits(), argcVa), argcSem, boolean_(true));
}

rose_addr_t
Dispatcher::concreteInstructionPointer() const {
    return RiscOperators::promote(get_operators())->process()->executionAddress();
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);

    // Concrete execution
    Debugger::Ptr process = RiscOperators::promote(get_operators())->process();
    process->executionAddress(insn->get_address());
    process->singleStep();

    // Symbolic execution
    Super::processInstruction(insn);
}

bool
Dispatcher::isTerminated() const {
    return RiscOperators::promote(get_operators())->process()->isTerminated();
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConcolicExecutor
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
        boost::filesystem::permissions(specimenFileName, boost::filesystem::owner_all);

        // Use the "run:" scheme to load the simulated virtual memory in order to get all the shared libraries.
        partitioner = engine.partition("run:replace:" + specimenFileName.native());

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

Debugger::Ptr
ConcolicExecutor::makeProcess(const Database::Ptr &db, const TestCase::Ptr &testCase,
                              Sawyer::FileSystem::TemporaryDirectory &tempDir) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);

    // Extract the executable into the working directory
    boost::filesystem::path exeName = boost::filesystem::path(testCase->specimen()->name()).filename();
    if (exeName.empty())
        exeName = "a.out";
    exeName = tempDir.name() / exeName;
    {
        std::ofstream executable(exeName.native().c_str(), std::ios_base::binary | std::ios_base::trunc);
        if (!executable)
            mlog[ERROR] <<"cannot write to " <<exeName <<"\n";
        if (!testCase->specimen()->content().empty()) {
            executable.write(reinterpret_cast<const char*>(&testCase->specimen()->content()[0]),
                             testCase->specimen()->content().size());
        }
    }
    boost::filesystem::permissions(exeName, boost::filesystem::owner_all);

    // Describe the process to be created from the executable
    Debugger::Specimen ds = exeName;
    ds.arguments(testCase->args());
    ds.workingDirectory(tempDir.name());
    ds.flags()
        .set(Debugger::REDIRECT_INPUT)
        .set(Debugger::REDIRECT_OUTPUT)
        .set(Debugger::REDIRECT_ERROR)
        .set(Debugger::CLOSE_FILES);

    // Create the process, which is where all the concrete state is stored.
    return Debugger::instance(ds);
}

std::vector<TestCase::Ptr>
ConcolicExecutor::execute(const Database::Ptr &db, const TestCase::Ptr &testCase) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);
    Sawyer::FileSystem::TemporaryDirectory tempDir;     // working files for this execution

    // Create the semantics layers. The symbolic semantics uses a Partitioner, and the concrete semantics uses a suborinate
    // process which is created from the specimen.
    P2::Partitioner partitioner = partition(db, testCase->specimen());
    Debugger::Ptr process = makeProcess(db, testCase, tempDir);
    Emulation::RiscOperatorsPtr ops =
        Emulation::RiscOperators::instance(partitioner, process, Emulation::SValue::instance(), SmtSolver::Ptr());
    Emulation::DispatcherPtr cpu = Emulation::Dispatcher::instance(ops);

    run(cpu);
    
    std::vector<TestCase::Ptr> newCases;
    return newCases;
}

void
ConcolicExecutor::run(const Emulation::DispatcherPtr &cpu) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream trace(mlog[TRACE]);
    Emulation::RiscOperatorsPtr ops = Emulation::RiscOperators::promote(cpu->get_operators());

    // Process instructions in execution order starting at startVa
    rose_addr_t va = cpu->concreteInstructionPointer();
    while (!cpu->isTerminated()) {
        SgAsmInstruction *insn = ops->partitioner().instructionProvider()[va];
        ASSERT_not_null2(insn, StringUtility::addrToString(va) + " is not a valid execution address");
        SAWYER_MESG_OR(trace, debug) <<"executing " <<insn->toString() <<"\n";
        cpu->processInstruction(insn);
        if (cpu->isTerminated()) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has terminated\n";
            break;
        }

#if 1 // DEBUGGING [Robb Matzke 2019-08-15]
        printVariables(std::cerr, ops->currentState());
#endif
        va = cpu->concreteInstructionPointer();
    }
}

// Print variable information for debugging
void
ConcolicExecutor::printVariables(std::ostream &out, const Emulation::StatePtr &state) const {
    out <<*state;
}

} // namespace
} // namespace
} // namespace
