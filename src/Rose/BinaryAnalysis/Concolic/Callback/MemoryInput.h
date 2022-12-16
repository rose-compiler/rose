#ifndef ROSE_BinaryAnalysis_Concolic_Callback_MemoryInput_H
#define ROSE_BinaryAnalysis_Concolic_Callback_MemoryInput_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#include <ByteOrder.h>                                  // rose

#include <Sawyer/IntervalMap.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace Callback {

/** Treats memory as input.
 *
 *  The first read from each byte of the memory location returns new input variable and subsequent reads return the previous
 *  variable. */
class MemoryInput: public SharedMemoryCallback {
private:
    using ByteMap = Sawyer::Container::IntervalMap<AddressInterval, SymbolicExpression::Ptr>;
    ByteMap previousReads_;
    ByteOrder::Endianness byteOrder_;

public:
    /** Shared ownership pointer. */
    using Ptr = MemoryInputPtr;

protected:
    explicit MemoryInput(ByteOrder::Endianness);
public:
    ~MemoryInput();

public:
    /** Allocating constructor. */
    static Ptr instance(ByteOrder::Endianness);

    /** Property: Byte order.
     *
     *  The order in which byte values are assembled when reading multi-byte values.
     *
     * @{ */
    ByteOrder::Endianness byteOrder() const;
    void byteOrder(ByteOrder::Endianness);
    /** @} */

public:
    void handlePreSharedMemory(SharedMemoryContext&) override;
    void playback(SharedMemoryContext&) override;
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
