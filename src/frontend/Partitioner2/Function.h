#ifndef ROSE_Partitioner2_Function_H
#define ROSE_Partitioner2_Function_H

#include <Partitioner2/Attribute.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>

#include <sawyer/Map.h>
#include <sawyer/SharedPointer.h>

#include <set>
#include <string>
#include <vector>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Describes one function.
 *
 *  A function consists of one or more basic blocks.  Exactly one block is special in that it serves as the entry point when
 *  this function is invoked from elsewhere; the only incoming inter-function edges are to this entry block.  This function may
 *  have outgoing inter-function edges that represent invocations of other functions, and the targets of all such edges will be
 *  the entry block of another function.  A function may also own zero or more data blocks consisting of a base address and
 *  size (type).
 *
 *  A function may exist as part of the partitioner's control flow graph, or in a detached state.  When a function is
 *  represented by the control flow graph then it is in a frozen state, meaning that its basic blocks and data blocks cannot be
 *  adjusted adjusted; one must use the partitioner interface to do so. */
class Function: public Sawyer::SharedObject, public Attribute::StoredValues {
public:
    /** Manner in which a function owns a block. */
    enum Ownership { OWN_UNOWNED=0,                     /**< Function does not own the block. */
                     OWN_EXPLICIT,                      /**< Function owns the block explicitly, the normal ownership. */
                     OWN_PROVISIONAL,                   /**< Function might own the block in the future. */
    };
    typedef Sawyer::SharedPointer<Function> Ptr;

private:
    rose_addr_t entryVa_;                               // entry address; destination for calls to this function
    std::string name_;                                  // optional function name
    unsigned reasons_;                                  // reason bits from SgAsmFunction::FunctionReason
    std::set<rose_addr_t> bblockVas_;                   // addresses of basic blocks
    std::vector<DataBlock::Ptr> dblocks_;               // data blocks owned by this function, sorted by starting address
    bool isFrozen_;                                     // true if function is represented by the CFG

protected:
    // Use instance() instead
    explicit Function(rose_addr_t entryVa, const std::string &name, unsigned reasons)
        : entryVa_(entryVa), name_(name), reasons_(reasons), isFrozen_(false) {
        bblockVas_.insert(entryVa);
    }

public:
    /** Static allocating constructor.  Creates a new function having the specified characteristics.
     *
     *  @{ */
    static Ptr instance(rose_addr_t entryVa, const std::string &name="", unsigned reasons=0) {
        return Ptr(new Function(entryVa, name, reasons));
    }
    static Ptr instance(rose_addr_t entryVa, unsigned reasons) {
        return Ptr(new Function(entryVa, "", reasons));
    }
    /** @} */

    /** Return the entry address.  The entry address also serves as an identifier for the function since the CFG can only hold
     *  one function per entry address.  Detached functions need not have unique entry addresses. */
    rose_addr_t address() const { return entryVa_; }

    /** Optional function name.
     *
     *  @{ */
    const std::string& name() const { return name_; }
    void name(const std::string &name) { name_ = name; }
    /** @} */

    /** Function reasons.  These are SgAsmFunction::FunctionReason bits.
     *
     *  @{ */
    unsigned reasons() const { return reasons_; }
    void reasons(unsigned reasons) { reasons_ = reasons; }
    void insertReasons(unsigned reasons) { reasons_ |= reasons; }
    void eraseReasons(unsigned reasons) { reasons_ &= ~reasons; }
    /** @} */

    /** Returns basic block addresses.  Because functions can exist in a detatched state, a function stores basic block
     *  addresses rather than basic blocks.  This allows a function to indicate which blocks will be ultimately part of its
     *  definition without requiring that the blocks actually exist.  When a detached function is inserted into the CFG then
     *  basic block placeholders will be created for any basic blocks that don't exist in the CFG (see @ref
     *  Partitioner::insertFunction). */
    const std::set<rose_addr_t>& basicBlockAddresses() const { return bblockVas_; }

    /** Add a basic block to this function.  This method does not adjust the partitioner CFG. Basic blocks cannot be added by
     *  this method when this function is attached to the CFG since it would cause the CFG to become outdated with respect to
     *  this function, but as long as the function is detached blocks can be inserted and removed arbitrarily.  If the
     *  specified address is already part of the function then it is not added a second time. */
    void insertBasicBlock(rose_addr_t bblockVa) {       // no-op if exists
        ASSERT_forbid(isFrozen_);
        bblockVas_.insert(bblockVa);
    }

    /** Remove a basic block from this function.  This method does not adjust the partitioner CFG.  Basic blocks cannot be
     * removed by this method when this function is attached to the CFG since it would cause the CFG to become outdated with
     * respect to this function, but as long as the function is detached blocks can be inserted and removed arbitrarily.  If
     * the specified address is not a basic block address for this function then this is a no-op.  Removing the function's
     * entry address is never permitted. */
    void eraseBasicBlock(rose_addr_t bblockVa) {        // no-op if not existing
        ASSERT_forbid(isFrozen_);
        ASSERT_forbid2(bblockVa==entryVa_, "function entry block cannot be removed");
        bblockVas_.erase(bblockVa);
    }

    /** Returns data blocks owned by this function.  Returns the data blocks that are owned by this function in order of their
     *  starting address. */
    const std::vector<DataBlock::Ptr>& dataBlocks() const { return dblocks_; }

    /** Add a data block to this function.
     *
     *  This method does not adjust the partitioner CFG.  Data blocks cannot be added by this method when this function is
     *  attached to the CFG since it would cause the CFG to become outdated with respect to this function, but as long as the
     *  function is detached blocks can be inserted and removed arbitrarily.  The specified data block cannot be a null
     *  pointer. If the data block is already owned by this function then nothing happens and this method returns false;
     *  otherwise the data block is inserted and the method returns true. */
    bool insertDataBlock(const DataBlock::Ptr&);

    /** Remove a data block from this function.  This method does not adjust the partitioner CFG.  Data blocks cannot be
     *  removed by this method when this function is attached to the CFG since it would cause the CFG to become outdated with
     *  respect to this function, but as long as the function is detached blocks can be inserted and removed arbitrarily.  If
     *  the specified pointer is null or the data block does not exist in this function then this method is a no-op. */
    void eraseDataBlock(const DataBlock::Ptr&);

    /** Determines whether a function is frozen.  The ownership relations (instructions, basic blocks, and data blocks) cannot
     *  be adjusted while a function is in a frozen state.  All functions that are represented in the control flow graph are in
     *  a frozen state; detaching a function from the CFG thaws it. */
    bool isFrozen() const { return isFrozen_; }

    /** True if function is a thunk.
     *
     *  This function is a thunk if it is marked as such in its reason codes via @ref SgAsmFunction::FUNC_THUNK and it has
     *  exactly one basic block.
     *
     *  See also, @ref Partitioner::functionThunkTarget that is a stronger predicate and also returns the address of the thunk
     *  target. */
    bool isThunk() const;

    /** Number of basic blocks in the function. */
    size_t nBasicBlocks() const { return bblockVas_.size(); }

    /** A printable name for the function.  Returns a string like 'function 0x10001234 "main"'.  The function name is not
     *  included if the name is empty. */
    std::string printableName() const;

private:
    friend class Partitioner;
    void freeze() { isFrozen_ = true; }
    void thaw() { isFrozen_ = false; }
};

typedef Sawyer::Container::Map<rose_addr_t, Function::Ptr> Functions;

} // namespace
} // namespace
} // namespace

#endif
