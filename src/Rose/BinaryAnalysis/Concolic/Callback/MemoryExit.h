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
private:
    Sawyer::Optional<uint8_t> status_;

public:
    /** Shared ownership pointer. */
    using Ptr = MemoryExitPtr;

protected:
    MemoryExit();
public:
    ~MemoryExit();

public:
    /** Allocating constructor. */
    static Ptr instance();

    /** Allocating constructor with specified exit status. */
    static Ptr instance(uint8_t);

    /** Property: Exit status.
     *
     *  If an exit status is specified, it overrides the exit status from a memory write. See class documentation.
     *
     * @{ */
    const Sawyer::Optional<uint8_t>& status() const;
    void status(const Sawyer::Optional<uint8_t>&);
    /** @} */

public:
    void playback(SharedMemoryContext&) override;
    void handlePreSharedMemory(SharedMemoryContext &ctx) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
