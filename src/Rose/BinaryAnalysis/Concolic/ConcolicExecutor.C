#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>

#include <Rose/CommandLine.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BitOps.h>

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

ConcolicExecutor::ConcolicExecutor() {}

ConcolicExecutor::~ConcolicExecutor() {
    // Delete things that depend on partitioner_ before we delete the partitioner.
    process_ = Architecture::Ptr();
}

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
ConcolicExecutor::partition(const Specimen::Ptr &specimen) {
    ASSERT_not_null(specimen);
    SpecimenId specimenId = database()->id(specimen, Update::NO);
    ASSERT_require2(specimenId, "specimen must be in the database");

    P2::Engine engine;
    engine.settings().engine = settings_.partitionerEngine;
    engine.settings().loader = settings_.loader;
    engine.settings().disassembler = settings_.disassembler;
    engine.settings().partitioner = settings_.partitioner;

    // Build the P2::Partitioner object for the specimen
    P2::Partitioner partitioner;
    if (!database()->rbaExists(specimenId)) {
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
        database()->saveRbaFile(rbaFileName, specimenId);
    } else {
        Sawyer::FileSystem::TemporaryFile rbaFile;
        database()->extractRbaFile(rbaFile.name(), specimenId);
        partitioner = engine.loadPartitioner(rbaFile.name());
    }

    return boost::move(partitioner);
}

void
ConcolicExecutor::startProcess() {
    ASSERT_require(testCase());
    ASSERT_require(process());

    process()->load(tmpDir_.name());

    if (testCase()->parent()) {
        // This test case was created by cloning another test case, which usually happens when the parent test case encounters
        // a conditional branch that depends on some input. Therefore, we need to fast forward this process so that it appears
        // to have just executed that branch.
        process()->playAllEvents(partitioner());

    } else {
        // This test case was not cloned from another test case; it was probably created by the user as an initial test case.
        // Therefore delete all existing execution events (left over from a prior concolic execution) and create the events that
        // would initialize memory and registers to their current (initial) state.
        process()->saveEvents(process()->createMemoryRestoreEvents(), When::PRE);
        process()->saveEvents(process()->createRegisterRestoreEvents(), When::PRE);
    }
}

Emulation::DispatcherPtr
ConcolicExecutor::makeDispatcher(const Architecture::Ptr &process) {
    ASSERT_not_null(process);
    ASSERT_not_null(solver());

    Emulation::RiscOperatorsPtr ops =
        Emulation::RiscOperators::instance(settings_.emulationSettings, database(), testCase(), partitioner(), process,
                                           Emulation::SValue::instance(), solver());

    Emulation::DispatcherPtr cpu;
    if (settings_.traceSemantics) {
        BS::RiscOperatorsPtr trace = IS::TraceSemantics::RiscOperators::instance(ops);
        cpu = Emulation::Dispatcher::instance(trace);
    } else {
        cpu = Emulation::Dispatcher::instance(ops);
    }

    return cpu;
}

void
ConcolicExecutor::startDispatcher() {
    ASSERT_require(cpu());
    ASSERT_require(testCase());
    auto ops = Emulation::RiscOperators::promote(cpu()->operators());

    if (testCase()->parent()) {
        ASSERT_require(database()->symbolicStateExists(testCaseId()));
        BS::StatePtr state = database()->extractSymbolicState(testCaseId());
        ASSERT_not_null(state);
        ops->currentState(state);
        solver()->insert(testCase()->assertions());
        ops->restoreInputVariables(solver());
    } else {
        ops->createInputVariables(solver());
    }
}

void
ConcolicExecutor::configureExecution(const Database::Ptr &db, const TestCase::Ptr &testCase) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);

    if (testCase == testCase_) {
        ASSERT_require(db == db_);
        return;                                         // already configured
    } else {
        ASSERT_forbid2(testCase_, "concolic executor is already set up for another test case");
        ASSERT_forbid(testCaseId_);
        db_ = db;
        testCase_ = testCase;
        testCaseId_ = db->id(testCase, Update::NO);

        // Mark the test case as having NOT been run concolically, and clear any data saved as part of a previous concolic run.
        testCase->concolicResult(0);

        // Allow the user to configure a solver
        if (!solver_)
            solver_ = SmtSolver::instance("best");

        partitioner_ = partition(testCase->specimen()); // must live for duration of cpu

        // Create the a new process from the executable.
        //
        // FIXME[Robb Matzke 2021-05-25]: This will need to eventually change so that the architecture type (Linux i386 in this
        // case) is not hard coded.
        process_ = LinuxI386::instance(db, testCase, partitioner_);

        cpu_ = makeDispatcher(process_);
    }
}

SmtSolver::Ptr
ConcolicExecutor::solver() const {
    return solver_;
}

void
ConcolicExecutor::solver(const SmtSolver::Ptr &s) {
    ASSERT_forbid2(testCase(), "solver cannot be changed after execution starts");
    solver_ = s;
}

Database::Ptr
ConcolicExecutor::database() const {
    return db_;
}

TestCase::Ptr
ConcolicExecutor::testCase() const {
    return testCase_;
}

TestCaseId
ConcolicExecutor::testCaseId() const {
    return testCaseId_;
}

const P2::Partitioner&
ConcolicExecutor::partitioner() const {
    return partitioner_;
}

Architecture::Ptr
ConcolicExecutor::process() const {
    return process_;
}

Emulation::Dispatcher::Ptr
ConcolicExecutor::cpu() const {
    return cpu_;
}

std::vector<TestCase::Ptr>
ConcolicExecutor::execute() {
    ASSERT_not_null(db_);
    ASSERT_not_null(testCase_);
    ASSERT_require(testCaseId_);
    ASSERT_not_null(process_);
    ASSERT_not_null(cpu_);

    SAWYER_MESG(mlog[DEBUG]) <<"concolically executing test case " <<*testCaseId() <<"\n";
    startProcess();
    startDispatcher();

    // Extend the test case execution path in order to create new test cases.
    try {
        run();
    } catch (const Emulation::Exit &e) {
        SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"subordinate has exited with status " <<e.status() <<"\n";
    }
    testCase()->concolicResult(1);
    database()->save(testCase());

    // FIXME[Robb Matzke 2020-01-16]
    std::vector<TestCase::Ptr> newCases;
    return newCases;
}

std::vector<TestCase::Ptr>
ConcolicExecutor::execute(const Database::Ptr &db, const TestCase::Ptr &testCase) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);

    configureExecution(db, testCase);
    return execute();
}

bool
ConcolicExecutor::updateCallStack(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);                              // the instruction that was just executed

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    bool wasChanged = false;

    Emulation::RiscOperatorsPtr ops = cpu()->emulationOperators();
    P2::BasicBlock::Ptr bb = partitioner().basicBlockContainingInstruction(insn->get_address());
    if (!bb || bb->nInstructions() == 0) {
        SAWYER_MESG(debug) <<"no basic block at " <<StringUtility::addrToString(insn->get_address()) <<"\n";
        return wasChanged;                              // we're executing something that ROSE didn't disassemble
    }

    // Get the current stack pointer.
    const RegisterDescriptor SP = partitioner().instructionProvider().stackPointerRegister();
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
    if (partitioner().basicBlockIsFunctionCall(bb) && insn->get_address() == bb->instructions().back()->get_address()) {
        // Get a name for the function we just called, if possible.
        const RegisterDescriptor IP = partitioner().instructionProvider().instructionPointerRegister();
        SymbolicExpr::Ptr ip = Emulation::SValue::promote(ops->peekRegister(IP, ops->undefined_(IP.nBits())))->get_expression();
        Sawyer::Optional<uint64_t> calleeVa = ip->toUnsigned();
        std::string calleeName;
        if (calleeVa) {
            if (P2::Function::Ptr callee = partitioner().functionExists(*calleeVa)) {
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
ConcolicExecutor::handleBranch(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    ASSERT_not_null(solver());

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);

    Emulation::RiscOperatorsPtr ops = cpu()->emulationOperators();
    const RegisterDescriptor IP = partitioner().instructionProvider().instructionPointerRegister();

    // If we processed a branch instruction whose condition depended on input variables, then the instruction pointer register
    // in the symbolic state will be non-constant. It should be an if-then-else expression that evaluates to two constants, the
    // true and false execution addresses, one of which should match the concrete execution address.
    SymbolicExpr::Ptr ip = Emulation::SValue::promote(ops->peekRegister(IP, ops->undefined_(IP.nBits())))->get_expression();
    SymbolicExpr::InteriorPtr inode = ip->isInteriorNode();
    if (inode && inode->getOperator() == SymbolicExpr::OP_ITE) {
        SymbolicExpr::Ptr actualTarget = SymbolicExpr::makeIntegerConstant(IP.nBits(), cpu()->concreteInstructionPointer());
        SymbolicExpr::Ptr trueTarget = inode->child(1);  // true branch next va
        SymbolicExpr::Ptr falseTarget = inode->child(2); // false branch next va
        SymbolicExpr::Ptr followedCond;                  // condition for the branch that is followed
        SymbolicExpr::Ptr notFollowedTarget;             // address that wasn't branched to
        if (!trueTarget->isIntegerConstant()) {
            error <<"expected constant value for true branch target at " <<partitioner().unparse(insn) <<"\n";
        } else if (!falseTarget->isIntegerConstant()) {
            error <<"expected constant value for false branch target at " <<partitioner().unparse(insn) <<"\n";
        } else if (actualTarget->mustEqual(trueTarget)) {
            followedCond = inode->child(0);             // taking the true branch
            notFollowedTarget = inode->child(2);        // the false target address
        } else if (actualTarget->mustEqual(falseTarget)) {
            followedCond = SymbolicExpr::makeInvert(inode->child(0)); // taking false branch
            notFollowedTarget = inode->child(1);        // the true target address
        } else {
            error <<"unrecognized symbolic execution address after " <<partitioner().unparse(insn) <<"\n"
                  <<"  concrete = " <<*actualTarget <<"\n"
                  <<"  symbolic = " <<*ip <<"\n";
        }
        SymbolicExpr::Ptr otherCond = SymbolicExpr::makeInvert(followedCond); // condition for branch not taken

        // Solve for branch not taken in terms of input values.
        if (otherCond) {
            SAWYER_MESG(debug) <<"condition for other path is " <<*otherCond <<"\n";
            SmtSolver::Transaction transaction(solver()); // because we'll need to cancel in order to follow the correct branch
            solver()->insert(otherCond);
            switch (solver()->check()) {
                case SmtSolver::SAT_YES:
                    if (debug) {
                        debug <<"conditions are satisfied when:\n";
                        BOOST_FOREACH (const std::string &varName, solver()->evidenceNames()) {
                            ExecutionEvent::Ptr inputEvent = ops->inputVariables()->event(varName);
                            if (inputEvent) {
                                debug <<"  " <<inputEvent->name() <<" (" <<varName <<") = ";
                            } else {
                                debug <<"  " <<varName <<" (no input event) = ";
                            }
                            if (SymbolicExpr::Ptr val = solver()->evidenceForName(varName)) {
                                debug <<*val <<"\n";
                            } else {
                                debug <<" = ???\n";
                            }
                        }
                    }
                    generateTestCase(ops, notFollowedTarget);
                    break;

                case SmtSolver::SAT_UNKNOWN:
                    SAWYER_MESG(debug) <<"satisfiability is unknown (possible time out); assuming unsat\n";
                    // fall through
                case SmtSolver::SAT_NO:
                    SAWYER_MESG(debug) <<"conditions cannot be satisified; path is not feasible\n";
                    break;
            }
        }

        // Add the branch taken condition to the solver since all future assertions will also depend on having taken this branch.
        solver()->insert(followedCond);
    } else if (inode) {
        SAWYER_MESG(mlog[ERROR]) <<"instruction pointer expression no handled (not a constant or ITE): " <<*inode <<"\n";
    }
}

void
ConcolicExecutor::run() {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream trace(mlog[TRACE]);
    Sawyer::Message::Stream where(mlog[WHERE]);
    Sawyer::Message::Stream error(mlog[ERROR]);

    Emulation::RiscOperatorsPtr ops = cpu()->emulationOperators();
    ops->printInputVariables(debug);
    ops->printAssertions(debug);
    if (debug) {
        BS::Formatter fmt;
        fmt.set_line_prefix("  ");
        debug <<"initial state:\n" <<(*ops->currentState() + fmt);
    }

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"partitioner memory map:\n";
        partitioner_.memoryMap()->dump(mlog[DEBUG], "  ");
    }

    // Process instructions in execution order
    rose_addr_t executionVa = cpu()->concreteInstructionPointer();
    while (!cpu()->isTerminated()) {
        BOOST_SCOPE_EXIT(ops) {
            ops->process()->nextInstructionLocation();
        } BOOST_SCOPE_EXIT_END;

        SgAsmInstruction *insn = partitioner().instructionProvider()[executionVa];

        // FIXME[Robb Matzke 2020-07-13]: I'm not sure how this ends up happening yet. Perhaps one way is because we don't
        // handle certain system calls like mmap, brk, etc.
        //
        // Update[Robb Matzke 2021-12-15]: It was because ROSE disassembled the "run::a.out" specimen with one memory mapping
        // when the partitioner result was computed, but because of address space layout randomization (ASLR), the subsequent
        // runs each had different mappings.  The fix was to change the "run::" loading method so it turns off ASLR by
        // default. The comment about mmap, brk, etc. still stands, however.
        if (!insn) {
            std::vector<uint8_t> buf = ops->process()->readMemory(executionVa, 64);
            if (buf.empty()) {
                mlog[FATAL] <<"no memory mapped at " <<StringUtility::addrToString(executionVa) <<"\n";
            } else {
                mlog[FATAL] <<"no executable memory mapped at " <<StringUtility::addrToString(executionVa) <<"\n";
                mlog[FATAL] <<StringUtility::plural(buf.size(), "bytes") <<" starting at this address:\n";
                SgAsmExecutableFileFormat::hexdump(mlog[FATAL], executionVa, "  ", buf, true);
            }
            ASSERT_not_reachable("terminating due to prior error");
        }

        // Process the instruction concretely and symbolically
        try {
            cpu()->processInstruction(insn);
        } catch (const BS::Exception &e) {
            if (error) {
                error <<e.what() <<", occurred at:\n";
                printCallStack(error);
                error <<"  insn " <<partitioner().unparse(insn) <<"\n";
                error <<"machine state at time of error:\n" <<(*ops->currentState()+"  ");
            }
// TEMPORARILY COMMENTED OUT FOR DEBUGGING [Robb Matzke 2020-07-13]. This exception is thrown when we get an address wrong,
// like for "mov eax, gs:[16]", and the debugger is reporting that the memory cannot be read.  We should probably be using
// debugger-specific exceptions for this kind of thing.
//        } catch (std::runtime_error &e) {
//            if (error) {
//                error <<e.what() <<", occurred at:\n";
//                printCallStack(error);
//                error <<"  insn " <<partitioner().unparse(insn) <<"\n";
//                error <<"machine state at time of error:\n" <<(*ops->currentState()+"  ");
//            }
        }

#if 0 // DEBUGGING [Robb Matzke 2021-12-10]
        SAWYER_MESG(debug) <<"symbolic state after instruction:\n" <<*ops;
#endif

        if (cpu()->isTerminated()) {
            SAWYER_MESG_OR(trace, debug) <<"subordinate has terminated\n";
            break;
        }

        if (ops->hadSystemCall())
            ops->process()->systemCall(partitioner(), ops);

        if (ops->hadSharedMemoryAccess())
            ops->process()->sharedMemoryAccessPost(partitioner(), ops);

        if (settings_.showingStates.exists(executionVa))
            SAWYER_MESG(debug) <<"state after instruction:\n" <<(*ops->currentState()+"  ");

        executionVa = cpu()->concreteInstructionPointer();
        if (updateCallStack(insn) && where) {
            where <<"function call stack:\n";
            printCallStack(where);
        }

        handleBranch(insn);
    }
}

void
ConcolicExecutor::generateTestCase(const BS::RiscOperatorsPtr &ops_, const SymbolicExpr::Ptr &childIp) {
    ASSERT_not_null(ops_);

    TestCase::Ptr oldTestCase = testCase();
    TestCaseId oldTestCaseId = testCaseId();
    ASSERT_not_null(oldTestCase);
    ASSERT_require(oldTestCaseId);
    ASSERT_not_null(solver());                          // assertions must have been checked and satisfiable

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
    const RegisterDescriptor IP = partitioner().instructionProvider().instructionPointerRegister();
    ops->writeRegister(IP, Emulation::SValue::instance_symbolic(childIp));

    // Create execution events for the child test case based on the execution events from the parent test case, but do not
    // commit the child events to the database yet because we don't know for sure that we're going to create the child test
    // case.  We'll abandon all these child execution events if we decide not to create the child test case.
    Sawyer::Container::Map<size_t, ExecutionEvent::Ptr> eventMap;
    for (ExecutionEventId parentEventId: database()->executionEvents(oldTestCaseId)) {
        ExecutionEvent::Ptr parentEvent = database()->object(parentEventId);
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
    SymbolicExpr::Ptr argc = SymbolicExpr::makeIntegerConstant(32, argv.size());
    std::vector<std::string> env;
    for (const EnvValue &pair: oldTestCase->env())
        env.push_back(pair.first + "=" + pair.second);

    // Process each piece of the solver evidence. These are the symbolic variables and their values that would cause test case
    // execution to have gone some other way at the current control flow branch. Each of the variables in the evidence will
    // correspond to a known symbolic input, which in turn corresponds to a concrete execution event.
    std::vector<ExecutionEvent::Ptr> modifiedEvents;
    bool hadError = false;
    for (const std::string &solverVar: solver()->evidenceNames()) {
        SymbolicExpr::Ptr newValue = solver()->evidenceForName(solverVar);
        ASSERT_not_null(newValue);
        ExecutionEvent::Ptr parentEvent = ops->inputVariables()->event(solverVar);
        if (!parentEvent) {
            error <<"solver variable \"" <<solverVar <<"\" doesn't correspond to any input event\n";
            hadError = true;
            continue;
        }
        if (!newValue->isIntegerConstant()) {
            error <<"solver variable \"" <<solverVar <<"\" corresponding to \"" <<parentEvent->name() <<"\" is not an integer\n";
            hadError = true;
            continue;
        }

        ExecutionEventId parentEventId = database()->id(parentEvent, Update::NO);
        ASSERT_require(parentEventId);
        ExecutionEvent::Ptr childEvent = eventMap[*parentEventId];

        switch (childEvent->inputType()) {
            case InputType::NONE:
                ASSERT_not_reachable("handled above");

            case InputType::ARGC: {
                ASSERT_require(32 == newValue->nBits());
                ASSERT_require(ops->currentState()->memoryState()->get_byteOrder() == ByteOrder::ORDER_LSB);
                ASSERT_require(childEvent->action() == ExecutionEvent::Action::MEMORY_WRITE);
                childEvent->value(newValue);
                if (!newValue->toUnsigned().isEqual(parentEvent->value()->toUnsigned())) {
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from " <<*argc <<" to " <<*newValue <<"\n";
                    argc = newValue;
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }

            case InputType::ARGV: {
                ASSERT_require(8 == newValue->nBits());
                ASSERT_require(childEvent->action() == ExecutionEvent::Action::MEMORY_WRITE);
                uint8_t byte = newValue->toUnsigned().get();
                const std::pair<size_t, size_t> indices = childEvent->inputIndices();
                ASSERT_require(indices.first < argv.size());
                ASSERT_require(indices.second < argv[indices.first].size());
                if (argv[indices.first][indices.second] != byte) {
                    childEvent->value(newValue);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from '"
                                       <<StringUtility::cEscape(argv[indices.first][indices.second]) <<"'"
                                       <<" to '" <<StringUtility::cEscape(byte) <<"'\n";
                    argv[indices.first][indices.second] = byte;
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }

            case InputType::ENVP: {
                ASSERT_require(8 == newValue->nBits());
                ASSERT_require(childEvent->action() == ExecutionEvent::Action::MEMORY_WRITE);
                uint8_t byte = newValue->toUnsigned().get();
                const std::pair<size_t, size_t> indices = childEvent->inputIndices();
                ASSERT_require(indices.first < env.size());
                ASSERT_require(indices.second < env[indices.first].size());
                if (env[indices.first][indices.second] != byte) {
                    childEvent->value(newValue);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from '"
                                       <<StringUtility::cEscape(env[indices.first][indices.second]) <<"'"
                                       <<" to '" <<StringUtility::cEscape(byte) <<"'\n";
                    env[indices.first][indices.second] = byte;
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }

            case InputType::SYSCALL_RET: {
                ASSERT_require(childEvent->action() == ExecutionEvent::Action::REGISTER_WRITE);
                SymbolicExpr::Ptr oldValue = childEvent->value();
                if (!newValue->isEquivalentTo(oldValue)) {
                    childEvent->value(newValue);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() <<" from " <<*oldValue <<" to " <<*newValue <<"\n";
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }

            case InputType::SHMEM_READ: {
                ASSERT_require(childEvent->action() == ExecutionEvent::Action::OS_SHARED_MEMORY);
                SymbolicExpr::Ptr oldValue = childEvent->value();
                if (!oldValue) {
                    childEvent->value(newValue);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() << " from nothing to " <<*newValue <<"\n";
                } else if (!newValue->isEquivalentTo(oldValue)) {
                    childEvent->value(newValue);
                    modifiedEvents.push_back(childEvent);
                    SAWYER_MESG(debug) <<"  adjusting " <<childEvent->name() << " from " <<*oldValue <<" to " <<*newValue <<"\n";
                } else {
                    SAWYER_MESG(debug) <<"  no adjustment necessary for " <<childEvent->name() <<"\n";
                }
                break;
            }
        }
    }

    // Check that program arguments are reasonable. These should never fail if we've set up the SMT solver constraints correctly.
    if (argc->toUnsigned().get() < argv.size()) {
        ASSERT_require(argc->toUnsigned().get() >= 0);
        argv.resize(argc->toUnsigned().get());
    } else if (argc->toUnsigned().get() > argv.size()) {
        argv.resize(argc->toUnsigned().get());
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
        newTestCase->assertions(solver()->assertions());
        ASSERT_forbid(argv.empty());
        newTestCase->args(argv);

        std::vector<EnvValue> envVars;
        for (const std::string &s: env) {
            size_t equalSign = s.find('=');
            ASSERT_require(equalSign != std::string::npos);
            envVars.push_back(EnvValue(s.substr(0, equalSign), s.substr(equalSign+1)));
        }
        newTestCase->env(envVars);

        TestCaseId newTestCaseId = database()->id(newTestCase);
        for (ExecutionEvent::Ptr childEvent: eventMap.values()) {
            childEvent->testCase(newTestCase);
            database()->save(childEvent);
        }

        saveSymbolicState(ops, newTestCaseId);

        auto currentIpEvent = ExecutionEvent::noAction(newTestCase,
                                                       ops->process()->nextEventLocation(When::POST),
                                                       childIp->toUnsigned().get());
        currentIpEvent->name("start of test case " + boost::lexical_cast<std::string>(*newTestCaseId));
        database()->save(currentIpEvent);

        if (debug) {
            debug <<"created new " <<newTestCase->printableName(database()) <<"\n";
            debug <<"  command-line:\n";
            for (const std::string &arg: argv)
                debug <<"    - " <<StringUtility::yamlEscape(arg) <<"\n";
            debug <<"  environment:\n";
            for (const std::string &s: env)
                debug <<"    - " <<StringUtility::yamlEscape(s) <<"\n";
            debug <<"  execution-events:  # Showing only those that differ from the parent test case\n";
            for (const ExecutionEvent::Ptr &event: modifiedEvents)
                event->toYaml(debug, database(), "    - ");
        }
    }
}

void
ConcolicExecutor::saveSymbolicState(const Emulation::RiscOperatorsPtr &ops, const TestCaseId &dstId) {
    ASSERT_require(dstId);

    database()->saveSymbolicState(dstId, ops->currentState());

    TestCase::Ptr dst = database()->object(dstId, Update::NO);
    database()->save(dst);
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

RiscOperators::RiscOperators(const Settings &settings, const DatabasePtr &db, const TestCasePtr &testCase,
                             const Partitioner2::Partitioner &partitioner, const ArchitecturePtr &process,
                             const InstructionSemantics2::BaseSemantics::StatePtr &state, const SmtSolverPtr &solver)
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

RiscOperatorsPtr
RiscOperators::instance(const Settings &settings, const Database::Ptr &db, const TestCase::Ptr &testCase,
                        const P2::Partitioner &partitioner, const Architecture::Ptr &process,
                        const BS::SValuePtr &protoval, const SmtSolver::Ptr &solver) {
    // Extend the register set with an additional Boolean register named "path"
    RegisterDictionary *regdict = new RegisterDictionary("Rose::BinaryAnalysis::Concolic");
    regdict->insert(partitioner.instructionProvider().registerDictionary());
    const RegisterDescriptor path(partitioner.instructionProvider().registerDictionary()->firstUnusedMajor(), 0, 0, 1);
    regdict->insert("path", path);

    // Initialize machine state
    RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
    MemoryStatePtr memory = MemoryState::instance(protoval, protoval);
    memory->set_byteOrder(ByteOrder::ORDER_LSB);
    memory->cellCompressor(IS::SymbolicSemantics::MemoryListState::CellCompressorSimple::instance());
    StatePtr state = State::instance(registers, memory);
    RiscOperatorsPtr ops(new RiscOperators(settings, db, testCase, partitioner, process, state, solver));
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

InputVariables::Ptr
RiscOperators::inputVariables() const {
    ASSERT_not_null(process_);
    return process_->inputVariables();
}

const RegisterDictionary*
RiscOperators::registerDictionary() const {
    return partitioner_.instructionProvider().registerDictionary();
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

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BS::SValue::Ptr &value) {
    if (currentInstruction() && hadSharedMemoryAccess()) {
        Sawyer::Message::Stream debug(mlog[DEBUG]);

        // This probably writing a previously-read value from shared memory into a register. It's common on RISC
        // architectures to have an instruction that copies a value from memory into a register, in which case the value
        // being written here is just the variable we're using to represent what was read from shared memory. On CISC
        // architectures the value being written might be a more complex function of the value that was read.
        //
        // In cany case, since we can't directly adjust the byte read from concrete memory (not even by pre-writing to that
        // address since it might be read-only or it might not follow normal memory semantics), we do the next best thing: we
        // adjust all the side effects of the instruction that read the byte from memory.
        SymbolicExpr::Ptr valueExpr = Emulation::SValue::promote(value)->get_expression();
        SAWYER_MESG(debug) <<"  register update for shared memory read: value = " <<*valueExpr <<"\n";
        auto event = ExecutionEvent::registerWrite(process()->testCase(), process()->nextEventLocation(When::POST),
                                                   currentInstruction()->get_address(), reg, SymbolicExpr::Ptr(),
                                                   SymbolicExpr::Ptr(), valueExpr);
        event->name(hadSharedMemoryAccess()->name() + "_toreg");
        database()->save(event);
        SAWYER_MESG(debug) <<"    created " <<event->printableName(database()) <<"\n";
    }
    Super::writeRegister(reg, value);
}

BS::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BS::SValuePtr &addr,
                          const BS::SValuePtr &dfltUnused, const BS::SValuePtr &cond) {
    if (isRecursive()) {
        // Don't do anything special because we're being called from inside a user callback.
        return Super::readMemory(segreg, addr, dfltUnused, cond);

    } else if (auto va = addr->toUnsigned()) {
        // Read the memory's value symbolically, and if we don't have a value then read it concretely and use the concrete value to
        // update the symbolic state. However, we can only read it concretely if the address is a constant.
        size_t nBytes = dfltUnused->nBits() / 8;
        SymbolicExpr::Ptr concrete = SymbolicExpr::makeIntegerConstant(dfltUnused->nBits(),
                                                                       process_->readMemoryUnsigned(*va, nBytes));

        // Handle shared memory by invoking user-defined callbacks
        SharedMemoryCallbacks callbacks = process()->sharedMemory().getOrDefault(*va);
        if (!callbacks.isEmpty()) {
            isRecursive(true);
            BOOST_SCOPE_EXIT(this_) {
                this_->isRecursive(false);
            } BOOST_SCOPE_EXIT_END;

            // FIXME[Robb Matzke 2021-09-09]: use structured bindings when ROSE requires C++17 or later
            auto x = process()->sharedMemoryAccess(callbacks, partitioner(), RiscOperators::promote(shared_from_this()),
                                                   *va, nBytes);
            ExecutionEvent::Ptr sharedMemoryEvent = x.first;
            SymbolicExpr::Ptr result = x.second;

            if (result) {
                hadSharedMemoryAccess(sharedMemoryEvent);
                return svalueExpr(result);
            }
        }

        SValuePtr dflt = SValue::promote(undefined_(dfltUnused->nBits()));
        dflt->set_expression(concrete);
        return Super::readMemory(segreg, addr, dflt, cond);

    } else {
        // Not a concrete address, so do things symbolically
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
    Emulation::RiscOperatorsPtr ops = emulationOperators();
    SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"executing insn #" <<ops->process()->currentLocation().primary()
                                             <<" " <<ops->partitioner().unparse(insn) <<"\n";
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
