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
    switch (whence) {
        case INVALID:
            out <<"nothing";
            break;
        case PROGRAM_ARGUMENT_COUNT:
            out <<"argc";
            break;
        case PROGRAM_ARGUMENT:
            out <<"argv[" <<index1 <<"][" <<index2 <<"]";
            break;
        case ENVIRONMENT:
            out <<"envp[" <<index1 <<"][" <<index2 <<"]";
            break;
    }
}

void
InputVariables::insertProgramArgumentCount(const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    Variable input;
    input.whence = Variable::PROGRAM_ARGUMENT_COUNT;
    variables_.insert(*symbolic->variableId(), input);
}

void
InputVariables::insertProgramArgument(size_t i, size_t j, const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    Variable input;
    input.whence = Variable::PROGRAM_ARGUMENT;
    input.index1 = i;
    input.index2 = j;
    variables_.insert(*symbolic->variableId(), input);
}

void
InputVariables::insertEnvironmentVariable(size_t i, size_t j, const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    Variable input;
    input.whence = Variable::ENVIRONMENT;
    input.index1 = i;
    input.index2 = j;
    variables_.insert(*symbolic->variableId(), input);
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
#ifdef __linux__
    if (32 == partitioner_.instructionProvider().instructionPointerRegister().nBits()) {
        // 32-bit Linux
        const RegisterDescriptor REG_AX = *regdict->lookup("rax");
        IS::BaseSemantics::SValuePtr ax = peekRegister(REG_AX, undefined_(REG_AX.nBits()));
        if (ax->is_number()) {
            if (1 == ax->get_number() || 252 == ax->get_number()) {
                const RegisterDescriptor REG_BX = *regdict->lookup("ebx");
                SValuePtr arg1 = SValue::promote(peekRegister(REG_BX, undefined_(REG_BX.nBits())));
                if (arg1->is_number()) {
                    int exitValue = arg1->get_number();
                    mlog[ERROR] <<"specimen exiting with " <<exitValue <<"\n"; // FIXME[Robb Matzke 2019-12-18]
                    throw Exit(arg1);
                } else {
                    mlog[ERROR] <<"specimen exiting with unknown value\n"; // FIXME[Robb Matzke 2019-12-18]
                    throw Exit(arg1);
                }
            } else {
                mlog[ERROR] <<"unhandled system call number " <<ax->get_number() <<"\n";
            }
        } else {
            mlog[ERROR] <<"unknown symbolic system call " <<*ax <<"\n";
        }
    } else {
        // 64-bit Linux
        const RegisterDescriptor REG_AX = *regdict->lookup("rax");
        IS::BaseSemantics::SValuePtr ax = peekRegister(REG_AX, undefined_(REG_AX.nBits()));
        if (ax->is_number()) {
            if (60 == ax->get_number() || 231 == ax->get_number()) {
                const RegisterDescriptor REG_DI = *regdict->lookup("edi");
                SValuePtr arg1 = SValue::promote(peekRegister(REG_DI, undefined_(REG_DI.nBits())));
                if (arg1->is_number()) {
                    int exitValue = arg1->get_number();
                    mlog[ERROR] <<"specimen exiting with " <<exitValue <<"\n"; // FIXME[Robb Matzke 2019-12-18]
                    throw Exit(arg1);
                } else {
                    mlog[ERROR] <<"specimen exiting with unknown value\n"; // FIXME[Robb Matzke 2019-12-18]
                    throw Exit(arg1);
                }
            } else {
                mlog[ERROR] <<"unhandled system call number " <<ax->get_number() <<"\n";
            }
        } else {
            mlog[ERROR] <<"unknown symbolic system call " <<*ax <<"\n";
        }
    }
#else
    mlog[ERROR] <<"unknown system call for architecture\n";
#endif
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
RiscOperators::markProgramArguments() {
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

    // argc
    rose_addr_t argcVa = process_->readRegister(SP).toInteger();
    size_t argc = process_->readMemory(argcVa, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();

    SValuePtr symbolicArgc = SValue::promote(undefined_(SP.nBits()));
    symbolicArgc->set_comment("argc");
    writeMemory(RegisterDescriptor(), number_(SP.nBits(), argcVa), symbolicArgc, boolean_(true));
    inputVariables_.insertProgramArgumentCount(symbolicArgc->get_expression());

    SAWYER_MESG(debug) <<"  argc @" <<StringUtility::addrToString(argcVa) <<" = " <<argc
                       <<"; symbolic = " <<(*symbolicArgc + fmt) <<"\n";

    // argv
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

    // envp
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

    // auxv
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
Dispatcher::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);

    // Symbolic execution
    Super::processInstruction(insn);

    // Concrete execution
    Debugger::Ptr process = emulationOperators()->process();
    process->executionAddress(insn->get_address());
    process->singleStep();
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
                              Sawyer::FileSystem::TemporaryDirectory &tempDir) {
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
    P2::Partitioner partitioner = partition(db, testCase->specimen());
    Debugger::Ptr process = makeProcess(db, testCase, tempDir);
    Emulation::RiscOperatorsPtr ops =
        Emulation::RiscOperators::instance(settings_.emulationSettings, partitioner, process, inputVariables_,
                                           Emulation::SValue::instance(), SmtSolver::Ptr());

#if 0 // [Robb Matzke 2019-12-11]
    Emulation::DispatcherPtr cpu = Emulation::Dispatcher::instance(ops);
#else // DEBUGGING
    IS::BaseSemantics::RiscOperatorsPtr trace = IS::TraceSemantics::RiscOperators::instance(ops);
    Emulation::DispatcherPtr cpu = Emulation::Dispatcher::instance(trace);
#endif

    run(db, testCase, cpu);
    testCase->concolicResult(1);
    db->save(testCase);

    // FIXME[Robb Matzke 2020-01-16]
    std::vector<TestCase::Ptr> newCases;
    return newCases;
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
    Sawyer::Message::Stream error(mlog[ERROR]);

    Emulation::RiscOperatorsPtr ops = cpu->emulationOperators();
    ops->printInputVariables(debug);
    const P2::Partitioner &partitioner = ops->partitioner();
    SmtSolver::Ptr solver = SmtSolver::instance("best");

    // Process instructions in execution order starting at startVa
    rose_addr_t executionVa = cpu->concreteInstructionPointer();
    while (!cpu->isTerminated()) {
        SgAsmInstruction *insn = partitioner.instructionProvider()[executionVa];
        ASSERT_not_null2(insn, StringUtility::addrToString(executionVa) + " is not a valid execution address");
        SAWYER_MESG_OR(trace, debug) <<"executing " <<partitioner.unparse(insn) <<"\n";
        try {
            cpu->processInstruction(insn);
        } catch (const Emulation::Exit &e) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has exited with status " <<*e.status() <<"\n";
            break;
        }
        if (cpu->isTerminated()) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has terminated\n";
            break;
        }
        SAWYER_MESG(debug) <<"state after instruction:\n" <<(*ops->currentState()+"  ");
        executionVa = cpu->concreteInstructionPointer();
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

        switch (inputVar.whence) {
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
                } else if (adjustedArgc && inputVar.index1 >= *adjustedArgc) {
                    error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                          <<"out of range due to prior adjustment of argc to " <<*adjustedArgc <<"\n";
                    hadError = true;
                } else if (0 == inputVar.index1) {
                    // we can't arbitrarily adjust argv[0] (the command name) because there's no way for us
                    // to be able to run test cases concretely at arbitrary locations in the filesystem.
                    error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\": "
                          <<": adjusting argv[0] is not supported\n";
                    hadError = true;
                } else {
                    ASSERT_require(inputVar.index1 > 0);

                    // Extend the size of the argv vector and/or the string affected
                    if (inputVar.index1 >= args.size())
                        args.resize(inputVar.index1 + 1);
                    if (inputVar.index2 >= args[inputVar.index1].size())
                        args[inputVar.index1].resize(inputVar.index2+1);

                    // Modify one character of the argument
                    char ch = *value->toUnsigned();
                    args[inputVar.index1][inputVar.index2] = ch;
                    maxArgvAdjusted = std::max(maxArgvAdjusted.orElse(0), inputVar.index1);
                    SAWYER_MESG(debug) <<"adjusting \"" <<inputVar <<"\" to '" <<StringUtility::cEscape(ch) <<"'\n";
                }
                break;

            case InputVariables::Variable::ENVIRONMENT:
                // would be similar to PROGRAM_ARGUMENT. Would also need to add checks for NUL characters like we do below for
                // argv.
                ASSERT_not_implemented("[Robb Matzke 2019-12-18]");
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
