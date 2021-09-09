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

SharedMemoryContext::SharedMemoryContext(const Architecture::Ptr &architecture, const ExecutionEvent::Ptr &sharedMemoryEvent)
    : phase(ConcolicPhase::REPLAY), architecture(architecture), ip(sharedMemoryEvent->instructionPointer()),
      memoryVa(sharedMemoryEvent->memoryLocation().least()), nBytes(sharedMemoryEvent->memoryLocation().size()),
      direction(IoDirection::READ), sharedMemoryEvent(sharedMemoryEvent), valueRead(sharedMemoryEvent->bytesAsSymbolic()) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(sharedMemoryEvent);
    ASSERT_require(sharedMemoryEvent->actionType() == ExecutionEvent::Action::OS_SHM_READ);
    ASSERT_not_null(valueRead);
}

SharedMemoryContext::SharedMemoryContext(const Architecture::Ptr &architecture, const Emulation::RiscOperators::Ptr &ops,
                                         const ExecutionEvent::Ptr &sharedMemoryEvent)
    : phase(ConcolicPhase::EMULATION), architecture(architecture), ops(ops), ip(sharedMemoryEvent->instructionPointer()),
      memoryVa(sharedMemoryEvent->memoryLocation().least()), nBytes(sharedMemoryEvent->memoryLocation().size()),
      direction(IoDirection::READ), sharedMemoryEvent(sharedMemoryEvent), valueRead(sharedMemoryEvent->inputVariable()) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(ops);
    ASSERT_not_null(sharedMemoryEvent);
}

SharedMemoryContext::~SharedMemoryContext() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryCallback
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SharedMemoryCallback::hello(const std::string &myName, const SharedMemoryContext &ctx) const {
    Sawyer::Message::Stream out = mlog[WHERE] ? mlog[WHERE] : mlog[DEBUG];
    if (out) {
        switch (ctx.phase) {
            case ConcolicPhase::REPLAY:
                out <<"replay ";
                break;
            case ConcolicPhase::EMULATION:
                out <<"called ";
                break;
            case ConcolicPhase::POST_EMULATION:
                out <<"post ";
                break;
        }
        out <<(myName.empty() ? ctx.sharedMemoryEvent->name() : myName)
            <<" at instruction " <<StringUtility::addrToString(ctx.ip)
            <<", address " <<StringUtility::addrToString(ctx.memoryVa)
            <<" for " <<StringUtility::plural(ctx.nBytes, "bytes") <<"\n";
        if (ConcolicPhase::REPLAY == ctx.phase) {
            ASSERT_not_null(ctx.sharedMemoryEvent);
            SAWYER_MESG(mlog[DEBUG]) <<"  value = " <<*ctx.sharedMemoryEvent->bytesAsSymbolic() <<"\n";
        }
    }
}

bool
SharedMemoryCallback::operator()(bool handled, SharedMemoryContext &ctx) {
    switch (ctx.phase) {
        case ConcolicPhase::REPLAY:
            playback(ctx);
            return true;
        case ConcolicPhase::EMULATION:
            handlePreSharedMemory(ctx);
            return true;
        case ConcolicPhase::POST_EMULATION:
            handlePostSharedMemory(ctx);
            return true;
    }
    return false;
}

} // namespace
} // namespace
} // namespace

#endif