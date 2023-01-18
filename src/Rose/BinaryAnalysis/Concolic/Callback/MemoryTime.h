#ifndef ROSE_BinaryAnalysis_Concolic_Callback_MemoryTime_H
#define ROSE_BinaryAnalysis_Concolic_Callback_MemoryTime_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

/** Return Unix time_t when reading shared memory.
 *
 *  Each read of shared memory in this region will return either a concrete Unix-epoch time_t value or a symbolic value. The
 *  values are unsigned 64-bit integers internally, and are truncated or zero extended to the requested return size. The return
 *  values are monotonically increasing (i.e., each subsequenct read is greater than or equal to the previous value, at least
 *  internally before the return value is truncated).
 *
 *  Writes to this memory are handled normally, but are effectively no-ops since the memory cannot be read. */
class MemoryTime: public SharedMemoryCallback {
public:
    /** Shared ownership pointer. */
    using Ptr = MemoryTimePtr;

private:
    SymbolicExpression::Ptr prevRead_;                  // value previously read by this callback

protected:
    explicit MemoryTime(const std::string &name);       // for factories
    MemoryTime(const AddressInterval &where, const std::string &name);
public:
    ~MemoryTime();

public:
    /** Allocating constructor. */
    static Ptr instance(const AddressInterval &where);

    /** Allocating constructor for factory. */
    static Ptr factory();

public:
    virtual SharedMemoryCallbackPtr instanceFromFactory(const AddressInterval&, const Yaml::Node &config) const override;
    virtual void handlePreSharedMemory(SharedMemoryContext&) override;
    virtual void playback(SharedMemoryContext&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
