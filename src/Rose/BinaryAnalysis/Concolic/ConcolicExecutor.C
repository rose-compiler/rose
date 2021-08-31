#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>

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

#include <boost/format.hpp>
#include <boost/scope_exit.hpp>

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

Architecture::Ptr
ConcolicExecutor::makeProcess(const Database::Ptr &db, const TestCaseId &testCaseId, const boost::filesystem::path &tmpDir) {
    ASSERT_not_null(db);
    ASSERT_require(testCaseId);
    TestCase::Ptr testCase = db->object(testCaseId, Update::NO);

    // Create the a new process from the executable.
    //
    // FIXME[Robb Matzke 2021-05-25]: This will need to eventually change so that the architecture type (Linux i386 in this
    // case) is not hard coded.
    Architecture::Ptr process = LinuxI386::instance(db, testCase);
    process->load(tmpDir);

    if (testCase->parent()) {
        // This test case was created by cloning another test case, which usually happens when the parent test case encounters
        // a conditional branch that depends on some input. Therefore, we need to fast forward this process so that it appears
        // to have just executed that branch.
        process->playAllEvents();

    } else {
        // This test case was not cloned from another test case; it was probably created by the user as an initial test case.
        // Therefore delete all existing execution events (left over from a prior concolic execution) and create the events that
        // would initialize memory and registers to their current (initial) state.
        process->saveEvents(process->createMemoryRestoreEvents());
        process->saveEvents(process->createRegisterRestoreEvents());
    }

    return process;
}

Emulation::DispatcherPtr
ConcolicExecutor::makeDispatcher(const Architecture::Ptr &process, const P2::Partitioner &partitioner,
                                 const SmtSolver::Ptr &solver) {
    ASSERT_not_null(process);
    ASSERT_not_null(solver);
    Database::Ptr db = process->database();
    ASSERT_not_null(db);
    TestCase::Ptr testCase = process->testCase();
    ASSERT_not_null(testCase);
    TestCaseId testCaseId = db->id(testCase, Update::NO);

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

    if (testCase->parent()) {
        ASSERT_require(db->symbolicStateExists(testCaseId));
        BS::StatePtr state = db->extractSymbolicState(testCaseId);
        ASSERT_not_null(state);
        ops->currentState(state);
        solver->insert(testCase->assertions());
        ops->restoreInputVariables(solver);
    } else {
        ops->createInputVariables(solver);
    }

    return cpu;
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

    // Create the semantics layers. The symbolic semantics uses a Partitioner, and the concrete semantics uses a subordinate
    // process which is created from the specimen.
    SmtSolver::Ptr solver = SmtSolver::instance("best");
    Architecture::Ptr process = makeProcess(db, testCaseId, tmpDir.name());
    P2::Partitioner partitioner = partition(db, testCase->specimen()); // must live for duration of cpu
    Emulation::DispatcherPtr cpu = makeDispatcher(process, partitioner, solver);

    // Extend the test case execution path in order to create new test cases.
    try {
        run(db, testCase, cpu);
    } catch (const Emulation::Exit &e) {
        SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"subordinate has exited with status " <<e.status() <<"\n";
    }
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
        SymbolicExpr::Ptr notFollowedTarget;             // address that wasn't branched to
        if (!trueTarget->isIntegerConstant()) {
            error <<"expected constant value for true branch target at " <<partitioner.unparse(insn) <<"\n";
        } else if (!falseTarget->isIntegerConstant()) {
            error <<"expected constant value for false branch target at " <<partitioner.unparse(insn) <<"\n";
        } else if (actualTarget->mustEqual(trueTarget)) {
            followedCond = inode->child(0);             // taking the true branch
            notFollowedTarget = inode->child(2);        // the false target address
        } else if (actualTarget->mustEqual(falseTarget)) {
            followedCond = SymbolicExpr::makeInvert(inode->child(0)); // taking false branch
            notFollowedTarget = inode->child(1);        // the true target address
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
                        ExecutionEvent::Ptr inputEvent = inputVariables_.get(varName);
                        ASSERT_not_null(inputEvent);
                        debug <<"  " <<inputEvent->name() <<" (" <<varName <<") = ";
                        if (SymbolicExpr::Ptr val = solver->evidenceForName(varName)) {
                            debug <<*val <<"\n";
                        } else {
                            debug <<" = ???\n";
                        }
                    }
                }

                generateTestCase(db, testCase, ops, solver, notFollowedTarget);
            }
        }

        // Add the branch taken condition to the solver since all future assertions will also depend on having taken this branch.
        solver->insert(followedCond);
    } else if (inode) {
        SAWYER_MESG(mlog[ERROR]) <<"instruction pointer expression no handled (not a constant or ITE): " <<*inode <<"\n";
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
    ops->printAssertions(debug);
    if (debug) {
        BS::Formatter fmt;
        fmt.set_line_prefix("  ");
        debug <<"initial state:\n" <<(*ops->currentState() + fmt);
    }
    const P2::Partitioner &partitioner = ops->partitioner();

    // Process instructions in execution order
    rose_addr_t executionVa = cpu->concreteInstructionPointer();
    while (!cpu->isTerminated()) {
        SgAsmInstruction *insn = partitioner.instructionProvider()[executionVa];
        // FIXME[Robb Matzke 2020-07-13]: I'm not sure how this ends up happening yet. Perhaps one way is because we don't
        // handle certain system calls like mmap, brk, etc.
        ASSERT_not_null(insn);
        SAWYER_MESG_OR(trace, debug) <<"executing " <<partitioner.unparse(insn) <<"\n";

        try {
            ops->hadSystemCall(false);
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
        }

#if 0 // DEBUGGING [Robb Matzke 2021-06-29]
        if (insn->get_address() == 0x0804903b)
            debug.enable();
        SAWYER_MESG(debug) <<"state:\n" <<*ops->currentState();
#endif
        if (cpu->isTerminated()) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has terminated\n";
            break;
        }

        if (ops->hadSystemCall()) {
            ops->hadSystemCall(false);
            ops->process()->systemCall(partitioner, ops);
        }

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
                                   const BS::RiscOperatorsPtr &ops_, const SmtSolver::Ptr &solver,
                                   const SymbolicExpr::Ptr &childIp) {
    ASSERT_not_null(db);
    ASSERT_not_null(oldTestCase);
    TestCaseId oldTestCaseId = db->id(oldTestCase, Update::NO);
    ASSERT_not_null(ops_);
    ASSERT_not_null(solver);                            // assertions must have been checked and satisfiable

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"generating new test case...\n";

    // During this function, set the RiscOperators state to be that of the child. This will allow us to easily update the child
    // state before saving it, without affecting the parent.
    auto ops = Emulation::RiscOperators::promote(ops_);
    BS::StatePtr parentState = ops->currentState();
    BS::StatePtr childState = parentState->clone();
    ops->currentState(childState);
    BOOST_SCOPE_EXIT(ops, parentState) {
        ops->currentState(parentState);
    } BOOST_SCOPE_EXIT_END;

    // The instruction pointer in the child is the childIp, which must be concrete, not the (probably) symbolic value
    // resulting from a conditional branch in the oldTestCase.
    ASSERT_require(childIp->isIntegerConstant());
    const RegisterDescriptor IP = ops->partitioner().instructionProvider().instructionPointerRegister();
    ops->writeRegister(IP, Emulation::SValue::instance_symbolic(childIp));

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
    // modifications.  These values are mostly redundant for concolic execution since the execution events are what really
    // control what inputs the test case gets, but they may be necessary for the concrete execution used to rank test cases,
    // and when running a test case manually.  We need to add the NUL terminator to the ends of the arguments while we're
    // making adjustments (because it's technically part of the argument), but then we'll remove it at the very end since all
    // C strings (and Linux arguments) are implicitly NUL terminated.
    std::vector<std::string> argv = oldTestCase->args();
    for (std::string &arg: argv)
        arg += '\0';
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
        ExecutionEvent::Ptr parentEvent = inputVariables_.get(solverVar);
        if (!parentEvent) {
            error <<"solver variable \"" <<solverVar <<"\" doesn't correspond to any input event\n";
            hadError = true;
            continue;
        }
        if (!solverValue->isIntegerConstant()) {
            error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<parentEvent->name() <<"\" is not an integer\n";
            hadError = true;
            continue;
        }

        ExecutionEventId parentEventId = db->id(parentEvent, Update::NO);
        ASSERT_require(parentEventId);
        ExecutionEvent::Ptr childEvent = eventMap[*parentEventId];

        switch (childEvent->inputType()) {
            case InputType::NONE:
                ASSERT_not_reachable("handled above");

            case InputType::PROGRAM_ARGUMENT_COUNT: {
                ASSERT_require(32 == solverValue->nBits());
                ASSERT_require(ops->currentState()->memoryState()->get_byteOrder() == ByteOrder::ORDER_LSB);
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_MEMORY);
                uint32_t newValue = solverValue->toUnsigned().get();
                if (argc != newValue) {
                    std::vector<uint8_t> bytes;
                    for (size_t i = 0; i < 4; ++i)
                        bytes.push_back(BitOps::bits(newValue, i*8, i*8+7));
                    childEvent->bytes(bytes);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from " <<argc <<" to " <<newValue <<"\n";
                    argc = newValue;
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }

            case InputType::PROGRAM_ARGUMENT: {
                ASSERT_require(8 == solverValue->nBits());
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_MEMORY);
                uint8_t newValue = solverValue->toUnsigned().get();
                size_t i = childEvent->inputI1();
                size_t j = childEvent->inputI2();
                ASSERT_require(i < argv.size());
                ASSERT_require(j < argv[i].size());
                if (argv[i][j] != newValue) {
                    std::vector<uint8_t> bytes{newValue};
                    childEvent->bytes(bytes);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from '" <<StringUtility::cEscape(argv[i][j]) <<"'"
                                       <<" to '" <<StringUtility::cEscape(newValue) <<"'\n";
                    argv[i][j] = newValue;
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }

            case InputType::ENVIRONMENT: {
                ASSERT_require(8 == solverValue->nBits());
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_MEMORY);
                uint8_t newValue = solverValue->toUnsigned().get();
                size_t i = childEvent->inputI1();
                size_t j = childEvent->inputI2();
                ASSERT_require(i < env.size());
                ASSERT_require(j < env[i].size());
                if (env[i][j] != newValue) {
                    std::vector<uint8_t> bytes{newValue};
                    childEvent->bytes(bytes);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from '" <<StringUtility::cEscape(env[i][j]) <<"'"
                                       <<" to '" <<StringUtility::cEscape(newValue) <<"'\n";
                    env[i][j] = newValue;
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }

            case InputType::SYSTEM_CALL_RETVAL: {
                ASSERT_require(childEvent->actionType() == ExecutionEvent::Action::WRITE_REGISTER);
                uint64_t oldValue = childEvent->words()[0];
                uint64_t newValue = solverValue->toUnsigned().get();
                if (oldValue != newValue) {
                    childEvent->words(std::vector<uint64_t>{newValue});
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from " <<oldValue <<" to " <<newValue <<"\n";
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }
        }
    }

    // Check that program arguments are reasonable. These should never fail if we've set up the SMT solver constraints correctly.
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
        newTestCase->parent(oldTestCaseId);
        newTestCase->assertions(solver->assertions());
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

        saveSymbolicState(ops, db, newTestCaseId);

        auto currentIpEvent = ExecutionEvent::instance(newTestCase, ops->process()->nextLocation(), childIp->toUnsigned().get());
        currentIpEvent->name("start of test case " + boost::lexical_cast<std::string>(*newTestCaseId));
        db->save(currentIpEvent);

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

void
ConcolicExecutor::saveSymbolicState(const Emulation::RiscOperatorsPtr &ops, const Database::Ptr &db, const TestCaseId &dstId) {
    ASSERT_not_null(db);
    ASSERT_require(dstId);

    db->saveSymbolicState(dstId, ops->currentState());

    TestCase::Ptr dst = db->object(dstId, Update::NO);
    db->save(dst);
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
// RiscOperators for concolic emulation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Emulation {

RiscOperatorsPtr
RiscOperators::instance(const Settings &settings, const Database::Ptr &db, const TestCase::Ptr &testCase,
                        const P2::Partitioner &partitioner, const Architecture::Ptr &process,
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
        SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(dfltUnused->nBits(),
                                                                       process_->readMemoryUnsigned(*va, nBytes));
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
        SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(dfltUnused->nBits(),
                                                                       process_->readMemoryUnsigned(*va, nBytes));
        SValuePtr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
        dflt->set_expression(concrete);
        return Super::peekMemory(segreg, addr, dflt);
    } else {
        return Super::peekMemory(segreg, addr, dfltUnused);
    }
}

void
RiscOperators::createInputVariables(const SmtSolver::Ptr &solver) {
    process_->createInputVariables(inputVariables_, partitioner_, shared_from_this(), solver);
}

void
RiscOperators::restoreInputVariables(const SmtSolver::Ptr &solver) {
    process_->restoreInputVariables(inputVariables_, partitioner_, shared_from_this(), solver);
}

void
RiscOperators::printInputVariables(std::ostream &out) const {
    out <<"input variables:\n";
    inputVariables_.print(out, "  ");
}

void
RiscOperators::printAssertions(std::ostream &out) const {
    out <<"SMT solver assertions:\n";
    for (SymbolicExpr::Ptr assertion: solver()->assertions())
        out <<"  " <<*assertion <<"\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
Dispatcher::processConcreteInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    emulationOperators()->process()->executeInstruction(operators(), insn);
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
