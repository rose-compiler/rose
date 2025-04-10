#ifndef ROSE_BinaryAnalysis_Partitioner2_JumpTable_H
#define ROSE_BinaryAnalysis_Partitioner2_JumpTable_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/MemoryTable.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Utilities for working with jump tables.
 *
 *  A jump table is a table that has addresses (direct or computed) that can be used as targets for computed jumps, i.e., indirect
 *  control flow. */
class JumpTable: public MemoryTable {
public:
    /** Shared ownership pointer.
     *
     * @{ */
    using Ptr = JumpTablePtr;
    using ConstPtr = JumpTableConstPtr;
    /** @} */

    /** How target addresses are computed from entries. */
    enum class EntryType {
        ABSOLUTE,                                       /**< Target is entry plus (nominally zero) constant offset. */
        TABLE_RELATIVE                                  /**< Like `ABSOLUTE` but add the table address. */
    };

    /** Entries in a jump table.
     *
     *  Entries are transformed to target addresses in a couple different ways:
     *
     *  @li The target address is the table entry plus some (nominally zero) offset.
     *
     *  @li The target address is the table entry plus some (nominally zero) offset plus the starting address of the table. */
    class Entries: public MemoryTable::Entries {
    public:
        using Ptr = std::shared_ptr<Entries>;

    private:
        std::vector<Address> targets_;

    protected:
        Entries();
    public:
        static Ptr instance();

    public:
        /** Return the targets. */
        const std::vector<Address>& targets() const;

    public:
        size_t size() const override;
        bool maybeAppend(const MemoryTable*, Address entryAddr, const std::vector<uint8_t> &bytes) override;
        bool maybePrepend(const MemoryTable*, Address entryAddr, const std::vector<uint8_t> &bytes) override;

    private:
        // Parse a jump table entry and return the potential target address
        Address parseEntry(const JumpTable*, const std::vector<uint8_t> &bytes) const;

        // Test whether a potential target address is valid
        bool isValidTarget(const JumpTable*, Address target) const;
    };

private:
    PartitionerConstPtr partitioner_;
    Address perEntryOffset_ = 0;
    EntryType entryType_ = EntryType::ABSOLUTE;
    AddressInterval targetLimits_;

protected:
    JumpTable(const PartitionerConstPtr&, const AddressInterval &tableLimits, size_t bytesPerEntry,
              Address perEntryOffset, EntryType);
public:
    /** Allocating constructor. */
    static Ptr
    instance(const PartitionerConstPtr&, const AddressInterval &tableLimits, size_t bytesPerEntry,
             Address perEntryOffset, EntryType);

public:
    /** Property: Partitioner supplied to the constructor.
     *
     *  This is a read-only property set during object construction. */
    PartitionerConstPtr partitioner() const;

    /** Property: Offset added to each table entry when calculating the target address.
     *
     *  This is a read-only property set during object construction. */
    Address perEntryOffset() const;

    /** Property: Entry type.
     *
     *  Describes how to calculate a target address from the raw bytes stored in the table entry.
     *
     *  This is a read-only property set during object construction. */
    EntryType entryType() const;

    /** Property: Predicate to determine whether a target is valid.
     *
     *  A target computed from the raw bytes of a table entry is valid only if the target is within this specified range.
     *
     * @{ */
    AddressInterval targetLimits() const;
    void targetLimits(const AddressInterval&);
    /** @} */

public:
    /** Result: Target addresses parsed from the table. */
    const std::vector<Address>& targets() const;

    /** Result: Unique target addresses parsed from the table. */
    std::set<Address> uniqueTargets() const;

public:
    /** Refine the table limits based on a particular basic block. */
    void refineLocationLimits(const BasicBlockPtr&, Address probableTableAddr);

    /** Refine target limits based on a particular basic block.
     *
     *  The jump table targets should be within the function that owns the basic block. */
    void refineTargetLimits(const BasicBlockPtr&);

    /** Change basic block successors to be the table's targets.
     *
     *  The basic blocks current successors are removed, and replaced with the target addresses scanned from this table. */
    void replaceBasicBlockSuccessors(const PartitionerConstPtr&, const BasicBlockPtr&) const;

    /** Build a data block to represent the table.
     *
     *  Returns a new detached basic block if this jump table is non-empty, otherwise returns a null pointer. */
    DataBlockPtr createDataBlock() const;

    /** Create a data block and attach it to the specified basic block.
     *
     *  The data block is created by @ref createDataBlock. Calling this function more than once will cause multiple identical data
     *  blocks to be attached to the basic block. */
    void attachTableToBasicBlock(const BasicBlockPtr&) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
