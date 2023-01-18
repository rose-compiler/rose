#ifndef ROSE_BinaryAnalysis_Concolic_Callback_MemoryExit_H
#define ROSE_BinaryAnalysis_Concolic_Callback_MemoryExit_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

#include <Sawyer/Optional.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

/** Exit the process when reading or writing certain addresses.
 *
 *  When a concrete value is written to a concrete memory address where this callback is registered, the test case
 *  being executed exits.
 *
 *  The exit status is the first of the following values:
 *
 *  @li If the callback has an exit status, then this is the exit status used for the test case.
 *
 *  @li If a concrete value is being written to the memory address, then this is used as the exit status, modulo 256.
 *
 *  @li The exit status is 255. */
class MemoryExit: public SharedMemoryCallback {
public:
    /** Shared ownership pointer. */
    using Ptr = MemoryExitPtr;

private:
    Sawyer::Optional<uint8_t> status_;

protected:
    explicit MemoryExit(const std::string &name);
    MemoryExit(const AddressInterval&, const std::string &name, const Sawyer::Optional<uint8_t> &status);
public:
    ~MemoryExit();

public:
    /** Allocating constructor. */
    static Ptr instance(const AddressInterval&);

    /** Allocating constructor with specified exit status. */
    static Ptr instance(const AddressInterval&, const Sawyer::Optional<uint8_t> &status);

    /** Allocating constructor for a new factory instance. */
    static Ptr factory();

    /** Property: Exit status.
     *
     *  If an exit status is specified, it overrides the exit status from a memory write. See class documentation.
     *
     * @{ */
    const Sawyer::Optional<uint8_t>& status() const;
    void status(const Sawyer::Optional<uint8_t>&);
    /** @} */

public:
    virtual SharedMemoryCallbackPtr instanceFromFactory(const AddressInterval&, const Yaml::Node &config) const override;
    virtual void playback(SharedMemoryContext&) override;
    virtual void handlePreSharedMemory(SharedMemoryContext &ctx) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
