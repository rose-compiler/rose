#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/LinuxI386.h>

#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/Debugger.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/StringUtility.h>

#include <boost/format.hpp>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics2;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

static const unsigned i386_NR_mmap   = 192;             //  | 0x40000000; // __X32_SYSCALL_BIT
static const unsigned i386_NR_munmap = 91;              //  | 0x40000000; // __X32_SYSCALL_BIT

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

LinuxI386::LinuxI386(const Database::Ptr &db, TestCaseId tcid)
    : Architecture(db, tcid) {}

LinuxI386::~LinuxI386() {}

LinuxI386::Ptr
LinuxI386::instance(const Database::Ptr &db, TestCaseId tcid) {
    ASSERT_not_null(db);
    ASSERT_require(tcid);
    auto retval = Ptr(new LinuxI386(db, tcid));
    retval->configureSystemCalls();
    return retval;
}

LinuxI386::Ptr
LinuxI386::instance(const Database::Ptr &db, const TestCase::Ptr &tc) {
    ASSERT_not_null(db);
    ASSERT_not_null(tc);
    TestCaseId tcid = db->id(tc);
    ASSERT_require(tcid);
    auto retval = Ptr(new LinuxI386(db, tcid));
    retval->configureSystemCalls();
    return retval;
}

void
LinuxI386::load(const boost::filesystem::path &targetDir) {
    // Extract the executable into the working directory.
    auto exeName = boost::filesystem::path(testCase()->specimen()->name()).filename();
    if (exeName.empty())
        exeName = "a.out";
    exeName = targetDir / exeName;
    {
        std::ofstream executable(exeName.string().c_str(), std::ios_base::binary | std::ios_base::trunc);
        if (!executable) {
            mlog[ERROR] <<"cannot write to " <<exeName <<"\n";
        } else if (testCase()->specimen()->content().empty()) {
            mlog[ERROR] <<"specimen content is empty\n";
        } else {
            executable.write(reinterpret_cast<const char*>(&testCase()->specimen()->content()[0]),
                             testCase()->specimen()->content().size());
        }
    }
    boost::filesystem::permissions(exeName, boost::filesystem::owner_all);

    // Describe the process to be created from the executable. The TestCase arguments include argv[0], but we don't have any
    // control over that in general, so we discard it. Other executors might have more control, which is why the program name
    // is supplied in the first place.
    std::vector<std::string> args = testCase()->args();
    ASSERT_forbid(args.empty());
    args.erase(args.begin());
    Debugger::Specimen ds = exeName;
    ds.arguments(args);
    ds.workingDirectory(targetDir);
    ds.randomizedAddresses(false);
    ds.flags()
        .set(Debugger::REDIRECT_INPUT)
        .set(Debugger::REDIRECT_OUTPUT)
        .set(Debugger::REDIRECT_ERROR)
        .set(Debugger::CLOSE_FILES);

    // Create the process
    debugger_ = Debugger::instance(ds);
    SAWYER_MESG(mlog[DEBUG]) <<"loaded pid=" <<debugger_->isAttached() <<" " <<exeName <<"\n";
    mapScratchPage();
}

bool
LinuxI386::isTerminated() {
    return !debugger_ || debugger_->isTerminated();
}

std::string
LinuxI386::readCString(rose_addr_t va, size_t maxBytes) {
    return debugger_->readCString(va, maxBytes);
}

ByteOrder::Endianness
LinuxI386::memoryByteOrder() {
    return ByteOrder::ORDER_LSB;
}

rose_addr_t
LinuxI386::ip() {
    return debugger_->executionAddress();
}

void
LinuxI386::ip(rose_addr_t va) {
    debugger_->executionAddress(va);
}

std::vector<ExecutionEvent::Ptr>
LinuxI386::createMemoryRestoreEvents() {
    SAWYER_MESG(mlog[DEBUG]) <<"saving subordinate memory\n";
    std::vector<ExecutionEvent::Ptr> events;
    auto map = MemoryMap::instance();
    std::vector<MemoryMap::ProcessMapRecord> segments = disposableMemory();
    map->insertProcessPid(debugger_->isAttached(), segments);

    for (const MemoryMap::Node &node: map->nodes()) {
        const AddressInterval &where = node.key();
        const MemoryMap::Segment &segment = node.value();

        if (where.least() != scratchVa_) {
            SAWYER_MESG(mlog[DEBUG]) <<"  memory at " <<StringUtility::addrToString(where)
                                     <<", " <<StringUtility::plural(where.size(), "bytes");
            std::string protStr;
            if ((segment.accessibility() & MemoryMap::READABLE) != 0)
                protStr += "r";
            if ((segment.accessibility() & MemoryMap::WRITABLE) != 0)
                protStr += "w";
            if ((segment.accessibility() & MemoryMap::EXECUTABLE) != 0)
                protStr += "x";
            SAWYER_MESG(mlog[DEBUG]) <<", perm=" <<(protStr.empty() ? "none" : protStr) <<"\n";
            auto eeMap = ExecutionEvent::instanceMapMemory(ip(), where, protStr);
            eeMap->name("map " + segment.name());
            events.push_back(eeMap);

            std::vector<uint8_t> buf(where.size());
            size_t nRead = map->at(where).read(buf).size();
            ASSERT_always_require(nRead == where.size());
            auto eeWrite = ExecutionEvent::instanceWriteMemory(ip(), where.least(), buf);
            eeWrite->name("init " + segment.name());
            events.push_back(eeWrite);
        }
    }
    return events;
}

std::vector<ExecutionEvent::Ptr>
LinuxI386::createMemoryHashEvents() {
    SAWYER_MESG(mlog[DEBUG]) <<"hashing subordinate memory\n";
    std::vector<ExecutionEvent::Ptr> events;
    auto map = MemoryMap::instance();
    std::vector<MemoryMap::ProcessMapRecord> segments = disposableMemory();
    map->insertProcessPid(debugger_->isAttached(), segments);
    for (const MemoryMap::Node &node: map->nodes()) {
        SAWYER_MESG(mlog[DEBUG]) <<"  memory at " <<StringUtility::addrToString(node.key())
                                 <<StringUtility::plural(node.key().size(), "bytes") <<"\n";
        Combinatorics::HasherSha256Builtin hasher;
        hashMemoryRegion(hasher, map, node.key());
        SAWYER_MESG(mlog[DEBUG]) <<"    hash = " <<hasher.toString() <<"\n";
        auto eeHash = ExecutionEvent::instanceHashMemory(ip(), node.key(), hasher.digest());
        events.push_back(eeHash);
    }
    return events;
}

std::vector<ExecutionEvent::Ptr>
LinuxI386::createRegisterRestoreEvents() {
    SAWYER_MESG(mlog[DEBUG]) <<"saving all registers\n";
    Debugger::AllRegisters allRegisters = debugger_->readAllRegisters();
    auto event = ExecutionEvent::instanceRestoreRegisters(ip(), allRegisters);
    return {event};
}

bool
LinuxI386::playEvent(const ExecutionEvent::Ptr &event) {
    ASSERT_not_null(event);
    bool handled = Super::playEvent(event);

    switch (event->actionType()) {
        case ExecutionEvent::Action::RESTORE_REGISTERS: {
            SAWYER_MESG(mlog[DEBUG]) <<"  restore registers\n";
            Debugger::AllRegisters allRegisters = event->allRegisters();
            debugger_->writeAllRegisters(allRegisters);
            return true;
        }

        case ExecutionEvent::Action::OS_SYSCALL: {
            // System call events adjust the simulated operating system but not the process. If the process needs to be
            // adjusted then the syscall event will be followed by additional events to adjust the memory and registers.  These
            // additional events are fairly generic, so we need to keep track of some state to know they're associated with a
            // prior system call event.
            uint64_t functionNumber = event->scalar();
            playingSyscall_ = std::make_pair(functionNumber, event->location().primary);
            ++syscallSequenceNumbers_[functionNumber];
            playingSyscall_ = {functionNumber, event->location().primary};
            return true;
        }

        case ExecutionEvent::Action::WRITE_REGISTER: {
            // The writing-to-register already happened in Super::playEvent, but if this event represents a system call return
            // value we should save it. This is important for system calls that always return the same value, like getpid.
            ASSERT_require(handled);
            if (playingSyscall_.second == event->location().primary) {
                uint64_t functionNumber = playingSyscall_.first;
                if (SystemCallPtr systemCall = systemCalls().getOrDefault(functionNumber)) {
                    if (systemCall->isConstantReturn()) {
                        if (!systemCall->constantReturnSymbolic()) {
                            SymbolicExpr::Ptr symbolicRetval = event->inputVariable();
                            SAWYER_MESG(mlog[DEBUG]) <<"    saved first symbolic return value: " <<*symbolicRetval <<"\n";
                            systemCall->constantReturnSymbolic(symbolicRetval);
                        }
                        if (!systemCall->constantReturnConcrete()) {
                            uint64_t concreteRetval = event->words()[0];
                            SAWYER_MESG(mlog[DEBUG]) <<"    saved first concrete return value: " <<concreteRetval <<"\n";
                            systemCall->constantReturnConcrete(concreteRetval);
                        }
                    }
                }
            }
            return true;
        }

        default:
            return handled;
    }
}

void
LinuxI386::mapMemory(const AddressInterval &where, unsigned permissions) {
    ASSERT_forbid(where.isEmpty());
    SAWYER_MESG(mlog[DEBUG]) <<"map " <<StringUtility::plural(where.size(), "bytes") <<" ";
    unsigned prot = 0;
    if ((permissions & MemoryMap::READABLE) != 0) {
        SAWYER_MESG(mlog[DEBUG]) <<"r";
        prot |= PROT_READ;
    }
    if ((permissions & MemoryMap::WRITABLE) != 0) {
        SAWYER_MESG(mlog[DEBUG]) <<"w";
        prot |= PROT_WRITE;
    }
    if ((permissions & MemoryMap::EXECUTABLE) != 0) {
        SAWYER_MESG(mlog[DEBUG]) <<"x";
        prot |= PROT_EXEC;
    }
    SAWYER_MESG(mlog[DEBUG]) <<" at " <<StringUtility::addrToString(where) <<", flags=private|anonymous|fixed\n";
    int32_t status = debugger_->remoteSystemCall(i386_NR_mmap, where.least(), where.size(), prot,
                                                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (status < 0 && status > -4096) {
        mlog[ERROR] <<"MAP_MEMORY event failed to map memory (" <<strerror(-status) <<")\n";
    } else {
        ASSERT_require((uint64_t)(uint32_t)status == where.least());
    }
}

void
LinuxI386::unmapMemory(const AddressInterval &where) {
    ASSERT_forbid(where.isEmpty());
    SAWYER_MESG(mlog[DEBUG]) <<"unmap " <<StringUtility::plural(where.size(), "bytes")
                             << " at " <<StringUtility::addrToString(where) <<"\n";
    int64_t status = debugger_->remoteSystemCall(i386_NR_munmap, where.least(), where.size());
    if (status < 0)
        mlog[ERROR] <<"UNMAP_MEMORY event failed to unmap memory\n";
}

size_t
LinuxI386::writeMemory(rose_addr_t va, const std::vector<uint8_t> &bytes) {
    return debugger_->writeMemory(va, bytes.size(), bytes.data());
}

std::vector<uint8_t>
LinuxI386::readMemory(rose_addr_t va, size_t nBytes) {
    return debugger_->readMemory(va, nBytes);
}

void
LinuxI386::writeRegister(RegisterDescriptor reg, uint64_t value) {
    debugger_->writeRegister(reg, value);
}

void
LinuxI386::writeRegister(RegisterDescriptor reg, const Sawyer::Container::BitVector &bv) {
    debugger_->writeRegister(reg, bv);
}

Sawyer::Container::BitVector
LinuxI386::readRegister(RegisterDescriptor reg) {
    return debugger_->readRegister(reg);
}

void
LinuxI386::executeInstruction() {
    debugger_->singleStep();
    incrementPathLength();
}

void
LinuxI386::executeInstruction(const BS::RiscOperatorsPtr &ops_, SgAsmInstruction *insn) {
    auto ops = Emulation::RiscOperators::promote(ops_);
    ASSERT_not_null(ops);
    ASSERT_not_null(insn);
    rose_addr_t va = insn->get_address();

    // Make sure the executable has the same instruction in those bytes.
    std::vector<uint8_t> buf = debugger_->readMemory(va, insn->get_size());
    if (buf.size() != insn->get_size() || !std::equal(buf.begin(), buf.end(), insn->get_raw_bytes().begin())) {
        if (mlog[ERROR]) {
            mlog[ERROR] <<"symbolic instruction doesn't match concrete instruction at " <<StringUtility::addrToString(va) <<"\n"
                        <<"  symbolic insn:  " <<insn->toString() <<"\n"
                        <<"  symbolic bytes:";
            for (uint8_t byte: insn->get_raw_bytes())
                mlog[ERROR] <<(boost::format(" %02x") % (unsigned)byte);
            mlog[ERROR] <<"\n"
                        <<"  concrete bytes:";
            for (uint8_t byte: buf)
                mlog[ERROR] <<(boost::format(" %02x") % (unsigned)byte);
            mlog[ERROR] <<"\n";
        }
        throw Exception("symbolic instruction doesn't match concrete instructon at " + StringUtility::addrToString(va));
    }

    debugger_->executionAddress(va);
    if (ops->hadSystemCall()) {
        debugger_->stepIntoSyscall();
    } else {
        debugger_->singleStep();
    }
    incrementPathLength();
}

void
LinuxI386::mapScratchPage() {
    ASSERT_require(debugger_->isAttached());

    // Create the scratch page
    int64_t status = debugger_->remoteSystemCall(i386_NR_mmap, 0, 4096,
                                                 PROT_EXEC | PROT_READ | PROT_WRITE,
                                                 MAP_ANONYMOUS | MAP_PRIVATE,
                                                 -1, 0);
    if (status < 0 && status > -4096) {
        mlog[ERROR] <<"mmap system call failed for scratch page: " <<strerror(-status) <<"\n";
    } else {
        scratchVa_ = (uint64_t)(uint32_t)status;
        SAWYER_MESG(mlog[DEBUG]) <<"scratch page mapped at " <<StringUtility::addrToString(scratchVa_) <<"\n";
    }

    // Write an "INT 0x80" instruction to the beginning of the page.
    static const uint8_t int80[] = {0xcd, 0x80};
    size_t nWritten = debugger_->writeMemory(scratchVa_, 2, int80);
    if (nWritten != 2)
        mlog[ERROR] <<"cannot write INT 0x80 instruction to scratch page\n";
}

std::vector<MemoryMap::ProcessMapRecord>
LinuxI386::disposableMemory() {
    std::vector<MemoryMap::ProcessMapRecord> segments = MemoryMap::readProcessMap(debugger_->isAttached());
    for (auto segment = segments.begin(); segment != segments.end(); /*void*/) {
        ASSERT_forbid(segment->interval.isEmpty());
        if ("[vvar]" == segment->comment) {
            // Reading and writing to this memory segment doesn't work
            segment = segments.erase(segment);
        } else if ("[vdso]" == segment->comment) {
            // Pointless to read and write this segment -- its contents never changes
            segment = segments.erase(segment);
        } else if (segment->interval.least() == scratchVa_) {
            // This segment is for our own personal use
            segment = segments.erase(segment);
        } else {
            ++segment;
        }
    }
    return segments;
}

void
LinuxI386::unmapAllMemory() {
    SAWYER_MESG(mlog[DEBUG]) <<"unmapping memory\n";
    std::vector<MemoryMap::ProcessMapRecord> segments = disposableMemory();
    for (const MemoryMap::ProcessMapRecord &segment: segments) {
        SAWYER_MESG(mlog[DEBUG]) <<"  at " <<StringUtility::addrToString(segment.interval) <<": " <<segment.comment <<"\n";
        int64_t status = debugger_->remoteSystemCall(i386_NR_munmap, segment.interval.least(), segment.interval.size());
        if (status < 0) {
            mlog[ERROR] <<"unamp memory failed at " <<StringUtility::addrToString(segment.interval)
                        <<" for " <<segment.comment <<"\n";
        }
    }
}

void
LinuxI386::createInputVariables(InputVariables &inputVariables, const P2::Partitioner &partitioner,
                                const BS::RiscOperatorsPtr &ops, const SmtSolver::Ptr &solver) {
    ASSERT_not_null(ops);
    ASSERT_not_null(solver);

    // For Linux ELF x86, the argc and argv values are on the stack, not in registers. Also note that there is no return
    // address on the stack (i.e., the stack pointer is pointing at argc, not a return address). This means all the stack
    // argument offsets are four (or eight) bytes less than usual.
    //
    // The stack looks like this:
    //   +-------------------------------------------------+
    //   | argv[argc] 4-byte zero                          |
    //   +-------------------------------------------------+
    //   | ...                                             |
    //   +-------------------------------------------------+
    //   | argv[1], 4-byte address for start of a C string |
    //   +-------------------------------------------------+
    //   | argv[0], 4-byte address for start of a C string |
    //   +-------------------------------------------------+
    //   | argc value, 4-byte integer, e.g., 2             |  <--- ESP points here
    //   +-------------------------------------------------+
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"creating program arguments\n";
    const RegisterDescriptor SP = partitioner.instructionProvider().stackPointerRegister();
    size_t wordSizeBytes = SP.nBits() / 8;
    IS::SymbolicSemantics::Formatter fmt;
    fmt.expr_formatter.show_comments = SymbolicExpr::Formatter::CMT_AFTER;

    //---------------------------------------------------------------------------------------------------------------------------
    // argc
    //---------------------------------------------------------------------------------------------------------------------------
    ASSERT_require(SP.nBits() == 32);                   // we only handle 32-bit for now
    ASSERT_require(ops->currentState()->memoryState()->get_byteOrder() == memoryByteOrder());
    rose_addr_t argcVa = readRegister(SP).toInteger();
    uint32_t argc = readMemoryUnsigned(argcVa, wordSizeBytes);

    BS::SValuePtr argcSValue = ops->undefined_(SP.nBits());
    SymbolicExpr::Ptr argcSymbolic = IS::SymbolicSemantics::SValue::promote(argcSValue)->get_expression();

    auto argcEvent = ExecutionEvent::instanceWriteMemory(testCase(), nextLocation(), ip(), argcVa, argc);
    inputVariables.insertProgramArgumentCount(argcEvent, argcSymbolic);
    argcSValue->comment(argcEvent->name());
    ops->writeMemory(RegisterDescriptor(), ops->number_(SP.nBits(), argcVa), argcSValue, ops->boolean_(true));
    ExecutionEventId argcEventId = database()->id(argcEvent);

    SAWYER_MESG(debug) <<"  argc @" <<StringUtility::addrToString(argcVa) <<" = " <<argc
                       <<"; symbolic = " <<(*argcSValue + fmt)
                       <<"; event = " <<*argcEventId <<"\n";

    // The argc value cannot be less than 1 since it always points to at least the program name.
    SymbolicExpr::Ptr argcMinConstraint = SymbolicExpr::makeSignedGt(argcSymbolic,
                                                                     SymbolicExpr::makeIntegerConstant(SP.nBits(), 0));
    solver->insert(argcMinConstraint);

    // The argc value cannot be greater than its current concrete value since making it greater would mean that the address
    // of the environment variable list would need to change, potentially affecting many other things in the program.
    SymbolicExpr::Ptr argcMaxConstraint = SymbolicExpr::makeSignedLe(argcSymbolic,
                                                                     SymbolicExpr::makeIntegerConstant(SP.nBits(), argc));
    solver->insert(argcMaxConstraint);

    //---------------------------------------------------------------------------------------------------------------------------
    // argv
    //---------------------------------------------------------------------------------------------------------------------------

    // The characters argv[0][...] are not inputs because we don't have much control over them.
    rose_addr_t argvVa = argcVa + wordSizeBytes;
    SAWYER_MESG(debug) <<"  argv @" <<StringUtility::addrToString(argvVa) <<" = [\n";
    for (size_t i = 1; i < argc; ++i) {
        rose_addr_t ptrVa = argvVa + i * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = readMemoryUnsigned(ptrVa, wordSizeBytes);
        std::string s = readCString(strVa);
        SAWYER_MESG(debug) <<"    " <<i <<": @" <<StringUtility::addrToString(strVa)
                           <<" \"" <<StringUtility::cEscape(s) <<"\"\n";

        if (markingArgvAsInput_) {
            SymbolicExpr::Ptr anyPreviousCharIsNul;     // is any previous char of this argument an ASCII NUL character?
            for (size_t j = 0; j <= s.size(); ++j) {
                rose_addr_t charVa = strVa + j;
                uint8_t charVal = s[j];

                BS::SValuePtr charSValue = ops->undefined_(8);
                SymbolicExpr::Ptr charSymbolic = IS::SymbolicSemantics::SValue::promote(charSValue)->get_expression();
                auto charEvent = ExecutionEvent::instanceWriteMemory(testCase(), nextLocation(), ip(), charVa, charVal);
                inputVariables.insertProgramArgument(charEvent, i, j, charSymbolic);
                charSValue->comment(charEvent->name());
                ops->writeMemory(RegisterDescriptor(), ops->number_(SP.nBits(), charVa), charSValue, ops->boolean_(true));
                ExecutionEventId charEventId = database()->id(charEvent);

                SAWYER_MESG(debug) <<"      byte " <<j <<" @" <<StringUtility::addrToString(charVa)
                                   <<"; symbolic = " <<(*charSValue + fmt)
                                   <<"; event = " <<*charEventId <<"\n";

                SymbolicExpr::Ptr currentCharIsNul = SymbolicExpr::makeEq(charSymbolic, SymbolicExpr::makeIntegerConstant(8, 0));
                if (s.size() == j) {
                    // Final byte of the argument's buffer must always be NUL
                    solver->insert(currentCharIsNul);

                } else if (j > 0) {
                    // Linux doesn't allow NUL characters to appear inside program arguments. A NUL terminates the argument and
                    // the next argument starts immediately thereafter. For concolic testing, if an argument is shortened (by
                    // making one of it's non-ending bytes NUL) we don't want to have to adjust the addresses of all following
                    // arguments, environment variables, and the auxv vector because that would end up being a lot of changes
                    // to the the program. Instead, we reserve some amount of space for each argument (based on the root test
                    // case) and write NULs into the interior of arguments to shorten them.  In order to prevent some
                    // impossible inputs (arguments with interior NUL characters) we add solver constraints so that any
                    // character of an argument after a NUL is also a NUL.
                    ASSERT_not_null(anyPreviousCharIsNul);
                    auto bothNul = SymbolicExpr::makeAnd(anyPreviousCharIsNul, currentCharIsNul);
                    auto assertion = SymbolicExpr::makeOr(SymbolicExpr::makeInvert(anyPreviousCharIsNul), bothNul);
                    solver->insert(assertion);
                } else {
                    // argv[i] must be empty (i.e., first byte is NUL character) if argc <= i
                    auto argcGreaterThanI = SymbolicExpr::makeGt(argcSymbolic, SymbolicExpr::makeIntegerConstant(SP.nBits(), i));
                    auto assertion = SymbolicExpr::makeOr(argcGreaterThanI, currentCharIsNul);
                    solver->insert(assertion);
                }

                // Extend or create the expression for any previous character being NUL
                if (anyPreviousCharIsNul) {
                    anyPreviousCharIsNul = SymbolicExpr::makeOr(anyPreviousCharIsNul, currentCharIsNul);
                } else {
                    anyPreviousCharIsNul = currentCharIsNul;
                }
            }
        }
    }
    SAWYER_MESG(debug) <<"    " <<argc <<": @" <<StringUtility::addrToString(argvVa + argc * wordSizeBytes) <<" null\n"
                       <<"  ]\n";

    //---------------------------------------------------------------------------------------------------------------------------
    // envp
    //---------------------------------------------------------------------------------------------------------------------------

    // Unlike for arguments, there's no count of the number of environment variables. We need a count, so the first thing we
    // do is scan the list of pointers to find the null pointer.
    const rose_addr_t envpVa = argvVa + (argc + 1) * wordSizeBytes;
    size_t envc = 0;
    while (true) {
        rose_addr_t ptrVa = envpVa + envc * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = readMemoryUnsigned(ptrVa, wordSizeBytes);
        if (0 == strVa)
            break;
        ++envc;
    }

    SAWYER_MESG(debug) <<"  envp @" <<StringUtility::addrToString(envpVa) <<" = [\n";
    for (size_t i = 0; i < envc; ++i) {
        rose_addr_t ptrVa = envpVa + i * wordSizeBytes; // address of string pointer
        rose_addr_t strVa = readMemoryUnsigned(ptrVa, wordSizeBytes);
        std::string s = readCString(strVa);
        SAWYER_MESG(debug) <<"    " <<i <<": @" <<StringUtility::addrToString(strVa)
                           <<" \"" <<StringUtility::cEscape(s) <<"\"\n";

        if (markingEnvpAsInput_) {
            SymbolicExpr::Ptr anyPreviousCharIsNul;     // is any previous char of this env an ASCII NUL character?
            for (size_t j = 0; j <= s.size(); ++j) {
                rose_addr_t charVa = strVa + j;
                uint8_t charVal = s[j];

                BS::SValuePtr charSValue = ops->undefined_(8);
                SymbolicExpr::Ptr charSymbolic = IS::SymbolicSemantics::SValue::promote(charSValue)->get_expression();
                auto charEvent = ExecutionEvent::instanceWriteMemory(testCase(), nextLocation(), ip(), charVa, charVal);
                inputVariables.insertEnvironmentVariable(charEvent, i, j, charSymbolic);
                charSValue->comment(charEvent->name());
                ops->writeMemory(RegisterDescriptor(), ops->number_(SP.nBits(), charVa), charSValue, ops->boolean_(true));
                ExecutionEventId charEventId = database()->id(charEvent);

                SAWYER_MESG(debug) <<"      byte " <<j <<" @" <<StringUtility::addrToString(charVa)
                                   <<"; symbolic = " <<(*charSValue + fmt)
                                   <<"; event = " <<charEventId <<"\n";

            SymbolicExpr::Ptr currentCharIsNul = SymbolicExpr::makeEq(charSymbolic, SymbolicExpr::makeIntegerConstant(8, 0));
            if (s.size() == j) {
                // Final byte of the argument's buffer must always be NUL
                solver->insert(currentCharIsNul);

            } else if (j > 0) {
                // Linux doesn't allow NUL characters to appear inside environment variables. A NUL terminates the
                // environment variable and the next environment variable starts immediately thereafter. For concolic
                // testing, if an environment variable is shortened (by making one of it's non-ending bytes NUL) we don't
                // want to have to adjust the addresses of all following environment variables and the auxv vector because
                // that would end up being a lot of changes to the the program. Instead, we reserve some amount of space
                // for each environment variable (based on the root test case) and write NULs into the interior of
                // environment variables to shorten them.  In order to prevent some impossible inputs (environment
                // variables with interior NUL characters) we add solver constraints so that any character of an
                // environment variable after a NUL is also a NUL.
                ASSERT_not_null(anyPreviousCharIsNul);
                auto bothNul = SymbolicExpr::makeAnd(anyPreviousCharIsNul, currentCharIsNul);
                auto assertion = SymbolicExpr::makeOr(SymbolicExpr::makeInvert(anyPreviousCharIsNul), bothNul);
                solver->insert(assertion);
            } else {
                // envp[i] must be empty (i.e., first byte is NUL character) if envc <= i
                auto argcGreaterThanI = SymbolicExpr::makeGt(argcSymbolic, SymbolicExpr::makeIntegerConstant(SP.nBits(), i));
                auto assertion = SymbolicExpr::makeOr(argcGreaterThanI, currentCharIsNul);
                solver->insert(assertion);
            }

            // Extend or create the expression for any previous character being NUL
            if (anyPreviousCharIsNul) {
                anyPreviousCharIsNul = SymbolicExpr::makeOr(anyPreviousCharIsNul, currentCharIsNul);
            } else {
                anyPreviousCharIsNul = currentCharIsNul;
            }

            }
        }
    }
    SAWYER_MESG(debug) <<"  ]\n";

    //---------------------------------------------------------------------------------------------------------------------------
    // auxv
    //---------------------------------------------------------------------------------------------------------------------------
    // Not marking these as inputs because the user that's running the program doesn't have much influence.
    rose_addr_t auxvVa = envpVa + (envc + 1) * wordSizeBytes;
    SAWYER_MESG(debug) <<"  auxv @" <<StringUtility::addrToString(auxvVa) <<" = [\n";
    size_t nAuxvPairs = 0;
    while (true) {
        rose_addr_t va = auxvVa + nAuxvPairs * 2 * wordSizeBytes;
        size_t key = readMemoryUnsigned(va, wordSizeBytes);
        size_t val = readMemoryUnsigned(va + wordSizeBytes, wordSizeBytes);
        SAWYER_MESG(debug) <<"    " <<nAuxvPairs <<": key=" <<StringUtility::addrToString(key)
                           <<", val=" <<StringUtility::addrToString(val) <<"\n";
        if (0 == key)
            break;
        ++nAuxvPairs;
    }
    SAWYER_MESG(debug) <<"  ]\n";
}

ExecutionEvent::Ptr
LinuxI386::applySystemCallReturn(const P2::Partitioner &partitioner, const BS::RiscOperatorsPtr &ops,
                                 const std::string &syscallName, rose_addr_t syscallVa) {
    ASSERT_not_null(ops);
    const RegisterDescriptor SYS_RET = systemCallReturnRegister(partitioner);

    // Get the concrete return value from the system call, and write it to the symbolic state.
    uint64_t retConcrete = debugger_->readRegister(SYS_RET).toInteger();
    SAWYER_MESG(mlog[DEBUG]) <<"  " <<syscallName <<" returned " <<StringUtility::toHex2(retConcrete, SYS_RET.nBits()) <<"\n";
    BS::SValuePtr retSValue = ops->number_(SYS_RET.nBits(), retConcrete);
    ops->writeRegister(SYS_RET, retSValue);

    // Create an execution event for the system call return value that we can replay later in a different process.
    auto event = ExecutionEvent::instanceWriteRegister(testCase(), nextLocation(), syscallVa, SYS_RET, retConcrete);
    event->name(syscallName + "-return");
    return event;
}

#if 0 // [Robb Matzke 2021-05-27]
BS::SValuePtr
LinuxI386::createSystemCallReturnInput(const P2::Partitioner &partitioner, const BS::RiscOperatorsPtr &ops_,
                                       const std::string &syscallName, const ExecutionEvent::Ptr &retEvent) {
    auto ops = Emulation::RiscOperators::promote(ops_);
    ASSERT_not_null(ops);
    const RegisterDescriptor SYS_RET = systemCallReturnRegister(partitioner);

    // Create a symolic variable to represent that there was a system call return value that's being treated as a program
    // input that could change in subsequent runs, and link this variable to the execution event.
    BS::SValuePtr variableSValue = ops->undefined_(SYS_RET.nBits());
    SymbolicExpr::Ptr variableSymbolic = IS::SymbolicSemantics::SValue::promote(variableSValue)->get_expression();
    variableSValue->comment(retEvent->name());
    ops->writeRegister(SYS_RET, variableSValue);
    ops->inputVariables().insertSystemCallReturn(retEvent, variableSymbolic);
    retEvent->name(syscallName + "-return");
    return variableSValue;
}
#endif

uint64_t
LinuxI386::systemCallFunctionNumber(const P2::Partitioner &partitioner, const BS::RiscOperatorsPtr &ops) {
    ASSERT_not_null(ops);

    const RegisterDescriptor AX = partitioner.instructionProvider().registerDictionary()->findOrThrow("eax");
    BS::SValuePtr retvalSValue = ops->readRegister(AX);
    ASSERT_require2(retvalSValue->isConcrete(), "non-concrete system call numbers not handled yet");
    return retvalSValue->toUnsigned().get();
}

BS::SValuePtr
LinuxI386::systemCallArgument(const P2::Partitioner &partitioner, const BS::RiscOperatorsPtr &ops, size_t idx) {
    switch (idx) {
        case 0: {
            const RegisterDescriptor r = partitioner.instructionProvider().registerDictionary()->findOrThrow("ebx");
            return ops->readRegister(r);
        }
        case 1: {
            const RegisterDescriptor r = partitioner.instructionProvider().registerDictionary()->findOrThrow("ecx");
            return ops->readRegister(r);
        }
        case 2: {
            const RegisterDescriptor r = partitioner.instructionProvider().registerDictionary()->findOrThrow("edx");
            return ops->readRegister(r);
        }
        case 3: {
            const RegisterDescriptor r = partitioner.instructionProvider().registerDictionary()->findOrThrow("esi");
            return ops->readRegister(r);
        }
        case 4: {
            const RegisterDescriptor r = partitioner.instructionProvider().registerDictionary()->findOrThrow("edi");
            return ops->readRegister(r);
        }
        case 5: {
            const RegisterDescriptor r = partitioner.instructionProvider().registerDictionary()->findOrThrow("ebp");
            return ops->readRegister(r);
        }
    }
    ASSERT_not_reachable("system calls have at most six arguments");

    // For future reference, amd64 are: rdi, rsi, rdx, r10, r8, and r9
}

RegisterDescriptor
LinuxI386::systemCallReturnRegister(const P2::Partitioner &partitioner) {
    return partitioner.instructionProvider().registerDictionary()->findOrThrow("eax");
}

BS::SValuePtr
LinuxI386::systemCallReturnValue(const P2::Partitioner &partitioner, const BS::RiscOperatorsPtr &ops) {
    ASSERT_not_null(ops);
    const RegisterDescriptor reg = systemCallReturnRegister(partitioner);
    return ops->readRegister(reg);
}

BS::SValuePtr
LinuxI386::systemCallReturnValue(const P2::Partitioner &partitioner, const BS::RiscOperatorsPtr &ops,
                                 const BS::SValuePtr &retval) {
    ASSERT_not_null(ops);
    const RegisterDescriptor reg = systemCallReturnRegister(partitioner);
    ops->writeRegister(reg, retval);
    return retval;
}

void
LinuxI386::configureSystemCalls() {
    // These are the generally useful configurations. Feel free to override these to accomplish whatever kind of testing you
    // need.

    SystemCall::Ptr sc;

    // SYS_exit
    sc = SystemCall::instance();
    sc->exitsProcess(true);
    systemCalls().insert(1, sc);

    // SYS_getpid
    sc = SystemCall::instance();
    sc->isConstantReturn(true);
    systemCalls().insert(20, sc);

    // SYS_getuid: assumes SYS_setuid is never successfully called
    sc = SystemCall::instance();
    sc->isConstantReturn(true);
    systemCalls().insert(24, sc);

    // SYS_getgid: assumes SYS_setgid is never successfully called
    sc = SystemCall::instance();
    sc->isConstantReturn(true);
    systemCalls().insert(47, sc);

    // SYS_geteuid: assumes SYS_setuid is never successfully called
    sc = SystemCall::instance();
    sc->isConstantReturn(true);
    systemCalls().insert(50, sc);

    // SYS_getppid
    sc = SystemCall::instance();
    sc->isConstantReturn(true);
    systemCalls().insert(64, sc);

    // SYS_getpgrp
    sc = SystemCall::instance();
    sc->isConstantReturn(true);
    systemCalls().insert(65, sc);

    // SYS_exit_group
    sc = SystemCall::instance();
    sc->exitsProcess(true);
    systemCalls().insert(252, sc);
}

void
LinuxI386::systemCall(const P2::Partitioner &partitioner, const BS::RiscOperatorsPtr &ops_) {
    auto ops = Emulation::RiscOperators::promote(ops_);
    ASSERT_not_null(ops);
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // A system call has been encountered. The INT instruction has been processed symbolically (basically a no-op other than
    // to adjust the instruction pointer), and the concrete execution has stepped into the system call but has not yet executed
    // it (i.e., the subordinate process is in the syscall-enter-stop state).

    //-------------------------------------
    // Create system call execution event.
    //-------------------------------------

    // Gather info about the system call such as its arguments. On Linux, system calls have up to six arguments stored
    // in registers, so we just grab all six for now since we don't want to maintain a big switch statement to say how
    // many arguments each system call actually uses.
    rose_addr_t va = debugger_->executionAddress();
    uint64_t functionNumber = systemCallFunctionNumber(partitioner, ops);
    std::vector<uint64_t> arguments;
    for (size_t i = 0; i < 6; ++i) {
        BS::SValuePtr argSymbolic = systemCallArgument(partitioner, ops, i);
        if (auto argConcrete = argSymbolic->toUnsigned()) {
            arguments.push_back(*argConcrete);
        } else {
            ASSERT_not_implemented("non-concrete system call argument");
        }
    }
    if (debug) {
        debug <<"syscall-" <<functionNumber <<", args = (";
        for (uint64_t arg: arguments)
            debug <<" " <<StringUtility::toHex(arg);
        debug <<" )\n";
    }

    // The execution event records the system call number and arguments, but not any side effects (because side effects haven't
    // happened yet). Since the side effect events (created shortly) are general things like "write this value to this
    // register", the fact that they're preceded by this syscall event is what marks them as being side effects of this system
    // call.
    auto syscallEvent = ExecutionEvent::instanceSyscall(testCase(), nextLocation(), va, functionNumber, arguments);
    syscallEvent->name((boost::format("syscall-%d.%d") % functionNumber % syscallSequenceNumbers_[functionNumber]++).str());
    ExecutionEventId syscallEventId = database()->id(syscallEvent);
    SAWYER_MESG(debug) <<"  created execution event " <<*syscallEventId <<" for " <<syscallEvent->name() <<"\n";

    //-----------------------------------------------------------------------------------------
    // Process the system call concretely. The debugger is in the syscall-enter-stop state now.
    //-----------------------------------------------------------------------------------------

    const RegisterDescriptor SYS_RET = systemCallReturnRegister(partitioner);
    ExecutionEvent::Ptr retEvent;                       // optional system call return to be replayed in child test cases
    BS::SValuePtr retSValue;                            // optional input variable for child test cases

    SystemCall::Ptr systemCall = systemCalls().getOrDefault(functionNumber);
    ASSERT_not_null2(systemCall, "system call " + boost::lexical_cast<std::string>(functionNumber) + " is not declared\n");

    if (systemCall->exitsProcess()) {
        // These don't return
        ops->doExit(arguments[0]);

    } else if (systemCall->isConstantReturn()) {
        // These always return the same value
        debugger_->stepIntoSyscall();                   // after this, we're in the syscall-exit-stop state
        retEvent = applySystemCallReturn(partitioner, ops, syscallEvent->name(), va);

        retSValue = ops->undefined_(SYS_RET.nBits());
        SymbolicExpr::Ptr retSymbolic = IS::SymbolicSemantics::SValue::promote(retSValue)->get_expression();
        uint64_t retConcrete = retEvent->words()[0];

        if (Sawyer::Optional<uint64_t> firstRetConcrete = systemCall->constantReturnConcrete()) {
            // Make the syscall return the same symbolic value as before. We do this by adding a solver constraint to say that
            // the current return value is the same as the past return value.
            SymbolicExpr::Ptr firstRetSymbolic = systemCall->constantReturnSymbolic();
            ASSERT_not_null(firstRetSymbolic);
            SAWYER_MESG(debug) <<"  symbolic return must equal previous symbolic return\n";
            SymbolicExpr::Ptr retvalsAreEqual = SymbolicExpr::makeEq(firstRetSymbolic, retSymbolic);
            ops->solver()->insert(retvalsAreEqual);

            // Make the syscall return the same concrete value as before.
            if (*firstRetConcrete != retConcrete) {
                SAWYER_MESG(debug) <<"  replacing return value with " <<StringUtility::toHex2(*firstRetConcrete, SYS_RET.nBits()) <<"\n";
                retEvent->words(std::vector<uint64_t>{*firstRetConcrete});
                debugger_->writeRegister(SYS_RET, *firstRetConcrete);
            }

        } else {
            // Save the concrete and symbolic return values so we can make subsequent calls return the same.
            SAWYER_MESG(debug) <<"  this is a constant-returning system call\n"
                               <<"    concrete returns will all be " <<StringUtility::toHex2(retConcrete, SYS_RET.nBits()) <<"\n"
                               <<"    symbolic returns will all equal " <<*retSymbolic <<"\n";
            systemCall->constantReturnConcrete(retConcrete);
            systemCall->constantReturnSymbolic(retSymbolic);
        }

        ops->writeRegister(SYS_RET, retSValue);

    } else {
        // Other system calls
        debugger_->stepIntoSyscall();                   // after this, we're in the syscall-exit-stop state

        // Create an event and input variable for the system call return value.
        retEvent = applySystemCallReturn(partitioner, ops, syscallEvent->name(), va);
        retSValue = ops->undefined_(SYS_RET.nBits());
        ops->writeRegister(SYS_RET, retSValue);
    }

    //------------------------------------
    // Record any additional side effects
    //------------------------------------

    if (retSValue) {
        ASSERT_not_null(retEvent);
        SymbolicExpr::Ptr retSymbolic = IS::SymbolicSemantics::SValue::promote(retSValue)->get_expression();
        ops->inputVariables().insertSystemCallReturn(retEvent, retSymbolic);
        SAWYER_MESG(mlog[DEBUG]) <<"  created input variable " <<*retSValue
                                 <<" for " <<retEvent->printableName(database()) <<"\n";
    }

    if (retEvent) {
        ExecutionEventId retEventId = database()->id(retEvent);
        SAWYER_MESG(mlog[DEBUG]) <<"  created execution event " <<*retEventId
                                 <<" for " <<retEvent->name() <<"\n";
    }
}

} // namespace
} // namespace
} // namespace

#endif
