#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryTime.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/Emulation.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/Concolic/InputVariables.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

static const char *defaultName = "time";

MemoryTime::~MemoryTime() {}

MemoryTime::MemoryTime(const std::string &name)
    : SharedMemoryCallback(name) {}

MemoryTime::MemoryTime(const AddressInterval &where, const std::string &name)
    : SharedMemoryCallback(where, name) {}

MemoryTime::Ptr
MemoryTime::instance(const AddressInterval &where) {
    return Ptr(new MemoryTime(where, defaultName));
}

MemoryTime::Ptr
MemoryTime::factory() {
    return Ptr(new MemoryTime(defaultName));
}

SharedMemoryCallback::Ptr
MemoryTime::instanceFromFactory(const AddressInterval &where, const Yaml::Node &/*config*/) const {
    Ptr retval = instance(where);
    retval->name(name());
    return retval;
}

void
MemoryTime::playback(SharedMemoryContext &ctx) {
    hello(ctx);
    prevRead_ = ctx.sharedMemoryEvent->calculateResult(ctx.architecture->inputVariables()->bindings());
}

void
MemoryTime::handlePreSharedMemory(SharedMemoryContext &ctx) {
    hello(ctx);
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
