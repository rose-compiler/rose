#ifndef ROSE_BinaryAnalysis_Partitioner2_MemoryTable_H
#define ROSE_BinaryAnalysis_Partitioner2_MemoryTable_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/Constants.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Utilities for working with tables.
 *
 *  A table is an area of memory that has homogeneously sized entries. The actual starting address of the table is often initially
 *  unknown as is its size. These values are obtained by attempting to parse the table from memory. */
class MemoryTable {
public:
    /** Shared ownership pointer.
     *
     * @{ */
    using Ptr = MemoryTablePtr;
    using ConstPtr = MemoryTableConstPtr;
    /** @} */

    /** Represents the entries in the table.
     *
     *  Each entry has a unique starting address in memory. */
    class Entries {
    public:
        /** Shared-ownership pointer. */
        using Ptr = std::shared_ptr<Entries>;

        virtual ~Entries();
        Entries();

        /** True if there are no entries. */
        bool empty() const;

        /** Number of entries stored. */
        virtual size_t size() const = 0;

        /** Parse a table entry and append to this list if the entry is valid.
         *
         *  Given the table entry address and bytes obtained from memory, parse the entry and append it to this object. Returns true
         *  if the entry is valid, false if invalid. */
        virtual bool maybeAppend(const MemoryTable*, Address entryAddr, const std::vector<uint8_t> &bytes) = 0;

        /** Parse a table entry and prepend to this list if the entry is valid.
         *
         *  Given the table entry address and bytes obtained from memory, parse the entry and prepend it to this object. Returns
         *  true if the entry is valid, false if invalid. */
        virtual bool maybePrepend(const MemoryTable*, Address entryAddr, const std::vector<uint8_t> &bytes) = 0;
    };

    /** Represents the storage for the table. */
    class Storage {
    public:
        /** Shared-ownership pointer. */
        using Ptr = std::shared_ptr<Storage>;

        virtual ~Storage() {};

        /** Read some bytes of storage.
         *
         *  Reading stops when `nBytes` have been read or if we run out of data, or if we read a byte that is not concrete. */
        virtual std::vector<uint8_t> read(Address, size_t nBytes) = 0;
    };

private:
    // Set before parsing
    AddressInterval tableLimits_;                       // limits for where the table could exist
    size_t bytesPerEntry_ = 4;                          // number of bytes in each table entry
    size_t maxSkippable_ = 0;                           // max invalid entries before reaching start of table
    size_t maxPreEntries_ = UNLIMITED;                  // max number of entries prior to initial start of table
    bool showingDebug_ = false;                         // turn on debugging even if the mlog[DEBUG] is disabled

    // Available after parsing
    Entries::Ptr entries_;                              // entries parsed for the table
    AddressInterval tableLocation_;                     // where the table actually exists
    size_t nPreEntries_ = 0;                            // entries before the original table start address

public:
    virtual ~MemoryTable();
protected:
    MemoryTable(const AddressInterval&, size_t bytesPerEntry, const Entries::Ptr &entries);
public:
    /** Allocating constructor.
     *
     *  Construct a table that may exist at the locations specified by the @p tableLimits. Each entry of the table is a known size.
     *  The supplied @p entries object is reponsible for parsing table entries. */
    static Ptr instance(const AddressInterval &tableLimits, size_t bytesPerEntry, const Entries::Ptr &entries);

public:
    /** Property: Max initial invalid entries.
     *
     *  The maximum number of invalid entries that can be skipped when looking for the start of the table.
     *
     * @{ */
    size_t maxInitialSkip() const;
    void maxInitialSkip(size_t);
    /** @} */

    /** Property: Max number of entries to prepend before the original start of the table.
     *
     * @{ */
    size_t maxPreEntries() const;
    void maxPreEntries(size_t);
    /** @} */

    /** Property: Enable the debug stream for this object.
     *
     *  Debugging output is enabled even if the `mlog[DEBUG]` stream is disabled. This can be used to emit debugging information
     *  only about specific memory tables instead of all tables.
     *
     * @{ */
    bool showingDebug() const;
    void showingDebug(bool);
    /** @} */

    /** Property: Limits for table location.
     *
     *  After scanning, the actual table (its entries) will all exist within these limits.
     *
     * @{ */
    AddressInterval tableLimits() const;
    void tableLimits(const AddressInterval&);
    /** @} */

    /** Property: Bytes per entry.
     *
     *  This is read-only, set by the constructor. */
    size_t bytesPerEntry() const;

    /** Property: Table entries. */
    Entries::Ptr entries() const;

public:
    /** Scan the table and return its location.
     *
     *  Its location is also saved in this object and available by calling @ref location.
     *
     * @{ */
    AddressInterval scan(const MemoryMap::Constraints&, Address probableTableAddr);
    AddressInterval scan(const PartitionerConstPtr&, const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&,
                         Address probableTableAddr);
    AddressInterval scan(const Storage::Ptr&, Address probableTableAddr);
    /** @} */

    /** Result: Table location in memory.
     *
     *  The table location is continually updated during the @ref scan call, making it available when parsing entries. */
    AddressInterval location() const;

    /** Result: True if the table has no valid entries. */
    bool empty() const;

    /** Result: Total number of valid entries scanned into the table. */
    size_t nEntries() const;

    /** Result: Number of entries detected before the start of table.
     *
     *  This is the number of entries detected by the @ref scan function that occur prior to its probable table
     *  address. */
    size_t nPreEntries() const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
