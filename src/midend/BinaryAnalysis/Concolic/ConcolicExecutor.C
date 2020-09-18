#include <sage3basic.h>
#include <BinaryConcolic.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <boost/format.hpp>
#include <CommandLine.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/FileSystem.h>
#include <SqlDatabase.h>
#include <TraceSemantics2.h>

#ifdef __linux__
#include <sys/mman.h>
#include <sys/syscall.h>
#endif

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
InputVariables::Variable::print(std::ostream &out) const {
    switch (whence_) {
        case INVALID:
            out <<"nothing";
            break;
        case PROGRAM_ARGUMENT_COUNT:
            out <<"argc";
            break;
        case PROGRAM_ARGUMENT:
            out <<"argv[" <<arrayOfStrings.idx1 <<"][" <<arrayOfStrings.idx2 <<"]";
            break;
        case ENVIRONMENT:
            out <<"envp[" <<arrayOfStrings.idx1 <<"][" <<arrayOfStrings.idx2 <<"]";
            break;
        case SYSTEM_CALL_RETVAL:
            out <<"syscall";
            break;
    }
}

void
InputVariables::insertProgramArgumentCount(const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    variables_.insert(*symbolic->variableId(), Variable::programArgc());
}

void
InputVariables::insertProgramArgument(size_t i, size_t j, const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    variables_.insert(*symbolic->variableId(), Variable::programArgument(i, j));
}

void
InputVariables::insertEnvironmentVariable(size_t i, size_t j, const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    variables_.insert(*symbolic->variableId(), Variable::environmentVariable(i, j));
}

InputVariables::Variable
InputVariables::get(const std::string &symbolicVariableName) const {
    ASSERT_require(symbolicVariableName.size() >= 2);
    ASSERT_require(symbolicVariableName[0] == 'v');
    uint64_t varId = rose_strtoull(symbolicVariableName.c_str()+1, NULL, 10);
    return variables_.getOrDefault(varId);
}

void
InputVariables::print(std::ostream &out, const std::string &prefix) const {
    BOOST_FOREACH (const Variables::Node &node, variables_.nodes())
        out <<prefix <<node.value() <<" = v" <<node.key() <<"\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RiscOperators for concolic emulation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Emulation {

RiscOperatorsPtr
RiscOperators::instance(const Settings &settings, const P2::Partitioner &partitioner, const Debugger::Ptr &process,
                        InputVariables &inputVariables, const IS::BaseSemantics::SValuePtr &protoval,
                        const SmtSolver::Ptr &solver) {
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
    RiscOperatorsPtr ops(new RiscOperators(settings, partitioner, process, inputVariables, state, solver));
    ASSERT_require(ops->REG_PATH == path);
    ops->writeRegister(path, ops->boolean_(true));

    // Mark program inputs
    ops->markProgramArguments(solver);

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

IS::BaseSemantics::SValuePtr
RiscOperators::systemCallNumber() {
    // FIXME[Robb Matzke 2020-08-28]: Assumes x86
    if (32 == partitioner_.instructionProvider().wordSize()) {
        const RegisterDescriptor AX = partitioner_.instructionProvider().registerDictionary()->findOrThrow("eax");
        return readRegister(AX);
    } else {
        ASSERT_require(64 == partitioner_.instructionProvider().wordSize());
        const RegisterDescriptor AX = partitioner_.instructionProvider().registerDictionary()->findOrThrow("rax");
        return readRegister(AX);
    }
}

IS::BaseSemantics::SValuePtr
RiscOperators::systemCallArgument(size_t idx) {
    // FIXME[Robb Matzke 2020-08-28]: assuming x86
    if (partitioner_.instructionProvider().wordSize() == 32) {
        ASSERT_require(idx < 6);
        switch (idx) {
            case 0: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("ebx");
                return readRegister(r);
            }
            case 1: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("ecx");
                return readRegister(r);
            }
            case 2: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("edx");
                return readRegister(r);
            }
            case 3: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("esi");
                return readRegister(r);
            }
            case 4: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("edi");
                return readRegister(r);
            }
            case 5: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("ebp");
                return readRegister(r);
            }
        }
    } else {
        ASSERT_require(idx < 6);
        switch (idx) {
            case 0: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("rdi");
                return readRegister(r);
            }
            case 1: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("rsi");
                return readRegister(r);
            }
            case 2: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("rdx");
                return readRegister(r);
            }
            case 3: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("r10");
                return readRegister(r);
            }
            case 4: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("r8");
                return readRegister(r);
            }
            case 5: {
                const RegisterDescriptor r = partitioner_.instructionProvider().registerDictionary()->findOrThrow("r9");
                return readRegister(r);
            }
        }
    }
    ASSERT_not_reachable("invalid system call number");
}

void
RiscOperators::doExit(const IS::BaseSemantics::SValuePtr &status) {
    if (status->is_number()) {
        int exitValue = status->get_number();
        mlog[INFO] <<"specimen exiting with " <<exitValue <<"\n";
        throw Exit(SValue::promote(status));
    } else {
        mlog[INFO] <<"specimen exiting with unknown value\n";
        throw Exit(SValue::promote(status));
    }
}

void
RiscOperators::doGetuid() {
    ASSERT_not_implemented("[Robb Matzke 2020-08-28]");
}

void
RiscOperators::systemCall() {
    ASSERT_always_require2(isSgAsmX86Instruction(currentInstruction()), "ISA not implemented yet");
    IS::BaseSemantics::SValuePtr scn = systemCallNumber();

    // A few system calls are handled directly.
    if (scn->is_number()) {
        if (32 == partitioner_.instructionProvider().wordSize()) {
            switch (scn->get_number()) {
                case 1:                                 // exit
                case 252:                               // exit_group
                    return doExit(systemCallArgument(0));
                case 24:                                // getuid
                    return doGetuid();
            }
        } else {
            ASSERT_require(partitioner_.instructionProvider().wordSize() == 64);
            switch (scn->get_number()) {
                case 60:                                // exit
                case 231:                               // exit_group
                    return doExit(systemCallArgument(0));
                case 102:                               // getuid
                    return doGetuid();
            }
        }
    }

    // FIXME[Robb Matzke 2020-08-28]: all other system calls are just ignored for now
}

IS::BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const IS::BaseSemantics::SValuePtr &dfltUnused) {
    // Read the register's value symbolically, and if we don't have a value then read it concretely and use the concrete value
    // to update the symbolic state.
    SValuePtr dflt = SValue::promote(undefined_(dfltUnused->get_width()));
    SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readRegister(reg));
    dflt->set_expression(concrete);
    return Super::readRegister(reg, dflt);
}

IS::BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const IS::BaseSemantics::SValuePtr &dfltUnused) {
    // Return the register's symbolic value if it exists, else the concrete value.
    SValuePtr dflt = SValue::promote(undefined_(dfltUnused->get_width()));
    SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readRegister(reg));
    dflt->set_expression(concrete);
    return Super::peekRegister(reg, dflt);
}

IS::BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const IS::BaseSemantics::SValuePtr &addr,
                          const IS::BaseSemantics::SValuePtr &dfltUnused, const IS::BaseSemantics::SValuePtr &cond) {
    // Read the memory's value symbolically, and if we don't have a value then read it concretely and use the concrete value to
    // update the symbolic state. However, we can only read it concretely if the address is a constant.
    if (addr->is_number()) {
        rose_addr_t va = addr->get_number();
        size_t nBytes = dfltUnused->get_width() / 8;
        SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readMemory(va, nBytes, ByteOrder::ORDER_LSB));
        SValuePtr dflt = SValue::promote(undefined_(dfltUnused->get_width()));
        dflt->set_expression(concrete);
        return Super::readMemory(segreg, addr, dflt, cond);
    } else {
        return Super::readMemory(segreg, addr, dfltUnused, cond);
    }
}

IS::BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg, const IS::BaseSemantics::SValuePtr &addr,
                          const IS::BaseSemantics::SValuePtr &dfltUnused) {
    // Read the memory's symbolic value if it exists, else read the concrete value. We can't read concretely if the address is
    // symbolic.
    if (addr->is_number()) {
        rose_addr_t va = addr->get_number();
        size_t nBytes = dfltUnused->get_width() / 8;
        SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readMemory(va, nBytes, ByteOrder::ORDER_LSB));
        SValuePtr dflt = SValue::promote(undefined_(dfltUnused->get_width()));
        dflt->set_expression(concrete);
        return Super::peekMemory(segreg, addr, dflt);
    } else {
        return Super::peekMemory(segreg, addr, dfltUnused);
    }
}

void
RiscOperators::markProgramArguments(const SmtSolver::Ptr &solver) {
    ASSERT_not_null(solver);

    // For Linux ELF x86 and amd64, the argc and argv values are on the stack, not in registers. Also note that there is
    // no return address on the stack (i.e., the stack pointer is pointing at argc, not a return address). This means all the
    // stack argument offsets are four (or eight) bytes less than usual.
    //
    // The stack looks like this:
    //   +-------------------------------------------------+
    //   | argv[argc] 4-byte zero                          |
    //   +-------------------------------------------------+
    //   | ...                                             |
    //   +-------------------------------------------------+
    //   | argv[1], 4-byte address for start of a C string |
    //   +-------------------------------------------------+
    //   | argv[0], 4-byte address for start of a C string |
    //   +-------------------------------------------------+
    //   | argc value, 4-byte integer, e.g., 2             |  <--- ESP points here
    //   +-------------------------------------------------+
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"marking program arguments\n";
    const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
    size_t wordSizeBytes = SP.nBits() / 8;
    SValueFormatter fmt;
    fmt.expr_formatter.show_comments = SymbolicExpr::Formatter::CMT_AFTER;

    //---------------------------------------------------------------------------------------------------------------------------
    // argc
    //---------------------------------------------------------------------------------------------------------------------------
    rose_addr_t argcVa = process_->readRegister(SP).toInteger();
    size_t argc = process_->readMemory(argcVa, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();

    SValuePtr symbolicArgc = SValue::promote(undefined_(SP.nBits()));
    symbolicArgc->set_comment("argc");
    writeMemory(RegisterDescriptor(), number_(SP.nBits(), argcVa), symbolicArgc, boolean_(true));
    inputVariables_.insertProgramArgumentCount(symbolicArgc->get_expression());

    SAWYER_MESG(debug) <<"  argc @" <<StringUtility::addrToString(argcVa) <<" = " <<argc
                       <<"; symbolic = " <<(*symbolicArgc + fmt) <<"\n";

    // The argc value cannot be less than 1 since it always points to at least the program name.
#if 1 // [Robb Matzke 2020-07-17]: Breaks concolic demo 0
    SymbolicExpr::Ptr argcConstraint = SymbolicExpr::makeSignedGt(symbolicArgc->get_expression(),
                                                                  SymbolicExpr::makeInteger(SP.nBits(), 0));
    solver->insert(argcConstraint);
#endif

    //---------------------------------------------------------------------------------------------------------------------------
    // argv
    //---------------------------------------------------------------------------------------------------------------------------
    rose_addr_t argvVa = argcVa + wordSizeBytes;
    SAWYER_MESG(debug) <<"  argv @" <<StringUtility::addrToString(argvVa) <<" = [\n";
    for (size_t i = 0; i < argc; ++i) {
        rose_addr_t ptrVa = argvVa + i * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = process_->readMemory(ptrVa, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();
        std::string s = process_->readCString(strVa);
        SAWYER_MESG(debug) <<"    " <<i <<": @" <<StringUtility::addrToString(strVa)
                           <<" \"" <<StringUtility::cEscape(s) <<"\"\n";

        if (settings_.markingArgvAsInput) {
            for (size_t j = 0; j <= s.size(); ++j) {
                SValuePtr symbolicChar = SValue::promote(undefined_(8));
                symbolicChar->set_comment((boost::format("argv_%d_%d") % i % j).str());
                SValuePtr charVa = SValue::promote(number_(SP.nBits(), strVa + j));
                writeMemory(RegisterDescriptor(), charVa, symbolicChar, boolean_(true));
                inputVariables_.insertProgramArgument(i, j, symbolicChar->get_expression());
                SAWYER_MESG(debug) <<"      byte " <<j <<" @" <<StringUtility::addrToString(strVa+j)
                                   <<"; symbolic = " <<(*symbolicChar + fmt) <<"\n";
            }
        }
    }
    SAWYER_MESG(debug) <<"    " <<argc <<": @" <<StringUtility::addrToString(argvVa + argc * wordSizeBytes) <<" null\n"
                       <<"  ]\n";

    //---------------------------------------------------------------------------------------------------------------------------
    // envp
    //---------------------------------------------------------------------------------------------------------------------------
    rose_addr_t envpVa = argvVa + (argc+1) * wordSizeBytes;
    SAWYER_MESG(debug) <<"  envp @" <<StringUtility::addrToString(envpVa) <<" = [\n";
    size_t nEnvVars = 0;                                // number of environment variables, excluding null terminator
    while (true) {
        rose_addr_t ptrVa = envpVa + nEnvVars * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = process_->readMemory(ptrVa, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();
        SAWYER_MESG(debug) <<"    " <<nEnvVars <<": @" <<StringUtility::addrToString(strVa);
        if (0 == strVa) {
            SAWYER_MESG(debug) <<" null\n";
            break;
        } else {
            std::string s = process_->readCString(strVa);
            SAWYER_MESG(debug) <<" \"" <<StringUtility::cEscape(s) <<"\"\n";

            if (settings_.markingEnvpAsInput) {
                for (size_t i = 0; i <= s.size(); ++i) {
                    SValuePtr symbolicChar = SValue::promote(undefined_(8));
                    symbolicChar->set_comment((boost::format("envp_%d_%d") % nEnvVars % i).str());
                    SValuePtr charVa = SValue::promote(number_(SP.nBits(), strVa + i));
                    writeMemory(RegisterDescriptor(), charVa, symbolicChar, boolean_(true));
                    inputVariables_.insertEnvironmentVariable(nEnvVars, i, symbolicChar->get_expression());
                    SAWYER_MESG(debug) <<"      byte " <<i <<" @" <<StringUtility::addrToString(strVa+i)
                                       <<"; symbolic = " <<(*symbolicChar + fmt) <<"\n";
                }
            }
            ++nEnvVars;
        }
    }
    SAWYER_MESG(debug) <<"  ]\n";

    //---------------------------------------------------------------------------------------------------------------------------
    // auxv
    //---------------------------------------------------------------------------------------------------------------------------
    // Not marking these as inputs because the user that's running the program doesn't have much influence.
    rose_addr_t auxvVa = envpVa + (nEnvVars + 1) * wordSizeBytes;
    SAWYER_MESG(debug) <<"  auxv @" <<StringUtility::addrToString(auxvVa) <<" = [\n";
    size_t nAuxvPairs = 0;
    while (true) {
        rose_addr_t va = auxvVa + nAuxvPairs * 2 * wordSizeBytes;
        size_t key = process_->readMemory(va, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();
        size_t val = process_->readMemory(va + wordSizeBytes, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();
        SAWYER_MESG(debug) <<"    " <<nAuxvPairs <<": key=" <<StringUtility::addrToString(key)
                           <<", val=" <<StringUtility::addrToString(val) <<"\n";
        if (0 == key)
            break;
        ++nAuxvPairs;
    }
    SAWYER_MESG(debug) <<"  ]\n";
}

void
RiscOperators::printInputVariables(std::ostream &out) const {
    out <<"input variables:\n";
    inputVariables_.print(out, "  ");
}

// class method
RiscOperatorsPtr
Dispatcher::unwrapEmulationOperators(const IS::BaseSemantics::RiscOperatorsPtr &ops) {
    if (IS::TraceSemantics::RiscOperatorsPtr trace = boost::dynamic_pointer_cast<IS::TraceSemantics::RiscOperators>(ops))
        return RiscOperators::promote(trace->subdomain());
    return RiscOperators::promote(ops);
}

RiscOperatorsPtr
Dispatcher::emulationOperators() const {
    return unwrapEmulationOperators(get_operators());
}

rose_addr_t
Dispatcher::concreteInstructionPointer() const {
    return emulationOperators()->process()->executionAddress();
}

void
Dispatcher::concreteSingleStep() {
    emulationOperators()->process()->singleStep();
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);

    // We need to single step the concrete execution regardless of whether the symbolic execution throw an exception, and we
    // need to make sure the symbolic execution happens before the concrete execution.
    struct Resources {
        Dispatcher *self;
        SgAsmInstruction *insn;
        Resources(Dispatcher *self, SgAsmInstruction *insn)
            : self(self), insn(insn) {}

        ~Resources() {
            Debugger::Ptr process = self->emulationOperators()->process();
            rose_addr_t ip = self->emulationOperators()->overrideNextIp().orElse(insn->get_address());
            self->emulationOperators()->clearOverrideNextIp();
            process->executionAddress(ip);
            process->singleStep();
        }
    } r(this, insn);

    // Symbolic execution (happens before the code above)
    Super::processInstruction(insn);
}

bool
Dispatcher::isTerminated() const {
    return emulationOperators()->process()->isTerminated();
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
    using namespace Sawyer::CommandLine;

    std::vector<SwitchGroup> sgroups;
    sgroups.push_back(P2::Engine::loaderSwitches(settings.loader));
    sgroups.push_back(P2::Engine::disassemblerSwitches(settings.disassembler));
    sgroups.push_back(P2::Engine::partitionerSwitches(settings.partitioner));

    SwitchGroup ce("Concolic executor switches");
    Rose::CommandLine::insertBooleanSwitch(ce, "show-states", settings.traceState,
                                           "Show the virtual machine state after each instruction is processed.");
    Rose::CommandLine::insertBooleanSwitch(ce, "show-semantics", settings.traceSemantics,
                                           "Show the semantic operations that are performed for each instruction.");
    sgroups.push_back(ce);

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

    // Build the P2::Partitioner object for the specimen
    P2::Partitioner partitioner;
    if (!db->rbaExists(specimenId)) {
        // Extract the specimen into a temporary file in order to parse it
        Sawyer::FileSystem::TemporaryDirectory tempDir;
        boost::filesystem::path specimenFileName = tempDir.name() / "specimen";
        std::ofstream specimenFile(specimenFileName.string().c_str());
        ASSERT_forbid(specimen->content().empty()); // &std::vector<T>::operator[](0) is UB if empty; "data" is C++11
        specimenFile.write((const char*)&specimen->content()[0], specimen->content().size());
        specimenFile.close();
        boost::filesystem::permissions(specimenFileName, boost::filesystem::owner_all);

        // Use the "run:" scheme to load the simulated virtual memory in order to get all the shared libraries.
        partitioner = engine.partition("run:replace:" + specimenFileName.string());

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
                              Sawyer::FileSystem::TemporaryDirectory &tempDir, const P2::Partitioner &partitioner) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);

    // Extract the executable into the working directory
    boost::filesystem::path exeName = boost::filesystem::path(testCase->specimen()->name()).filename();
    if (exeName.empty())
        exeName = "a.out";
    exeName = tempDir.name() / exeName;
    {
        std::ofstream executable(exeName.string().c_str(), std::ios_base::binary | std::ios_base::trunc);
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
    ds.randomizedAddresses(false);
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
    SmtSolver::Ptr solver = SmtSolver::instance("best");
    P2::Partitioner partitioner = partition(db, testCase->specimen());
    Debugger::Ptr process = makeProcess(db, testCase, tempDir, partitioner);
    Emulation::RiscOperatorsPtr ops =
        Emulation::RiscOperators::instance(settings_.emulationSettings, partitioner, process, inputVariables_,
                                           Emulation::SValue::instance(), solver);


    Emulation::DispatcherPtr cpu;
    if (settings_.traceSemantics) {
        IS::BaseSemantics::RiscOperatorsPtr trace = IS::TraceSemantics::RiscOperators::instance(ops);
        cpu = Emulation::Dispatcher::instance(trace);
    } else {
        cpu = Emulation::Dispatcher::instance(ops);
    }

    run(db, testCase, cpu);
    testCase->concolicResult(1);
    db->save(testCase);

    // FIXME[Robb Matzke 2020-01-16]
    std::vector<TestCase::Ptr> newCases;
    return newCases;
}

bool
ConcolicExecutor::updateCallStack(const Emulation::DispatcherPtr &cpu, SgAsmInstruction *insn) {
    ASSERT_not_null(cpu);
    ASSERT_not_null(insn);                              // the instruction that was just executed

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    bool wasChanged = false;

    Emulation::RiscOperatorsPtr ops = cpu->emulationOperators();
    const P2::Partitioner &partitioner = ops->partitioner();
    P2::BasicBlock::Ptr bb = partitioner.basicBlockContainingInstruction(insn->get_address());
    if (!bb || bb->nInstructions() == 0) {
        SAWYER_MESG(debug) <<"no basic block at " <<StringUtility::addrToString(insn->get_address()) <<"\n";
        return wasChanged;                              // we're executing something that ROSE didn't disassemble
    }

    // Get the current stack pointer.
    const RegisterDescriptor SP = partitioner.instructionProvider().stackPointerRegister();
    if (SP.isEmpty())
        return wasChanged;                              // no stack pointer for this architecture?!
    SymbolicExpr::Ptr sp = Emulation::SValue::promote(ops->peekRegister(SP, ops->undefined_(SP.nBits())))->get_expression();
    Sawyer::Optional<uint64_t> stackVa = sp->toUnsigned();
    if (!stackVa) {
        SAWYER_MESG(debug) <<"no concrete SP after function call at " <<StringUtility::addrToString(insn->get_address());
        return wasChanged;                              // no concrete stack pointer. This should be unusual
    }

    // If the stack pointer is larger than the saved stack pointer for the most recent function on the stack (assuming
    // stack-grows-down), then pop that function from the call stack under the assumption that we've returned from that
    // function.
    while (!functionCallStack_.empty() && functionCallStack_.back().stackVa < *stackVa) {
        SAWYER_MESG(debug) <<"assume returned from " <<functionCallStack_.back().printableName <<"\n";
        functionCallStack_.pop_back();
        wasChanged = true;
    }

    // If the last instruction we just executed looks like a function call then push a new record onto our function call stack.
    if (partitioner.basicBlockIsFunctionCall(bb) && insn->get_address() == bb->instructions().back()->get_address()) {
        // Get a name for the function we just called, if possible.
        const RegisterDescriptor IP = partitioner.instructionProvider().instructionPointerRegister();
        SymbolicExpr::Ptr ip = Emulation::SValue::promote(ops->peekRegister(IP, ops->undefined_(IP.nBits())))->get_expression();
        Sawyer::Optional<uint64_t> calleeVa = ip->toUnsigned();
        std::string calleeName;
        if (calleeVa) {
            if (P2::Function::Ptr callee = partitioner.functionExists(*calleeVa)) {
                calleeName = callee->printableName();
            } else {
                calleeName = "function " + StringUtility::addrToString(*calleeVa);
            }
        } else {
            SAWYER_MESG(debug) <<"no concrete IP after function call at " <<StringUtility::addrToString(insn->get_address());
            return wasChanged;
        }

        // Push this function call onto the stack.
        SAWYER_MESG(debug) <<"assume call to " <<calleeName <<" at " <<StringUtility::addrToString(insn->get_address()) <<"\n";
        functionCallStack_.push_back(FunctionCall(calleeName, insn->get_address(), calleeVa.orElse(0), *stackVa));
        wasChanged = true;
    }

    return wasChanged;
}

void
ConcolicExecutor::printCallStack(std::ostream &out) {
    for (const FunctionCall &call: functionCallStack_)
        out <<"  call to " <<call.printableName <<" from " <<StringUtility::addrToString(call.sourceVa) <<"\n";
}

void
ConcolicExecutor::handleBranch(const Database::Ptr &db, const TestCase::Ptr &testCase, const Emulation::DispatcherPtr &cpu,
                               SgAsmInstruction *insn, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);
    ASSERT_not_null(cpu);
    ASSERT_not_null(insn);
    ASSERT_not_null(solver);

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream trace(mlog[TRACE]);
    Sawyer::Message::Stream error(mlog[ERROR]);

    Emulation::RiscOperatorsPtr ops = cpu->emulationOperators();
    const P2::Partitioner &partitioner = ops->partitioner();
    const RegisterDescriptor IP = partitioner.instructionProvider().instructionPointerRegister();

    // If we processed a branch instruction whose condition depended on input variables, then the instruction pointer register
    // in the symbolic state will be non-constant. It should be an if-then-else expression that evaluates to two constants, the
    // true and false execution addresses, one of which should match the concrete execution address.
    SymbolicExpr::Ptr ip = Emulation::SValue::promote(ops->peekRegister(IP, ops->undefined_(IP.nBits())))->get_expression();
    SymbolicExpr::InteriorPtr inode = ip->isInteriorNode();
    if (inode && inode->getOperator() == SymbolicExpr::OP_ITE) {
        SymbolicExpr::Ptr actualTarget = SymbolicExpr::makeIntegerConstant(IP.nBits(), cpu->concreteInstructionPointer());
        SymbolicExpr::Ptr trueTarget = inode->child(1);  // true branch next va
        SymbolicExpr::Ptr falseTarget = inode->child(2); // false branch next va
        SymbolicExpr::Ptr followedCond;                  // condition for the branch that is followed
        if (!trueTarget->isIntegerConstant()) {
            error <<"expected constant value for true branch target at " <<partitioner.unparse(insn) <<"\n";
        } else if (!falseTarget->isIntegerConstant()) {
            error <<"expected constant value for false branch target at " <<partitioner.unparse(insn) <<"\n";
        } else if (actualTarget->mustEqual(trueTarget)) {
            followedCond = inode->child(0);              // taking the true branch
        } else if (actualTarget->mustEqual(falseTarget)) {
            followedCond = SymbolicExpr::makeInvert(inode->child(0)); // taking false branch
        } else {
            error <<"unrecognized symbolic execution address after " <<partitioner.unparse(insn) <<"\n"
                  <<"  concrete = " <<*actualTarget <<"\n"
                  <<"  symbolic = " <<*ip <<"\n";
        }
        SymbolicExpr::Ptr otherCond = SymbolicExpr::makeInvert(followedCond); // condition for branch not taken

        // Solve for branch not taken in terms of input values.
        if (otherCond) {
            SAWYER_MESG(debug) <<"condition for other path is " <<*otherCond <<"\n";
            SmtSolver::Transaction transaction(solver); // because we'll need to cancel in order to follow the correct branch
            solver->insert(otherCond);
            if (SmtSolver::SAT_YES == solver->check()) {
                generateTestCase(db, testCase, solver);

                SAWYER_MESG(debug) <<"conditions are satisfiable when:\n";
                BOOST_FOREACH (const std::string &varName, solver->evidenceNames()) {
                    InputVariables::Variable inputVar = inputVariables_.get(varName);
                    SAWYER_MESG(debug) <<"  " <<inputVar <<" (" <<varName <<") = ";
                    if (SymbolicExpr::Ptr val = solver->evidenceForName(varName)) {
                        SAWYER_MESG(debug) <<*val <<"\n";
                    } else {
                        SAWYER_MESG(debug) <<" = ???\n";
                    }
                }
            }
        }

        // Add the branch taken condition to the solver since all future assertions will also depend on having taken this branch.
        solver->insert(followedCond);
    }
}

void
ConcolicExecutor::run(const Database::Ptr &db, const TestCase::Ptr &testCase, const Emulation::DispatcherPtr &cpu) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);
    ASSERT_not_null(cpu);

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream trace(mlog[TRACE]);
    Sawyer::Message::Stream where(mlog[WHERE]);
    Sawyer::Message::Stream error(mlog[ERROR]);

    Emulation::RiscOperatorsPtr ops = cpu->emulationOperators();
    SmtSolver::Ptr solver = ops->solver();
    ops->printInputVariables(debug);
    const P2::Partitioner &partitioner = ops->partitioner();

    // Process instructions in execution order
    rose_addr_t executionVa = cpu->concreteInstructionPointer();
    while (!cpu->isTerminated()) {
        SgAsmInstruction *insn = partitioner.instructionProvider()[executionVa];
        if (insn) {
            SAWYER_MESG_OR(trace, debug) <<"executing " <<partitioner.unparse(insn) <<"\n";
        } else {
            // FIXME[Robb Matzke 2020-07-13]: I'm not sure how this ends up happening yet. Perhaps one way is because we don't
            // handle certain system calls like mmap, brk, etc. In any case, report an error and execute the instruction concretely.
            SAWYER_MESG(error) <<"executing " <<StringUtility::addrToString(executionVa)
                               <<": not mapped or not executable\n";
            cpu->concreteSingleStep();
            continue;
        }

        try {
            cpu->processInstruction(insn);
        } catch (const IS::BaseSemantics::Exception &e) {
            if (error) {
                error <<e.what() <<", occurred at:\n";
                printCallStack(error);
                error <<"  insn " <<partitioner.unparse(insn) <<"\n";
                error <<"machine state at time of error:\n" <<(*ops->currentState()+"  ");
            }
// TEMPORARILY COMMENTED OUT FOR DEBUGGING [Robb Matzke 2020-07-13]. This exception is thrown when we get an address wrong,
// like for "mov eax, gs:[16]", and the debugger is reporting that the memory cannot be read.  We should probably be using
// debugger-specific exceptions for this kind of thing.
//        } catch (std::runtime_error &e) {
//            if (error) {
//                error <<e.what() <<", occurred at:\n";
//                printCallStack(error);
//                error <<"  insn " <<partitioner.unparse(insn) <<"\n";
//                error <<"machine state at time of error:\n" <<(*ops->currentState()+"  ");
//            }
        } catch (const Emulation::Exit &e) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has exited with status " <<*e.status() <<"\n";
            break;
        }

        if (cpu->isTerminated()) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has terminated\n";
            break;
        }

        if (settings_.traceState)
            SAWYER_MESG(debug) <<"state after instruction:\n" <<(*ops->currentState()+"  ");
        executionVa = cpu->concreteInstructionPointer();
        if (updateCallStack(cpu, insn) && where) {
            where <<"function call stack:\n";
            printCallStack(where);
        }
        handleBranch(db, testCase, cpu, insn, solver);
    }
}

void
ConcolicExecutor::generateTestCase(const Database::Ptr &db, const TestCase::Ptr &oldTestCase, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(db);
    ASSERT_not_null(oldTestCase);
    ASSERT_not_null(solver);                            // assertions must have been checked and satisfiable

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"generating new test case...\n";

    std::vector<std::string> args = oldTestCase->args();   // like argv, but excluding argv[argc]
    args.insert(args.begin(), oldTestCase->specimen()->name());
    Sawyer::Optional<size_t> maxArgvAdjusted;           // max index of any adjusted argument
    Sawyer::Optional<size_t> adjustedArgc;                 // whether we have a new argc value from the solver
    std::vector<EnvValue> env = oldTestCase->env();
    bool hadError = false;

    BOOST_FOREACH (const std::string &solverVar, solver->evidenceNames()) {
        InputVariables::Variable inputVar = inputVariables_.get(solverVar);
        SymbolicExpr::Ptr value = solver->evidenceForName(solverVar);
        ASSERT_not_null(value);

        switch (inputVar.whence()) {
            case InputVariables::Variable::INVALID:
                error <<"solver variable \"" <<solverVar <<"\" doesn't correspond to any input variable\n";
                hadError = true;
                break;

            case InputVariables::Variable::PROGRAM_ARGUMENT_COUNT:
                if (!value->isIntegerConstant()) {
                    error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                          <<"not an integer\n";
                    hadError = true;
                } else {
                    size_t argc = *value->toUnsigned();
                    if ((int)argc < 1 || argc > 2097152/2) {
                        error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                              <<"unreasonable value: " <<(int)argc <<"\n";
                        hadError = true;
                    } else if (maxArgvAdjusted && *maxArgvAdjusted >= argc) {
                        error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                              <<"adjusting size to " <<argc
                              <<" but solver also specifies a value for \"argv[" <<*maxArgvAdjusted <<"]\n";
                        hadError = true;
                    } else {
                        SAWYER_MESG(debug) <<"adjusting \"" <<inputVar <<"\""
                                           <<" from " <<(oldTestCase->args().size()+1) <<" to " <<argc <<"\n";
                        args.resize(argc);
                        adjustedArgc = argc;
                    }
                }
                break;

            case InputVariables::Variable::PROGRAM_ARGUMENT:
                if (!value->isIntegerConstant() || value->type().nBits() != 8) {
                    error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                          <<"value is not a one-byte integral type\n";
                    hadError = true;
                } else if (adjustedArgc && inputVar.variableIndex() >= *adjustedArgc) {
                    error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                          <<"out of range due to prior adjustment of argc to " <<*adjustedArgc <<"\n";
                    hadError = true;
                } else if (0 == inputVar.variableIndex()) {
                    // we can't arbitrarily adjust argv[0] (the command name) because there's no way for us
                    // to be able to run test cases concretely at arbitrary locations in the filesystem.
                    error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                          <<": adjusting argv[0] is not supported\n";
                    hadError = true;
                } else {
                    ASSERT_require(inputVar.variableIndex() > 0);

                    // Extend the size of the argv vector and/or the string affected
                    if (inputVar.variableIndex() >= args.size())
                        args.resize(inputVar.variableIndex() + 1);
                    if (inputVar.charIndex() >= args[inputVar.variableIndex()].size())
                        args[inputVar.variableIndex()].resize(inputVar.charIndex()+1);

                    // Modify one character of the argument
                    char ch = *value->toUnsigned();
                    args[inputVar.variableIndex()][inputVar.charIndex()] = ch;
                    maxArgvAdjusted = std::max(maxArgvAdjusted.orElse(0), inputVar.variableIndex());
                    SAWYER_MESG(debug) <<"adjusting \"" <<inputVar <<"\" to '" <<StringUtility::cEscape(ch) <<"'\n";
                }
                break;

            case InputVariables::Variable::ENVIRONMENT:
                // would be similar to PROGRAM_ARGUMENT. Would also need to add checks for NUL characters like we do below for
                // argv.
                ASSERT_not_implemented("[Robb Matzke 2019-12-18]");

            case InputVariables::Variable::SYSTEM_CALL_RETVAL:
                ASSERT_not_implemented("[Robb Matzke 2020-08-28]");
        }
    }

    // Check that program arguments are reasonable.
    for (size_t i = 0; i < args.size(); ++i) {
        // Remove trailing NULs
        while (!args[i].empty() && '\0' == args[i][args[i].size()-1])
            args[i].resize(args[i].size()-1);

        // Check for NULs
        for (size_t j = 0; j < args[i].size(); ++j) {
            if ('\0' == args[i][j]) {
                error <<"argv[" <<i <<"][" <<j <<"] is NUL\n";
                hadError = true;
            }
        }
    }

    // Create the new test case
    TestCase::Ptr newTestCase;
    if (hadError) {
        SAWYER_MESG(debug) <<"test case not created due to prior errors\n";
    } else {
        newTestCase = TestCase::instance(oldTestCase->specimen());
        ASSERT_forbid(args.empty());
        args.erase(args.begin()); // argv[0] is not to be included
        newTestCase->args(args);
        newTestCase->env(env);
    }

    // Save the test case if the database doesn't already have one that's the same.
    // FIXME[Robb Matzke 2020-01-16]: This could be much improved.
    if (newTestCase) {
        TestCase::Ptr similarTestCase;
        BOOST_FOREACH (TestCaseId tid, db->testCases()) {
            TestCase::Ptr otherTestCase = db->object(tid);
            if (areSimilar(newTestCase, otherTestCase)) {
                similarTestCase = otherTestCase;
                break;
            }
        }

        if (similarTestCase) {
            debug <<"new test case not saved to DB because similar " <<similarTestCase->printableName(db) <<" already exists\n";
        } else {
            db->save(newTestCase);
            if (debug) {
                debug <<"inserted into database: " <<newTestCase->printableName(db) <<"\n";
                debug <<"  argv[0] = \"" <<StringUtility::cEscape(newTestCase->specimen()->name()) <<"\"\n";
                for (size_t i = 0; i < args.size(); ++i)
                    debug <<"  argv[" <<(i+1) <<"] = \"" <<StringUtility::cEscape(args[i]) <<"\"\n";
            }
        }
    }
}

bool
ConcolicExecutor::areSimilar(const TestCase::Ptr &a, const TestCase::Ptr &b) const {
    if (a->specimen() != b->specimen())
        return false;

    std::vector<std::string> aArgs = a->args();
    std::vector<std::string> bArgs = b->args();
    if (aArgs.size() != bArgs.size() || !std::equal(aArgs.begin(), aArgs.end(), bArgs.begin()))
        return false;

    std::vector<EnvValue> aEnv = a->env();
    std::vector<EnvValue> bEnv = b->env();
    if (aEnv.size() != bEnv.size() || !std::equal(aEnv.begin(), aEnv.end(), bEnv.begin()))
        return false;

    return true;
}

} // namespace
} // namespace
} // namespace

#endif
