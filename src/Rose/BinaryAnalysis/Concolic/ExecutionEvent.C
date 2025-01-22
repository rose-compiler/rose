#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Concolic/TestSuite.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BitOps.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/NumberToString.h>
#include <Rose/StringUtility/SplitJoin.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::vector<uint8_t>
toBytes(const std::vector<uint64_t> &values) {
    std::vector<uint8_t> bytes;
    bytes.reserve(values.size() * sizeof(uint64_t));
    for (uint64_t value: values) {
        for (size_t i = 0; i < sizeof(uint64_t); ++i)
            bytes.push_back((uint8_t)(BitOps::shiftRight(value, i*8) & 0xff));
    }
    return bytes;
}

static std::vector<uint64_t>
toU64(const std::vector<uint8_t> &bytes) {
    ASSERT_require(bytes.size() % sizeof(uint64_t) == 0);
    std::vector<uint64_t> values;
    values.reserve(bytes.size() / sizeof(uint64_t));
    for (size_t i = 0; i < bytes.size(); i += sizeof(uint64_t)) {
        uint64_t u64 = 0;
        for (size_t j = 0; j < sizeof(uint64_t); ++j)
            u64 |= BitOps::shiftLeft((uint64_t)bytes[i+j], j*8);
        values.push_back(u64);
    }
    return values;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExecutionEvent
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ExecutionEvent::ExecutionEvent() {}

ExecutionEvent::~ExecutionEvent() {}

ExecutionEvent::Ptr
ExecutionEvent::copy() const {
    Ptr e = Ptr(new ExecutionEvent);
    e->testCase_ = testCase_;
    e->name_ = name_;
    e->location_ = location_;
    e->ip_ = ip_;
    e->action_ = action_;
    e->memoryVas_ = memoryVas_;
    e->u_ = u_;
    e->bytes_ = bytes_;
    e->variable_ = variable_;
    e->value_ = value_;
    e->expression_ = expression_;
    e->inputType_ = inputType_;
    e->idx1_ = idx1_;
    e->idx2_ = idx2_;
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::instance() {
    return Ptr(new ExecutionEvent);
}

ExecutionEvent::Ptr
ExecutionEvent::noAction(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::NONE;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::bulkMemoryMap(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                              const AddressInterval &where, unsigned permissions) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::BULK_MEMORY_MAP;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->memoryVas_ = where;
    e->u_ = permissions;
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::bulkMemoryUnmap(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                                const AddressInterval &where) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::BULK_MEMORY_UNMAP;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->memoryVas_ = where;
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::bulkMemoryWrite(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                                const AddressInterval &where, const std::vector<uint8_t> &bytes) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::BULK_MEMORY_WRITE;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->memoryVas_ = where;
    e->bytes_ = bytes;
    ASSERT_require(where.size() == bytes.size());
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::bulkMemoryHash(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                               const AddressInterval &where, const Combinatorics::Hasher::Digest &digest) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::BULK_MEMORY_HASH;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->memoryVas_ = where;
    e->bytes_ = digest;
    ASSERT_require(digest.size() == 32);
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::memoryWrite(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                            const AddressInterval &where, const SymbolicExpression::Ptr &variable,
                            const SymbolicExpression::Ptr &value, const SymbolicExpression::Ptr &expression) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::MEMORY_WRITE;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->memoryVas_ = where;
    e->variable_ = variable;
    ASSERT_require(!variable || variable->isScalarVariable());
    e->value_ = value;
    ASSERT_require(!value || value->isScalarConstant());
    e->expression_ = expression;
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::bulkRegisterWrite(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                                  const Sawyer::Container::BitVector &values) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::BULK_REGISTER_WRITE;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->registerValues(values);
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::registerWrite(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                              RegisterDescriptor where, const SymbolicExpression::Ptr &variable,
                              const SymbolicExpression::Ptr &value, const SymbolicExpression::Ptr &expression) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::REGISTER_WRITE;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->u_ = where.raw();
    e->variable_ = variable;
    ASSERT_require(!variable || variable->isScalarVariable());
    e->value_ = value;
    ASSERT_require(!value || value->isScalarConstant());
    e->expression_ = expression;
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::osSyscall(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                          unsigned function, const std::vector<uint64_t> &arguments) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::OS_SYSCALL;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->u_ = function;
    e->bytes_ = toBytes(arguments);
    return e;
}

ExecutionEvent::Ptr
ExecutionEvent::osSharedMemory(const TestCase::Ptr &tc, const ExecutionLocation &loc, rose_addr_t ip,
                               const AddressInterval &where, const SymbolicExpression::Ptr &variable,
                               const SymbolicExpression::Ptr &value, const SymbolicExpression::Ptr &expression) {
    Ptr e = Ptr(new ExecutionEvent);
    e->action_ = Action::OS_SHARED_MEMORY;
    e->testCase_ = tc;
    e->location_ = loc;
    e->ip_ = ip;
    e->memoryVas_ = where;
    e->variable_ = variable;
    ASSERT_require(!variable || variable->isScalarVariable());
    e->value_ = value;
    ASSERT_require(!value || value->isScalarConstant());
    e->expression_ = expression;
    return e;
}

TestCase::Ptr
ExecutionEvent::testCase() const {
    return testCase_;
}

void
ExecutionEvent::testCase(const TestCase::Ptr &tc) {
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

std::string
ExecutionEvent::name() const {
    if (!name_.empty()) {
        return name_;
    } else if (variable_) {
        if (!variable_->comment().empty()) {
            return variable_->comment();
        } else {
            ASSERT_not_null(variable_->isLeafNode());
            return variable_->isLeafNode()->toString();
        }
    } else {
        return "";
    }
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
    return ip_;
}

void
ExecutionEvent::instructionPointer(rose_addr_t ip) {
    ip_ = ip;
}

ExecutionEvent::Action
ExecutionEvent::action() const {
    return action_;
}

void
ExecutionEvent::action(Action a) {
    action_ = a;
}

AddressInterval
ExecutionEvent::memoryLocation() const {
    switch (action_) {
        case Action::BULK_MEMORY_MAP:
        case Action::BULK_MEMORY_UNMAP:
        case Action::BULK_MEMORY_WRITE:
        case Action::BULK_MEMORY_HASH:
        case Action::MEMORY_WRITE:
        case Action::OS_SHARED_MEMORY:
            return memoryVas_;
        default:
            return {};
    }
}

void
ExecutionEvent::memoryLocation(const AddressInterval &where) {
    switch (action_) {
        case Action::BULK_MEMORY_MAP:
        case Action::BULK_MEMORY_UNMAP:
        case Action::BULK_MEMORY_WRITE:
        case Action::BULK_MEMORY_HASH:
        case Action::MEMORY_WRITE:
        case Action::OS_SHARED_MEMORY:
            memoryVas_ = where;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

RegisterDescriptor
ExecutionEvent::registerDescriptor() const {
    switch (action_) {
        case Action::REGISTER_WRITE:
            return RegisterDescriptor::fromRaw(u_);
        default:
            return {};
    }
}

void
ExecutionEvent::registerDescriptor(RegisterDescriptor reg) {
    switch (action_) {
        case Action::REGISTER_WRITE:
            u_ = reg.raw();
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

unsigned
ExecutionEvent::permissions() const {
    switch (action_) {
        case Action::BULK_MEMORY_MAP:
            return u_;
        default:
            return 0;
    }
}

void
ExecutionEvent::permissions(unsigned perms) {
    switch (action_) {
        case Action::BULK_MEMORY_MAP:
            u_ = perms;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

const std::vector<uint8_t>&
ExecutionEvent::bytes() const {
    switch (action_) {
        case Action::BULK_MEMORY_WRITE:
        case Action::BULK_REGISTER_WRITE:
            return bytes_;
        default: {
            static const std::vector<uint8_t> empty;
            return empty;
        }
    }
}

void
ExecutionEvent::bytes(const std::vector<uint8_t> &v) {
    switch (action_) {
        case Action::BULK_MEMORY_WRITE:
        case Action::BULK_REGISTER_WRITE:
            bytes_ = v;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

Sawyer::Container::BitVector
ExecutionEvent::registerValues() const {
    switch (action_) {
        case Action::BULK_REGISTER_WRITE: {
            Sawyer::Container::BitVector values(8*bytes_.size());
            values.fromBytes(bytes_);
            return values;
        }
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

void
ExecutionEvent::registerValues(const Sawyer::Container::BitVector &values) {
    switch (action_) {
        case Action::BULK_REGISTER_WRITE: {
            bytes_ = values.toBytes();
            break;
        }
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

const Combinatorics::Hasher::Digest&
ExecutionEvent::hash() const {
    switch (action_) {
        case Action::BULK_MEMORY_HASH:
            ASSERT_require(bytes_.size() == 32);
            return bytes_;
        default: {
            static Combinatorics::Hasher::Digest empty;
            return empty;
        }
    }
}

void
ExecutionEvent::hash(const Combinatorics::Hasher::Digest &digest) {
    switch (action_) {
        case Action::BULK_MEMORY_HASH:
            ASSERT_require(digest.size() == 32);
            bytes_ = digest;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

SymbolicExpression::Ptr
ExecutionEvent::variable() const {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            ASSERT_require(!variable_ || variable_->isScalarVariable());
            return variable_;
        default:
            return {};
    }
}

void
ExecutionEvent::variable(const SymbolicExpression::Ptr &v) {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            ASSERT_require(!v || v->isScalarVariable());
            variable_ = v;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

SymbolicExpression::Ptr
ExecutionEvent::value() const {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            ASSERT_require(!value_ || value_->isScalarConstant());
            return value_;
        default:
            return {};
    }
}

void
ExecutionEvent::value(const SymbolicExpression::Ptr &v) {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            ASSERT_require(!v || v->isScalarConstant());
            value_ = v;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

SymbolicExpression::Ptr
ExecutionEvent::expression() const {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            return expression_;
        default:
            return {};
    }
}

void
ExecutionEvent::expression(const SymbolicExpression::Ptr &expr) {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            expression_ = expr;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

unsigned
ExecutionEvent::syscallFunction() const {
    switch (action_) {
        case Action::OS_SYSCALL:
            return u_;
        default:
            return 0;
    }
}

void
ExecutionEvent::syscallFunction(unsigned func) {
    switch (action_) {
        case Action::OS_SYSCALL:
            u_ = func;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

std::vector<uint64_t>
ExecutionEvent::syscallArguments() const {
    switch (action_) {
        case Action::OS_SYSCALL:
            return toU64(bytes_);
        default:
            return {};
    }
}

void
ExecutionEvent::syscallArguments(const std::vector<uint64_t> &args) {
    switch (action_) {
        case Action::OS_SYSCALL:
            bytes_ = toBytes(args);
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

InputType
ExecutionEvent::inputType() const {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            return inputType_;
        default:
            return InputType::NONE;
    }
}

void
ExecutionEvent::inputType(InputType it, size_t i, size_t j) {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            inputType_ = it;
            idx1_ = i;
            idx2_ = j;
            break;
        default:
            ASSERT_not_reachable("property not valid for event type");
    }
}

std::pair<size_t, size_t>
ExecutionEvent::inputIndices() const {
    switch (action_) {
        case Action::MEMORY_WRITE:
        case Action::REGISTER_WRITE:
        case Action::OS_SHARED_MEMORY:
            return {idx1_, idx2_};
        default:
            return {INVALID_INDEX, INVALID_INDEX};
    }
}

SymbolicExpression::Ptr
ExecutionEvent::inputVariable() const {
    return InputType::NONE == inputType_ ? SymbolicExpression::Ptr() : variable();
}

SymbolicExpression::Ptr
ExecutionEvent::calculateResult(const SymbolicExpression::ExprExprHashMap &bindings) const {
    SymbolicExpression::Ptr result = expression_ ? expression()->substituteMultiple(bindings) : value();
    ASSERT_not_null(result);
    ASSERT_require(result->isScalarConstant());
    return result;
}

std::string
ExecutionEvent::printableName(const Database::Ptr &db) {
    std::string retval = "execution event";                    // no white space
    if (db) {
        if (ExecutionEventId id = db->id(sharedFromThis(), Update::NO))
            retval += " " + boost::lexical_cast<std::string>(*id);
    }
    retval += " at " + location().toString();
    std::string s = name();
    if (!s.empty())
        retval += " \"" + StringUtility::cEscape(s) + "\"";
    return retval;
}

void
ExecutionEvent::toYaml(std::ostream &out, const Database::Ptr &db, std::string prefix) {
    ASSERT_not_null(db);
    ExecutionEventId id = db->id(sharedFromThis(), Update::NO);
    SymbolicExpression::Formatter fmt;
    fmt.show_comments = SymbolicExpression::Formatter::CMT_BEFORE;

    auto nameValue = boost::format("%-16s %s\n");
    auto nameValueIJ = boost::format("%-16s %s[%d][%d]\n");

    out <<prefix <<(nameValue % "event:" % *id);
    prefix = std::string(prefix.size(), ' ');

    out <<prefix <<(nameValue % "test-case:" % *db->id(testCase(), Update::NO));
    out <<prefix <<(nameValue % "created:" % timestamp());
    out <<prefix <<(nameValue % "location:" % location());
    out <<prefix <<(nameValue % "ip:" % StringUtility::addrToString(instructionPointer()));
    out <<prefix <<(nameValue % "name:" % StringUtility::yamlEscape(name()));

    if (variable_) {
        out <<prefix <<(nameValue % "variable:" % (*variable_ + fmt));
        switch (inputType_) {
            case InputType::NONE:
                out <<prefix <<(nameValue % "input-type:" % "none");
                break;
            case InputType::ARGC:
                out <<prefix <<(nameValue % "input-type:" % "argc");
                break;
            case InputType::ARGV:
                out <<prefix <<(nameValueIJ % "input-type:" % "argv" % idx1_ % idx2_);
                break;
            case InputType::ENVP:
                out <<prefix <<(nameValueIJ % "input-type:" % "envp" % idx1_ % idx2_);
                break;
            case InputType::SYSCALL_RET:
                out <<prefix <<(nameValue % "input-type:" % "syscall-return");
                break;
            case InputType::SHMEM_READ:
                out <<prefix <<(nameValue % "input-type:" % "shmem-read");
                break;
        }
    } else {
        out <<prefix <<(nameValue % "variable:" % "none");
    }

    switch (action_) {
        case Action::NONE: {
            out <<prefix <<(nameValue % "action:" % "none");
            break;
        }

        case Action::BULK_MEMORY_MAP: {
            out <<prefix <<(nameValue % "action:" % "bulk-memory-map");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            std::string s;
            s += (permissions() & MemoryMap::READABLE) ? "r" : "-";
            s += (permissions() & MemoryMap::WRITABLE) ? "w" : "-";
            s += (permissions() & MemoryMap::EXECUTABLE) ? "x" : "-";
            out <<prefix <<(nameValue % "permissions:" % s);
            break;
        }

        case Action::BULK_MEMORY_UNMAP: {
            out <<prefix <<(nameValue % "action:" % "bulk-memory-unmap");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            break;
        }

        case Action::BULK_MEMORY_WRITE: {
            out <<prefix <<(nameValue % "action:" % "bulk-memory-write");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            std::vector<std::string> vs;
            for (size_t i = 0; i < 32 && i < bytes().size(); ++i)
                vs.push_back((boost::format("%02x") % (unsigned)bytes()[i]).str());
            std::string s = StringUtility::join(" ", vs);
            if (bytes().size() > 32)
                s += "...";
            out <<prefix <<(nameValue % "value:" % s);
            Combinatorics::HasherSha256Builtin hasher;
            hasher.insert(bytes());
            out <<prefix <<(nameValue % "hash:" % hasher.toString());
            break;
        }

        case Action::BULK_MEMORY_HASH: {
            out <<prefix <<(nameValue % "action:" % "bulk-memory-hash");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            std::vector<std::string> vs;
            for (unsigned byte: bytes())
                vs.push_back((boost::format("%02x") % byte).str());
            out <<prefix <<(nameValue % "hash:" % StringUtility::join(" ", vs));
            break;
        }

        case Action::MEMORY_WRITE: {
            out <<prefix <<(nameValue % "action:" % "memory-write");
            out <<prefix <<(nameValue % "where:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            if (value()) {
                out <<prefix <<(nameValue % "value:" % toString(value(), fmt));
            } else {
                out <<prefix <<(nameValue % "value:" % "none");
            }
            if (expression()) {
                out <<prefix <<(nameValue % "expression:" % toString(expression(), fmt));
            } else {
                out <<prefix <<(nameValue % "expression:" % "none");
            }
            break;
        }

        case Action::BULK_REGISTER_WRITE: {
            out <<prefix <<(nameValue % "action:" % "bulk-register-write");
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            Combinatorics::HasherSha256Builtin hasher;
            hasher.insert(bytes());
            out <<prefix <<(nameValue % "hash:" % hasher.toString());
            break;
        }

        case Action::REGISTER_WRITE: {
            out <<prefix <<(nameValue % "action:" % "register-write");
            out <<prefix <<(nameValue % "register:" % registerDescriptor().toString());
            if (value()) {
                out <<prefix <<(nameValue % "value:" % toString(value(), fmt));
            } else {
                out <<prefix <<(nameValue % "value:" % "none");
            }
            if (expression()) {
                out <<prefix <<(nameValue % "expression:" % toString(expression(), fmt));
            } else {
                out <<prefix <<(nameValue % "expression:" % "none");
            }
            break;
        }

        case Action::OS_SYSCALL: {
            out <<prefix <<(nameValue % "action:" % "os-syscall");
            out <<prefix <<(nameValue % "function:" % syscallFunction());
            out <<prefix <<"arguments:\n";
            for (uint64_t arg: syscallArguments())
                out <<prefix <<"  - " <<arg <<"\n";
            break;
        }

        case Action::OS_SHARED_MEMORY: {
            out <<prefix <<(nameValue % "action:" % "os-shm-read");
            out <<prefix <<(nameValue % "memory-address:" % StringUtility::addrToString(memoryLocation().least()));
            out <<prefix <<(nameValue % "size:" % StringUtility::plural(memoryLocation().size(), "# bytes"));
            if (value()) {
                out <<prefix <<(nameValue % "value:" % toString(value(), fmt));
            } else {
                out <<prefix <<(nameValue % "value:" % "none");
            }
            if (expression()) {
                out <<prefix <<(nameValue % "expression:" % toString(expression(), fmt));
            } else {
                out <<prefix <<(nameValue % "expression:" % "none");
            }
            break;
        }
    }
}

void
ExecutionEvent::recreateTable(Sawyer::Database::Connection db) {
    db.run("drop table if exists execution_events");
    db.run("create table execution_events ("
           " id integer primary key,"
           " created_ts varchar(32) not null,"
           " test_suite integer not null,"

           // Identification
           " test_case integer not null,"               // test case to which this event belongs
           " name varchar(32) not null,"                // arbitrary name for debugging
           " location_primary integer not null,"        // event location.primary property
           " location_secondary integer not null,"      // events location.secondary property
           " location_when integer not null,"           // events location.when property
           " ip integer not null,"                      // value of the instruction pointer register at this event

           // Actions. The interpretation of these fields depends on the action type.
           " action_type varchar(32) not null,"         // action to be performed
           " start_va integer,"                         // starting address in memory
           " nbytes integer,"                           // size of memory location
           " scalar integer not null,"                  // permissions, register, or syscall function number
           " bytes bytea,"                              // vector value for those actions that need one
           " variable bytea,"                           // serialized variable
           " value bytea,"                              // serialized constant
           " expression bytea,"                         // serailized expression

           // Associated optional test case input variable
           " input_type string not null,"               // one of the InputType constants
           " input_i1 integer not null,"                // first integer field
           " input_i2 integer not null,"                // second integer field

           "constraint fk_test_suite foreign key (test_suite) references test_suites (id),"
           "constraint fk_test_case foreign key (test_case) references test_cases (id))");
}

void
ExecutionEvent::toDatabase(const Database::Ptr &db, ExecutionEventId eventId) {
    ASSERT_not_null(db);
    ASSERT_require(eventId);

    // SQL doesn't have unsigned types, so we'll reinterpret all our u64 values as signed 64-bit on the way to the
    // database, and reinterpret as u64 on the way back out. This means that if you look at the database directly,
    // you may see some strange values.

    Sawyer::Database::Statement stmt;
    if (*db->connection().stmt("select count(*) from execution_events where id = ?id").bind("id", *eventId).get<size_t>()) {
        stmt = db->connection().stmt("update execution_events set"
                                     "  test_case = ?test_case, "
                                     "  name = ?name,"
                                     "  location_primary = ?location_primary,"
                                     "  location_secondary = ?location_secondary,"
                                     "  location_when = ?location_when,"
                                     "  ip = ?ip,"
                                     "  action_type = ?action_type,"
                                     "  start_va = ?start_va,"
                                     "  nbytes = ?nbytes,"
                                     "  scalar = ?scalar,"
                                     "  bytes = ?bytes,"
                                     "  variable = ?variable,"
                                     "  value = ?value,"
                                     "  expression = ?expression,"
                                     "  input_type = ?input_type,"
                                     "  input_i1 = ?input_i1,"
                                     "  input_i2 = ?input_i2"
                                     " where id = ?id");
    } else {
        if (timestamp_.empty())
            timestamp_ = Database::timestamp();
        stmt = db->connection().stmt("insert into execution_events ("
                                     " id, created_ts, test_suite,"
                                     " test_case, name, location_primary, location_secondary, location_when,"
                                     " ip, action_type, start_va, nbytes, scalar, bytes,"
                                     " variable, value, expression, input_type, input_i1, input_i2"
                                     ") values ("
                                     " ?id, ?created_ts, ?test_suite,"
                                     " ?test_case, ?name, ?location_primary, ?location_secondary, ?location_when,"
                                     " ?ip, ?action_type, ?start_va, ?nbytes, ?scalar, ?bytes,"
                                     " ?variable, ?value, ?expression, ?input_type, ?input_i1, ?input_i2"
                                     ")")
               .bind("created_ts", timestamp_)
               .bind("test_suite", *db->id(db->testSuite()));
    }

    stmt.bind("id", *eventId);
    ASSERT_not_null(testCase_);
    stmt.bind("test_case", *db->id(testCase_, Update::YES));
    stmt.bind("name", name_);
    stmt.bind("location_primary", (int64_t)location_.primary());
    stmt.bind("location_secondary", (int64_t)location_.secondary());

    switch (location_.when()) {
        case When::PRE:
            stmt.bind("location_when", 0);
            break;
        case When::POST:
            stmt.bind("location_when", 1);
            break;
    }

    stmt.bind("ip", (int64_t)ip_);

    switch (action_) {
        case Action::NONE:
            stmt.bind("action_type", "none");
            break;
        case Action::BULK_MEMORY_MAP:
            stmt.bind("action_type", "bulk_memory_map");
            break;
        case Action::BULK_MEMORY_UNMAP:
            stmt.bind("action_type", "bulk_memory_unmap");
            break;
        case Action::BULK_MEMORY_WRITE:
            stmt.bind("action_type", "bulk_memory_write");
            break;
        case Action::BULK_MEMORY_HASH:
            stmt.bind("action_type", "bulk_memory_hash");
            break;
        case Action::MEMORY_WRITE:
            stmt.bind("action_type", "memory_write");
            break;
        case Action::BULK_REGISTER_WRITE:
            stmt.bind("action_type", "bulk_register_write");
            break;
        case Action::REGISTER_WRITE:
            stmt.bind("action_type", "register_write");
            break;
        case Action::OS_SYSCALL:
            stmt.bind("action_type", "os_syscall");
            break;
        case Action::OS_SHARED_MEMORY:
            stmt.bind("action_type", "os_shared_memory");
            break;
    }

    if (memoryVas_) {
        stmt.bind("start_va", (int64_t)memoryVas_.least());
        stmt.bind("nbytes", (int64_t)memoryVas_.size());
    } else {
        stmt.bind("start_va", Sawyer::Nothing());
        stmt.bind("nbytes", Sawyer::Nothing());
    }

    stmt.bind("scalar", (int64_t)u_);
    stmt.bind("bytes", bytes_);

    if (variable_) {
        std::ostringstream ss;
        {
            boost::archive::binary_oarchive archive(ss);
            archive <<variable_;
        }
        stmt.bind("variable", ss.str());
    } else {
        stmt.bind("variable", Sawyer::Nothing());
    }

    if (value_) {
        std::ostringstream ss;
        {
            boost::archive::binary_oarchive archive(ss);
            archive <<value_;
        }
        stmt.bind("value", ss.str());
    } else {
        stmt.bind("value", Sawyer::Nothing());
    }

    if (expression_) {
        std::ostringstream ss;
        {
            boost::archive::binary_oarchive archive(ss);
            archive <<expression_;
        }
        stmt.bind("expression", ss.str());
    } else {
        stmt.bind("expression", Sawyer::Nothing());
    }

    switch (inputType_) {
        case InputType::NONE:
            stmt.bind("input_type", "none");
            break;
        case InputType::ARGC:
            stmt.bind("input_type", "argc");
            break;
        case InputType::ARGV:
            stmt.bind("input_type", "argv");
            break;
        case InputType::ENVP:
            stmt.bind("input_type", "environment");
            break;
        case InputType::SYSCALL_RET:
            stmt.bind("input_type", "syscall-ret");
            break;
        case InputType::SHMEM_READ:
            stmt.bind("input_type", "shared-memory-read");
            break;
    }

    stmt.bind("input_i1", (int64_t)idx1_);
    stmt.bind("input_i2", (int64_t)idx2_);

    stmt.run();
}

void
ExecutionEvent::fromDatabase(const Database::Ptr &db, ExecutionEventId eventId) {
    ASSERT_not_null(db);
    ASSERT_require(eventId);

    // SQL doesn't have unsigned types, so we'll reinterpret all our u64 values as signed 64-bit on the way to the
    // database, and reinterpret as u64 on the way back out. This means that if you look at the database directly,
    // you may see some strange values.

    //                                        0           1          2     3                 4
    auto iter = db->connection().stmt("select created_ts, test_case, name, location_primary, location_secondary,"
                                      // 5             6   7            8         9        10     11
                                      " location_when, ip, action_type, start_va, nbytes, scalar, bytes,"
                                      // 12        13     14          15          16        17
                                      "  variable, value, expression, input_type, input_i1, input_i2"
                                      " from execution_events"
                                      " where id = ?id"
                                      " order by created_ts")
                .bind("id", *eventId)
                .begin();
    if (!iter)
        throw Exception("no such executon event in database where id=" + boost::lexical_cast<std::string>(*eventId));

    timestamp_ = *iter->get<std::string>(0);

    TestCaseId tcid(*iter->get<size_t>(1));
    TestCase::Ptr testcase = db->object(tcid);
    ASSERT_not_null(testcase);
    testCase_ = testcase;

    name_ = *iter->get<std::string>(2);

    When when;
    if (*iter->get<int>(5) == 0) {
        when = When::PRE;
    } else if (*iter->get<int>(5) == 1) {
        when = When::POST;
    } else {
        ASSERT_not_reachable("invalid 'when' field: " + boost::lexical_cast<std::string>(*iter->get<std::string>(5)));
    }
    location_ = ExecutionLocation((uint64_t)*iter->get<int64_t>(3), (uint64_t)*iter->get<int64_t>(4), when);

    ip_ = (uint64_t)*iter->get<int64_t>(6);

    std::string s = *iter->get<std::string>(7);
    if ("none" == s) {
        action_ = Action::NONE;
    } else if ("bulk_memory_map" == s) {
        action_ = Action::BULK_MEMORY_MAP;
    } else if ("bulk_memory_unmap" == s) {
        action_ = Action::BULK_MEMORY_UNMAP;
    } else if ("bulk_memory_write" == s) {
        action_ = Action::BULK_MEMORY_WRITE;
    } else if ("bulk_memory_hash" == s) {
        action_ = Action::BULK_MEMORY_HASH;
    } else if ("memory_write" == s) {
        action_ = Action::MEMORY_WRITE;
    } else if ("bulk_register_write" == s) {
        action_ = Action::BULK_REGISTER_WRITE;
    } else if ("register_write" == s) {
        action_ = Action::REGISTER_WRITE;
    } else if ("os_syscall" == s) {
        action_ = Action::OS_SYSCALL;
    } else if ("os_shared_memory" == s) {
        action_ = Action::OS_SHARED_MEMORY;
    } else {
        ASSERT_not_reachable("invalid action: \"" + StringUtility::cEscape(s) + "\"");
    }

    if (auto va = iter->get<int64_t>(8)) {
        memoryVas_ = AddressInterval::baseSize((uint64_t)*va, (uint64_t)*iter->get<int64_t>(9));
    } else {
        memoryVas_ = AddressInterval();
    }

    u_ = (uint64_t)*iter->get<int64_t>(10);

    bytes_ = iter->get<std::vector<uint8_t>>(11).orDefault();

    if (auto serialized = iter->get<std::string>(12)) {
        std::istringstream ss(*serialized);
        boost::archive::binary_iarchive archive(ss);
        SymbolicExpression::Ptr var;
        archive >>variable_;
        ASSERT_not_null(variable_);
        ASSERT_require(variable_->isScalarVariable());
    } else {
        variable_ = SymbolicExpression::Ptr();
    }

    if (auto serialized = iter->get<std::string>(13)) {
        std::istringstream ss(*serialized);
        boost::archive::binary_iarchive archive(ss);
        SymbolicExpression::Ptr var;
        archive >>value_;
        ASSERT_not_null(value_);
        ASSERT_require(value_->isScalarConstant());
    } else {
        value_ = SymbolicExpression::Ptr();
    }

    if (auto serialized = iter->get<std::string>(14)) {
        std::istringstream ss(*serialized);
        boost::archive::binary_iarchive archive(ss);
        SymbolicExpression::Ptr var;
        archive >>expression_;
        ASSERT_not_null(expression_);
    } else {
        expression_ = SymbolicExpression::Ptr();
    }

    s = iter->get<std::string>(15).orElse("none");
    if ("none" == s) {
        inputType_ = InputType::NONE;
    } else if ("argc" == s) {
        inputType_ = InputType::ARGC;
    } else if ("argv" == s) {
        inputType_ = InputType::ARGV;
    } else if ("envp" == s) {
        inputType_ = InputType::ENVP;
    } else if ("syscall-ret" == s) {
        inputType_ = InputType::SYSCALL_RET;
    } else if ("shared-memory-read" == s) {
        inputType_ = InputType::SHMEM_READ;
    } else {
        ASSERT_not_reachable("invalid input type \"" + StringUtility::cEscape(s) + "\"");
    }

    idx1_ = (uint64_t)iter->get<int64_t>(16).orElse(0);
    idx2_ = (uint64_t)iter->get<int64_t>(17).orElse(0);
}

} // namespace
} // namespace
} // namespace

#endif
