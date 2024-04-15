#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryInput.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

static const char *defaultName = "input";

MemoryInput::~MemoryInput() {}

MemoryInput::MemoryInput(const std::string &name)
    : SharedMemoryCallback(name) {}

MemoryInput::MemoryInput(const AddressInterval &where, const std::string &name, ByteOrder::Endianness sex)
    : SharedMemoryCallback(where, name), byteOrder_(sex) {}

MemoryInput::Ptr
MemoryInput::instance(const AddressInterval &where, ByteOrder::Endianness sex) {
    return Ptr(new MemoryInput(where, defaultName, sex));
}

MemoryInput::Ptr
MemoryInput::factory() {
    return Ptr(new MemoryInput(defaultName));
}

SharedMemoryCallback::Ptr
MemoryInput::instanceFromFactory(const AddressInterval &where, const Yaml::Node &config) const {
    Ptr retval = instance(where, byteOrder());
    retval->name(name());
    if (config["byte-order"]) {
        const std::string order = config["byte-order"].as<std::string>();
        if (order == "be" || order == "big-endian") {
            retval->byteOrder(ByteOrder::ORDER_MSB);
        } else if (order == "le" || order == "el" || order == "little-endian") {
            retval->byteOrder(ByteOrder::ORDER_LSB);
        } else {
            throw Exception("unrecognized byte order \"" + StringUtility::cEscape(order) + "\"");
        }
    }

    return retval;
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
    hello(ctx);
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
        mlog[WARN] <<"shared memory write is ignored\n"
                   <<"  address = " <<StringUtility::addrToString(ctx.accessedVas) <<"\n"
                   <<"  value   = " <<*ctx.valueWritten <<"\n";
    }
}

void
MemoryInput::playback(SharedMemoryContext &ctx) {
    hello(ctx);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
