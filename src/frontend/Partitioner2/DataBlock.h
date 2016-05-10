#ifndef ROSE_Partitioner2_DataBlock_H
#define ROSE_Partitioner2_DataBlock_H

#include <Partitioner2/BasicTypes.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/SharedPointer.h>

#include <string>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Data block information.
 *
 *  A data block represents data with a type. */
class DataBlock: public Sawyer::SharedObject, public Sawyer::Attribute::Storage {
public:
    /** Shared pointer to a data block. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<DataBlock> Ptr;

private:
    bool isFrozen_;                                     // true if object is read-only because it's in the CFG
    rose_addr_t startVa_;                               // starting address
    size_t size_;                                       // size in bytes; FIXME[Robb P. Matzke 2014-08-12]: replace with type
    size_t nAttachedOwners_;                            // number of attached basic blocks and functions that own this data

protected:
    // use instance() instead
    DataBlock(rose_addr_t startVa, size_t size): startVa_(startVa), size_(size), nAttachedOwners_(0) {
        ASSERT_require(size_ > 0);
    }

public:
    /** Static allocating constructor.
     *
     *  The @p startVa is the starting address of the data block. */
    static Ptr instance(rose_addr_t startVa, size_t size) {
        return Ptr(new DataBlock(startVa, size));
    }

    /** Virtual constructor.
     *
     *  The @p startVa is the starting address for this data block. */
    virtual Ptr create(rose_addr_t startVa, size_t size) const {
        return instance(startVa, size);
    }

    /** Determine if data block is read-only.
     *
     *  Returns true if read-only, false otherwise. */
    bool isFrozen() const { return isFrozen_; }

    /** Returns the starting address. */
    rose_addr_t address() const { return startVa_; }

    /** Returns the size in bytes. */
    size_t size() const { return size_; }

    /** Change size of data block.
     *
     *  The size of a data block can only be changed directly when it is not represented by the control flow graph. That
     *  is, when this object is not in a frozen state.
     *
     *  @todo In the future, data block sizes will be modified only by changing the associated data type. */
    void size(size_t nBytes);

    /** Number of attached basic block and function owners.
     *
     *  Returns the number of data blocks and functions that are attached to the CFG/AUM and that own this data block. */
    size_t nAttachedOwners() const { return nAttachedOwners_; }

    /** Addresses represented. */
    AddressInterval extent() const { return AddressInterval::baseSize(address(), size()); }

    /** A printable name for this data block.  Returns a string like 'data block 0x10001234'. */
    std::string printableName() const;

private:
    friend class Partitioner;
    void freeze() { isFrozen_ = true; }
    void thaw() { isFrozen_ = false; }
    size_t incrementOwnerCount();
    size_t decrementOwnerCount();
};


} // namespace
} // namespace
} // namespace

#endif
