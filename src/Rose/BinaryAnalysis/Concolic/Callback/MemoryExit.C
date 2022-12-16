#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryExit.h>

#include <Rose/BinaryAnalysis/Concolic/Emulation.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

MemoryExit::~MemoryExit() {}

MemoryExit::MemoryExit() {}

MemoryExit::Ptr
MemoryExit::instance() {
    return Ptr(new MemoryExit);
}

MemoryExit::Ptr
MemoryExit::instance(uint8_t status) {
    auto retval = instance();
    retval->status(status);
    return retval;
}

const Sawyer::Optional<uint8_t>&
MemoryExit::status() const {
    return status_;
}

void
MemoryExit::status(const Sawyer::Optional<uint8_t> &s) {
    status_ = s;
}

void
MemoryExit::playback(SharedMemoryContext&) {}

void
MemoryExit::handlePreSharedMemory(SharedMemoryContext &ctx) {
    hello("shared-memory exit", ctx);

    if (status_) {
        ctx.ops->doExit(*status_);

    } else if (ctx.valueWritten && ctx.valueWritten->toUnsigned()) {
        ctx.ops->doExit(*ctx.valueWritten->toUnsigned());

    } else {
        ctx.ops->doExit(255);
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
