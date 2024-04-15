#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SyscallContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallContext::SyscallContext(const Architecture::Ptr &architecture, const ExecutionEvent::Ptr &syscallEvent,
                               const std::vector<ExecutionEvent::Ptr> &relatedEvents)
    : phase(ConcolicPhase::REPLAY), architecture(architecture), ip(syscallEvent->instructionPointer()),
      argsConcrete(syscallEvent->syscallArguments()), syscallEvent(syscallEvent), relatedEvents(relatedEvents) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(syscallEvent);
}

SyscallContext::SyscallContext(const Architecture::Ptr &architecture, const Emulation::RiscOperators::Ptr &ops,
                               const ExecutionEvent::Ptr &syscallEvent)
    : phase(ConcolicPhase::EMULATION), architecture(architecture), ops(ops), ip(syscallEvent->instructionPointer()),
      argsConcrete(syscallEvent->syscallArguments()), syscallEvent(syscallEvent) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(ops);
    ASSERT_not_null(syscallEvent);
}

SyscallContext::~SyscallContext() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SyscallCallback
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SyscallCallback::hello(const std::string &myName, const SyscallContext &ctx) const {
    Sawyer::Message::Stream out = mlog[WHERE] ? mlog[WHERE] : mlog[DEBUG];
    if (out) {
        switch (ctx.phase) {
            case ConcolicPhase::REPLAY:
                out <<"replaying ";
                break;
            case ConcolicPhase::EMULATION:
                out <<"calling ";
                break;
            case ConcolicPhase::POST_EMULATION:
                ASSERT_not_implemented("should be called here");
        }
        out <<(myName.empty() ? ctx.syscallEvent->name() + " system call" : myName)
            <<" at instruction " <<StringUtility::addrToString(ctx.ip) <<"\n";
    }
}

void
SyscallCallback::notAnInput(SyscallContext &ctx, const ExecutionEvent::Ptr &event) const {
    ASSERT_not_null(event);
    SAWYER_MESG(mlog[DEBUG]) <<"  " <<event->printableName(ctx.ops->database()) <<" is now not an input variable\n";
    ctx.ops->inputVariables()->deactivate(event);
    ctx.symbolicReturn = SymbolicExpression::Ptr();
}

} // namespace
} // namespace
} // namespace

#endif
