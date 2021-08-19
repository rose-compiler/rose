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

Architecture::Architecture(const Database::Ptr &db, TestCaseId testCaseId)
    : db_(db), testCaseId_(testCaseId) {
    ASSERT_not_null(db);
    testCase_ = db->object(testCaseId, Update::NO);
    ASSERT_not_null(testCase_);
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

ExecutionLocation
Architecture::currentLocation() const {
    return currentLocation_;
}

void
Architecture::currentLocation(const ExecutionLocation &loc) {
    currentLocation_ = loc;
}

const Architecture::SystemCallMap&
Architecture::systemCalls() const {
    return systemCalls_;
}

Architecture::SystemCallMap&
Architecture::systemCalls() {
    return systemCalls_;
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

bool
Architecture::playEvent(const ExecutionEvent::Ptr &event) {
    ASSERT_not_null(event);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    SAWYER_MESG(debug) <<"processing " <<event->printableName(database())
                       <<" ip=" <<StringUtility::addrToString(event->instructionPointer()) <<"\n";
    switch (event->actionType()) {
        case ExecutionEvent::Action::NONE:
            SAWYER_MESG(debug) <<"  no action necessary\n";
            return true;

        case ExecutionEvent::Action::MAP_MEMORY: {
            AddressInterval where = event->memoryLocation();
            ASSERT_forbid(where.isEmpty());
            SAWYER_MESG(debug) <<"  map " <<where.size() <<" bytes at " <<StringUtility::addrToString(where) <<", prot=";
            unsigned prot = 0;
            for (char letter: event->bytes()) {
                SAWYER_MESG(debug) <<letter;
                switch (letter) {
                    case 'r':
                        prot |= MemoryMap::READABLE;
                        break;
                    case 'w':
                        prot |= MemoryMap::WRITABLE;
                        break;
                    case 'x':
                        prot |= MemoryMap::EXECUTABLE;
                        break;
                    default:
                        mlog[ERROR] <<"MAP_MEMORY event invalid protection letter\n";
                        break;
                }
            }
            mapMemory(where, prot);
            return true;
        }

        case ExecutionEvent::Action::UNMAP_MEMORY: {
            AddressInterval where = event->memoryLocation();
            ASSERT_forbid(where.isEmpty());
            unmapMemory(where);
            return true;
        }

        case ExecutionEvent::Action::WRITE_MEMORY: {
            AddressInterval where = event->memoryLocation();
            SAWYER_MESG(debug) <<"  write memory " <<StringUtility::plural(where.size(), "bytes")
                               <<" at " <<StringUtility::addrToString(where) <<"\n";
            ASSERT_forbid(where.isEmpty());
            ASSERT_require(where.size() == event->bytes().size());
            size_t nWritten = writeMemory(where.least(), event->bytes());
            if (nWritten != where.size())
                mlog[ERROR] <<"WRITE_MEMORY event failed to write to memory\n";
            return true;
        }

        case ExecutionEvent::Action::HASH_MEMORY: {
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

        case ExecutionEvent::Action::WRITE_REGISTER: {
            const RegisterDescriptor reg = RegisterDescriptor::fromRaw(event->scalar());
            if (SymbolicExpr::Ptr symbolicValue = event->symbolic()) {
                if (debug) {
                    debug <<"  write register " <<reg <<" = " <<*symbolicValue <<"\n";
                    for (auto varVal: variableValues_)
                        debug <<"    where " <<*varVal.first <<" = " <<*varVal.second <<"\n";
                }
                SymbolicExpr::Ptr substValue = symbolicValue->substituteMultiple(variableValues_);
                SAWYER_MESG(debug) <<"    substitution result: " <<*substValue <<"\n";
                ASSERT_require(substValue->isIntegerConstant());
                ASSERT_require(substValue->nBits() == reg.nBits());
                ASSERT_require(substValue->nBits() <= 64);
                writeRegister(reg, *substValue->toUnsigned());
            } else {
                const uint64_t concreteValue = event->words()[0];
                SAWYER_MESG(debug) <<"  write register "
                                   <<reg <<" = " <<StringUtility::toHex2(concreteValue, reg.nBits()) <<"\n";
                writeRegister(reg, concreteValue);
            }
            return true;
        }

        case ExecutionEvent::Action::OS_SHM_READ: {
            SymbolicExpr::Ptr variable = event->inputVariable();
            ASSERT_not_null(variable);
            SymbolicExpr::Ptr value = event->bytesAsSymbolic();
            ASSERT_not_null(value);
            variableValues_.insert(std::make_pair(variable, value));
            SAWYER_MESG(debug) <<"  shared memory read " <<*variable <<" = " <<*value <<"\n";
            return true;
        }

        default:
            // Not handled here, so may need to be handled by a subclass.
            return false;
    }
}

const SymbolicExpr::ExprExprHashMap&
Architecture::variableValues() const {
    return variableValues_;
}

SymbolicExpr::ExprExprHashMap&
Architecture::variableValues() {
    return variableValues_;
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
Architecture::restoreInputVariables(InputVariables &inputVariables, const Partitioner2::Partitioner&,
                                    const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr&, const SmtSolver::Ptr&) {
    for (ExecutionEventId eventId: database()->executionEvents(testCaseId())) {
        ExecutionEvent::Ptr event = database()->object(eventId, Update::NO);
        if (event->inputVariable())
            inputVariables.insertEvent(event);
    }
}

BS::SValue::Ptr
Architecture::sharedMemoryRead(const P2::Partitioner&, const BS::RiscOperators::Ptr&, const SharedMemory::Ptr&,
                               rose_addr_t /*memoryVa*/, size_t /*nBytes*/) {
    return BS::SValue::Ptr();
}

} // namespace
} // namespace
} // namespace

#endif
