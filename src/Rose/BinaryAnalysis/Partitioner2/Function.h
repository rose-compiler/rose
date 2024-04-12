#ifndef ROSE_BinaryAnalysis_Partitioner2_Function_H
#define ROSE_BinaryAnalysis_Partitioner2_Function_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>

#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/StackDelta.h>
#include <Rose/SourceLocation.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/Cached.h>
#include <Sawyer/Map.h>
#include <Sawyer/Set.h>
#include <Sawyer/SharedPointer.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/access.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Shared-ownership pointer for function. */
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
    std::vector<DataBlockPtr> dblocks_;                 // data blocks owned by this function, sorted by starting address
    bool isFrozen_;                                     // true if function is represented by the CFG
    CallingConvention::Analysis ccAnalysis_;            // analysis computing how registers etc. are used
    CallingConvention::DefinitionPtr ccDefinition_;     // best definition or null
    StackDelta::Analysis stackDeltaAnalysis_;           // analysis computing stack deltas for each block and whole function
    InstructionSemantics::BaseSemantics::SValuePtr stackDeltaOverride_; // special value to override stack delta analysis
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
    template<class S> void serialize(S&, unsigned version);
#endif
    
public:
    ~Function();

protected:
    // Needed for serialization
    Function();

    // Use instance() instead
    explicit Function(rose_addr_t entryVa, const std::string &name, unsigned reasons);

public:
    /** Static allocating constructor.  Creates a new function having the specified characteristics.
     *
     *  @{ */
    static Ptr instance(rose_addr_t entryVa, const std::string &name="", unsigned reasons=0);
    static Ptr instance(rose_addr_t entryVa, unsigned reasons);
    /** @} */

    /** Read-only property: Entry address.
     *
     *  The entry address also serves as an identifier for the function since the CFG can only hold one function per entry
     *  address.  Detached functions need not have unique entry addresses. */
    rose_addr_t address() const;

    /** Property: Optional function name.
     *
     *  This is the official name. See also @ref demangledName, which can also return the value of this @ref name property.
     *
     *  @{ */
    const std::string& name() const;
    void name(const std::string&);
    /** @} */

    /** Property: Optional demangled name.
     *
     *  This property holds the override string to use as the demangled name. If set to the empty string, then reading this
     *  property returns the true @ref name instead.
     *
     * @{ */
    const std::string& demangledName() const;
    void demangledName(const std::string&);
    /** @} */

    /** Property: Optional function comment.
     *
     *  Comments are multi-line, plain-text (not HTML), ASCII.
     *
     * @{ */
    const std::string& comment() const;
    void comment(const std::string&);
    /** @} */

    /** Property: Location of function definition in source code, if known.
     *
     * @{ */
    const SourceLocation& sourceLocation() const;
    void sourceLocation(const SourceLocation&);
    /** @} */

    /** Property: Bit vector of function reasons.  These are SgAsmFunction::FunctionReason bits.
     *
     *  @{ */
    unsigned reasons() const;
    void reasons(unsigned);
    /** @} */

    /** Insert additional function reason bits.
     *
     *  The high-order bits 16 bits are OR'd into the @ref reasons property, while the low-order 16 bits given in the argument
     *  replace the low-order 16 bits stored in the @ref reasons property. */
    void insertReasons(unsigned reasons);

    /** Remove function reason bits.
     *
     *  Removes the high-order 16 bits that appear in the argument from the @ref reasons property. The low-order 16 bits are
     *  all cleared if any of the low-order 16 bits of the argument are set. */
    void eraseReasons(unsigned reasons);

    /** Property: Additional comment for why function was detected.
     *
     * @{ */
    const std::string& reasonComment() const;
    void reasonComment(const std::string&);
    /** @} */

    /** Returns basic block addresses.  Because functions can exist in a detatched state, a function stores basic block
     *  addresses rather than basic blocks.  This allows a function to indicate which blocks will be ultimately part of its
     *  definition without requiring that the blocks actually exist.  When a detached function is inserted into the CFG then
     *  basic block placeholders will be created for any basic blocks that don't exist in the CFG (see @ref
     *  Partitioner::attachFunction). */
    const std::set<rose_addr_t>& basicBlockAddresses() const;

    /** Predicate to test whether a function owns a basic block address. */
    bool ownsBasicBlock(rose_addr_t bblockVa) const;

    /** Add a basic block to this function.
     *
     *  This method does not adjust the partitioner CFG. Basic blocks cannot be added by this method when this function is
     *  attached to the CFG since it would cause the CFG to become outdated with respect to this function, but as long as the
     *  function is detached blocks can be inserted and removed arbitrarily.  If the specified address is already part of the
     *  function then it is not added a second time.
     *
     *  Returns true if the block is inserted, false if the block was already part of this function. */
    bool insertBasicBlock(rose_addr_t bblockVa);

    /** Remove a basic block from this function.  This method does not adjust the partitioner CFG.  Basic blocks cannot be
     * removed by this method when this function is attached to the CFG since it would cause the CFG to become outdated with
     * respect to this function, but as long as the function is detached blocks can be inserted and removed arbitrarily.  If
     * the specified address is not a basic block address for this function then this is a no-op.  Removing the function's
     * entry address is never permitted. */
    void eraseBasicBlock(rose_addr_t bblockVa);         // no-op if not existing

    /** Returns data blocks owned by this function.  Returns the data blocks that are owned by this function in order of their
     *  starting address. */
    const std::vector<DataBlockPtr>& dataBlocks() const;

    /** Add a data block to this function.
     *
     *  This method does not adjust the partitioner CFG.  Data blocks cannot be added by this method when this function is
     *  attached to the CFG since it would cause the CFG to become outdated with respect to this function, but as long as the
     *  function is detached blocks can be inserted and removed arbitrarily.  The specified data block cannot be a null
     *  pointer. If the data block is already owned by this function then nothing happens and this method returns false;
     *  otherwise the data block is inserted and the method returns true. */
    bool insertDataBlock(const DataBlockPtr&);

    /** Remove specified or equivalent data block from this function.
     *
     *  If this function is in a detached state (i.e., not part of the CFG/AUM) then the specified data block or equivalent
     *  data block is removed from this function. Returns the data block that was erased, or null if none was erased.
     *
     *  It is an error to invoke this method on function that is attached to the CFG/AUM, for which @ref isFrozen returns
     *  true. This method is a no-op if the specified data block is a null pointer. */
    DataBlockPtr eraseDataBlock(const DataBlockPtr&);

    /** Determine if this function contains the specified data block, or equivalent.
     *
     *  If this function owns the specified data block or an equivalent data block then this method returns a pointer to the
     *  existing data block, otherwise it returns the null pointer.
     *
     *  Thread safety: This method is not thread safe. */
    DataBlockPtr dataBlockExists(const DataBlockPtr&) const;

    /** Addresses that are part of static data.
     *
     *  Returns all addresses that are part of static data.
     *
     *  Thread safety: This method is not thread safe. */
    AddressIntervalSet dataAddresses() const;

    /** Determines whether a function is frozen.  The ownership relations (instructions, basic blocks, and data blocks) cannot
     *  be adjusted while a function is in a frozen state.  All functions that are represented in the control flow graph are in
     *  a frozen state; detaching a function from the CFG thaws it. */
    bool isFrozen() const;

    /** True if function is a thunk.
     *
     *  This function is a thunk if it is marked as such in its reason codes via @ref SgAsmFunction::FUNC_THUNK and it has
     *  exactly one basic block.
     *
     *  See also, @ref Partitioner::functionIsThunk that is a stronger predicate and also returns the address of the thunk
     *  target. */
    bool isThunk() const;

    /** Number of basic blocks in the function. */
    size_t nBasicBlocks() const;

    /** Number of data blocks in the function. */
    size_t nDataBlocks() const;

    /** Property: Stack delta.
     *
     *  The set or computed stack delta. If a stack delta override has been set (@ref stackDeltaOverride) then that value is
     *  returned. Otherwise, if the stack delta analysis has been run and a stack delta is known, it is returned. Otherwise a
     *  null pointer is returned. Calling this method returns previously computed values rather than running a potentially
     *  expensive analysis.
     *
     * @{ */
    InstructionSemantics::BaseSemantics::SValuePtr stackDelta() const;
    int64_t stackDeltaConcrete() const;
    /** @} */

    /** Property: Stack delta override.
     *
     *  This is the value returned by @ref stackDelta in preference to using the stack delta analysis results. It allows a user
     *  to override the stack delta analysis.  The partitioner will not run stack delta analysis if an override value is set.
     *
     * @{ */
    InstructionSemantics::BaseSemantics::SValuePtr stackDeltaOverride() const;
    void stackDeltaOverride(const InstructionSemantics::BaseSemantics::SValuePtr &delta);
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
    const StackDelta::Analysis& stackDeltaAnalysis() const;
    StackDelta::Analysis& stackDeltaAnalysis();
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
    const CallingConvention::Analysis& callingConventionAnalysis() const;
    CallingConvention::Analysis& callingConventionAnalysis();
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
    CallingConvention::DefinitionPtr callingConventionDefinition();
    void callingConventionDefinition(const CallingConvention::DefinitionPtr&);
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
    const Sawyer::Cached<bool>& isNoop() const;

private:
    friend class Partitioner;
    void freeze();
    void thaw();

    // Find an equivalent data block and replace it with the specified data block, or insert the specified data block
    void replaceOrInsertDataBlock(const DataBlockPtr&);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
