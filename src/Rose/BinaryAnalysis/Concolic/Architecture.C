#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/Architecture.h>

#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

Architecture::Architecture(const Database::Ptr &db, TestCaseId testCaseId, const P2::Partitioner &partitioner)
    : db_(db), testCaseId_(testCaseId), partitioner_(partitioner) {
    ASSERT_not_null(db);
    testCase_ = db->object(testCaseId, Update::NO);
    ASSERT_not_null(testCase_);
    inputVariables_ = InputVariables::instance();
}

Architecture::~Architecture() {}

Database::Ptr
Architecture::database() const {
    ASSERT_not_null(db_);
    return db_;
}

TestCaseId
Architecture::testCaseId() const {
    ASSERT_require(testCaseId_);
    return testCaseId_;
}

TestCase::Ptr
Architecture::testCase() const {
    ASSERT_not_null(testCase_);
    return testCase_;
}

const P2::Partitioner&
Architecture::partitioner() const {
    return partitioner_;
}

ExecutionLocation
Architecture::currentLocation() const {
    return currentLocation_;
}

void
Architecture::currentLocation(const ExecutionLocation &loc) {
    currentLocation_ = loc;
}

InputVariables::Ptr
Architecture::inputVariables() const {
    return inputVariables_;
}

void
Architecture::inputVariables(const InputVariablesPtr &iv) {
    inputVariables_ = iv;
}

const Architecture::SystemCallMap&
Architecture::systemCalls() const {
    return systemCalls_;
}

void
Architecture::systemCalls(size_t syscallId, const SyscallCallback::Ptr &callback) {
    ASSERT_not_null(callback);
    SyscallCallbacks &callbacks = systemCalls_.insertMaybeDefault(syscallId);
    callbacks.append(callback);
}

const Architecture::SharedMemoryMap&
Architecture::sharedMemory() const {
    return sharedMemory_;
}

void
Architecture::sharedMemory(const AddressInterval &where, const SharedMemoryCallback::Ptr &callback) {
    ASSERT_forbid(where.isEmpty());
    ASSERT_not_null(callback);
    if (callback->registeredVas().isEmpty())
        callback->registeredVas(where);

    AddressInterval remaining = where;
    while (!remaining.isEmpty()) {
        SharedMemoryCallbacks callbacks;
        auto iter = sharedMemory_.findFirstOverlap(remaining);
        AddressInterval whereToInsert;
        if (iter == sharedMemory_.nodes().end()) {
            whereToInsert = remaining;
        } else if (remaining.least() < iter->key().least()) {
            whereToInsert = AddressInterval::hull(remaining.least(), iter->key().least() - 1);
        } else {
            whereToInsert = remaining & iter->key();
            callbacks = sharedMemory_.get(whereToInsert.least());
        }

        callbacks.append(callback);
        sharedMemory_.insert(whereToInsert, callbacks);

        if (whereToInsert == remaining)
            break;
        remaining = AddressInterval::hull(whereToInsert.greatest() + 1, remaining.greatest());
    }
}

void
Architecture::saveEvents(const std::vector<ExecutionEvent::Ptr> &events, When when) {
    for (const ExecutionEvent::Ptr &event: events) {
        ASSERT_not_null(event);
        event->testCase(testCase_);
        event->location(nextEventLocation(when));
        database()->save(event);
    }
}

size_t
Architecture::playAllEvents(const P2::Partitioner &partitioner) {
    size_t retval = 0;
    std::vector<ExecutionEventId> eventIds = db_->executionEvents(testCaseId_);
    for (ExecutionEventId eventId: eventIds) {
        ExecutionEvent::Ptr event = db_->object(eventId, Update::NO);
        ASSERT_not_null(event);
        runToEvent(event, partitioner);
        bool handled = playEvent(event);
        ASSERT_always_require(handled);
        ++retval;
    }
    return retval;
}

std::vector<ExecutionEvent::Ptr>
Architecture::getRelatedEvents(const ExecutionEvent::Ptr &parent) const {
    ASSERT_not_null(parent);
    TestCaseId testCaseId = database()->id(parent->testCase());
    std::vector<ExecutionEventId> eventIds = database()->executionEvents(testCaseId, parent->location().primary());
    std::vector<ExecutionEvent::Ptr> events;

    // Process from last event toward first event because it results in fewer database reads.
    std::reverse(eventIds.begin(), eventIds.end());
    for (ExecutionEventId eventId: eventIds) {
        ExecutionEvent::Ptr event = database()->object(eventId, Update::NO);
        if (parent->location() < event->location()) {
            events.push_back(event);
        } else {
            break;
        }
    }

    // We processed the events backward, but need to return them forward.
    std::reverse(events.begin(), events.end());
    return events;
}

bool
Architecture::playEvent(const ExecutionEvent::Ptr &event) {
    ASSERT_not_null(event);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    SAWYER_MESG(debug) <<"processing " <<event->printableName(database())
                       <<" ip=" <<StringUtility::addrToString(event->instructionPointer()) <<"\n";
    inputVariables()->bind(event);

    switch (event->action()) {
        case ExecutionEvent::Action::NONE:
            SAWYER_MESG(debug) <<"  no action necessary\n";
            return true;

        case ExecutionEvent::Action::BULK_MEMORY_MAP: {
            AddressInterval where = event->memoryLocation();
            ASSERT_forbid(where.isEmpty());
            SAWYER_MESG(debug) <<"  map " <<where.size() <<" bytes at " <<StringUtility::addrToString(where) <<", prot=";
            mapMemory(where, event->permissions());
            return true;
        }

        case ExecutionEvent::Action::BULK_MEMORY_UNMAP: {
            AddressInterval where = event->memoryLocation();
            ASSERT_forbid(where.isEmpty());
            unmapMemory(where);
            return true;
        }

        case ExecutionEvent::Action::BULK_MEMORY_WRITE: {
            AddressInterval where = event->memoryLocation();
            SAWYER_MESG(debug) <<"  write memory " <<StringUtility::plural(where.size(), "bytes")
                               <<" at " <<StringUtility::addrToString(where) <<"\n";
            ASSERT_forbid(where.isEmpty());
            ASSERT_require(where.size() == event->bytes().size());
            size_t nWritten = writeMemory(where.least(), event->bytes());
            if (nWritten != where.size())
                mlog[ERROR] <<"failed to write to memory\n";
            return true;
        }

        case ExecutionEvent::Action::MEMORY_WRITE: {
            AddressInterval where = event->memoryLocation();
            SAWYER_MESG(debug) <<"  write memory " <<StringUtility::plural(where.size(), "bytes")
                               <<" at " <<StringUtility::addrToString(where) <<"\n";
            ASSERT_forbid(where.isEmpty());
            SymbolicExpression::Ptr value = event->calculateResult(inputVariables()->bindings());
            ASSERT_require(8*where.size() == value->nBits());

            // We do it this way because the value could, theoretically, be very wide.
            std::vector<uint8_t> bytes;
            bytes.reserve(where.size());
            Sawyer::Container::BitVector bits = value->isLeafNode()->bits();
            for (size_t i = 0; i < where.size(); ++i) {
                uint8_t byte = bits.toInteger(Sawyer::Container::BitVector::BitRange::baseSize(8*i, 8));
                bytes.push_back(byte);
            }
            size_t nWritten = writeMemory(where.least(), bytes);
            if (nWritten != where.size())
                mlog[ERROR] <<"failed to write to memory\n";
            return true;
        }

        case ExecutionEvent::Action::BULK_MEMORY_HASH: {
            AddressInterval where = event->memoryLocation();
            SAWYER_MESG(debug) <<"  hash memory " <<StringUtility::plural(where.size(), "bytes")
                               <<" at " <<StringUtility::addrToString(where) <<"\n";
            ASSERT_forbid(where.isEmpty());
            std::vector<uint8_t> buf = readMemory(where.least(), where.size());
            if (buf.size() != where.size()) {
                mlog[ERROR] <<"memory hash comparison failed at " <<StringUtility::addrToString(where) <<": read error\n";
            } else {
                Combinatorics::HasherSha256Builtin hasher;
                hasher.insert(buf);
                Combinatorics::Hasher::Digest currentDigest = hasher.digest();
                const Combinatorics::Hasher::Digest &savedDigest = event->bytes();
                ASSERT_require(currentDigest.size() == savedDigest.size());
                if (!std::equal(currentDigest.begin(), currentDigest.end(), savedDigest.begin()))
                    mlog[ERROR] <<"memory hash comparison failed at " <<StringUtility::addrToString(where) <<": hash differs\n";
            }
            return true;
        }

        case ExecutionEvent::Action::REGISTER_WRITE: {
            const RegisterDescriptor reg = event->registerDescriptor();
            const uint64_t concreteValue = event->calculateResult(inputVariables()->bindings())->toUnsigned().get();
            SAWYER_MESG(debug) <<"  write register "
                               <<reg <<" = " <<StringUtility::toHex2(concreteValue, reg.nBits()) <<"\n";
            writeRegister(reg, concreteValue);
            return true;
        }

        case ExecutionEvent::Action::OS_SYSCALL: {
            // This is only the start of a system call. Additional following events for the same instruction will describe the
            // effects of the system call.
            const uint64_t functionNumber = event->syscallFunction();
            SyscallCallbacks callbacks = systemCalls().getOrDefault(functionNumber);
            SyscallContext ctx(sharedFromThis(), event, getRelatedEvents(event));
            return callbacks.apply(false, ctx);
        }

        case ExecutionEvent::Action::OS_SHARED_MEMORY: {
            // This is only the start of a shared memory read. Additional following events for the same instruction will
            // describe the effects of the read.
            SharedMemoryCallbacks callbacks = sharedMemory().getOrDefault(event->memoryLocation().least());
            SharedMemoryContext ctx(sharedFromThis(), event);
            return callbacks.apply(false, ctx);
        }

        default:
            // Not handled here, so may need to be handled by a subclass.
            return false;
    }
}

void
Architecture::runToEvent(const ExecutionEvent::Ptr &event, const P2::Partitioner &partitioner) {
    ASSERT_not_null(event);

    if (event->location().when() == When::PRE) {
        while (currentLocation().primary() < event->location().primary()) {
            executeInstruction(partitioner);
            nextInstructionLocation();
        }
    } else {
        ASSERT_require(event->location().when() == When::POST);
        while (currentLocation().primary() <= event->location().primary()) {
            executeInstruction(partitioner);
            nextInstructionLocation();
        }
    }
}

uint64_t
Architecture::readMemoryUnsigned(rose_addr_t va, size_t nBytes) {
    ASSERT_require(nBytes >= 1 && nBytes <= 8);
    std::vector<uint8_t> bytes = readMemory(va, nBytes);
    ASSERT_require(bytes.size() == nBytes);
    uint64_t retval = 0;
    switch (memoryByteOrder()) {
        case ByteOrder::ORDER_LSB:
            for (size_t i = 0; i < nBytes; ++i)
                retval |= (uint64_t)bytes[i] << (8*i);
            break;
        case ByteOrder::ORDER_MSB:
            for (size_t i = 0; i < nBytes; ++i)
                retval = (retval << 8) | (uint64_t)bytes[i];
            break;
        default:
            ASSERT_not_implemented("byte order is not LSB or MSB");
    }
    return retval;
}

std::string
Architecture::readCString(rose_addr_t va, size_t maxBytes) {
    std::string retval;
    while (retval.size() < maxBytes) {
        auto byte = readMemory(va++, 1);
        if (byte.empty() || byte[0] == 0)
            break;
        retval += (char)byte[0];
    }
    return retval;
}

void
Architecture::mapMemory(const AddressInterval&, unsigned /*permissions*/) {}

void
Architecture::unmapMemory(const AddressInterval&) {}

const ExecutionLocation&
Architecture::nextInstructionLocation() {
    currentLocation_ = currentLocation_.nextPrimary();
    return currentLocation_;
}

const ExecutionLocation&
Architecture::nextEventLocation(When when) {
    currentLocation_ = currentLocation_.nextSecondary(when);
    return currentLocation_;
}

void
Architecture::restoreInputVariables(const Partitioner2::Partitioner&, const Emulation::RiscOperators::Ptr&, const SmtSolver::Ptr&) {
    for (ExecutionEventId eventId: database()->executionEvents(testCaseId())) {
        ExecutionEvent::Ptr event = database()->object(eventId, Update::NO);
        inputVariables_->playback(event);
    }
}

std::pair<ExecutionEvent::Ptr, SymbolicExpression::Ptr>
Architecture::sharedMemoryAccess(const SharedMemoryCallbacks &callbacks, const P2::Partitioner&,
                                 const Emulation::RiscOperators::Ptr &ops, rose_addr_t addr, size_t nBytes) {
    // A shared memory read has just been encountered, and we're in the middle of executing the instruction that caused it.
    ASSERT_not_null(ops);
    ASSERT_not_null2(ops->currentInstruction(), "must be called during instruction execution");
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    const rose_addr_t ip = ops->currentInstruction()->get_address();
    SAWYER_MESG(debug) <<"  shared memory read at instruction " <<StringUtility::addrToString(ip)
                       <<" from memory address " <<StringUtility::addrToString(addr)
                       <<" for " <<StringUtility::plural(nBytes, "bytes") <<"\n";

    // Create an input variable for the value read from shared memory, and bind it to a new event that indicates that this
    // instruction is reading from shared memory.
    ExecutionLocation loc = nextEventLocation(When::PRE);
    std::string name = (boost::format("shm_read_%s_%d") % StringUtility::addrToString(addr).substr(2) % loc.primary()).str();
    auto valueRead = SymbolicExpression::makeIntegerVariable(8 * nBytes, name);
    auto sharedMemoryEvent = ExecutionEvent::osSharedMemory(testCase(), loc, ip,
                                                            AddressInterval::baseSize(addr, nBytes), valueRead,
                                                            SymbolicExpression::Ptr(), /*concrete value not known yet*/
                                                            valueRead);
    inputVariables()->activate(sharedMemoryEvent, InputType::SHMEM_READ);
    database()->save(sharedMemoryEvent);
    SAWYER_MESG(debug) <<"    created input variable " <<*valueRead <<" (v" <<*valueRead->variableId() <<")"
                       <<" for " <<sharedMemoryEvent->printableName(database()) <<"\n";

    // Invoke the callbacks
    SharedMemoryContext ctx(sharedFromThis(), ops, sharedMemoryEvent);
    bool handled = callbacks.apply(false, ctx);
    ASSERT_require(ctx.sharedMemoryEvent == sharedMemoryEvent);

    if (!handled) {
        mlog[ERROR] <<"    shared memory read not handled by any callbacks; treating it as normal memory\n";
        return {ExecutionEvent::Ptr(), SymbolicExpression::Ptr()};
    } else if (!ctx.valueRead) {
        SAWYER_MESG(debug) <<"    shared memory read did not return a special value; doing a normal read\n";
    } else {
        SAWYER_MESG(debug) <<"    shared memory read returns " <<*ctx.valueRead <<"\n";
        ASSERT_require(ctx.valueRead->nBits() == 8 * nBytes);
        if (ctx.valueRead->isScalarConstant())
            sharedMemoryEvent->value(ctx.valueRead);
    }

    // Post-callback actions
    database()->save(sharedMemoryEvent);            // just in case the user modified it.
    return {ctx.sharedMemoryEvent, ctx.valueRead};
}

void
Architecture::runSharedMemoryPostCallbacks(const ExecutionEvent::Ptr &sharedMemoryEvent, const Emulation::RiscOperators::Ptr &ops) {
    ASSERT_not_null(sharedMemoryEvent);
    ASSERT_not_null(ops);

    rose_addr_t memoryVa = sharedMemoryEvent->memoryLocation().least();
    SharedMemoryCallbacks callbacks = sharedMemory().getOrDefault(memoryVa);
    SharedMemoryContext ctx(sharedFromThis(), ops, sharedMemoryEvent);
    ctx.phase = ConcolicPhase::POST_EMULATION;
    callbacks.apply(false, ctx);
}

void
Architecture::fixupSharedMemoryEvents(const ExecutionEvent::Ptr &sharedMemoryEvent, const Emulation::RiscOperators::Ptr &ops) {
    ASSERT_not_null(sharedMemoryEvent);
    ASSERT_not_null(ops);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"  fixup shared memory events...\n";

    // Update the related events, and at the same time create assertions that can be used to solve for the memory read event's
    // value.  The register write for a shared-memory read instruction has a symbolic value that depends on the shared memory
    // variable. Therefore, we can set the expression equal to the concrete value stored in this register and solve to get the
    // value that was read from memory represented by the sharedMemoryEvent.
    SmtSolver::Transaction tx(ops->solver());
    std::vector<SymbolicExpression::Ptr> newAssertions;
    std::vector<ExecutionEvent::Ptr> relatedEvents = getRelatedEvents(sharedMemoryEvent);
    for (const ExecutionEvent::Ptr &relatedEvent: relatedEvents) {
        if (relatedEvent->action() == ExecutionEvent::Action::REGISTER_WRITE) {
            ASSERT_not_null(relatedEvent->expression());
            ASSERT_require(relatedEvent->value() == nullptr);

            const RegisterDescriptor REG = relatedEvent->registerDescriptor();
            relatedEvent->value(SymbolicExpression::makeIntegerConstant(readRegister(REG)));

            if (!sharedMemoryEvent->value()) {
                SymbolicExpression::Ptr eq = SymbolicExpression::makeEq(relatedEvent->expression(), relatedEvent->value());
                SAWYER_MESG(debug) <<"    from " <<relatedEvent->printableName(database()) <<":\n";
                SAWYER_MESG(debug) <<"      asserting:  (eq[u1] " <<*relatedEvent->expression()
                                                        <<" " <<*relatedEvent->value() <<")\n";
                if (debug && eq->getOperator() != SymbolicExpression::OP_EQ)
                    debug <<"      simplified: " <<*eq <<"\n";
                newAssertions.push_back(eq);
            }
        }
    }
    ops->solver()->insert(newAssertions);

    // Figure out a concrete value for the shared memory read event based on the assertions we added above.
    if (!sharedMemoryEvent->value()) {
        std::string varName = "v" + boost::lexical_cast<std::string>(*sharedMemoryEvent->variable()->variableId());
        SymbolicExpression::Ptr concreteRead;           // value read concretely

        // First, we try to figure out the value that was concretely read from memory, but we do this in the context of all the
        // previous assertions also, so it might fail. For instance, if we're reading shared memory that represents a timer,
        // then there's probably an assertion that says the value is monotonically increasing. But if we've already replayed
        // events for this memory, then the replayed value might be higher than the one we just read concretely, causing the
        // assertions to be unsatisfiable.
        if (debug) {
            debug <<"    all assertions:\n";
            for (const SymbolicExpression::Ptr &assertion: ops->solver()->assertions())
                debug <<"      asserting:  " <<*assertion <<"\n";
        }
        SmtSolver::Satisfiable isSatisfied = ops->solver()->check();

        // If we couldn't figure out a value with all the assertions, then try again with only the assertion for the memory
        // read instruction: the shared memory event and any register writes.
        if (SmtSolver::SAT_YES == isSatisfied) {
            concreteRead = ops->solver()->evidenceForName(varName);
        } else {
            SAWYER_MESG(debug) <<"    not satisified; trying again with assertions for just this instruction\n";
            SmtSolver::Ptr extraSolver = ops->solver()->create();
            extraSolver->insert(newAssertions);
            isSatisfied = extraSolver->check();
            if (SmtSolver::SAT_YES == isSatisfied)
                concreteRead = extraSolver->evidenceForName(varName);
        }

        switch (isSatisfied) {
            case SmtSolver::SAT_NO:
                SAWYER_MESG(debug) <<"    not satisfiable\n";
                ASSERT_not_implemented("how to recover?");  // [Robb Matzke 2021-09-14]
            case SmtSolver::SAT_UNKNOWN:
                SAWYER_MESG(debug) <<"    unknown satisfiability (timed out?)\n";
                ASSERT_not_implemented("how to recover?");  // [Robb Matzke 2021-09-14]
            case SmtSolver::SAT_YES: {
                ASSERT_not_null2(concreteRead, "no evidence for " + varName);
                SAWYER_MESG(debug) <<"    presumptive concrete value read from memory: " <<*concreteRead <<"\n";
                ASSERT_require(concreteRead->isScalarConstant());
                sharedMemoryEvent->value(concreteRead);
            }
        }
    }
    ASSERT_not_null(sharedMemoryEvent->value());
    if (sharedMemoryEvent->variable())
        inputVariables()->bindVariableValue(sharedMemoryEvent->variable(), sharedMemoryEvent->value());


    // Make sure the concrete register state is what we think it should be
    SAWYER_MESG(debug) <<"  fixing up concrete state\n";
    for (const ExecutionEvent::Ptr &relatedEvent: relatedEvents) {
        if (relatedEvent->action() == ExecutionEvent::Action::REGISTER_WRITE) {
            SymbolicExpression::Ptr registerValue = relatedEvent->calculateResult(inputVariables()->bindings());
            ASSERT_require(registerValue->isScalarConstant());
            SAWYER_MESG(debug) <<"    for " <<relatedEvent->printableName(database()) <<"\n"
                               <<"      writing " <<*registerValue <<"to register " <<relatedEvent->registerDescriptor() <<"\n";
            writeRegister(relatedEvent->registerDescriptor(), registerValue->isLeafNode()->bits());
        }
    }

    // If the shared memory event is not a test case input variable, then make the subsequent register updates constant.
    if (!sharedMemoryEvent->inputVariable()) {
        for (const ExecutionEvent::Ptr &relatedEvent: relatedEvents) {
            if (relatedEvent->action() == ExecutionEvent::Action::REGISTER_WRITE) {
                if (relatedEvent->expression()->isConstant()) {
                    std::cerr <<"ROBB: relatedEvent->expression() = " <<*relatedEvent->expression() <<"\n";
                } else {
                    relatedEvent->expression(relatedEvent->value());
                    ops->writeRegister(relatedEvent->registerDescriptor(), ops->svalueExpr(relatedEvent->value()));
                }
                SAWYER_MESG(debug) <<"    " <<relatedEvent->printableName(database()) <<" is constant; "
                                   <<"value = "<<*relatedEvent->value() <<"\n";
            }
        }
    }
}

void
Architecture::printSharedMemoryEvents(const ExecutionEvent::Ptr &sharedMemoryEvent, const Emulation::RiscOperators::Ptr &ops) {
    ASSERT_not_null(sharedMemoryEvent);
    ASSERT_not_null(ops);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"  shared memory final results:\n";

    SAWYER_MESG(debug) <<"    shared memory " <<sharedMemoryEvent->printableName(ops->database()) <<"\n";
    if (sharedMemoryEvent->variable()) {
        if (InputType::NONE == sharedMemoryEvent->inputType()) {
            SAWYER_MESG(debug) <<"      non-input variable: " <<*sharedMemoryEvent->variable() <<"\n";
        } else {
            SAWYER_MESG(debug) <<"      input variable: " <<*sharedMemoryEvent->variable() <<"\n";
        }
    } else {
        SAWYER_MESG(debug) <<"      variable: none\n";
    }
    ASSERT_not_null(sharedMemoryEvent->value());
    SAWYER_MESG(debug) <<"      concrete value: " <<*sharedMemoryEvent->value() <<"\n";
    ASSERT_require(sharedMemoryEvent->value()->isScalarConstant());
    ASSERT_not_null(sharedMemoryEvent->expression());
    SAWYER_MESG(debug) <<"      expression: " <<*sharedMemoryEvent->expression() <<"\n";

    for (const ExecutionEvent::Ptr &related: getRelatedEvents(sharedMemoryEvent)) {
        SAWYER_MESG(debug) <<"    related " <<related->printableName(ops->database()) <<"\n";
        ASSERT_forbid(related->inputVariable());
        if (related->variable()) {
            SAWYER_MESG(debug) <<"      non-input variable: " <<*related->variable() <<"\n";
        } else {
            SAWYER_MESG(debug) <<"      variable: none\n";
        }
        ASSERT_not_null(related->value());
        SAWYER_MESG(debug) <<"      concrete value: " <<*related->value() <<"\n";
        ASSERT_require(related->value()->isScalarConstant());
        ASSERT_not_null(related->expression());
        SAWYER_MESG(debug) <<"      expression: " <<*related->expression() <<"\n";
    }
}

void
Architecture::sharedMemoryAccessPost(const P2::Partitioner&, const Emulation::RiscOperators::Ptr &ops) {
    // Called after a shared memory accessing instruction has completed.
    ASSERT_not_null(ops);
    ASSERT_require2(ops->currentInstruction() == nullptr, "must be called after instruction execution");

    ExecutionEvent::Ptr sharedMemoryEvent = ops->hadSharedMemoryAccess();
    ASSERT_not_null(sharedMemoryEvent);
    runSharedMemoryPostCallbacks(sharedMemoryEvent, ops);
    fixupSharedMemoryEvents(sharedMemoryEvent, ops);
    printSharedMemoryEvents(sharedMemoryEvent, ops);

    // Update the database since events may have changed since they were created.
    database()->save(sharedMemoryEvent);
    for (const ExecutionEvent::Ptr &relatedEvent: getRelatedEvents(sharedMemoryEvent))
        database()->save(relatedEvent);
}

} // namespace
} // namespace
} // namespace

#endif
