#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/SValue.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SharedMemoryContext::~SharedMemoryContext() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryCallback
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SharedMemoryCallback::hello(const std::string &myName, const SharedMemoryContext &ctx) const {
    SAWYER_MESG(mlog[WHERE]) <<"called " <<myName
                             <<" at instruction " <<StringUtility::addrToString(ctx.ip)
                             <<", address " <<StringUtility::addrToString(ctx.memoryVa)
                             <<" for " <<StringUtility::plural(ctx.nBytes, "bytes") <<"\n";
}

ExecutionEvent::Ptr
SharedMemoryCallback::createReadEvent(SharedMemoryContext &ctx) const {
    ASSERT_require(!ctx.event);

    auto ops = Emulation::RiscOperators::promote(ctx.ops);
    Architecture::Ptr arch = ops->process();

    // Create a variable for the value read.
    ctx.result = ops->undefined_(8 * ctx.nBytes);
    SymbolicExpr::Ptr variable = Emulation::SValue::promote(ctx.result)->get_expression();

    // Create an event that when running concretely later will cause the concolic testing system to realize that there's been a
    // shared memory read. Since we're not guaranteed to be able to pre-write the desired value to memory and read it back
    // (shmem doesn't always have those semantics), and since the memory might not be writable anyway, we need to track the
    // side effects of where the data that was read eventually went.  We do that by creating a shared-memory-read event to mark
    // the start of these events, followed by additional events that fix things up after the instruction has completed. On RISC
    // architectures, the side effect is usually just writing the value that was read into a register, but CISC architectures
    // could have more complex side effects.
    //
    // All we need to do here is create the initial shared-memory-read marker and optionally give it an input variable. The
    // side-effect fixup events will be generated as the rest of the current instruction executes.
    ctx.event = ExecutionEvent::instanceSharedMemoryRead(arch->testCase(),
                                                         arch->nextEventLocation(When::PRE),
                                                         ctx.ip, ctx.memoryVa, ctx.nBytes);
    ctx.event->name("shm-read-" + StringUtility::addrToString(ctx.memoryVa).substr(2));
    ops->inputVariables().insertSharedMemoryRead(ctx.event, variable);
    SAWYER_MESG(mlog[DEBUG]) <<"  created input variable " <<*variable
                             <<" for " <<ctx.event->printableName(arch->database()) <<"\n";
    return ctx.event;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SharedMemory::SharedMemory() {}

SharedMemory::~SharedMemory() {}

SharedMemory::Ptr
SharedMemory::instance() {
    return Ptr(new SharedMemory);
}

Sawyer::Optional<uint8_t>
SharedMemory::previousReadConcreteAtOffset(size_t offset) const {
    return offset < prevReadConcrete_.size() ? prevReadConcrete_[offset] : Sawyer::Nothing();
}

void
SharedMemory::previousReadConcreteAtOffset(size_t offset, const Sawyer::Optional<uint8_t> &byte) {
    if (offset >= prevReadConcrete_.size())
        prevReadConcrete_.resize(offset + 1);
    prevReadConcrete_[offset] = byte;
}

SymbolicExpr::Ptr
SharedMemory::previousReadSymbolicAtOffset(size_t offset) const {
    return offset < prevReadSymbolic_.size() ? prevReadSymbolic_[offset] : SymbolicExpr::Ptr();
}

void
SharedMemory::previousReadSymbolicAtOffset(size_t offset, const SymbolicExpr::Ptr &byte) {
    if (offset >= prevReadSymbolic_.size())
        prevReadSymbolic_.resize(offset + 1);
    prevReadSymbolic_[offset] = byte;
}

const SharedMemory::Callbacks&
SharedMemory::callbacks() const {
    return callbacks_;
}

SharedMemory::Callbacks&
SharedMemory::callbacks() {
    return callbacks_;
}

} // namespace
} // namespace
} // namespace

#endif
