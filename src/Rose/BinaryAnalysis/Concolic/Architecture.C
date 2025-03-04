#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Architecture.h>

#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Debugger/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

// Architectures pre-registered as factories
#include <Rose/BinaryAnalysis/Concolic/I386Linux/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/Architecture.h>

#include <SgAsmInstruction.h>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

static SAWYER_THREAD_TRAITS::Mutex registryMutex;
static std::vector<Architecture::Ptr> registry;
static boost::once_flag registryInitFlag = BOOST_ONCE_INIT;

static void
initRegistryHelper() {
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(I386Linux::Architecture::factory());
    registry.push_back(M68kSystem::Architecture::factory());
}

static void
initRegistry() {
    boost::call_once(&initRegistryHelper, registryInitFlag);
}

Architecture::Architecture(const std::string &name)
    : name_(name) {}

Architecture::Architecture(const Database::Ptr &db, TestCaseId testCaseId)
    : db_(db), testCaseId_(testCaseId) {
    ASSERT_not_null(db);
    testCase_ = db->object(testCaseId, Update::NO);
    ASSERT_not_null(testCase_);
    inputVariables_ = InputVariables::instance();
}

Architecture::~Architecture() {}

void
Architecture::registerFactory(const Ptr &factory) {
    ASSERT_not_null(factory);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    registry.push_back(factory);
}

bool
Architecture::deregisterFactory(const Ptr &factory) {
    ASSERT_not_null(factory);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto iter = registry.rbegin(); iter != registry.rend(); ++iter) {
        if (*iter == factory) {
            registry.erase(std::next(iter).base());
            return true;
        }
    }
    return false;
}

std::vector<Architecture::Ptr>
Architecture::registeredFactories() {
    initRegistry();
    std::vector<Ptr> retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    retval.reserve(registry.size());
    for (const Ptr &factory: registry)
        retval.push_back(factory);
    return retval;
}

Architecture::Ptr
Architecture::forge(const Database::Ptr &db, TestCaseId tcid, const Yaml::Node &config) {
    ASSERT_not_null(db);
    ASSERT_require(tcid);
    initRegistry();
    SAWYER_THREAD_TRAITS::LockGuard lock(registryMutex);
    for (auto factory = registry.rbegin(); factory != registry.rend(); ++factory) {
        if ((*factory)->matchFactory(config))
            return (*factory)->instanceFromFactory(db, tcid, config);
    }
    return {};
}

Architecture::Ptr
Architecture::forge(const Database::Ptr &db, const TestCase::Ptr &tc, const Yaml::Node &config) {
    ASSERT_not_null(db);
    ASSERT_not_null(tc);
    return forge(db, db->id(tc), config);
}

bool
Architecture::isFactory() const {
    return !db_;
}

void
Architecture::configureSharedMemory(const Yaml::Node &config) {
    if (auto list = config["memory-drivers"]) {
        if (!list.isSequence())
            throw Exception("configuration \"memory-drivers\" must be a YAML sequence");

        for (size_t i = 0; i < list.size(); ++i) {
            const std::string err = "configuration memory-drivers[" + boost::lexical_cast<std::string>(i) + "]";
            if (!list[i].isMap())
                throw Exception(err + " must be a YAML map");
            const std::string driverName = list[i]["driver"].as<std::string>();
            if (driverName.empty())
                throw Exception(err + "[driver] is not specified or is not valid");

            const auto va = StringUtility::toNumber<Address>(list[i]["address"].as<std::string>()).ok();
            if (!va)
                throw Exception(err + "[address] is not specified or is not valid");

            const auto nBytes = StringUtility::toNumber<size_t>(list[i]["size"].as<std::string>()).ok();
            if (!nBytes || *nBytes == 0)
                throw Exception(err + "[size] is not specified or is not valid");

            const AddressInterval where = AddressInterval::baseSize(*va, *nBytes);
            auto driver = SharedMemoryCallback::forge(where, list[i]);
            if (!driver)
                throw Exception(err + " driver \"" + StringUtility::cEscape(driverName) + "\" not found");

            const std::string message = list[i]["message"].as<std::string>();
            if (!message.empty())
                driver->name(message);

            sharedMemory(driver);
        }
    }
}

const std::string&
Architecture::name() const {
    return name_;
}

void
Architecture::name(const std::string &s) {
    name_ = s;
}

Database::Ptr
Architecture::database() const {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(db_);
    return db_;
}

TestCaseId
Architecture::testCaseId() const {
    ASSERT_forbid(isFactory());
    ASSERT_require(testCaseId_);
    return testCaseId_;
}

TestCase::Ptr
Architecture::testCase() const {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(testCase_);
    return testCase_;
}

P2::PartitionerConstPtr
Architecture::partitioner() const {
    ASSERT_forbid(isFactory());
    return partitioner_;
}

void
Architecture::partitioner(const P2::Partitioner::ConstPtr &p) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(p);
    partitioner_ = p;
}

Debugger::Base::Ptr
Architecture::debugger() const {
    return debugger_;
}

void
Architecture::debugger(const Debugger::Base::Ptr &d) {
    debugger_ = d;
}

Sawyer::Optional<Address>
Architecture::scratchVa() const {
    return scratchVa_;
}

void
Architecture::scratchVa(const Sawyer::Optional<Address> &va) {
    scratchVa_ = va;
}

ExecutionLocation
Architecture::currentLocation() const {
    ASSERT_forbid(isFactory());
    return currentLocation_;
}

void
Architecture::currentLocation(const ExecutionLocation &loc) {
    ASSERT_forbid(isFactory());
    currentLocation_ = loc;
}

InputVariables::Ptr
Architecture::inputVariables() const {
    return inputVariables_;
}

void
Architecture::inputVariables(const InputVariables::Ptr &iv) {
    inputVariables_ = iv;
}

bool
Architecture::isTerminated() {
    ASSERT_forbid(isFactory());
    return !debugger() || debugger()->isTerminated();
}

Address
Architecture::ip() {
    ASSERT_forbid(isFactory());
    return debugger()->executionAddress(Debugger::ThreadId::unspecified());
}

void
Architecture::ip(Address va) {
    ASSERT_forbid(isFactory());
    debugger()->executionAddress(Debugger::ThreadId::unspecified(), va);
}

std::vector<ExecutionEvent::Ptr>
Architecture::createRegisterRestoreEvents() {
    ASSERT_forbid(isFactory());
    SAWYER_MESG(mlog[DEBUG]) <<"saving all registers\n";
    Sawyer::Container::BitVector allRegisters = debugger()->readAllRegisters(Debugger::ThreadId::unspecified());
    auto event = ExecutionEvent::bulkRegisterWrite(TestCase::Ptr(), ExecutionLocation(), ip(), allRegisters);
    return {event};
}

size_t
Architecture::writeMemory(Address va, const std::vector<uint8_t> &bytes) {
    ASSERT_forbid(isFactory());
    return debugger()->writeMemory(va, bytes.size(), bytes.data());
}

std::vector<uint8_t>
Architecture::readMemory(Address va, size_t nBytes) {
    ASSERT_forbid(isFactory());
    return debugger()->readMemory(va, nBytes);
}

void
Architecture::writeRegister(RegisterDescriptor reg, uint64_t value) {
    ASSERT_forbid(isFactory());
    debugger()->writeRegister(Debugger::ThreadId::unspecified(), reg, value);
}

void
Architecture::writeRegister(RegisterDescriptor reg, const Sawyer::Container::BitVector &bv) {
    ASSERT_forbid(isFactory());
    debugger()->writeRegister(Debugger::ThreadId::unspecified(), reg, bv);
}

Sawyer::Container::BitVector
Architecture::readRegister(RegisterDescriptor reg) {
    ASSERT_forbid(isFactory());
    return debugger()->readRegister(Debugger::ThreadId::unspecified(), reg);
}

void
Architecture::executeInstruction(const P2::PartitionerConstPtr &partitioner) {
    ASSERT_forbid(isFactory());
    if (mlog[DEBUG]) {
        Address va = debugger()->executionAddress(Debugger::ThreadId::unspecified());
        if (SgAsmInstruction *insn = partitioner->instructionProvider()[va]) {
            mlog[DEBUG] <<"concretely executing insn #" <<currentLocation().primary()
                        <<" " <<partitioner->unparse(insn) <<"\n";
        } else {
            mlog[DEBUG] <<"concretely executing insn #" <<currentLocation().primary()
                        <<" " <<StringUtility::addrToString(va) <<": null instruction\n";
        }
    }

    debugger()->singleStep(Debugger::ThreadId::unspecified());
}

void
Architecture::checkInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    std::vector<uint8_t> buf = debugger()->readMemory(insn->get_address(), insn->get_size());
    if (buf.size() != insn->get_size() || !std::equal(buf.begin(), buf.end(), insn->get_rawBytes().begin())) {
        if (mlog[ERROR]) {
            mlog[ERROR] <<"symbolic instruction doesn't match concrete instruction at "
                        <<StringUtility::addrToString(insn->get_address()) <<"\n"
                        <<"  symbolic insn:  " <<insn->toString() <<"\n"
                        <<"  symbolic bytes:";
            for (uint8_t byte: insn->get_rawBytes())
                mlog[ERROR] <<(boost::format(" %02x") % (unsigned)byte);
            mlog[ERROR] <<"\n"
                        <<"  concrete bytes:";
            for (uint8_t byte: buf)
                mlog[ERROR] <<(boost::format(" %02x") % (unsigned)byte);
            mlog[ERROR] <<"\n";
        }
        throw Exception("symbolic instruction doesn't match concrete instructon at " +
                        StringUtility::addrToString(insn->get_address()));
    }
}

void
Architecture::advanceExecution(const BS::RiscOperators::Ptr&) {
    debugger()->singleStep(Debugger::ThreadId::unspecified());
}

void
Architecture::executeInstruction(const BS::RiscOperators::Ptr &ops, SgAsmInstruction *insn) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(ops);
    ASSERT_not_null(insn);
    Address va = insn->get_address();

    checkInstruction(insn);
    debugger()->executionAddress(Debugger::ThreadId::unspecified(), va);
    advanceExecution(ops);
}

std::string
Architecture::readCString(Address va, size_t maxBytes) {
    ASSERT_forbid(isFactory());
    return debugger()->readCString(va, maxBytes);
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
Architecture::sharedMemory(const SharedMemoryCallback::Ptr &callback) {
    ASSERT_not_null(callback);
    sharedMemory(callback, callback->registrationVas());
}

void
Architecture::sharedMemory(const SharedMemoryCallback::Ptr &callback, const AddressInterval &where) {
    ASSERT_forbid(where.isEmpty());
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
    ASSERT_forbid(isFactory());
    for (const ExecutionEvent::Ptr &event: events) {
        ASSERT_not_null(event);
        event->testCase(testCase_);
        event->location(nextEventLocation(when));
        database()->save(event);
    }
}

size_t
Architecture::playAllEvents(const P2::PartitionerConstPtr &partitioner) {
    ASSERT_forbid(isFactory());
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
    ASSERT_forbid(isFactory());
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
    ASSERT_forbid(isFactory());
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
            if (debug) {
                debug <<"  map " <<where.size() <<" bytes at " <<StringUtility::addrToString(where) <<", prot=";
                if ((event->permissions() & MemoryMap::READABLE) != 0)
                    debug <<"r";
                if ((event->permissions() & MemoryMap::WRITABLE) != 0)
                    debug <<"w";
                if ((event->permissions() & MemoryMap::EXECUTABLE) != 0)
                    debug <<"x";
                if ((event->permissions() & MemoryMap::IMMUTABLE) != 0)
                    debug <<"i";
                if ((event->permissions() & MemoryMap::PRIVATE) != 0)
                    debug <<"p";
                const unsigned otherMask = ~(MemoryMap::READ_WRITE_EXECUTE | MemoryMap::IMMUTABLE | MemoryMap::PRIVATE);
                if (const unsigned other = event->permissions() & otherMask)
                    debug <<(boost::format("+0x%08x") % (event->permissions() & otherMask));
                debug <<"\n";
            }
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
                               <<reg <<" = " <<toString(concreteValue, reg.nBits()) <<"\n";
            writeRegister(reg, concreteValue);
            return true;
        }

        case ExecutionEvent::Action::BULK_REGISTER_WRITE: {
            SAWYER_MESG(mlog[DEBUG]) <<"  restore registers\n";
            Sawyer::Container::BitVector allRegisters = event->registerValues();
            debugger()->writeAllRegisters(Debugger::ThreadId::unspecified(), allRegisters);
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
Architecture::runToEvent(const ExecutionEvent::Ptr &event, const P2::PartitionerConstPtr &partitioner) {
    ASSERT_forbid(isFactory());
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
Architecture::readMemoryUnsigned(Address va, size_t nBytes) {
    ASSERT_forbid(isFactory());
    ASSERT_require(nBytes >= 1 && nBytes <= 8);
    std::vector<uint8_t> bytes = readMemory(va, nBytes);
    ASSERT_require2(bytes.size() == nBytes,
                    "va=" + StringUtility::addrToString(va) +
                    ", requested=" + boost::lexical_cast<std::string>(nBytes) +
                    ", read_size=" + boost::lexical_cast<std::string>(bytes.size()));
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

void
Architecture::mapMemory(const AddressInterval&, unsigned /*permissions*/) {}

void
Architecture::unmapMemory(const AddressInterval&) {}

const ExecutionLocation&
Architecture::nextInstructionLocation() {
    ASSERT_forbid(isFactory());
    currentLocation_ = currentLocation_.nextPrimary();
    return currentLocation_;
}

const ExecutionLocation&
Architecture::nextEventLocation(When when) {
    ASSERT_forbid(isFactory());
    currentLocation_ = currentLocation_.nextSecondary(when);
    return currentLocation_;
}

std::vector<ExecutionEvent::Ptr>
Architecture::createMemoryRestoreEvents(const MemoryMap::Ptr &map) {
    ASSERT_not_null(map);
    std::vector<ExecutionEvent::Ptr> events;
    for (const MemoryMap::Node &node: map->nodes()) {
        const AddressInterval &where = node.key();
        const MemoryMap::Segment &segment = node.value();

        // Don't save our own scratch space
        if (scratchVa() && where.least() == *scratchVa())
            continue;

        SAWYER_MESG(mlog[DEBUG]) <<"  memory at " <<StringUtility::addrToString(where)
                                 <<", " <<StringUtility::plural(where.size(), "bytes");
        if (mlog[DEBUG]) {
            std::string protStr;
            if ((segment.accessibility() & MemoryMap::READABLE) != 0)
                protStr += "r";
            if ((segment.accessibility() & MemoryMap::WRITABLE) != 0)
                protStr += "w";
            if ((segment.accessibility() & MemoryMap::EXECUTABLE) != 0)
                protStr += "x";
            SAWYER_MESG(mlog[DEBUG]) <<", perm=" <<(protStr.empty() ? "none" : protStr) <<"\n";
        }

        auto eeMap = ExecutionEvent::bulkMemoryMap(TestCase::Ptr(), ExecutionLocation(), ip(), where,
                                                   segment.accessibility());
        eeMap->name("map " + segment.name());
        events.push_back(eeMap);

        std::vector<uint8_t> buf(where.size());
        size_t nRead = map->at(where).read(buf).size();
        ASSERT_always_require(nRead == where.size());
        auto eeWrite = ExecutionEvent::bulkMemoryWrite(TestCase::Ptr(), ExecutionLocation(), ip(), where, buf);
        eeWrite->name("init " + segment.name());
        events.push_back(eeWrite);
    }
    return events;
}

void
Architecture::restoreInputVariables(const Partitioner2::PartitionerConstPtr&, const Emulation::RiscOperators::Ptr&,
                                    const SmtSolver::Ptr&) {
    ASSERT_forbid(isFactory());
    for (ExecutionEventId eventId: database()->executionEvents(testCaseId())) {
        ExecutionEvent::Ptr event = database()->object(eventId, Update::NO);
        inputVariables_->playback(event);
    }
}

std::pair<ExecutionEvent::Ptr, SymbolicExpression::Ptr>
Architecture::sharedMemoryRead(const SharedMemoryCallbacks &callbacks, const P2::Partitioner::ConstPtr&,
                               const Emulation::RiscOperators::Ptr &ops, Address addr, size_t nBytes) {
    // A shared memory read has just been encountered, and we're in the middle of executing the instruction that caused it.
    ASSERT_forbid(isFactory());
    ASSERT_not_null(ops);
    ASSERT_not_null2(ops->currentInstruction(), "must be called during instruction execution");
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    const Address ip = ops->currentInstruction()->get_address();
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

bool
Architecture::sharedMemoryWrite(const SharedMemoryCallbacks &callbacks, const P2::Partitioner::ConstPtr&,
                                const Emulation::RiscOperators::Ptr &ops, Address addr, const BS::SValue::Ptr &value) {
    // A shared memory write has just been encountered, and we're in the middle of executing the instruction that caused it.
    ASSERT_forbid(isFactory());
    ASSERT_not_null(ops);
    ASSERT_not_null2(ops->currentInstruction(), "must be called during instruction execution");
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    const Address ip = ops->currentInstruction()->get_address();
    SAWYER_MESG(debug) <<"  shared memory write at instruction " <<StringUtility::addrToString(ip)
                       <<" to memory address " <<StringUtility::addrToString(addr)
                       <<" writing " <<*value <<"\n";

    // Invoke the callback
    SharedMemoryContext ctx(sharedFromThis(), ops, ops->currentInstruction()->get_address(), addr,
                            IS::SymbolicSemantics::SValue::promote(value)->get_expression());
    const bool handled = callbacks.apply(false, ctx);
    return handled;
}


void
Architecture::runSharedMemoryPostCallbacks(const ExecutionEvent::Ptr &sharedMemoryEvent, const Emulation::RiscOperators::Ptr &ops) {
    ASSERT_forbid(isFactory());
    ASSERT_not_null(sharedMemoryEvent);
    ASSERT_not_null(ops);

    Address memoryVa = sharedMemoryEvent->memoryLocation().least();
    SharedMemoryCallbacks callbacks = sharedMemory().getOrDefault(memoryVa);
    SharedMemoryContext ctx(sharedFromThis(), ops, sharedMemoryEvent);
    ctx.phase = ConcolicPhase::POST_EMULATION;
    callbacks.apply(false, ctx);
}

void
Architecture::fixupSharedMemoryEvents(const ExecutionEvent::Ptr &sharedMemoryEvent, const Emulation::RiscOperators::Ptr &ops) {
    ASSERT_forbid(isFactory());
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
                               <<"      writing " <<*registerValue <<" to register " <<relatedEvent->registerDescriptor() <<"\n";
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
    ASSERT_forbid(isFactory());
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
Architecture::sharedMemoryAccessPost(const P2::PartitionerConstPtr&, const Emulation::RiscOperators::Ptr &ops) {
    // Called after a shared memory accessing instruction has completed.
    ASSERT_forbid(isFactory());
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
