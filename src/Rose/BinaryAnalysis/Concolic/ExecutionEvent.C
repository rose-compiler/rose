#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.h>
#include <Rose/BitOps.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

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
ExecutionEvent::instance(rose_addr_t ip) {
    Ptr retval = instance();
    retval->instructionPointer(ip);
    retval->actionType(Action::NONE);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instance(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip) {
    Ptr retval = instance(ip);
    retval->testCase(tc);
    retval->location(location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceMapMemory(rose_addr_t ip,
                                  const AddressInterval &addresses, const std::string &prot) {
    ASSERT_forbid(addresses.isEmpty());
    Ptr retval = instance(ip);
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
ExecutionEvent::instanceMapMemory(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                  const AddressInterval &addresses, const std::string &prot) {
    Ptr retval = instanceMapMemory(ip, addresses, prot);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceUnmapMemory(rose_addr_t ip,
                                    const AddressInterval &addresses) {
    ASSERT_forbid(addresses.isEmpty());
    Ptr retval = instance(ip);
    retval->actionType(Action::UNMAP_MEMORY);
    retval->memoryLocation(addresses);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceUnmapMemory(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                    const AddressInterval &addresses) {
    Ptr retval = instanceUnmapMemory(ip, addresses);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceWriteMemory(rose_addr_t ip,
                                    rose_addr_t va, const std::vector<uint8_t> &bytes) {
    ASSERT_forbid(bytes.empty());
    Ptr retval = instance(ip);
    retval->actionType(Action::WRITE_MEMORY);
    retval->memoryLocation(AddressInterval::baseSize(va, bytes.size()));
    retval->bytes(bytes);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceWriteMemory(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                    rose_addr_t va, const std::vector<uint8_t> &bytes) {
    Ptr retval = instanceWriteMemory(ip, va, bytes);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceHashMemory(rose_addr_t ip,
                                   const AddressInterval &addresses, const Combinatorics::Hasher::Digest &digest) {
    ASSERT_forbid(addresses.isEmpty());
    Ptr retval = instance(ip);
    retval->actionType(Action::HASH_MEMORY);
    retval->memoryLocation(addresses);
    retval->bytes(digest);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceHashMemory(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                   const AddressInterval &addresses, const Combinatorics::Hasher::Digest &digest) {
    Ptr retval = instanceHashMemory(ip, addresses, digest);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceWriteRegister(rose_addr_t ip,
                                      RegisterDescriptor reg, uint64_t value) {
    Ptr retval = instance(ip);
    retval->actionType(Action::WRITE_REGISTER);
    retval->scalar(reg.raw());
    retval->words(std::vector<uint64_t>{value});
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceWriteRegister(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                      RegisterDescriptor reg, uint64_t value) {
    Ptr retval = instanceWriteRegister(ip, reg, value);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceWriteRegister(rose_addr_t ip,
                                      RegisterDescriptor reg, const SymbolicExpr::Ptr &value) {
    ASSERT_not_null(value);
    Ptr retval = instance(ip);
    retval->actionType(Action::WRITE_REGISTER);
    retval->scalar(reg.raw());
    retval->symbolic(value);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceWriteRegister(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                      RegisterDescriptor reg, const SymbolicExpr::Ptr &value) {
    ASSERT_not_null(value);
    Ptr retval = instanceWriteRegister(ip, reg, value);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceRestoreRegisters(rose_addr_t ip,
                                         const Debugger::AllRegisters &allRegisters) {
    Ptr retval = instance(ip);
    retval->actionType(Action::RESTORE_REGISTERS);
    std::vector<uint8_t> bytes(allRegisters.regs.begin(), allRegisters.regs.end());
    bytes.insert(bytes.end(), allRegisters.fpregs.begin(), allRegisters.fpregs.end());
    retval->bytes(bytes);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceRestoreRegisters(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                         const Debugger::AllRegisters &allRegisters) {
    Ptr retval = instanceRestoreRegisters(ip, allRegisters);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceSyscall(rose_addr_t ip,
                                uint64_t functionNumber, const std::vector<uint64_t> &arguments) {
    Ptr retval = instance(ip);
    retval->actionType(Action::OS_SYSCALL);
    retval->scalar(functionNumber);

    std::vector<uint8_t> bytes;
    bytes.reserve(8 * arguments.size());
    for (uint64_t argument: arguments) {
        for (size_t i = 0; i < sizeof argument; ++i)
            bytes.push_back((uint8_t)BitOps::bits(argument, i*8, i*8+7));
    }
    retval->bytes(bytes);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceSyscall(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                uint64_t functionNumber, const std::vector<uint64_t> &arguments) {
    Ptr retval = instanceSyscall(ip, functionNumber, arguments);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceSharedMemoryRead(rose_addr_t ip, rose_addr_t memoryVa, size_t nBytes) {
    Ptr retval = instance(ip);
    retval->actionType(Action::OS_SHM_READ);
    retval->memoryLocation(AddressInterval::baseSize(memoryVa, nBytes));
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::instanceSharedMemoryRead(const TestCase::Ptr &tc, const ExecutionLocation &location, rose_addr_t ip,
                                         rose_addr_t memoryVa, size_t nBytes) {
    Ptr retval = instanceSharedMemoryRead(ip, memoryVa, nBytes);
    retval->bind(tc, location);
    return retval;
}

ExecutionEvent::Ptr
ExecutionEvent::copy() const {
    Ptr retval = instance(testCase_, location_, instructionPointer_);
    retval->timestamp_ = "";
    retval->name_ = name_;
    retval->inputType_ = inputType_;
    retval->inputVariable_ = inputVariable_;
    retval->inputI1_ = inputI1_;
    retval->inputI2_ = inputI2_;
    retval->action_ = action_;
    retval->memoryLocation_ = memoryLocation_;
    retval->scalar_ = scalar_;
    retval->bytes_ = bytes_;
    retval->symbolic_ = symbolic_;
    return retval;
}

void
ExecutionEvent::bind(const TestCase::Ptr &tc, const ExecutionLocation &loc) {
    testCase(tc);
    location(loc);
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

const std::string&
ExecutionEvent::name() const {
    return name_;
}

void
ExecutionEvent::name(const std::string &s) {
    name_ = s;
}

ExecutionLocation
ExecutionEvent::location() const {
    return location_;
}

void
ExecutionEvent::location(const ExecutionLocation &loc) {
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

void
ExecutionEvent::inputVariable(const BS::SValue::Ptr &svalue) {
    SymbolicExpr::Ptr expr = IS::SymbolicSemantics::SValue::promote(svalue)->get_expression();
    ASSERT_require(expr->isVariable2());
    inputVariable(expr);
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

SymbolicExpr::Ptr
ExecutionEvent::bytesAsSymbolic() const {
    uint64_t concreteValue = 0;
    const size_t nBytes = bytes_.size();
    ASSERT_require(nBytes <= sizeof concreteValue);     // we can fix this if it's a problem
    for (size_t i = 0; i < nBytes; ++i)
        concreteValue |= (uint64_t)bytes_[i] << (8*i);
    return SymbolicExpr::makeIntegerConstant(8 * nBytes, concreteValue);
}

std::vector<uint64_t>
ExecutionEvent::words() const {
    std::vector<uint64_t> retval;
    size_t nWords = bytes_.size() / 8;
    for (size_t i = 0; i < nWords; ++i) {
        retval.push_back(0);
        for (size_t j = 0; j < 8; ++j)
            retval.back() |= (uint64_t)bytes_[i*8 + j] << (j*8);
    }
    return retval;
}

void
ExecutionEvent::words(const std::vector<uint64_t> &words) {
    bytes_.clear();
    for (uint64_t word: words) {
        for (size_t i = 0; i < 8; ++i)
            bytes_.push_back(BitOps::bits(word, i*8, i*8+7));
    }
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
ExecutionEvent::scalar(uint64_t value) {
    scalar_ = value;
}

uint64_t
ExecutionEvent::scalar() const {
    return scalar_;
}

SymbolicExpr::Ptr
ExecutionEvent::symbolic() const {
    return symbolic_;
}

void
ExecutionEvent::symbolic(const SymbolicExpr::Ptr &s) {
    symbolic_ = s;
}

std::string
ExecutionEvent::printableName(const Database::Ptr &db) {
    std::string retval = "execution event";                    // no white space
    if (db) {
        if (ExecutionEventId id = db->id(sharedFromThis(), Update::NO))
            retval += " " + boost::lexical_cast<std::string>(*id);
    }
    retval += " at " + location().toString();
    if (!name().empty())
        retval += " \"" + StringUtility::cEscape(name()) + "\"";
    return retval;
}

void
ExecutionEvent::toYaml(std::ostream &out, const Database::Ptr &db, std::string prefix) {
    ASSERT_not_null(db);
    ExecutionEventId id = db->id(sharedFromThis(), Update::NO);

    auto nameValue = boost::format("%-16s %s\n");

    out <<prefix <<(nameValue % "event:" % *id);
    prefix = std::string(prefix.size(), ' ');

    out <<prefix <<(nameValue % "test-case:" % *db->id(testCase(), Update::NO));
    out <<prefix <<(nameValue % "created:" % timestamp());
    out <<prefix <<(nameValue % "location:" % location());
    out <<prefix <<(nameValue % "ip:" % StringUtility::addrToString(instructionPointer()));
    out <<prefix <<(nameValue % "name:" % StringUtility::yamlEscape(name()));

    if (inputVariable_) {
        out <<prefix <<(nameValue % "input-variable: " % *inputVariable_);
        switch (inputType_) {
            case InputType::NONE:
                out <<prefix <<(nameValue % "input-type:" % "none");
                break;
            case InputType::PROGRAM_ARGUMENT_COUNT:
                out <<prefix <<(nameValue % "input-type:" % "program-argument-count");
                break;
            case InputType::PROGRAM_ARGUMENT:
                out <<prefix <<(nameValue % "input-type:" % "program-argument");
                break;
            case InputType::ENVIRONMENT:
                out <<prefix <<(nameValue % "input-type:" % "environment variable");
                break;
            case InputType::SYSTEM_CALL_RETVAL:
                out <<prefix <<(nameValue % "input-type:" % "syscall-return-value");
                break;
            case InputType::SHARED_MEMORY_READ:
                out <<prefix <<(nameValue % "input-type:" % "shared-memory-read");
                break;
        }
        out <<prefix <<(nameValue % "input-i1:" % inputI1_);
        out <<prefix <<(nameValue % "input-i2:" % inputI2_);
    } else {
        out <<prefix <<(nameValue % "input-variable:" % "none");
    }

    switch (actionType()) {
        case Action::NONE: {
            out <<prefix <<(nameValue % "action:" % "none");
            break;
        }

        case Action::MAP_MEMORY: {
            out <<prefix <<(nameValue % "action:" % "map-memory");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            if (bytes().empty()) {
                out <<(nameValue % "permission:" % "none");
            } else {
                std::vector<std::string> vs;
                for (uint8_t ch: bytes()) {
                    switch (ch) {
                        case 'r':
                            vs.push_back("read");
                            break;
                        case 'w':
                            vs.push_back("write");
                            break;
                        case 'x':
                            vs.push_back("execute");
                            break;
                        default:
                            vs.push_back("unknown");
                            break;
                    }
                }
                out <<prefix <<(nameValue % "permissions:" % StringUtility::join(" ", vs));
            }
            break;
        }

        case Action::UNMAP_MEMORY: {
            out <<prefix <<(nameValue % "action:" % "unmap-memory");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            break;
        }

        case Action::WRITE_MEMORY: {
            out <<prefix <<(nameValue % "action:" % "write-memory");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            std::vector<std::string> vs;
            for (size_t i = 0; i < 32 && i < bytes().size(); ++i)
                vs.push_back((boost::format("%02x") % (unsigned)bytes()[i]).str());
            std::string s = StringUtility::join(" ", vs);
            if (bytes().size() > 32)
                s += "...";
            out <<prefix <<(nameValue % "value:" % s);
            break;
        }

        case Action::HASH_MEMORY: {
            out <<prefix <<(nameValue % "action:" % "hash-memory");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            std::vector<std::string> vs;
            for (unsigned byte: bytes())
                vs.push_back((boost::format("%02x") % byte).str());
            out <<prefix <<(nameValue % "hash:" % StringUtility::join(" ", vs));
            break;
        }

        case Action::WRITE_REGISTER: {
            out <<prefix <<(nameValue % "action:" % "write-register");
            RegisterDescriptor reg;
            reg.raw(scalar());
            std::ostringstream ss;
            ss <<reg.majorNumber() <<"." <<reg.minorNumber() <<"." <<reg.offset() <<"." <<reg.nBits();
            out <<prefix <<(nameValue % "register:" % ss.str());
            if (!bytes().empty()) {
                uint64_t value = 0;
                for (size_t i = 0; i < bytes().size(); ++i)
                    value |= (uint64_t)bytes()[i] << (i * 8);
                out <<prefix <<(nameValue % "value:" % StringUtility::toHex2(value, 8*bytes().size()));
            } else {
                ASSERT_not_null(symbolic());
                out <<prefix <<(nameValue % "value:" % *symbolic());
            }
            break;
        }

        case Action::RESTORE_REGISTERS: {
            out <<prefix <<(nameValue % "action:" % "restore-registers");
            Combinatorics::HasherSha256Builtin hasher;
            hasher.insert(bytes());
            out <<prefix <<(nameValue % "hash:" % hasher.toString());
            break;
        }

        case Action::OS_SYSCALL: {
            out <<prefix <<(nameValue % "action:" % "os-syscall");
            out <<prefix <<(nameValue % "function:" % scalar());
            out <<prefix <<"arguments:\n";
            for (uint64_t arg: words())
                out <<prefix <<"  - " <<arg <<"\n";
            break;
        }

        case Action::OS_SHM_READ: {
            out <<prefix <<(nameValue % "action:" % "os-shm-read");
            out <<prefix <<(nameValue % "memory-address:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            if (bytes().empty()) {
                out <<prefix <<(nameValue % "value:" % "none");
            } else {
                std::vector<std::string> vs;
                for (size_t i = 0; i < 32 && i < bytes().size(); ++i)
                    vs.push_back((boost::format("0x%02x") % (unsigned)bytes()[i]).str());
                std::string s = StringUtility::join(" ", vs);
                if (bytes().size() > 32)
                    s += "...";
                out <<prefix <<(nameValue % "value:" % s);
            }
            break;
        }
    }
}

} // namespace
} // namespace
} // namespace

#endif
