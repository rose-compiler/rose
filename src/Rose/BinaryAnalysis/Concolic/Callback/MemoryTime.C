#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryTime.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/Emulation.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

MemoryTime::~MemoryTime() {}

MemoryTime::MemoryTime() {}

MemoryTime::Ptr
MemoryTime::instance() {
    return Ptr(new MemoryTime);
}

void
MemoryTime::playback(SharedMemoryContext &ctx) {
    hello("simulated Unix time", ctx);
    prevRead_ = ctx.sharedMemoryEvent->calculateResult(ctx.architecture->inputVariables()->bindings());
}

void
MemoryTime::handlePreSharedMemory(SharedMemoryContext &ctx) {
    hello("simulated Unix time", ctx);
    if (IoDirection::READ == ctx.direction) {
        if (prevRead_) {
            SymbolicExpression::Ptr increasing = SymbolicExpression::makeGe(ctx.valueRead, prevRead_);
            ctx.ops->solver()->insert(increasing);
        }
        prevRead_ = ctx.valueRead;
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
