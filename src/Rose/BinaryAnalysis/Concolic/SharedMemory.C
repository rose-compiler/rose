#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

using namespace Sawyer::Message::Common;

namespace IS = Rose::BinaryAnalysis::InstructionSemantics;
namespace BS = IS::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SharedMemoryContext::SharedMemoryContext(const Architecture::Ptr &architecture, const ExecutionEvent::Ptr &sharedMemoryEvent)
    : phase(ConcolicPhase::REPLAY), architecture(architecture), ip(sharedMemoryEvent->instructionPointer()),
      accessedVas(sharedMemoryEvent->memoryLocation()), direction(IoDirection::READ), sharedMemoryEvent(sharedMemoryEvent),
      valueRead(sharedMemoryEvent->value()) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(sharedMemoryEvent);
    ASSERT_require(sharedMemoryEvent->action() == ExecutionEvent::Action::OS_SHARED_MEMORY);
}

SharedMemoryContext::SharedMemoryContext(const Architecture::Ptr &architecture, const Emulation::RiscOperators::Ptr &ops,
                                         const ExecutionEvent::Ptr &sharedMemoryEvent)
    : phase(ConcolicPhase::EMULATION), architecture(architecture), ops(ops), ip(sharedMemoryEvent->instructionPointer()),
      accessedVas(sharedMemoryEvent->memoryLocation()), direction(IoDirection::READ), sharedMemoryEvent(sharedMemoryEvent),
      valueRead(sharedMemoryEvent->inputVariable()) {
    ASSERT_not_null(architecture);
    ASSERT_not_null(ops);
    ASSERT_not_null(sharedMemoryEvent);
}

SharedMemoryContext::~SharedMemoryContext() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryCallback
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const AddressInterval&
SharedMemoryCallback::registeredVas() const {
    return registeredVas_;
}

void
SharedMemoryCallback::registeredVas(const AddressInterval &i) {
    registeredVas_ = i;
}

void
SharedMemoryCallback::hello(const std::string &myName, const SharedMemoryContext &ctx) const {
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
                out <<"finishing ";
                break;
        }
        out <<(myName.empty() ? ctx.sharedMemoryEvent->name() : myName)
            <<" at instruction " <<StringUtility::addrToString(ctx.ip)
            <<", addresses " <<StringUtility::addrToString(ctx.accessedVas)
            <<" (" <<StringUtility::plural(ctx.accessedVas.size(), "bytes") <<")\n";
        if (mlog[DEBUG] && ConcolicPhase::REPLAY == ctx.phase) {
            ASSERT_not_null(ctx.sharedMemoryEvent);
            if (SymbolicExpression::Ptr value = ctx.sharedMemoryEvent->value()) {
                SAWYER_MESG(mlog[DEBUG]) <<"  value = " <<*value <<"\n";
            } else {
                SAWYER_MESG(mlog[DEBUG]) <<"  no concrete value (treated as non-shared memory)\n";
            }
        }
    }
}

void
SharedMemoryCallback::normalRead(SharedMemoryContext &ctx) const {
    SAWYER_MESG(mlog[DEBUG]) <<"    canceled: this read will be treated as non-shared memory\n";
    ctx.ops->inputVariables()->deactivate(ctx.sharedMemoryEvent);
    ctx.sharedMemoryEvent->variable(SymbolicExpression::Ptr());
    ctx.valueRead = SymbolicExpression::Ptr();
}

void
SharedMemoryCallback::notAnInput(SharedMemoryContext &ctx) const {
    SAWYER_MESG(mlog[DEBUG]) <<"    this shared memory read will not be treated as a test case input\n";
    ctx.ops->inputVariables()->deactivate(ctx.sharedMemoryEvent);
}

void
SharedMemoryCallback::returns(SharedMemoryContext &ctx, const SymbolicExpression::Ptr &value) const {
    ASSERT_not_null(value);
    SAWYER_MESG(mlog[DEBUG]) <<"    returning " <<*value <<"\n";
    if (value->isConstant()) {
        ctx.ops->inputVariables()->deactivate(ctx.sharedMemoryEvent);
        ctx.sharedMemoryEvent->variable(SymbolicExpression::Ptr());
        ctx.valueRead = value;
        SAWYER_MESG(mlog[DEBUG]) <<"    this shared memory read will not be treated as a test case input\n";
    } else {
        ctx.valueRead = value;
    }
}

void
SharedMemoryCallback::returns(SharedMemoryContext &ctx, const BS::SValue::Ptr &value) const {
    SymbolicExpression::Ptr expr = IS::SymbolicSemantics::SValue::promote(value)->get_expression();
    returns(ctx, expr);
}

SymbolicExpression::Ptr
SharedMemoryCallback::inputVariable(const SharedMemoryContext &ctx) const {
    return ctx.sharedMemoryEvent->inputVariable();
}

bool
SharedMemoryCallback::operator()(bool /*handled*/, SharedMemoryContext &ctx) {
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
