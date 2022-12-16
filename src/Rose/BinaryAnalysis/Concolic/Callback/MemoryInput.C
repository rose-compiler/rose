#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryInput.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

MemoryInput::~MemoryInput() {}

MemoryInput::MemoryInput(ByteOrder::Endianness sex)
    : byteOrder_(sex) {}

MemoryInput::Ptr
MemoryInput::instance(ByteOrder::Endianness sex) {
    return Ptr(new MemoryInput(sex));
}

ByteOrder::Endianness
MemoryInput::byteOrder() const {
    return byteOrder_;
}

void
MemoryInput::byteOrder(ByteOrder::Endianness sex) {
    byteOrder_ = sex;
}

void
MemoryInput::handlePreSharedMemory(SharedMemoryContext &ctx) {
    hello("shared memory input", ctx);
    if (IoDirection::READ == ctx.direction) {

        // Implementation simplification: We must access the memory the same way each time. That is, using the same address and
        // size.
        for (auto node: previousReads_.findAll(ctx.accessedVas)) {
            if (node.key() == ctx.accessedVas) {
                returns(ctx, node.value());
                return;
            } else {
                throw Exception("MemoryInput memory must be accessed the same way each time");
            }
        }

        // We haven't read this memory before
        returns(ctx, ctx.sharedMemoryEvent->expression());

    } else {
        ASSERT_not_implemented("[Robb Matzke 2022-12-16] writes to MemoryInput are not yet implemented");
    }
}

void
MemoryInput::playback(SharedMemoryContext &ctx) {
    hello("shared memory input", ctx);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
