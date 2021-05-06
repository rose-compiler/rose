#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>

#include <boost/format.hpp>
#include <boost/scope_exit.hpp>
#include <Rose/CommandLine.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BitOps.h>
#include <Sawyer/FileSystem.h>
#include <SqlDatabase.h>                                // ROSE

#ifdef __linux__
#include <sys/mman.h>
#include <sys/syscall.h>
#endif

namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

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

    Rose::CommandLine::insertBooleanSwitch(ce, "show-semantics", settings.traceSemantics,
                                           "Show the semantic operations that are performed for each instruction.");

    ce.insert(Switch("show-state")
              .argument("address", P2::addressIntervalParser(settings.showingStates), "all")
              .doc("Addresses of instructions after which to show instruction states. This is intended for debugging, and the "
                   "state will only be shown if the Rose::BinaryAnalysis::FeasiblePath(debug) diagnostic stream is enabled. "
                   "This switch may occur multiple times to specify multiple addresses or address ranges. " +
                   P2::AddressIntervalParser::docString() + " The default, if no argument is specified, is all addresses."));

    sgroups.push_back(ce);

    return sgroups;
}

P2::Partitioner
ConcolicExecutor::partition(const Database::Ptr &db, const Specimen::Ptr &specimen) {
    ASSERT_not_null(db);
    ASSERT_not_null(specimen);
    SpecimenId specimenId = db->id(specimen, Update::NO);
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

LinuxI386Executor::Ptr
ConcolicExecutor::makeProcess(const Database::Ptr &db, const TestCase::Ptr &testCase, const boost::filesystem::path &tmpDir) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);

    // Create the a new process from the executable. Then make the executable look as if it has executed to a particular point (the
    // point where the test case was created).
    auto process = LinuxI386Executor::instance(db, testCase);
    process->load(tmpDir);
    process->processAllEvents();
    return process;
}

std::vector<TestCase::Ptr>
ConcolicExecutor::execute(const Database::Ptr &db, const TestCase::Ptr &testCase) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);
    Sawyer::FileSystem::TemporaryDirectory tmpDir;
    TestCaseId testCaseId = db->id(testCase, Update::NO);
    SAWYER_MESG(mlog[DEBUG]) <<"concolically executing test case " <<*testCaseId <<"\n";

    // Mark the test case as having NOT been run concolically, and clear any data saved as part of a previous concolic run.
    testCase->concolicResult(0);

    // Create the semantics layers. The symbolic semantics uses a Partitioner, and the concrete semantics uses a suborinate
    // process which is created from the specimen.
    SmtSolver::Ptr solver = SmtSolver::instance("best");
    P2::Partitioner partitioner = partition(db, testCase->specimen());
    LinuxI386Executor::Ptr process = makeProcess(db, testCase, tmpDir.name());
    Emulation::RiscOperatorsPtr ops =
        Emulation::RiscOperators::instance(settings_.emulationSettings, db, testCase, partitioner, process, inputVariables_,
                                           Emulation::SValue::instance(), solver);
    Emulation::DispatcherPtr cpu;
    if (settings_.traceSemantics) {
        BS::RiscOperatorsPtr trace = IS::TraceSemantics::RiscOperators::instance(ops);
        cpu = Emulation::Dispatcher::instance(trace);
    } else {
        cpu = Emulation::Dispatcher::instance(ops);
    }

    // Initialize the test case by either running past all existing execution events to get to the same execution path as from
    // whence this test case was created, or create the execution events that describe the initial memory layout and
    // registers. The latter only occurs for test cases that were created directly by the user without a parent test case.
    if (db->nExecutionEvents(testCaseId) > 0) {
        ASSERT_not_implemented("[Robb Matzke 2021-05-14]");
    } else {
        process->saveMemory();
        process->saveRegisters();
        ops->createProgramArguments(solver);
    }

    // Run the test case from the end of its current execution path in order to create new test cases.
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
                if (debug) {
                    debug <<"conditions are satisfiable when:\n";
                    BOOST_FOREACH (const std::string &varName, solver->evidenceNames()) {
                        InputVariables::Variable inputVar = inputVariables_.get(varName);
                        debug <<"  " <<inputVar <<" (" <<varName <<") = ";
                        if (SymbolicExpr::Ptr val = solver->evidenceForName(varName)) {
                            debug <<*val <<"\n";
                        } else {
                            debug <<" = ???\n";
                        }
                    }
                }

                generateTestCase(db, testCase, ops, solver);
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
    SAWYER_MESG(mlog[DEBUG]) <<"initial state:\n" <<*ops->currentState();
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

        ops->systemCall() = Sawyer::Nothing();
        try {
            cpu->processInstruction(insn);
        } catch (const BS::Exception &e) {
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
            SAWYER_MESG_OR(trace, debug) <<"subordinate has exited with status " <<e.status() <<"\n";
            break;
        }

        if (cpu->isTerminated()) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has terminated\n";
            break;
        }

        ops->finishSystemCall();

        if (settings_.showingStates.exists(executionVa))
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
ConcolicExecutor::generateTestCase(const Database::Ptr &db, const TestCase::Ptr &oldTestCase,
                                   const BS::RiscOperatorsPtr &ops_, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(db);
    ASSERT_not_null(oldTestCase);
    TestCaseId oldTestCaseId = db->id(oldTestCase, Update::NO);
    ASSERT_not_null(ops_);
    auto ops = Emulation::RiscOperators::promote(ops_);
    ASSERT_not_null(solver);                            // assertions must have been checked and satisfiable

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"generating new test case...\n";

    // Create execution events for the child test case based on the execution events from the parent test case, but do not
    // commit the child events to the database yet because we don't know for sure that we're going to create the child test
    // case.  We'll abandon all these child execution events if we decide not to create the child test case.
    Sawyer::Container::Map<size_t, ExecutionEvent::Ptr> eventMap;
    for (ExecutionEventId parentEventId: db->executionEvents(oldTestCaseId)) {
        ExecutionEvent::Ptr parentEvent = db->object(parentEventId);
        ExecutionEvent::Ptr childEvent = parentEvent->copy();
        eventMap.insert(*parentEventId, childEvent);
    }

    // Get the parent's argc, argument list, and environment list which we'll use as the child's, with possibly some
    // modifications.  These values are mostly redundant since the execution events are what really control what inputs the
    // test case gets.
    std::vector<std::string> argv = oldTestCase->args();
    size_t argc = argv.size();
    std::vector<std::string> env;
    for (const EnvValue &pair: oldTestCase->env())
        env.push_back(pair.first + "=" + pair.second);

    // Process each piece of the solver evidence. These are the symbolic variables and their values that would cause test case
    // execution to have gone some other way at the current control flow branch. Each of the variables in the evidence will
    // correspond to a known symbolic input, which in turn corresponds to a concrete execution event.
    std::vector<ExecutionEvent::Ptr> modifiedEvents;
    bool hadError = false;
    for (const std::string &solverVar: solver->evidenceNames()) {
        SymbolicExpr::Ptr solverValue = solver->evidenceForName(solverVar);
        ASSERT_not_null(solverValue);
        InputVariables::Variable inputVar = inputVariables_.get(solverVar);
        if (InputVariables::Variable::INVALID == inputVar.whence()) {
            error <<"solver variable \"" <<solverVar <<"\" doesn't correspond to any input variable\n";
            hadError = true;
            continue;
        }
        if (!solverValue->isIntegerConstant()) {
            error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<inputVar <<"\" is not an integer\n";
            hadError = true;
            continue;
        }
        ASSERT_require(inputVar.executionEventId());
        ExecutionEvent::Ptr childEvent = eventMap[*inputVar.executionEventId()];

        switch (inputVar.whence()) {
            case InputVariables::Variable::INVALID:
                ASSERT_not_reachable("handled above");

            case InputVariables::Variable::PROGRAM_ARGUMENT_COUNT: {
                ASSERT_require(32 == solverValue->nBits());
                ASSERT_require(ops->currentState()->memoryState()->get_byteOrder() == ByteOrder::ORDER_LSB);
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_MEMORY);
                uint32_t newValue = solverValue->toUnsigned().get();
                std::vector<uint8_t> bytes;
                for (size_t i = 0; i < 4; ++i)
                    bytes.push_back(BitOps::bits(newValue, i*8, i*8+7));
                childEvent->bytes(bytes);
                modifiedEvents.push_back(childEvent);
                SAWYER_MESG(debug) <<"adjusting " <<inputVar <<" from " <<argc <<" to " <<newValue <<"\n";
                argc = newValue;
                break;
            }

            case InputVariables::Variable::PROGRAM_ARGUMENT: {
                ASSERT_require(8 == solverValue->nBits());
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_MEMORY);
                uint8_t newValue = solverValue->toUnsigned().get();
                std::vector<uint8_t> bytes{newValue};
                childEvent->bytes(bytes);
                size_t i = inputVar.variableIndex();
                size_t j = inputVar.charIndex();
                modifiedEvents.push_back(childEvent);
                ASSERT_require(i < argv.size());
                ASSERT_require(j < argv[i].size());
                SAWYER_MESG(debug) <<"adjusting " <<inputVar <<" from '" <<StringUtility::cEscape(argv[i][j]) <<"'"
                                   <<" to '" <<StringUtility::cEscape(newValue) <<"'\n";
                argv[i][j] = newValue;
                break;
            }

            case InputVariables::Variable::ENVIRONMENT: {
                ASSERT_require(8 == solverValue->nBits());
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_MEMORY);
                uint8_t newValue = solverValue->toUnsigned().get();
                std::vector<uint8_t> bytes{newValue};
                childEvent->bytes(bytes);
                size_t i = inputVar.variableIndex();
                size_t j = inputVar.charIndex();
                modifiedEvents.push_back(childEvent);
                ASSERT_require(i < env.size());
                ASSERT_require(j < env[i].size());
                SAWYER_MESG(debug) <<"adjusting " <<inputVar <<" from '" <<StringUtility::cEscape(env[i][j]) <<"'"
                                   <<" to '" <<StringUtility::cEscape(newValue) <<"'\n";
                env[i][j] = newValue;
                break;
            }

            case InputVariables::Variable::SYSTEM_CALL_RETVAL: {
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_REGISTER);
                uint64_t oldValue = childEvent->words()[0];
                uint64_t newValue = solverValue->toUnsigned().get();
                childEvent->words(std::vector<uint64_t>{newValue});
                modifiedEvents.push_back(childEvent);
                SAWYER_MESG(debug) <<"adjusting " <<inputVar <<" from " <<oldValue <<" to " <<newValue <<"\n";
                break;
            }
        }
    }

    // Check that program arguments are reasonable.
    if (argc < argv.size()) {
        ASSERT_require(argc >= 0);
        argv.resize(argc);
    } else if (argc > argv.size()) {
        argv.resize(argc);
    }
    for (size_t i = 0; i < argv.size(); ++i) {
        // Remove trailing NULs.
        while (!argv[i].empty() && '\0' == argv[i][argv[i].size()-1])
            argv[i].resize(argv[i].size()-1);

        // Check for embedded NULs.
        for (size_t j = 0; j < argv[i].size(); ++j) {
            if ('\0' == argv[i][j]) {
                error <<"argv[" <<i <<"][" <<j <<"] is NUL\n";
                hadError = true;
            }
        }
    }
    for (size_t i = 0; i < env.size(); ++i) {
        // Remove trailing NULs.
        while (!env[i].empty() && '\0' == env[i][env[i].size()-1])
            env[i].resize(env[i].size()-1);

        // Check for embedded NULs.
        for (size_t j = 0; j < env[i].size(); ++j) {
            if ('\0' == env[i][j]) {
                error <<"env[" <<i <<"][" <<j <<"] is NUL\n";
                hadError = true;
            }
        }
    }

    // Create the new test case.
    TestCase::Ptr newTestCase;
    if (hadError) {
        SAWYER_MESG(debug) <<"test case not created due to prior errors\n";
    } else {
        newTestCase = TestCase::instance(oldTestCase->specimen());
        ASSERT_forbid(argv.empty());
        newTestCase->args(argv);
        std::vector<EnvValue> envVars;
        for (const std::string &s: env) {
            size_t equalSign = s.find('=');
            ASSERT_require(equalSign != std::string::npos);
            envVars.push_back(EnvValue(s.substr(0, equalSign), s.substr(equalSign+1)));
        }
        newTestCase->env(envVars);

        TestCaseId newTestCaseId = db->id(newTestCase);
        for (ExecutionEvent::Ptr childEvent: eventMap.values()) {
            childEvent->testCase(newTestCase);
            db->save(childEvent);
        }

        if (debug) {
            debug <<"created new " <<newTestCase->printableName(db) <<"\n";
            debug <<"  command-line:\n";
            for (const std::string &arg: argv)
                debug <<"    - " <<StringUtility::yamlEscape(arg) <<"\n";
            debug <<"  environment:\n";
            for (const std::string &s: env)
                debug <<"    - " <<StringUtility::yamlEscape(s) <<"\n";
            debug <<"  execution-events:  # Showing only those that differ from the parent test case\n";
            for (const ExecutionEvent::Ptr &event: modifiedEvents)
                event->toYaml(debug, db, "    - ");
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

#if 0 // [Robb Matzke 2021-05-12]
    if (aSyscalls.size() != bSyscalls.size())
        return false;
    for (size_t i = 0; i < aSyscalls.size(); ++i) {
        if (aSyscalls[i]->functionId() != bSyscalls[i]->functionId())
            return false;
        if (aSyscalls[i]->callSite() != bSyscalls[i]->callSite())
            return false;
        if (aSyscalls[i]->returnValue() != bSyscalls[i]->returnValue())
            return false;
    }
#else
    ASSERT_not_implemented("[Robb Matzke 2021-05-12]");
#endif

    return true;
}

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
            out <<"syscall-retval";
            break;
    }
}

void
InputVariables::insertProgramArgumentCount(const ExecutionEventId &eeid, const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    variables_.insert(*symbolic->variableId(), Variable::programArgc(eeid));
}

void
InputVariables::insertProgramArgument(const ExecutionEventId &eeid, size_t i, size_t j, const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    variables_.insert(*symbolic->variableId(), Variable::programArgument(eeid, i, j));
}

void
InputVariables::insertEnvironmentVariable(const ExecutionEventId &eeid, size_t i, size_t j,
                                          const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    variables_.insert(*symbolic->variableId(), Variable::environmentVariable(eeid, i, j));
}

void
InputVariables::insertSystemCallReturn(const ExecutionEventId &eeid, const SymbolicExpr::Ptr &symbolic) {
    ASSERT_not_null(symbolic);
    ASSERT_require(symbolic->isVariable2());
    variables_.insert(*symbolic->variableId(), Variable::systemCallReturn(eeid));
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

#if 0 // [Robb Matzke 2021-05-17]
void
SystemCall::print(std::ostream &out) const {
    out <<"syscall " <<*functionNumber <<"\n";
    for (size_t i = 0; i < arguments.size(); ++i) {
        out <<"  arg[" <<i <<"] = ";
        if (arguments[i]) {
            out <<*arguments[i] <<"\n";
        } else {
            out <<"null\n";
        }
    }
    if (returnValue)
        out <<"  returning " <<*returnValue <<"\n";
}

std::ostream&
operator<<(std::ostream &out, const SystemCall &sc) {
    sc.print(out);
    return out;
}
#endif

RiscOperatorsPtr
RiscOperators::instance(const Settings &settings, const Database::Ptr &db, const TestCase::Ptr &testCase,
                        const P2::Partitioner &partitioner, const LinuxI386Executor::Ptr &process,
                        InputVariables &inputVariables, const BS::SValuePtr &protoval,
                        const SmtSolver::Ptr &solver) {
    // Extend the register set with an additional Boolean register named "path"
    RegisterDictionary *regdict = new RegisterDictionary("Rose::BinaryAnalysis::Concolic");
    regdict->insert(partitioner.instructionProvider().registerDictionary());
    const RegisterDescriptor path(partitioner.instructionProvider().registerDictionary()->firstUnusedMajor(), 0, 0, 1);
    regdict->insert("path", path);

    // Initialize machine state
    RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
    MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
    memory->set_byteOrder(ByteOrder::ORDER_LSB);
    StatePtr state = State::instance(registers, memory);
    RiscOperatorsPtr ops(new RiscOperators(settings, db, testCase, partitioner, process, inputVariables, state, solver));
    ASSERT_require(ops->REG_PATH == path);
    ops->writeRegister(path, ops->boolean_(true));
    return ops;
}

RiscOperatorsPtr
RiscOperators::promote(const BS::RiscOperatorsPtr &x) {
    RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

size_t
RiscOperators::wordSizeBits() const {
    return partitioner_.instructionProvider().instructionPointerRegister().nBits();
}

TestCase::Ptr
RiscOperators::testCase() const {
    return testCase_;
}

Database::Ptr
RiscOperators::database() const {
    return db_;
}

const RegisterDictionary*
RiscOperators::registerDictionary() const {
    return partitioner_.instructionProvider().registerDictionary();
}

void
RiscOperators::interrupt(int majr, int minr) {
    if (x86_exception_int == majr && 0x80 == minr) {
        startSystemCall();
    } else {
        Super::interrupt(majr, minr);
    }
}

uint64_t
RiscOperators::systemCallFunctionNumber() {
    BS::SValuePtr retvalSymbolic;
    ASSERT_require2(isSgAsmX86Instruction(currentInstruction()), "only x86 system calls are handled");
    if (32 == partitioner_.instructionProvider().wordSize()) {
        const RegisterDescriptor AX = partitioner_.instructionProvider().registerDictionary()->findOrThrow("eax");
        retvalSymbolic = readRegister(AX);
    } else {
        ASSERT_require(64 == partitioner_.instructionProvider().wordSize());
        const RegisterDescriptor AX = partitioner_.instructionProvider().registerDictionary()->findOrThrow("rax");
        retvalSymbolic = readRegister(AX);
    }

    ASSERT_not_null(retvalSymbolic);
    ASSERT_require2(retvalSymbolic->isConcrete(), "non-concrete system call numbers not handled yet");
    return retvalSymbolic->toUnsigned().get();
}

BS::SValuePtr
RiscOperators::systemCallArgument(size_t idx) {
    ASSERT_require2(isSgAsmX86Instruction(currentInstruction()), "only x86 system calls are handled");
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

RegisterDescriptor
RiscOperators::systemCallReturnRegister() {
    if (32 == partitioner_.instructionProvider().wordSize()) {
        return partitioner_.instructionProvider().registerDictionary()->findOrThrow("eax");
    } else {
        ASSERT_require(64 == partitioner_.instructionProvider().wordSize());
        return partitioner_.instructionProvider().registerDictionary()->findOrThrow("rax");
    }
}

BS::SValuePtr
RiscOperators::systemCallReturnValue() {
    const RegisterDescriptor reg = systemCallReturnRegister();
    return readRegister(reg);
}

BS::SValuePtr
RiscOperators::systemCallReturnValue(const BS::SValuePtr &retval) {
    ASSERT_require2(isSgAsmX86Instruction(currentInstruction()), "only x86 system calls are handled");
    const RegisterDescriptor reg = systemCallReturnRegister();
    writeRegister(reg, retval);
    return retval;
}

void
RiscOperators::doSyscallExit(uint64_t status) {
    // This is called during the symbolic phase. The concrete system call hasn't happened yet.
    mlog[INFO] <<"specimen exiting with " <<status <<"\n";
    throw Exit(status);
}


#if 0 // [Robb Matzke 2021-05-12]
void
RiscOperators::systemCallInputs(const Database::Ptr &db, const TestCase::Ptr &tc) {
    ASSERT_not_null(db);
    ASSERT_not_null(tc);
    TestCaseId tcid = db->id(tc, Update::NO);

    // For each system call that this test case is expected to encounter when fast forwarding to the starting location, create a system
    // call record in this operators object and the corresponding input variables.
    for (SystemCallId scid: db->systemCalls(tcid)) {
        Concolic::SystemCall::Ptr dbsc = db->object(scid); // database system call record
        systemCalls_.push_back(SystemCall());              // new system call semantic record
        systemCalls_.back().callSite = dbsc->callSite();
        systemCalls_.back().functionNumber = number_(wordSizeBits(), dbsc->functionId());
        systemCalls_.back().returnValue = number_(wordSizeBits(), dbsc->returnValue());
    }
}
#endif

// At the beginning of a system call we record a system call execution event. The purpose of this event is so that when it's
// replayed, the operating system emulator knows that a system call occurred. For instance, if we encounter a getpid (get process
// ID) system call we need to record that this was the call so that when we extend the execution path in a child test case the
// emulated operating system continues to return the same PID that the recorded part of the pat was returning.  The system call
// execution event itself doesn't record the return value (or any other side effects for that matter), but the following events
// will provide this information. In the case of a return value, the system call event will be followed by an event that writes
// the return value to the return register.
void
RiscOperators::startSystemCall() {
    ASSERT_always_require2(isSgAsmX86Instruction(currentInstruction()), "ISA not implemented yet");
    rose_addr_t va = currentInstruction()->get_address();

    // Gather and save useful information about the system call
    uint64_t functionNumber = systemCallFunctionNumber();
    std::vector<uint64_t> arguments;
    for (size_t i = 0; i < 6; ++i) {
        BS::SValuePtr argSymbolic = systemCallArgument(i);
        if (auto argConcrete = argSymbolic->toUnsigned()) {
            arguments.push_back(*argConcrete);
        } else {
            ASSERT_not_implemented("non-concrete system call argument");
        }
    }
    ASSERT_forbid(systemCall_);
    systemCall_ = SystemCall{va, functionNumber, arguments};

    // Create the system call execution event
    auto event = ExecutionEvent::instanceSyscall(testCase_, process_->newLocation(), va, functionNumber, arguments);
    db_->save(event);

    // Some system calls require special handling.
    if (isSgAsmX86Instruction(currentInstruction()) && 32 == partitioner_.instructionProvider().wordSize()) {
        switch (functionNumber) {
            case 1:                                     // exit
            case 252:                                   // exit_group
                return doSyscallExit(arguments[0]);
        }
    } else if (isSgAsmX86Instruction(currentInstruction()) && 64 == partitioner_.instructionProvider().wordSize()) {
        switch (functionNumber) {
            case 60:                                    // exit
            case 231:                                   // exit_group
                return doSyscallExit(arguments[0]);
        }
    }
}

void
RiscOperators::finishSystemCall() {
    if (systemCall_) {
        if (mlog[DEBUG]) {
            mlog[DEBUG] <<"syscall-" <<systemCall_->functionNumber <<", args = (";
            for (uint64_t arg: systemCall_->arguments)
                mlog[DEBUG] <<" " <<StringUtility::toHex(arg);
            mlog[DEBUG] <<" )\n";
        }

        RegisterDescriptor reg = systemCallReturnRegister();
        BS::SValuePtr retSymbolic = systemCallReturnValue();
        SAWYER_MESG(mlog[DEBUG]) <<"  returned " <<*retSymbolic <<"\n";
        ASSERT_require(retSymbolic->isConcrete());
        uint64_t retConcrete = retSymbolic->toUnsigned().get();

        // Create an execution event for the system call return value.
        auto event = ExecutionEvent::instanceWriteRegister(testCase_, process_->newLocation(), systemCall_->ip,
                                                           reg, retConcrete);
        event->comment("system call return value");
        ExecutionEventId eventId = db_->id(event);
        SAWYER_MESG(mlog[DEBUG]) <<"  created execution event " <<*eventId <<" for return value\n";

        // Create an input variable for the system call return value.
        SValuePtr variable = SValue::promote(undefined_(reg.nBits()));
        writeRegister(reg, variable);
        inputVariables_.insertSystemCallReturn(eventId, variable->get_expression());
        SAWYER_MESG(mlog[DEBUG]) <<"  created input variable " <<*variable <<" for return value\n";
    }
}

BS::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BS::SValuePtr &dfltUnused) {
    // Read the register's value symbolically, and if we don't have a value then read it concretely and use the concrete value
    // to update the symbolic state.
    SValuePtr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
    SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readRegister(reg));
    dflt->set_expression(concrete);
    return Super::readRegister(reg, dflt);
}

BS::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const BS::SValuePtr &dfltUnused) {
    // Return the register's symbolic value if it exists, else the concrete value.
    SValuePtr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
    SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readRegister(reg));
    dflt->set_expression(concrete);
    return Super::peekRegister(reg, dflt);
}

BS::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BS::SValuePtr &addr,
                          const BS::SValuePtr &dfltUnused, const BS::SValuePtr &cond) {
    // Read the memory's value symbolically, and if we don't have a value then read it concretely and use the concrete value to
    // update the symbolic state. However, we can only read it concretely if the address is a constant.
    if (auto va = addr->toUnsigned()) {
        size_t nBytes = dfltUnused->nBits() / 8;
        SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readMemory(*va, nBytes, ByteOrder::ORDER_LSB));
        SValuePtr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
        dflt->set_expression(concrete);
        return Super::readMemory(segreg, addr, dflt, cond);
    } else {
        return Super::readMemory(segreg, addr, dfltUnused, cond);
    }
}

BS::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BS::SValuePtr &addr,
                          const BS::SValuePtr &dfltUnused) {
    // Read the memory's symbolic value if it exists, else read the concrete value. We can't read concretely if the address is
    // symbolic.
    if (auto va = addr->toUnsigned()) {
        size_t nBytes = dfltUnused->nBits() / 8;
        SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(process_->readMemory(*va, nBytes, ByteOrder::ORDER_LSB));
        SValuePtr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
        dflt->set_expression(concrete);
        return Super::peekMemory(segreg, addr, dflt);
    } else {
        return Super::peekMemory(segreg, addr, dfltUnused);
    }
}

void
RiscOperators::createProgramArguments(const SmtSolver::Ptr &solver) {
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
    SAWYER_MESG(debug) <<"creating program arguments\n";
    const RegisterDescriptor SP = partitioner_.instructionProvider().stackPointerRegister();
    size_t wordSizeBytes = SP.nBits() / 8;
    SValueFormatter fmt;
    fmt.expr_formatter.show_comments = SymbolicExpr::Formatter::CMT_AFTER;

    //---------------------------------------------------------------------------------------------------------------------------
    // argc
    //---------------------------------------------------------------------------------------------------------------------------
    ASSERT_require(SP.nBits() == 32);                   // we only handle 32-bit for now
    ASSERT_require(currentState()->memoryState()->get_byteOrder() == ByteOrder::ORDER_LSB);
    rose_addr_t argcVa = process_->readRegister(SP).toInteger();
    uint32_t argc = process_->readMemory(argcVa, wordSizeBytes, ByteOrder::ORDER_LSB).toInteger();

    auto argcEvent = ExecutionEvent::instanceWriteMemory(testCase_, process_->newLocation(), process_->ip(), argcVa, argc);
    argcEvent->comment("argc");
    ExecutionEventId argcEventId = db_->id(argcEvent);

    SValuePtr argcSymbolic = SValue::promote(undefined_(SP.nBits()));
    argcSymbolic->comment("argc");
    writeMemory(RegisterDescriptor(), number_(SP.nBits(), argcVa), argcSymbolic, boolean_(true));

    inputVariables_.insertProgramArgumentCount(argcEventId, argcSymbolic->get_expression());
    SAWYER_MESG(debug) <<"  argc @" <<StringUtility::addrToString(argcVa) <<" = " <<argc
                       <<"; symbolic = " <<(*argcSymbolic + fmt)
                       <<"; event = " <<*argcEventId <<"\n";

    // The argc value cannot be less than 1 since it always points to at least the program name.
    SymbolicExpr::Ptr argcMinConstraint = SymbolicExpr::makeSignedGt(argcSymbolic->get_expression(),
                                                                     SymbolicExpr::makeIntegerConstant(SP.nBits(), 0));
    solver->insert(argcMinConstraint);

    // The argc value cannot be greater than its current concrete value since making it greater would mean that the address
    // of the environment variable list would need to change, potentially affecting many other things in the program.
    SymbolicExpr::Ptr argcMaxConstraint = SymbolicExpr::makeSignedLe(argcSymbolic->get_expression(),
                                                                     SymbolicExpr::makeIntegerConstant(SP.nBits(), argc));
    solver->insert(argcMaxConstraint);

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
                rose_addr_t charVa = strVa + j;
                uint8_t charVal = s[j];

                auto charEvent = ExecutionEvent::instanceWriteMemory(testCase_, process_->newLocation(), process_->ip(),
                                                                     charVa, charVal);
                charEvent->comment((boost::format("argv[%d][%d]") % i % j).str());
                ExecutionEventId charEventId = db_->id(charEvent);

                SValuePtr charSymbolic = SValue::promote(undefined_(8));
                charSymbolic->comment((boost::format("argv_%d_%d") % i % j).str());
                writeMemory(RegisterDescriptor(), number_(SP.nBits(), charVa), charSymbolic, boolean_(true));

                inputVariables_.insertProgramArgument(charEventId, i, j, charSymbolic->get_expression());
                SAWYER_MESG(debug) <<"      byte " <<j <<" @" <<StringUtility::addrToString(charVa)
                                   <<"; symbolic = " <<(*charSymbolic + fmt)
                                   <<"; event = " <<*charEventId <<"\n";

                // TODO[Robb Matzke 2021-05-14]: If any previous character of the string is NUL then this character
                // must also be NUL.
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
                    rose_addr_t charVa = strVa + i;
                    uint8_t charVal = s[i];

                    auto charEvent = ExecutionEvent::instanceWriteMemory(testCase_, process_->newLocation(), process_->ip(),
                                                                         charVa, charVal);
                    charEvent->comment((boost::format("envp[%d][%d]") % nEnvVars % i).str());
                    ExecutionEventId charEventId = db_->id(charEvent);

                    SValuePtr charSymbolic = SValue::promote(undefined_(8));
                    charSymbolic->comment((boost::format("envp_%d_%d") % nEnvVars % i).str());
                    writeMemory(RegisterDescriptor(), number_(SP.nBits(), charVa), charSymbolic, boolean_(true));

                    inputVariables_.insertEnvironmentVariable(charEventId, nEnvVars, i, charSymbolic->get_expression());
                    SAWYER_MESG(debug) <<"      byte " <<i <<" @" <<StringUtility::addrToString(charVa)
                                       <<"; symbolic = " <<(*charSymbolic + fmt)
                                       <<"; event = " <<charEventId <<"\n";
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
Dispatcher::unwrapEmulationOperators(const BS::RiscOperatorsPtr &ops) {
    if (IS::TraceSemantics::RiscOperatorsPtr trace = boost::dynamic_pointer_cast<IS::TraceSemantics::RiscOperators>(ops))
        return RiscOperators::promote(trace->subdomain());
    return RiscOperators::promote(ops);
}

RiscOperatorsPtr
Dispatcher::emulationOperators() const {
    return unwrapEmulationOperators(operators());
}

rose_addr_t
Dispatcher::concreteInstructionPointer() const {
    return emulationOperators()->process()->ip();
}

void
Dispatcher::concreteSingleStep() {
#if 0 // [Robb Matzke 2021-05-12]
    emulationOperators()->process()->singleStep();
#else
    ASSERT_not_implemented("[Robb Matzke 2021-05-12]");
#endif
}

void
Dispatcher::processConcreteInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    emulationOperators()->process()->executeInstruction(insn);
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);

    // We do things this way so as to not interfere with debugging exceptions. We don't want to catch and re-throw.
    BOOST_SCOPE_EXIT(this_, insn) {
        this_->processConcreteInstruction(insn);
    } BOOST_SCOPE_EXIT_END;

    // Symbolic execution happens before the concrete execution (code above), but may throw an exception.
    Super::processInstruction(insn);
}

bool
Dispatcher::isTerminated() const {
    return emulationOperators()->process()->isTerminated();
}

} // namespace


} // namespace
} // namespace
} // namespace

#endif
