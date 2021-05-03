#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/LinuxI386Executor.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Debugger.h>

#include <sys/mman.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

static const unsigned i386_NR_mmap = 90;
static const unsigned i386_NR_munmap = 91;

LinuxI386Executor::LinuxI386Executor(const Database::Ptr &db, TestCaseId tcid, const TestCase::Ptr &tc)
    : db_(db), testCaseId_(tcid), testCase_(tc) {
    eventKeyFrames_ = db_->executionEventKeyFrames(tcid);
    std::reverse(eventKeyFrames_.begin(), eventKeyFrames_.end());
}

LinuxI386Executor::~LinuxI386Executor() {}

LinuxI386Executor::Ptr
LinuxI386Executor::instance(const Database::Ptr &db, TestCaseId tcid) {
    ASSERT_not_null(db);
    ASSERT_require(tcid);
    TestCase::Ptr tc = db->object(tcid);
    return Ptr(new LinuxI386Executor(db, tcid, tc));
}

LinuxI386Executor::Ptr
LinuxI386Executor::instance(const Database::Ptr &db, const TestCase::Ptr &tc) {
    ASSERT_not_null(db);
    ASSERT_not_null(tc);
    TestCaseId tcid = db->id(tc);
    return Ptr(new LinuxI386Executor(db, tcid, tc));
}

void
LinuxI386Executor::mapScratchPage() {
    ASSERT_require(debugger_->isAttached());

    // Create the scratch page
    int64_t status = debugger_->remoteSystemCall(i386_NR_mmap, 0, 4096,
                                                 PROT_EXEC | PROT_READ | PROT_WRITE,
                                                 MAP_ANONYMOUS | MAP_PRIVATE,
                                                 -1, 0);
    if (status < 0) {
        mlog[ERROR] <<"mmap system call failed for scratch page\n";
    } else {
        scratchVa_ = (uint64_t)status;
    }

    // Write an "INT 0x80" instruction to the beginning of the page.
    static const uint8_t int80[] = {0xcd, 0x80};
    size_t nWritten = debugger_->writeMemory(scratchVa_, 2, int80);
    if (nWritten != 2)
        mlog[ERROR] <<"cannot write INT 0x80 instruction to scratch page\n";
}

void
LinuxI386Executor::load(const boost::filesystem::path &targetDir) {
    // Extract the executable into the working directory.
    auto exeName = boost::filesystem::path(testCase_->specimen()->name()).filename();
    if (exeName.empty())
        exeName = "a.out";
    exeName = targetDir / exeName;
    {
        std::ofstream executable(exeName.string().c_str(), std::ios_base::binary | std::ios_base::trunc);
        if (!executable) {
            mlog[ERROR] <<"cannot write to " <<exeName <<"\n";
        } else if (testCase_->specimen()->content().empty()) {
            mlog[ERROR] <<"specimen content is empty\n";
        } else {
            executable.write(reinterpret_cast<const char*>(&testCase_->specimen()->content()[0]),
                             testCase_->specimen()->content().size());
        }
    }
    boost::filesystem::permissions(exeName, boost::filesystem::owner_all);

    // Describe the process to be created from the executable.
    Debugger::Specimen ds = exeName;
    ds.arguments(testCase_->args());
    ds.workingDirectory(targetDir);
    ds.randomizedAddresses(false);
    ds.flags()
        .set(Debugger::REDIRECT_INPUT)
        .set(Debugger::REDIRECT_OUTPUT)
        .set(Debugger::REDIRECT_ERROR)
        .set(Debugger::CLOSE_FILES);

    // Create the process
    debugger_ = Debugger::instance(ds);
}

void
LinuxI386Executor::runUnhinderedTo(rose_addr_t stopVa) {
    ASSERT_require(debugger_->isAttached());
    SAWYER_MESG(mlog[DEBUG]) <<"running unhindered to " <<StringUtility::addrToString(stopVa) <<"\n";
    debugger_->setBreakpoint(stopVa);
    debugger_->runToBreakpoint();
    if (!debugger_->isAttached()) {
        SAWYER_MESG(mlog[ERROR]) <<"subordinate " <<debugger_->howTerminated() <<"\n";
    } else {
        ASSERT_require(ip() == stopVa);
        debugger_->clearBreakpoint(stopVa);
        SAWYER_MESG(mlog[DEBUG]) <<"reached " <<StringUtility::addrToString(ip()) <<"\n";
    }
    incrementLocation();
}

void
LinuxI386Executor::incrementLocation() {
    ++curLocation_.primary;
    curLocation_.secondary = 0;
}

ExecutionEvent::Location
LinuxI386Executor::newLocation() {
    ++curLocation_.secondary;
    return curLocation_;
}

rose_addr_t
LinuxI386Executor::ip() const {
    return debugger_->executionAddress();
}

void
LinuxI386Executor::saveMemory() {
    auto map = MemoryMap::instance();
    SAWYER_MESG(mlog[DEBUG]) <<"saving subordinate memory\n";
    map->insertProcess(debugger_->isAttached(), MemoryMap::Attach::NO);
    for (const MemoryMap::Node &node: map->nodes()) {
        if (node.key().least() != scratchVa_) {
            SAWYER_MESG(mlog[DEBUG]) <<"  memory at " <<StringUtility::addrToString(node.key())
                                     <<", " <<StringUtility::plural(node.key().size(), "bytes");
            std::string protStr;
            if ((node.value().accessibility() & MemoryMap::READABLE) != 0)
                protStr += "r";
            if ((node.value().accessibility() & MemoryMap::WRITABLE) != 0)
                protStr += "w";
            if ((node.value().accessibility() & MemoryMap::EXECUTABLE) != 0)
                protStr += "x";
            SAWYER_MESG(mlog[DEBUG]) <<", perm=" <<(protStr.empty() ? "none" : protStr) <<"\n";
            auto eeMap = ExecutionEvent::instanceMapMemory(testCase_, newLocation(), ip(), node.key(), protStr);
            db_->save(eeMap);

            std::vector<uint8_t> buf(node.key().size());
            size_t nRead = map->at(node.key()).read(buf).size();
            ASSERT_always_require(nRead == node.key().size());
            auto eeWrite = ExecutionEvent::instanceWriteMemory(testCase_, newLocation(), ip(), node.key(), buf);
            db_->save(eeWrite);
        }
    }
}

static void
hashMemoryRegion(Combinatorics::Hasher &hasher, const MemoryMap::Ptr &map, AddressInterval where) {
    while (!where.isEmpty()) {
        uint8_t buffer[4096];
        size_t nToRead = std::min(where.size(), sizeof buffer);
        AddressInterval regionRead = map->at(where.least()).limit(nToRead).read(buffer);
        ASSERT_require(!regionRead.isEmpty() && regionRead.size() == nToRead);
        hasher.append(buffer, nToRead);
        if (regionRead.greatest() == where.greatest())
            break;                              // prevents overflow in next statement
        where = AddressInterval::hull(regionRead.greatest() + 1, where.greatest());
    }
}

void
LinuxI386Executor::hashMemory() {
    auto map = MemoryMap::instance();
    SAWYER_MESG(mlog[DEBUG]) <<"hashing subordinate memory\n";
    map->insertProcess(debugger_->isAttached(), MemoryMap::Attach::NO);
    for (const MemoryMap::Node &node: map->nodes()) {
        if (node.key().least() != scratchVa_) {
            Combinatorics::HasherSha256Builtin hasher;
            hashMemoryRegion(hasher, map, node.key());
            SAWYER_MESG(mlog[DEBUG]) <<"  memory at " <<StringUtility::addrToString(node.key())
                                     <<", " <<StringUtility::plural(node.key().size(), "bytes")
                                     <<", hash = " <<hasher.toString() <<"\n";
            auto eeHash = ExecutionEvent::instanceHashMemory(testCase_, newLocation(), ip(), node.key(), hasher.digest());
            db_->save(eeHash);
        }
    }
}

void
LinuxI386Executor::unmapMemory() {
    SAWYER_MESG(mlog[DEBUG]) <<"unmapping memory\n";
    std::vector<MemoryMap::ProcessMapRecord> segments = MemoryMap::readProcessMap(debugger_->isAttached());
    for (const MemoryMap::ProcessMapRecord &segment: segments) {
        if (segment.interval.least() != scratchVa_ && "[vvar]" != segment.comment) {
            SAWYER_MESG(mlog[DEBUG]) <<"  at " <<StringUtility::addrToString(segment.interval) <<": " <<segment.comment <<"\n";
            int64_t status = debugger_->remoteSystemCall(i386_NR_munmap, segment.interval.least(), segment.interval.size());
            if (status < 0)
                mlog[ERROR] <<"unamp memory failed at " <<StringUtility::addrToString(segment.interval)
                            <<" for " <<segment.comment <<"\n";
        }
    }
}

void
LinuxI386Executor::saveRegisters() {
    SAWYER_MESG(mlog[DEBUG]) <<"saving all registers\n";
    Debugger::AllRegisters allRegisters = debugger_->readAllRegisters();
    auto ee = ExecutionEvent::instanceRestoreRegisters(testCase_, newLocation(), ip(), allRegisters);
    db_->save(ee);
}

void
LinuxI386Executor::processAllEvents() {
    while (curLocation_.primary > eventKeyFrames_.back()) {
        mlog[ERROR] <<"passed execution events with key=" <<eventKeyFrames_.back() <<" without processing them\n";
        eventKeyFrames_.pop_back();
    }

    while (!eventKeyFrames_.empty()) {
        while (curLocation_.primary < eventKeyFrames_.back()) {
            debugger_->singleStep();
            incrementLocation();
        }
        ASSERT_require(curLocation_.primary == eventKeyFrames_.back());
        processEvents();
        eventKeyFrames_.pop_back();
    }
}

void
LinuxI386Executor::processEvents() {
    std::vector<ExecutionEventId> eventIds = db_->executionEvents(testCaseId_, curLocation_.primary);
    for (ExecutionEventId eventId: eventIds) {
        ExecutionEvent::Ptr event = db_->object(eventId);
        SAWYER_MESG(mlog[DEBUG]) <<"processing execution event " <<*eventId
                                 <<" at " <<event->location().primary <<":" <<event->location().secondary
                                 <<" ip=" <<StringUtility::addrToString(event->instructionPointer()) <<"\n";
        switch (event->actionType()) {
            case ExecutionEvent::Action::NONE:
                SAWYER_MESG(mlog[DEBUG]) <<"  no action necessary\n";
                break;

            case ExecutionEvent::Action::MAP_MEMORY: {
                AddressInterval where = event->memoryLocation();
                ASSERT_forbid(where.isEmpty());
                SAWYER_MESG(mlog[DEBUG]) <<"  map memory at " <<StringUtility::addrToString(where) <<"\n";
                unsigned prot = 0;
                for (char letter: event->bytes()) {
                    switch (letter) {
                        case 'r':
                            prot |= PROT_READ;
                            break;
                        case 'w':
                            prot |= PROT_WRITE;
                            break;
                        case 'x':
                            prot |= PROT_EXEC;
                            break;
                        default:
                            mlog[ERROR] <<"MAP_MEMORY event invalid protection letter\n";
                            break;
                    }
                }
                int64_t status = debugger_->remoteSystemCall(i386_NR_mmap, where.least(), where.size(), prot,
                                                             MAP_ANONYMOUS | MAP_FIXED, -1, 0);
                if (status < 0)
                    mlog[ERROR] <<"MAP_MEMORY event failed to map memory\n";
                break;
            }

            case ExecutionEvent::Action::UNMAP_MEMORY: {
                AddressInterval where = event->memoryLocation();
                ASSERT_forbid(where.isEmpty());
                SAWYER_MESG(mlog[DEBUG]) <<"  unmap memory at " <<StringUtility::addrToString(where) <<"\n";
                int64_t status = debugger_->remoteSystemCall(i386_NR_munmap, where.least(), where.size());
                if (status < 0)
                    mlog[ERROR] <<"UNMAP_MEMORY event failed to unmap memory\n";
                break;
            }

            case ExecutionEvent::Action::WRITE_MEMORY: {
                AddressInterval where = event->memoryLocation();
                SAWYER_MESG(mlog[DEBUG]) <<"  write memory at " <<StringUtility::addrToString(where) <<"\n";
                ASSERT_forbid(where.isEmpty());
                ASSERT_require(where.size() == event->bytes().size());
                size_t nWritten = debugger_->writeMemory(where.least(), where.size(), event->bytes().data());
                if (nWritten != where.size())
                    mlog[ERROR] <<"WRITE_MEMORY event failed to write to memory\n";
                break;
            }

            case ExecutionEvent::Action::HASH_MEMORY: {
                AddressInterval where = event->memoryLocation();
                SAWYER_MESG(mlog[DEBUG]) <<"  hash memory at " <<StringUtility::addrToString(where) <<"\n";
                ASSERT_forbid(where.isEmpty());
                auto map = MemoryMap::instance();
                map->insertProcess(debugger_->isAttached(), MemoryMap::Attach::NO);
                Combinatorics::HasherSha256Builtin hasher;
                hashMemoryRegion(hasher, map, where);
                Combinatorics::Hasher::Digest currentDigest = hasher.digest();
                const Combinatorics::Hasher::Digest &savedDigest = event->bytes();
                ASSERT_require(currentDigest.size() == savedDigest.size());
                if (!std::equal(currentDigest.begin(), currentDigest.end(), savedDigest.begin()))
                    mlog[ERROR] <<"memory hash comparison failed at " <<StringUtility::addrToString(where) <<"\n";
                break;
            }

            case ExecutionEvent::Action::RESTORE_REGISTERS: {
                SAWYER_MESG(mlog[DEBUG]) <<"  restore registers\n";
                Debugger::AllRegisters allRegisters = event->allRegisters();
                debugger_->writeAllRegisters(allRegisters);
                break;
            }
        }
    }
}

} // namespace
} // namespace
} // namespace

#endif
