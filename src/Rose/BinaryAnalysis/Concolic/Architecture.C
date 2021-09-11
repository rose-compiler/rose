#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/Architecture.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
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

Architecture::SystemCallMap&
Architecture::systemCalls() {
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

Architecture::SharedMemoryMap&
Architecture::sharedMemory() {
    return sharedMemory_;
}

void
Architecture::sharedMemory(const AddressInterval &where, const SharedMemoryCallback::Ptr &callback) {
    ASSERT_not_null(callback);
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
        ExecutionEvent::Ptr event = database()->object(eventId);
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
            SymbolicExpr::Ptr value = event->calculateResult(inputVariables()->bindings());
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
Architecture::restoreInputVariables(const Partitioner2::Partitioner&, const Emulation::RiscOperatorsPtr&, const SmtSolver::Ptr&) {
    for (ExecutionEventId eventId: database()->executionEvents(testCaseId())) {
        ExecutionEvent::Ptr event = database()->object(eventId, Update::NO);
        inputVariables_->playback(event);
    }
}

std::pair<ExecutionEvent::Ptr, SymbolicExpr::Ptr>
Architecture::sharedMemoryRead(const SharedMemoryCallbacks&, const P2::Partitioner&, const BS::RiscOperators::Ptr&,
                               rose_addr_t /*memoryVa*/, size_t /*nBytes*/) {
    return {ExecutionEvent::Ptr(), SymbolicExpr::Ptr()};
}

} // namespace
} // namespace
} // namespace

#endif
