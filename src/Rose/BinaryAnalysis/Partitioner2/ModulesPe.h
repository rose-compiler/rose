#ifndef ROSE_BinaryAnalysis_Partitioner2_ModulesPe_H
#define ROSE_BinaryAnalysis_Partitioner2_ModulesPe_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Sawyer/Map.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Disassembly and partitioning utilities for PE files. */
namespace ModulesPe {

/** Convert function name to system representation.
 *
 *  ROSE always stores library function names as "function@library", and this function rewrites such names to be similar to the
 *  way thy're typically presented on a particular operating system.  For instance, "EncodePointer@KERNEL32.dll" will be
 *  rewritten to "KERNEL32.dll:EncodePointer" since that's how it typically appears in Windows tools.
 *
 *  @sa Modules::canonicalFunctionName */
std::string systemFunctionName(const std::string&);

/** Index for PE import addresses.
 *
 *  This is a map from possible import addresses to the item in the Import Address Table (IAT) that describes the function that
 *  will be imported to that address. The map probably contains at least two addresses per function, the absolute address and a
 *  relative address, because we're not sure which is contained in the IAT -- it depends on whether a linker has run. */
typedef Sawyer::Container::Map<Address, SgAsmPEImportItem*> ImportIndex;

/** Reads PE export sections to find functions.
 *
 *  Scans all PE export sections under the specified PE file header to obtain names and addresses for functions.  Returns a
 *  list of new, unique functions sorted by entry address.
 *
 * @{ */
std::vector<FunctionPtr> findExportFunctions(const PartitionerConstPtr&, SgAsmPEFileHeader*);
std::vector<FunctionPtr> findExportFunctions(const PartitionerConstPtr&, SgAsmInterpretation*);
size_t findExportFunctions(const PartitionerConstPtr&, SgAsmInterpretation*, std::vector<FunctionPtr>&);
/** @} */

/** Reads PE import sections to find functions.
 *
 *  Scans all PE import sections in the specified interpretation to obtain addresses where imported functions will be mapped or
 *  have been mapped, depending on whether a dynamic linker has run.  The return value is a list of new, unique functions
 *  sorted by entry address.
 *
 * @{ */
std::vector<FunctionPtr> findImportFunctions(const PartitionerConstPtr&, SgAsmPEFileHeader*);
std::vector<FunctionPtr> findImportFunctions(const PartitionerConstPtr&, SgAsmInterpretation*);
size_t findImportFunctions(const PartitionerConstPtr&, SgAsmPEFileHeader*, const ImportIndex&, std::vector<FunctionPtr>&);
/** @} */

/** Scans PE import sections to build an index.
 *
 * @{ */
ImportIndex getImportIndex(const PartitionerConstPtr&, SgAsmPEFileHeader*);
ImportIndex getImportIndex(const PartitionerConstPtr&, SgAsmInterpretation*);
size_t getImportIndex(const PartitionerConstPtr&, SgAsmPEFileHeader*, ImportIndex&);
/** @} */

/** Update import address tables to reflect addresses of imported functions. */
void rebaseImportAddressTables(const PartitionerPtr &partitioner, const ImportIndex &index);

/** Names functions that look like they're thunks for imports.
 *
 *  An import thunk is an indirect unconditional jump through a read-only import address table to a virtual address where an
 *  external function will be dynamically linked.  The dynamic linking will have already taken place if a linker was used prior
 *  to disassembly. */
void nameImportThunks(const PartitionerConstPtr&, SgAsmInterpretation*);

/** Build may-return white and black lists. */
void buildMayReturnLists(const PartitionerPtr&);

/** Callback to restore PEScrambler function call edges.
 *
 *  This basic block callback is invoked each time the partitioner discovers a new instruction. It looks at the instruction's
 *  successors, and if the only successor is the PEScrambler dispatch function (as specified during callback construction) then
 *  the successor edge is replace with the edge to the original callee as if PEScrambler's dispatcher did the replacement. */
class PeDescrambler: public BasicBlockCallback {
public:
    /** Shared-ownership pointer to a @ref PeDescrambler. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<PeDescrambler> Ptr;

    /** One dispatch table entry in native format. */
    struct DispatchEntry {                              // one entry of PEScrambler's dispatch table
        uint32_t returnVa;                              // return address for dispatcher call
        uint32_t calleeVa;                              // address if function that should have been called
        DispatchEntry(): returnVa(0), calleeVa(0) {}
        DispatchEntry(uint32_t returnVa, uint32_t calleeVa): returnVa(returnVa), calleeVa(calleeVa) {}
    };

    /** The function dispatch table.
     *
     *  The first part of the table, up to and including the first zero-valued @c returnVa, has @c calleeVa values which are
     *  function addresses. The second part of the table's @c calleeVa values are addresses containing the address of a
     *  function (i.e., an extra level of indirection). */
    typedef std::vector<DispatchEntry> DispatchTable;

private:
    Address dispatcherVa_;                              // address of PEScrambler's indirection decoder and dispatcher
    Address dispatchTableVa_;                           // address of PEScrambler's dispatch table
    bool reachedEndOfTable_;                            // true when we cannot read any more table entries
    bool checkedPreconditions_;                         // true after we did some first-call precondition checking
    static const size_t sizeofDispatcherFunction = 65;  // default size of PEscrambler dispatcher function in bytes
    static const size_t bitsPerWord = 32;               // this callback only works for 32-bit specimens
    DispatchTable dispatchTable_;                       // the entire dispatch table (and possibly more)

protected:
    // Non-subclass users: please use the instance() method instead; these objects are reference counted.
    PeDescrambler(Address dispatcherVa, Address dispatchTableVa)
        : dispatcherVa_(dispatcherVa), dispatchTableVa_(dispatchTableVa), reachedEndOfTable_(false),
          checkedPreconditions_(false) {}

public:
    /** Construct a new PeDescrambler.
     *
     *  The @p dispatcherVa is the virtual address of the PEScrambler dispatch function.  One can easily find it by looking at
     *  the call graph since it will be the function that probably has many more callers than any other function.  The @p
     *  dispatchTableVa is the address of the PEScrambler dispatch table, which normally starts at the first byte past the end
     *  of the dispatch function. */
    static Ptr instance(Address dispatcherVa, Address dispatchTableVa) {
        return Ptr(new PeDescrambler(dispatcherVa, dispatchTableVa));
    }

    /** Construct a new PeDescrambler.
     *
     *  This is the same as the two-argument constructor, but the dispatch table address is assumed to be at a fixed offset
     *  from the dispatch function. */
    static Ptr instance(Address dispatcherVa) {
        return Ptr(new PeDescrambler(dispatcherVa, dispatcherVa + sizeofDispatcherFunction));
    }

    /** Name certain addresses in the specimen.
     *
     *  Names the PEScrambler dispatch address and dispatch table address if they don't have names yet. */
    void nameKeyAddresses(const PartitionerPtr&);

    /** Virtual address of PEScrambler dispatch function. */
    Address dispatcherVa() const { return dispatcherVa_; }

    /** Virtual address of PEScrambler dispatch table. */
    Address dispatchTableVa() const { return dispatchTableVa_; }

    /** Dispatch table.
     *
     *  Returns a reference to the dispatch table as it currently exists in this callback.  The callback extends the table only
     *  as needed in order to locate a return address, and the table is extended in arbitrary sized chunks (so it may contain
     *  data that isn't actually part of the table as far as the specimen is concerned).  The caller is free to modify the
     *  table.  See @ref DispatchTable for more information.
     *
     * @{ */
    const DispatchTable& dispatchTable() const { return dispatchTable_; }
    DispatchTable& dispatchTable() { return dispatchTable_; }
    /** @} */

    // Callback invoked by the partitioner each time an instruction is appended to a basic block.
    virtual bool operator()(bool chain, const Args&) override;

private:
    // True if the only CFG successor for the specified block is the PEScrambler dispatcher and the block ends with a CALL
    // instruction.
    bool basicBlockCallsDispatcher(const PartitionerConstPtr&, const BasicBlockPtr&);

    // Look up the return address in the PEScrambler dispatch table as if we were the dispatcher and return it if found.
    Sawyer::Optional<Address> findCalleeAddress(const PartitionerConstPtr&, Address returnVa);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
