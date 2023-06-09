#ifndef ROSE_BinaryAnalysis_Concolic_Callback_MemoryInput_H
#define ROSE_BinaryAnalysis_Concolic_Callback_MemoryInput_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

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
    MemoryInput(const std::string &name);               // for factories
    MemoryInput(const AddressInterval &where, const std::string &name, ByteOrder::Endianness);
public:
    ~MemoryInput();

public:
    /** Allocating constructor. */
    static Ptr instance(const AddressInterval &where, ByteOrder::Endianness = ByteOrder::ORDER_UNSPECIFIED);

    /** Allocating constructor for a factory. */
    static Ptr factory();

    /** Property: Byte order.
     *
     *  The order in which byte values are assembled when reading multi-byte values.
     *
     * @{ */
    ByteOrder::Endianness byteOrder() const;
    void byteOrder(ByteOrder::Endianness);
    /** @} */

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
