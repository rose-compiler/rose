#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/Callback/MemoryExit.h>

#include <Rose/BinaryAnalysis/Concolic/Emulation.h>
#include <Rose/StringUtility/StringToNumber.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

static const char *defaultName = "exit";

MemoryExit::~MemoryExit() {}

MemoryExit::MemoryExit(const std::string &name)
    : SharedMemoryCallback(name) {}

MemoryExit::MemoryExit(const AddressInterval &where, const std::string &name, const Sawyer::Optional<uint8_t> &status)
    : SharedMemoryCallback(where, name), status_(status) {}

MemoryExit::Ptr
MemoryExit::instance(const AddressInterval &where) {
    return Ptr(new MemoryExit(where, defaultName, Sawyer::Nothing()));
}

MemoryExit::Ptr
MemoryExit::instance(const AddressInterval &where, const Sawyer::Optional<uint8_t> &status) {
    return Ptr(new MemoryExit(where, defaultName, status));
}

MemoryExit::Ptr
MemoryExit::factory() {
    return Ptr(new MemoryExit(defaultName));
}

const Sawyer::Optional<uint8_t>&
MemoryExit::status() const {
    return status_;
}

void
MemoryExit::status(const Sawyer::Optional<uint8_t> &s) {
    status_ = s;
}

SharedMemoryCallback::Ptr
MemoryExit::instanceFromFactory(const AddressInterval &where, const Yaml::Node &config) const {
    ASSERT_require(isFactory());
    Ptr retval = instance(where, status());
    retval->name(name());
    if (config["exit-status"])
        retval->status(StringUtility::toNumber<uint8_t>(config["exit-status"].as<std::string>()).unwrap());
    return retval;
}

void
MemoryExit::playback(SharedMemoryContext &ctx) {
    ASSERT_forbid(isFactory());
    hello(ctx);
}

void
MemoryExit::handlePreSharedMemory(SharedMemoryContext &ctx) {
    ASSERT_forbid(isFactory());
    hello(ctx);

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
