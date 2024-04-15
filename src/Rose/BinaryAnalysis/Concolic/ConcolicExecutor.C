#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>

#include <Rose/CommandLine.h>
#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BitOps.h>

#include <SgAsmInstruction.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/scope_exit.hpp>

#ifdef __linux__
#include <sys/mman.h>
#include <sys/syscall.h>
#endif

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
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

const ConcolicExecutorSettings&
ConcolicExecutor::settings() const {
    return settings_;
}

ConcolicExecutorSettings&
ConcolicExecutor::settings() {
    return settings_;
}

void
ConcolicExecutor::settings(const ConcolicExecutorSettings &s) {
    settings_ = s;
}

// class method
ConcolicExecutor::Ptr
ConcolicExecutor::instance() {
    return Ptr(new ConcolicExecutor);
}

// class method
std::vector<Sawyer::CommandLine::SwitchGroup>
ConcolicExecutor::commandLineSwitches(ConcolicExecutorSettings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;

    std::vector<SwitchGroup> sgroups;
    sgroups.push_back(P2::EngineBinary::loaderSwitches(settings.loader));
    sgroups.push_back(P2::EngineBinary::disassemblerSwitches(settings.disassembler));
    sgroups.push_back(P2::EngineBinary::partitionerSwitches(settings.partitioner));

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

P2::Partitioner::Ptr
ConcolicExecutor::partition(const Specimen::Ptr &specimen, const Architecture::Ptr &architecture) {
    ASSERT_not_null(specimen);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    SpecimenId specimenId = database()->id(specimen, Update::NO);
    ASSERT_require2(specimenId, "specimen must be in the database");

    P2::Engine::Ptr engine = P2::EngineBinary::instance();
    engine->settings().engine = settings_.partitionerEngine;
    engine->settings().loader = settings_.loader;
    engine->settings().disassembler = settings_.disassembler;
    engine->settings().partitioner = settings_.partitioner;

    // Build the P2::Partitioner object for the specimen
    P2::Partitioner::Ptr partitioner;
    if (!database()->rbaExists(specimenId)) {
        // Extract the specimen into a temporary file in order to parse it
        Sawyer::FileSystem::TemporaryDirectory tempDir;
        boost::filesystem::path specimenFileName = tempDir.name() / "specimen";
        std::ofstream specimenFile(specimenFileName.string().c_str());
        ASSERT_forbid(specimen->content().empty()); // &std::vector<T>::operator[](0) is UB if empty; "data" is C++11
        specimenFile.write((const char*)&specimen->content()[0], specimen->content().size());
        specimenFile.close();
        boost::filesystem::permissions(specimenFileName, boost::filesystem::owner_all);

        // Parse the specimen
        partitioner = architecture->partition(engine, specimenFileName.string());

        // Cache the results in the database.
        boost::filesystem::path rbaFileName = tempDir.name() / "specimen.rba";
        partitioner->saveAsRbaFile(rbaFileName, SerialIo::BINARY);
        database()->saveRbaFile(rbaFileName, specimenId);
    } else {
        Sawyer::FileSystem::TemporaryFile rbaFile;
        database()->extractRbaFile(rbaFile.name(), specimenId);
        partitioner = P2::Partitioner::instanceFromRbaFile(rbaFile.name(), SerialIo::BINARY);
    }

    ASSERT_not_null(partitioner);
    return partitioner;
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

Emulation::Dispatcher::Ptr
ConcolicExecutor::makeDispatcher(const Architecture::Ptr &process) {
    ASSERT_not_null(process);
    ASSERT_not_null(solver());

    Emulation::RiscOperators::Ptr ops =
        Emulation::RiscOperators::instance(settings_.emulationSettings, database(), testCase(), partitioner(), process,
                                           Emulation::SValue::instance(), solver());

    Emulation::Dispatcher::Ptr cpu;
    if (settings_.traceSemantics) {
        BS::RiscOperators::Ptr trace = IS::TraceSemantics::RiscOperators::instance(ops);
        cpu = Emulation::Dispatcher::instance(trace, process);
    } else {
        cpu = Emulation::Dispatcher::instance(ops, process);
    }

    return cpu;
}

void
ConcolicExecutor::startDispatcher() {
    ASSERT_require(cpu());
    ASSERT_require(testCase());
    Emulation::RiscOperators::Ptr ops = Emulation::Dispatcher::unwrapEmulationOperators(cpu()->operators());

    if (testCase()->parent()) {
        ASSERT_require(database()->symbolicStateExists(testCaseId()));
        BS::State::Ptr state = database()->extractSymbolicState(testCaseId());
        ASSERT_not_null(state);
        ops->currentState(state);
        solver()->insert(testCase()->assertions());
        ops->restoreInputVariables(solver());
    } else {
        ops->createInputVariables(solver());
    }
}

void
ConcolicExecutor::configureExecution(const Database::Ptr &db, const TestCase::Ptr &testCase, const Yaml::Node &config) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);

    const std::string architectureName = config["architecture"].as<std::string>();

    if (testCase == testCase_) {
        ASSERT_require(db == db_);
        ASSERT_require(process_);
        ASSERT_require(process_->name() == architectureName);
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

        // Create the a new process from the executable.
        process_ = Architecture::forge(db, testCase, config);
        if (!process_)
            throw Exception("unknown architecture \"" + StringUtility::cEscape(architectureName) + "\"");
        partitioner_ = partition(testCase->specimen(), process_);
        process_->partitioner(partitioner_);

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

P2::Partitioner::ConstPtr
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
ConcolicExecutor::execute(const Database::Ptr &db, const TestCase::Ptr &testCase, const Yaml::Node &config) {
    ASSERT_not_null(db);
    ASSERT_not_null(testCase);

    configureExecution(db, testCase, config);
    return execute();
}

bool
ConcolicExecutor::updateCallStack(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);                              // the instruction that was just executed

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    bool wasChanged = false;

    Emulation::RiscOperators::Ptr ops = cpu()->emulationOperators();
    P2::BasicBlock::Ptr bb = partitioner()->basicBlockContainingInstruction(insn->get_address());
    if (!bb || bb->nInstructions() == 0) {
        SAWYER_MESG(debug) <<"no basic block at " <<StringUtility::addrToString(insn->get_address()) <<"\n";
        return wasChanged;                              // we're executing something that ROSE didn't disassemble
    }

    // Get the current stack pointer.
    const RegisterDescriptor SP = partitioner()->instructionProvider().stackPointerRegister();
    if (SP.isEmpty())
        return wasChanged;                              // no stack pointer for this architecture?!
    SymbolicExpression::Ptr sp =
        Emulation::SValue::promote(ops->peekRegister(SP, ops->undefined_(SP.nBits())))->get_expression();
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
    if (partitioner()->basicBlockIsFunctionCall(bb) && insn->get_address() == bb->instructions().back()->get_address()) {
        // Get a name for the function we just called, if possible.
        const RegisterDescriptor IP = partitioner()->instructionProvider().instructionPointerRegister();
        SymbolicExpression::Ptr ip =
            Emulation::SValue::promote(ops->peekRegister(IP, ops->undefined_(IP.nBits())))->get_expression();
        Sawyer::Optional<uint64_t> calleeVa = ip->toUnsigned();
        std::string calleeName;
        if (calleeVa) {
            if (P2::Function::Ptr callee = partitioner()->functionExists(*calleeVa)) {
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

    Emulation::RiscOperators::Ptr ops = cpu()->emulationOperators();
    const RegisterDescriptor IP = partitioner()->instructionProvider().instructionPointerRegister();

    // If we processed a branch instruction whose condition depended on input variables, then the instruction pointer register
    // in the symbolic state will be non-constant. It should be an if-then-else expression that evaluates to two constants, the
    // true and false execution addresses, one of which should match the concrete execution address.
    SymbolicExpression::Ptr ip = Emulation::SValue::promote(ops->peekRegister(IP, ops->undefined_(IP.nBits())))->get_expression();
    SymbolicExpression::InteriorPtr inode = ip->isInteriorNode();
    if (inode && inode->getOperator() == SymbolicExpression::OP_ITE) {
        SymbolicExpression::Ptr actualTarget = SymbolicExpression::makeIntegerConstant(IP.nBits(),
                                                                                       cpu()->concreteInstructionPointer());
        SymbolicExpression::Ptr trueTarget = inode->child(1);  // true branch next va
        SymbolicExpression::Ptr falseTarget = inode->child(2); // false branch next va
        SymbolicExpression::Ptr followedCond;                  // condition for the branch that is followed
        SymbolicExpression::Ptr notFollowedTarget;             // address that wasn't branched to
        if (!trueTarget->isIntegerConstant()) {
            error <<"expected constant value for true branch target at " <<partitioner()->unparse(insn) <<"\n";
        } else if (!falseTarget->isIntegerConstant()) {
            error <<"expected constant value for false branch target at " <<partitioner()->unparse(insn) <<"\n";
        } else if (actualTarget->mustEqual(trueTarget)) {
            followedCond = inode->child(0);             // taking the true branch
            notFollowedTarget = inode->child(2);        // the false target address
        } else if (actualTarget->mustEqual(falseTarget)) {
            followedCond = SymbolicExpression::makeInvert(inode->child(0)); // taking false branch
            notFollowedTarget = inode->child(1);        // the true target address
        } else {
            error <<"unrecognized symbolic execution address after " <<partitioner()->unparse(insn) <<"\n"
                  <<"  concrete = " <<*actualTarget <<"\n"
                  <<"  symbolic = " <<*ip <<"\n";
        }
        SymbolicExpression::Ptr otherCond = SymbolicExpression::makeInvert(followedCond); // condition for branch not taken

        // Solve for branch not taken in terms of input values.
        if (otherCond) {
            SAWYER_MESG(debug) <<"condition for other path is " <<*otherCond <<"\n";
            SmtSolver::Transaction transaction(solver()); // because we'll need to cancel in order to follow the correct branch
            solver()->insert(otherCond);
            switch (solver()->check()) {
                case SmtSolver::SAT_YES:
                    if (debug) {
                        debug <<"conditions are satisfied when:\n";
                        for (const std::string &varName: solver()->evidenceNames()) {
                            ExecutionEvent::Ptr inputEvent = ops->inputVariables()->event(varName);
                            if (inputEvent) {
                                debug <<"  " <<inputEvent->name() <<" (" <<varName <<") = ";
                            } else {
                                debug <<"  " <<varName <<" (no input event) = ";
                            }
                            if (SymbolicExpression::Ptr val = solver()->evidenceForName(varName)) {
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

    Emulation::RiscOperators::Ptr ops = cpu()->emulationOperators();
    ops->printInputVariables(debug);
    ops->printAssertions(debug);
    if (debug) {
        BS::Formatter fmt;
        fmt.set_line_prefix("  ");
        debug <<"initial state:\n" <<(*ops->currentState() + fmt);
    }

    if (mlog[DEBUG]) {
        mlog[DEBUG] <<"partitioner memory map:\n";
        partitioner_->memoryMap()->dump(mlog[DEBUG], "  ");
    }

    // Set up periodic state reporting
    Sawyer::ProgressBar<size_t> nInstructions(mlog[MARCH], "concolic execution");
    nInstructions.suffix(" instructions");
    Sawyer::Stopwatch timeSinceLastReport;
    IS::SymbolicSemantics::Formatter formatter;
    formatter.registerDictionary(cpu()->registerDictionary());
    formatter.set_line_prefix("    ");
    formatter.expr_formatter.max_depth = 5;             // arbitrary
    size_t reportInsns = 0;                             // number of instructions remaining to be reported

    // Process instructions in execution order
    rose_addr_t executionVa = cpu()->concreteInstructionPointer();
    while (!cpu()->isTerminated()) {
        BOOST_SCOPE_EXIT(ops) {
            ops->process()->nextInstructionLocation();
        } BOOST_SCOPE_EXIT_END;

        SgAsmInstruction *insn = partitioner()->instructionProvider()[executionVa];

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
                hexdump(mlog[FATAL], executionVa, "  ", buf, true);
            }
            ASSERT_not_reachable("terminating due to prior error");
        }

        // Process the instruction concretely and symbolically
        try {
            if (where) {
                if (timeSinceLastReport.report() > 60 /*seconds, arbitrary*/) {
                    where <<"periodic concolic execution state:\n"
                          <<"  before instruction #" <<ops->process()->currentLocation().primary()
                          <<" " <<partitioner()->unparse(insn) <<"\n";
                    ops->currentState()->registerState()->print(where, formatter);
                    timeSinceLastReport.restart();
                    reportInsns = 10;                   // arbitrary number of subsequent instructions to report
                }
                if (reportInsns > 0) {
                    if (!trace && !debug) {             // Emulation::Dispatcher::processInstruction also prints things
                        where <<"executing insn #" <<ops->process()->currentLocation().primary()
                              <<" " <<partitioner()->unparse(insn) <<"\n";
                    }
                    --reportInsns;
                }
            }
            ++nInstructions;
            cpu()->processInstruction(insn);
        } catch (const BS::Exception &e) {
            if (error) {
                error <<e.what() <<", occurred at:\n";
                printCallStack(error);
                error <<"  insn " <<partitioner()->unparse(insn) <<"\n";
                error <<"register state at time of error:\n" <<(*ops->currentState()->registerState()+"  ");
            }
// TEMPORARILY COMMENTED OUT FOR DEBUGGING [Robb Matzke 2020-07-13]. This exception is thrown when we get an address wrong,
// like for "mov eax, gs:[16]", and the debugger is reporting that the memory cannot be read.  We should probably be using
// debugger-specific exceptions for this kind of thing.
//        } catch (std::runtime_error &e) {
//            if (error) {
//                error <<e.what() <<", occurred at:\n";
//                printCallStack(error);
//                error <<"  insn " <<partitioner()->unparse(insn) <<"\n";
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
ConcolicExecutor::generateTestCase(const BS::RiscOperators::Ptr &ops_, const SymbolicExpression::Ptr &childIp) {
    ASSERT_not_null(ops_);

    TestCase::Ptr oldTestCase = testCase();
    TestCaseId oldTestCaseId = testCaseId();
    ASSERT_not_null(oldTestCase);
    ASSERT_require(oldTestCaseId);
    ASSERT_not_null(solver());                          // assertions must have been checked and satisfiable

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream info(mlog[INFO]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"generating new test case...\n";

    // During this function, set the RiscOperators state to be that of the child. This will allow us to easily update the child
    // state before saving it, without affecting the parent.
    auto ops = Emulation::RiscOperators::promote(ops_);
    BS::State::Ptr parentState = ops->currentState();
    BS::State::Ptr childState = parentState->clone();
    ops->currentState(childState);
    BOOST_SCOPE_EXIT(ops, parentState) {
        ops->currentState(parentState);
    } BOOST_SCOPE_EXIT_END;

    // The instruction pointer in the child is the childIp, which must be concrete, not the (probably) symbolic value
    // resulting from a conditional branch in the oldTestCase.
    ASSERT_require(childIp->isIntegerConstant());
    const RegisterDescriptor IP = partitioner()->instructionProvider().instructionPointerRegister();
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
    SymbolicExpression::Ptr argc = SymbolicExpression::makeIntegerConstant(32, argv.size());
    std::vector<std::string> env;
    for (const EnvValue &pair: oldTestCase->env())
        env.push_back(pair.first + "=" + pair.second);

    // Process each piece of the solver evidence. These are the symbolic variables and their values that would cause test case
    // execution to have gone some other way at the current control flow branch. Each of the variables in the evidence will
    // correspond to a known symbolic input, which in turn corresponds to a concrete execution event.
    std::vector<ExecutionEvent::Ptr> modifiedEvents;
    bool hadError = false;
    for (const std::string &solverVar: solver()->evidenceNames()) {
        SymbolicExpression::Ptr newValue = solver()->evidenceForName(solverVar);
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
                SymbolicExpression::Ptr oldValue = childEvent->value();
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
                SymbolicExpression::Ptr oldValue = childEvent->value();
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
            SAWYER_MESG_OR(info, debug) <<"created new " <<newTestCase->printableName(database()) <<"\n";
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
ConcolicExecutor::saveSymbolicState(const Emulation::RiscOperators::Ptr &ops, const TestCaseId &dstId) {
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

} // namespace
} // namespace
} // namespace

#endif
