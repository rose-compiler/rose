#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <boost/format.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ExecutionEvent::ExecutionEvent() {}

ExecutionEvent::~ExecutionEvent() {}

ExecutionEvent::Ptr
ExecutionEvent::instance() {
    return Ptr(new ExecutionEvent);
}

ExecutionEvent::Ptr
ExecutionEvent::instance(const TestCase::Ptr &tc, const Location &location, rose_addr_t ip) {
    ASSERT_not_null(tc);
    ExecutionEvent::Ptr retval = instance();
    retval->testCase(tc);
    retval->location(location);
    retval->instructionPointer(ip);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceMapMemory(const TestCase::Ptr &tc, const Location &location, rose_addr_t ip,
                                  const AddressInterval &addresses, const std::string &prot) {
    ASSERT_forbid(addresses.isEmpty());
    ExecutionEvent::Ptr retval = instance(tc, location, ip);
    retval->actionType(Action::MAP_MEMORY);
    retval->memoryLocation(addresses);

    std::vector<uint8_t> protBytes;
    if (prot.find('r') != std::string::npos)
        protBytes.push_back('r');
    if (prot.find('w') != std::string::npos)
        protBytes.push_back('w');
    if (prot.find('x') != std::string::npos)
        protBytes.push_back('x');
    retval->bytes(protBytes);

    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceUnmapMemory(const TestCase::Ptr &tc, const Location &location, rose_addr_t ip,
                                    const AddressInterval &addresses) {
    ASSERT_forbid(addresses.isEmpty());
    ExecutionEvent::Ptr retval = instance(tc, location, ip);
    retval->actionType(Action::UNMAP_MEMORY);
    retval->memoryLocation(addresses);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceWriteMemory(const TestCase::Ptr &tc, const Location &location, rose_addr_t ip,
                                    const AddressInterval &addresses, const std::vector<uint8_t> &bytes) {
    ASSERT_forbid(addresses.isEmpty());
    ASSERT_require(addresses.size() == bytes.size());
    ExecutionEvent::Ptr retval = instance(tc, location, ip);
    retval->actionType(Action::WRITE_MEMORY);
    retval->memoryLocation(addresses);
    retval->bytes(bytes);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceHashMemory(const TestCase::Ptr &tc, const Location &location, rose_addr_t ip,
                                   const AddressInterval &addresses, const Combinatorics::Hasher::Digest &digest) {
    ASSERT_forbid(addresses.isEmpty());
    ExecutionEvent::Ptr retval = instance(tc, location, ip);
    retval->actionType(Action::HASH_MEMORY);
    retval->memoryLocation(addresses);
    retval->bytes(digest);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceRestoreRegisters(const TestCase::Ptr &tc, const Location &location, rose_addr_t ip,
                                         const Debugger::AllRegisters &allRegisters) {
    ExecutionEvent::Ptr retval = instance(tc, location, ip);
    retval->actionType(Action::RESTORE_REGISTERS);

    std::vector<uint8_t> bytes(allRegisters.regs.begin(), allRegisters.regs.end());
    bytes.insert(bytes.end(), allRegisters.fpregs.begin(), allRegisters.fpregs.end());
    retval->bytes(bytes);
    return retval;
}

TestCase::Ptr
ExecutionEvent::testCase() const {
    return testCase_;
}

void
ExecutionEvent::testCase(const TestCase::Ptr &tc) {
    ASSERT_not_null(tc);
    testCase_ = tc;
}

const std::string&
ExecutionEvent::timestamp() const {
    return timestamp_;
}

void
ExecutionEvent::timestamp(const std::string &ts) {
    timestamp_ = ts;
}

ExecutionEvent::Location
ExecutionEvent::location() const {
    return location_;
}

void
ExecutionEvent::location(const Location &loc) {
    location_ = loc;
}

rose_addr_t
ExecutionEvent::instructionPointer() const {
    return instructionPointer_;
}

void
ExecutionEvent::instructionPointer(rose_addr_t ip) {
    instructionPointer_ = ip;
}

ExecutionEvent::Action
ExecutionEvent::actionType() const {
    return action_;
}

void
ExecutionEvent::actionType(Action a) {
    action_ = a;
}

AddressInterval
ExecutionEvent::memoryLocation() const {
    return memoryLocation_;
}

void
ExecutionEvent::memoryLocation(const AddressInterval &where) {
    memoryLocation_ = where;
}

const std::vector<uint8_t>&
ExecutionEvent::bytes() const {
    return bytes_;
}

std::vector<uint8_t>&
ExecutionEvent::bytes() {
    return bytes_;
}

void
ExecutionEvent::bytes(const std::vector<uint8_t> &v) {
    bytes_ = v;
}

Debugger::AllRegisters
ExecutionEvent::allRegisters() const {
    Debugger::AllRegisters retval;
    ASSERT_require(Action::RESTORE_REGISTERS == action_);
    ASSERT_require(bytes_.size() == retval.regs.size() + retval.fpregs.size());
    const uint8_t *byte = bytes_.data();
    for (size_t i = 0; i < retval.regs.size(); ++i)
        retval.regs[i] = *byte++;
    for (size_t i = 0; i < retval.fpregs.size(); ++i)
        retval.fpregs[i] = *byte++;
    return retval;
}

void
ExecutionEvent::toYaml(std::ostream &out, const Database::Ptr &db, std::string prefix) {
    ASSERT_not_null(db);
    ExecutionEventId id = db->id(sharedFromThis(), Update::NO);
    out <<prefix <<"event:      " <<*id <<"\n";
    prefix = std::string(prefix.size(), ' ');

    out <<prefix <<"test-case:  " <<*db->id(testCase(), Update::NO) <<"\n";
    out <<prefix <<"created:    " <<timestamp() <<"\n";
    out <<prefix <<"location:   " <<location().primary <<":" <<location().secondary <<"\n";
    out <<prefix <<"ip:         " <<StringUtility::addrToString(instructionPointer()) <<"\n";

    switch (actionType()) {
        case Action::NONE: {
            out <<prefix <<"action:     none\n";
            break;
        }

        case Action::MAP_MEMORY: {
            out <<prefix <<"action:     map-memory\n";
            out <<prefix <<"where:      " <<StringUtility::addrToString(memoryLocation().least()) <<"\n";
            out <<prefix <<"size:       " <<StringUtility::plural(memoryLocation().size(), "# bytes") <<"\n";
            out <<prefix <<"permission:";
            if (bytes().empty()) {
                out <<" none";
            } else {
                for (uint8_t ch: bytes()) {
                    switch (ch) {
                        case 'r':
                            out <<" read";
                            break;
                        case 'w':
                            out <<" write";
                            break;
                        case 'x':
                            out <<" execute";
                            break;
                        default:
                            out <<" unknown";
                            break;
                    }
                }
                out <<"\n";
            }
            break;
        }

        case Action::UNMAP_MEMORY: {
            out <<prefix <<"action:     unmap-memory\n";
            out <<prefix <<"where:      " <<StringUtility::addrToString(memoryLocation().least()) <<"\n";
            out <<prefix <<"size:       " <<StringUtility::plural(memoryLocation().size(), "# bytes") <<"\n";
            break;
        }

        case Action::WRITE_MEMORY: {
            out <<prefix <<"action:     write-memory\n";
            out <<prefix <<"where:      " <<StringUtility::addrToString(memoryLocation().least()) <<"\n";
            out <<prefix <<"size:       " <<StringUtility::plural(memoryLocation().size(), "# bytes") <<"\n";
            break;
        }

        case Action::HASH_MEMORY: {
            out <<prefix <<"action:     hash-memory\n";
            out <<prefix <<"where:      " <<StringUtility::addrToString(memoryLocation().least()) <<"\n";
            out <<prefix <<"size:       " <<StringUtility::plural(memoryLocation().size(), "# bytes") <<"\n";
            out <<prefix <<"hash:       ";
            for (unsigned byte: bytes())
                out <<(boost::format("%02x") % byte);
            out <<"\n";
            break;
        }

        case Action::RESTORE_REGISTERS: {
            out <<prefix <<"action:     restore-registers\n";
            Combinatorics::HasherSha256Builtin hasher;
            hasher.insert(bytes());
            out <<prefix <<"hash:       " <<hasher.toString() <<"\n";
            break;
        }
    }
}

} // namespace
} // namespace
} // namespace

#endif
