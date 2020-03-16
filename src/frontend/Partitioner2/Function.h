#ifndef ROSE_Partitioner2_Function_H
#define ROSE_Partitioner2_Function_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemantics2.h>
#include <BinaryCallingConvention.h>
#include <BinaryStackDelta.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>
#include <SourceLocation.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/Cached.h>
#include <Sawyer/Map.h>
#include <Sawyer/Set.h>
#include <Sawyer/SharedPointer.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Shared-ownership pointer for function. See @ref heap_object_shared_ownership. */
typedef Sawyer::SharedPointer<class Function> FunctionPtr;

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
class Function: public Sawyer::SharedObject, public Sawyer::Attribute::Storage<> {
public:
    /** Manner in which a function owns a block. */
    enum Ownership { OWN_UNOWNED=0,                     /**< Function does not own the block. */
                     OWN_EXPLICIT,                      /**< Function owns the block explicitly, the normal ownership. */
                     OWN_PROVISIONAL,                   /**< Function might own the block in the future. */
    };

    /** Shared-ownership pointer for function. */
    typedef FunctionPtr Ptr;

private:
    rose_addr_t entryVa_;                               // entry address; destination for calls to this function
    std::string name_;                                  // optional function name
    std::string demangledName_;                         // optional demangled name
    std::string comment_;                               // optional multi-line, plain-text, commment
    unsigned reasons_;                                  // reason bits from SgAsmFunction::FunctionReason
    std::string reasonComment_;                         // additional commentary about reasons_
    std::set<rose_addr_t> bblockVas_;                   // addresses of basic blocks
    std::vector<DataBlock::Ptr> dblocks_;               // data blocks owned by this function, sorted by starting address
    bool isFrozen_;                                     // true if function is represented by the CFG
    CallingConvention::Analysis ccAnalysis_;            // analysis computing how registers etc. are used
    CallingConvention::Definition::Ptr ccDefinition_;   // best definition or null
    StackDelta::Analysis stackDeltaAnalysis_;           // analysis computing stack deltas for each block and whole function
    InstructionSemantics2::BaseSemantics::SValuePtr stackDeltaOverride_; // special value to override stack delta analysis
    SourceLocation sourceLocation_;                     // corresponding location of function in source code if known

    // The following members are caches either because their value is seldom needed and expensive to compute, or because the
    // value is best computed at a higher layer (e.g., in the partitioner) yet it makes the most sense to store it here. Make
    // sure clearCache() resets these to initial values.
    Sawyer::Cached<bool> isNoop_;

    void clearCache() {
        isNoop_.clear();
    }

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        //s & boost::serialization::base_object<Sawyer::Attribute::Storage<> >(*this); -- not stored
        s & BOOST_SERIALIZATION_NVP(entryVa_);
        s & BOOST_SERIALIZATION_NVP(name_);
        s & BOOST_SERIALIZATION_NVP(demangledName_);
        s & BOOST_SERIALIZATION_NVP(comment_);
        s & BOOST_SERIALIZATION_NVP(reasons_);
        s & BOOST_SERIALIZATION_NVP(bblockVas_);
        s & BOOST_SERIALIZATION_NVP(dblocks_);
        s & BOOST_SERIALIZATION_NVP(isFrozen_);
        s & BOOST_SERIALIZATION_NVP(ccAnalysis_);
        s & BOOST_SERIALIZATION_NVP(ccDefinition_);
        s & BOOST_SERIALIZATION_NVP(stackDeltaAnalysis_);
        s & BOOST_SERIALIZATION_NVP(stackDeltaOverride_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(reasonComment_);
        if (version >= 2)
            s & BOOST_SERIALIZATION_NVP(sourceLocation_);
    }
#endif
    
protected:
    // Needed for serialization
    Function()
        : entryVa_(0), reasons_(0), isFrozen_(false) {}

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

    /** Read-only property: Entry address.
     *
     *  The entry address also serves as an identifier for the function since the CFG can only hold one function per entry
     *  address.  Detached functions need not have unique entry addresses. */
    rose_addr_t address() const { return entryVa_; }

    /** Property: Optional function name.
     *
     *  This is the official name. See also @ref demangledName, which can also return the value of this @ref name property.
     *
     *  @{ */
    const std::string& name() const { return name_; }
    void name(const std::string &name) { name_ = name; }
    /** @} */

    /** Property: Optional demangled name.
     *
     *  This property holds the override string to use as the demangled name. If set to the empty string, then reading this
     *  property returns the true @ref name instead.
     *
     * @{ */
    const std::string& demangledName() const;
    void demangledName(const std::string &name) { demangledName_ = name; }
    /** @} */

    /** Property: Optional function comment.
     *
     *  Comments are multi-line, plain-text (not HTML), ASCII.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    void comment(const std::string &s) { comment_ = s; }
    /** @} */

    /** Property: Location of function definition in source code, if known.
     *
     * @{ */
    const SourceLocation& sourceLocation() const { return sourceLocation_; }
    void sourceLocation(const SourceLocation &loc) { sourceLocation_ = loc; }
    /** @} */

    /** Property: Bit vector of function reasons.  These are SgAsmFunction::FunctionReason bits.
     *
     *  @{ */
    unsigned reasons() const { return reasons_; }
    void reasons(unsigned reasons) { reasons_ = reasons; }
    /** @} */

    /** Insert additional function reason bits.
     *
     *  The high-order bits 16 bits are OR'd into the @ref reasons property, while the low-order 16 bits given in the argument
     *  replace the low-order 16 bits stored in the @ref reasons property. */
    void insertReasons(unsigned reasons) { reasons_ = (reasons_ & 0xffff0000) | reasons; }

    /** Remove function reason bits.
     *
     *  Removes the high-order 16 bits that appear in the argument from the @ref reasons property. The low-order 16 bits are
     *  all cleared if any of the low-order 16 bits of the argument are set. */
    void eraseReasons(unsigned reasons) { reasons_ &= ~((0xffff0000 & reasons) | ((reasons & 0xffff) != 0 ? 0xffff : 0x0)); }
    /** @} */

    /** Property: Additional comment for why function was detected.
     *
     * @{ */
    const std::string& reasonComment() const { return reasonComment_; }
    void reasonComment(const std::string &s) { reasonComment_ = s; }
    /** @} */

    /** Returns basic block addresses.  Because functions can exist in a detatched state, a function stores basic block
     *  addresses rather than basic blocks.  This allows a function to indicate which blocks will be ultimately part of its
     *  definition without requiring that the blocks actually exist.  When a detached function is inserted into the CFG then
     *  basic block placeholders will be created for any basic blocks that don't exist in the CFG (see @ref
     *  Partitioner::insertFunction). */
    const std::set<rose_addr_t>& basicBlockAddresses() const { return bblockVas_; }

    /** Predicate to test whether a function owns a basic block address. */
    bool ownsBasicBlock(rose_addr_t bblockVa) const {
        return bblockVas_.find(bblockVa) != bblockVas_.end();
    }

    /** Add a basic block to this function.
     *
     *  This method does not adjust the partitioner CFG. Basic blocks cannot be added by this method when this function is
     *  attached to the CFG since it would cause the CFG to become outdated with respect to this function, but as long as the
     *  function is detached blocks can be inserted and removed arbitrarily.  If the specified address is already part of the
     *  function then it is not added a second time.
     *
     *  Returns true if the block is inserted, false if the block was already part of this function. */
    bool insertBasicBlock(rose_addr_t bblockVa) {
        ASSERT_forbid(isFrozen_);
        bool wasInserted = bblockVas_.insert(bblockVa).second;
        if (wasInserted)
            clearCache();
        return wasInserted;
    }

    /** Remove a basic block from this function.  This method does not adjust the partitioner CFG.  Basic blocks cannot be
     * removed by this method when this function is attached to the CFG since it would cause the CFG to become outdated with
     * respect to this function, but as long as the function is detached blocks can be inserted and removed arbitrarily.  If
     * the specified address is not a basic block address for this function then this is a no-op.  Removing the function's
     * entry address is never permitted. */
    void eraseBasicBlock(rose_addr_t bblockVa) {        // no-op if not existing
        ASSERT_forbid(isFrozen_);
        ASSERT_forbid2(bblockVa==entryVa_, "function entry block cannot be removed");
        clearCache();
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

    /** Remove specified or equivalent data block from this function.
     *
     *  If this function is in a detached state (i.e., not part of the CFG/AUM) then the specified data block or equivalent
     *  data block is removed from this function. Returns the data block that was erased, or null if none was erased.
     *
     *  It is an error to invoke this method on function that is attached to the CFG/AUM, for which @ref isFrozen returns
     *  true. This method is a no-op if the specified data block is a null pointer. */
    DataBlock::Ptr eraseDataBlock(const DataBlock::Ptr&);

    /** Determine if this function contains the specified data block, or equivalent.
     *
     *  If this function owns the specified data block or an equivalent data block then this method returns a pointer to the
     *  existing data block, otherwise it returns the null pointer.
     *
     *  Thread safety: This method is not thread safe. */
    DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const;

    /** Addresses that are part of static data.
     *
     *  Returns all addresses that are part of static data.
     *
     *  Thread safety: This method is not thread safe. */
    AddressIntervalSet dataAddresses() const;

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

    /** Number of data blocks in the function. */
    size_t nDataBlocks() const { return dblocks_.size(); }

    /** Property: Stack delta.
     *
     *  The set or computed stack delta. If a stack delta override has been set (@ref stackDeltaOverride) then that value is
     *  returned. Otherwise, if the stack delta analysis has been run and a stack delta is known, it is returned. Otherwise a
     *  null pointer is returned. Calling this method returns previously computed values rather than running a potentially
     *  expensive analysis.
     *
     * @{ */
    InstructionSemantics2::BaseSemantics::SValuePtr stackDelta() const;
    int64_t stackDeltaConcrete() const;
    /** @} */

    /** Property: Stack delta override.
     *
     *  This is the value returned by @ref stackDelta in preference to using the stack delta analysis results. It allows a user
     *  to override the stack delta analysis.  The partitioner will not run stack delta analysis if an override value is set.
     *
     * @{ */
    InstructionSemantics2::BaseSemantics::SValuePtr stackDeltaOverride() const;
    void stackDeltaOverride(const InstructionSemantics2::BaseSemantics::SValuePtr &delta);
    /** @} */

    /** Property: Stack delta analysis results.
     *
     *  This property holds the results from stack delta analysis. It contains the stack entry and exit values for each basic
     *  block computed from data flow, and the overall stack delta for the function. The analysis is not updated by this class;
     *  objects of this class only store the results provided by something else.
     *
     *  The @c hasResults and @c didConverge methods invoked on the return value will tell you whether an analysis has run and
     *  whether the results are valid, respectively.
     *
     * @{ */
    const StackDelta::Analysis& stackDeltaAnalysis() const { return stackDeltaAnalysis_; }
    StackDelta::Analysis& stackDeltaAnalysis() { return stackDeltaAnalysis_; }
    /** @} */

    /** Property: Calling convention analysis results.
     *
     *  This property holds the results from calling convention analysis. It contains information about what registers and
     *  stack locations are accessed and whether they serve as inputs or outputs and which registers are used but restored
     *  before returning (callee-saved).  It also stores a concrete stack delta.  The analysis is not updated by this class;
     *  objects of this class only store the results provided by something else.
     *
     *  The analysis itself does not fully describe a calling convention since a function might not use all features of the
     *  calling convention.  For instance, a no-op function could match any number of calling convention definitions.
     *
     *  The @c hasResults and @c didConverge methods invoked on the return value will tell you whether an analysis has run and
     *  whether the results are valid, respectively.
     *
     * @{ */
    const CallingConvention::Analysis& callingConventionAnalysis() const { return ccAnalysis_; }
    CallingConvention::Analysis& callingConventionAnalysis() { return ccAnalysis_; }
    /** @} */

    /** Property: Best calling convention definition.
     *
     *  This is the best calling convention definition for this function. Calling conventions have two parts: (1) the behavior
     *  of the function such as which locations serve as inputs (read-before-write) and outputs (write-last), and callee-saved
     *  locations (read-before-write and write-last and same initial and final value), and (2) a list of well-known calling
     *  convention definitions that match the function's behavior.  More than one definition can match. This property holds one
     *  defintion which is usually the "best" one.
     *
     * @{ */
    CallingConvention::Definition::Ptr callingConventionDefinition() { return ccDefinition_; }
    void callingConventionDefinition(const CallingConvention::Definition::Ptr &ccdef) { ccDefinition_ = ccdef; }
    /** @} */

    /** A printable name for the function.
     *
     *  Returns a string like 'function 0x10001234 "main"'.  The function name is not included if this function has neither a
     *  demangled name nor a true name. The @ref demangledName overrides the true @ref name. */
    std::string printableName() const;

    /** Cached results of function no-op analysis.
     *
     *  If a value is cached, then the analysis has run and the cached value is true if the analysis proved that the function
     *  is a no-op. */
    const Sawyer::Cached<bool>& isNoop() const { return isNoop_; }

private:
    friend class Partitioner;
    void freeze() { isFrozen_ = true; }
    void thaw() { isFrozen_ = false; }

    // Find an equivalent data block and replace it with the specified data block, or insert the specified data block
    void replaceOrInsertDataBlock(const DataBlock::Ptr&);
};

typedef Sawyer::Container::Map<rose_addr_t, Function::Ptr> Functions;
typedef Sawyer::Container::Set<Function::Ptr> FunctionSet;

} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::Function, 2);

#endif
#endif
