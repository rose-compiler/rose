#ifndef ROSE_Partitioner2_BasicBlock_H
#define ROSE_Partitioner2_BasicBlock_H

#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Semantics.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/Cached.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <Sawyer/SharedPointer.h>
#include <Sawyer/Synchronization.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

namespace BaseSemantics = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BasicBlockSemantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information related to instruction semantics.
 *
 *  The purpose of this class is mainly to encapsulate all instruction semantics information so it can be accessed
 *  atomically in a multi-threaded application. */
class BasicBlockSemantics {
public:
    /** How instructions are dispatched.
     *
     *  Dispatchers are specific to the instruction architecture, and also contain a pointer to the register dictionary
     *  that was used.  The register dictionary can be employed to obtain names for the registers in the semantic
     *  states. The disptcher is null if this basic block is empty or instruction semantics are not implemented for
     *  the instruction set architecture being used. */
    BaseSemantics::DispatcherPtr dispatcher;

    /** Risc operators.
     *
     *  This is just the dispatcher's RiscOperators if there is a disptcher. If no dispatcher, then the operators
     *  are still present. */
    BaseSemantics::RiscOperatorsPtr operators;

    /** Initial state for semantics.
     *
     *  This is the semantic state prior to execution of the first instruction of the basic block. This is a nulll pointer
     *  if semantics have been dropped. */
    BaseSemantics::StatePtr initialState;

    /** Whether semantic state is up-to-date.
     *
     *  True if dispatcher's state is up-to-date, i.e., represents the semantic state after executing the last instruction
     *  of the basic block. */
    bool usingDispatcher;

    /** The state just prior to executing the final instruction.
     *
     *  The penultimate state is saved so that one instruction can be efficiently popped from the end of the basic block,
     *  after which the final state is the old penultimate state and the new penultimate state is not stored. */
    Sawyer::Optional<BaseSemantics::StatePtr> optionalPenultimateState;

    /** Whether the semantics had been dropped and reconstructed. */
    bool wasDropped;

public:
    /** Construct an empty semantics object. */
    BasicBlockSemantics()
        : usingDispatcher(false), wasDropped(false) {}

    /** Determines whether semantics have been dropped.
     *
     *  Returns true if a basic block's semantics have been dropped and a dispatcher is available. Always returns false if a
     *  dispatcher is not available. */
    bool isSemanticsDropped() const {
        return dispatcher && !initialState;
    }

    /** Determines whether a semantics error was encountered.
     *
     *  Returns true if an error was encountered in the block's instruction semantics.  Always returns false if a dispatcher is
     *  not available or if semantics have been dropped. */
    bool isSemanticsError() const {
        return dispatcher && initialState && !usingDispatcher;
    }
    
    /** Return the final semantic state.
     *
     *  The returned state is equivalent to starting with the initial state and processing each instruction.  If a semantic
     *  error occurs during processing then the null pointer is returned.  The null pointer is also returned if the basic
     *  block is empty. */
    BaseSemantics::StatePtr finalState() const {
        return usingDispatcher && operators ? operators->currentState() : BaseSemantics::StatePtr();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BasicBlock
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Basic block information.
 *
 *  A basic block is a sequence of distinct instructions with linear control flow from the first instruction to the last.  No
 *  edges are permitted to enter or leave the basic block except to the first instruction and from the last instruction,
 *  respectively.  The instructions of a basic block are not required to be contiguous or non-overlapping or at increasing
 *  addresses.
 *
 *  In the absense of interrupt handling, the instructions of a basic block are executed entirely. In the absense of
 *  multi-threading, no other instructions intervene.
 *
 *  A basic block is a read-only object once it reaches the BB_COMPLETE state, and can thus be shared between partitioners and
 *  threads.  The memory for these objects is shared and managed by a shared pointer implementation.
 *
 *  A basic block may belong to multiple functions.
 *
 *  Thread safety: Most of this API is not thread safe, however it is possible to call the non-safe functions as long as no
 *  other thread is modifying the basic block at the same tme. The functions for querying and modifying the semantic state are
 *  thread safe since they're often called from parallel analysis, but no non-safe methods should be called at the same time by
 *  other threads.
 *
 *  See also, @ref SgAsmBlock which is how a basic block (and some other things) are represented in the AST. */
class BasicBlock: public Sawyer::SharedObject, public Sawyer::Attribute::Storage<> {
public:
    /** Shared pointer to a basic block. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<BasicBlock> Ptr;

    /** Basic block successor. */
    class Successor {
    private:
        Semantics::SValuePtr expr_;
        EdgeType type_;
        Confidence confidence_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(expr_);
            s & BOOST_SERIALIZATION_NVP(type_);
            s & BOOST_SERIALIZATION_NVP(confidence_);
        }
#endif

    public: // "protected" fails for boost-1.58.
        // intentionally undocumented; needed for serialization
        Successor()
            : type_(E_USER_DEFINED), confidence_(ASSUMED) {}

    public:
        explicit Successor(const Semantics::SValuePtr &expr, EdgeType type=E_NORMAL, Confidence confidence=ASSUMED)
            : expr_(expr), type_(type), confidence_(confidence) {}

        /** Symbolic expression for the successor address. */
        const Semantics::SValuePtr& expr() const { return expr_; }

        /** Type of successor. */
        EdgeType type() const { return type_; }
        void type(EdgeType t) { type_ = t; }

        /** Confidence level of this successor.  Did we prove that this is a successor, or only assume it is?
         *
         * @{ */
        Confidence confidence() const { return confidence_; }
        void confidence(Confidence c) { confidence_ = c; }
        /** @} */
    };

    /** All successors in no particular order. */
    typedef std::vector<Successor> Successors;

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;

    bool isFrozen_;                                     // True when the object becomes read-only
    rose_addr_t startVa_;                               // Starting address, perhaps redundant with insns_[0]->p_address
    std::string comment_;                               // Mutli-line plain-text comment
    std::vector<SgAsmInstruction*> insns_;              // Instructions in the order they're executed
    BasicBlockSemantics semantics_;                     // All semantics-related information
    std::vector<DataBlock::Ptr> dblocks_;               // Data blocks owned by this basic block, sorted

    // When a basic block gets lots of instructions some operations become slow due to the linear nature of the instruction
    // list. Therefore, we also keep a mapping from instruction address to position in the list. The mapping is only used when
    // the bigBlock size is reached.
    static const size_t bigBlock_ = 200;
    typedef Sawyer::Container::Map<rose_addr_t, size_t> InsnAddrMap;
    InsnAddrMap insnAddrMap_;                           // maps instruction address to index in insns_ vector

    // The following members are caches either because their value is seldom needed and expensive to compute, or because
    // the value is best computed at a higher layer than a single basic block (e.g., in the partitioner) yet it makes the
    // most sense to store it here. Make sure clearCache() resets these to initial values.
    Sawyer::Cached<Successors> successors_;             // control flow successors out of final instruction
    Sawyer::Cached<std::set<rose_addr_t> > ghostSuccessors_;// non-followed successors from opaque predicates, all insns
    Sawyer::Cached<bool> isFunctionCall_;               // is this block semantically a function call?
    Sawyer::Cached<bool> isFunctionReturn_;             // is this block semantically a return from the function?
    Sawyer::Cached<bool> mayReturn_;                    // a function return is reachable from this basic block in the CFG
    Sawyer::Cached<bool> popsStack_;                    // basic block has a net popping effect

    void clearCacheNS() const {
        successors_.clear();
        ghostSuccessors_.clear();
        isFunctionCall_.clear();
        isFunctionReturn_.clear();
        mayReturn_.clear();
        popsStack_.clear();
    }

public:
    void copyCache(const BasicBlock::Ptr &other) {
        ASSERT_not_null(other);
        SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other->mutex_);
        successors_ = other->successors_;
        ghostSuccessors_ = other->ghostSuccessors_;
        isFunctionCall_ = other->isFunctionCall_;
        isFunctionReturn_ = other->isFunctionReturn_;
        mayReturn_ = other->mayReturn_;
        popsStack_ = other->popsStack_;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        //s & boost::serialization::base_object<Sawyer::Attribute::Storage<> >(*this); -- not saved
        s & BOOST_SERIALIZATION_NVP(isFrozen_);
        s & BOOST_SERIALIZATION_NVP(startVa_);
        s & BOOST_SERIALIZATION_NVP(comment_);
        s & BOOST_SERIALIZATION_NVP(insns_);
        s & boost::serialization::make_nvp("dispatcher_", semantics_.dispatcher);
        s & boost::serialization::make_nvp("operators_", semantics_.operators);
        s & boost::serialization::make_nvp("initialState_", semantics_.initialState);
        s & boost::serialization::make_nvp("usingDispatcher_", semantics_.usingDispatcher);
        s & boost::serialization::make_nvp("optionalPenultimateState_", semantics_.optionalPenultimateState);
        s & BOOST_SERIALIZATION_NVP(dblocks_);
        s & BOOST_SERIALIZATION_NVP(insnAddrMap_);
        s & BOOST_SERIALIZATION_NVP(successors_);
        s & BOOST_SERIALIZATION_NVP(ghostSuccessors_);
        s & BOOST_SERIALIZATION_NVP(isFunctionCall_);
        s & BOOST_SERIALIZATION_NVP(isFunctionReturn_);
        s & BOOST_SERIALIZATION_NVP(mayReturn_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(popsStack_);
    }
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    BasicBlock()                                        // needed for serialization
        : isFrozen_(false), startVa_(0) {}

    // use instance() instead
    BasicBlock(rose_addr_t startVa, const Partitioner &partitioner)
        : isFrozen_(false), startVa_(startVa) {
        semantics_.usingDispatcher = true;
        init(partitioner);
    }

public:
    /** Static allocating constructor.
     *
     *  The @p startVa is the starting address for this basic block.  The @p partitioner is the partitioner on whose behalf
     *  this basic block is created.  The partitioner is not stored in the basic block, but is only used to initialize
     *  certain data members of the block (such as its instruction dispatcher). */
    static Ptr instance(rose_addr_t startVa, const Partitioner &partitioner) {
        return Ptr(new BasicBlock(startVa, partitioner));
    }

    /** Virtual constructor.
     *
     *  The @p startVa is the starting address for this basic block.  The @p partitioner is the partitioner on whose behalf
     *  this basic block is created.  The partitioner is not stored in the basic block, but is only used to initialize
     *  certain data members of the block (such as its instruction dispatcher). */
    virtual Ptr create(rose_addr_t startVa, const Partitioner &partitioner) const {
        return instance(startVa, partitioner);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Cache
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Clear all cached data.
     *
     *  Thread safety: This method is thread safe. */
    void clearCache() {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        clearCacheNS();
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Status
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Determine if basic block is read-only.
     *
     *  Returns true if read-only, false otherwise.
     *
     *  Thread safety: This method is thread safe. */
    bool isFrozen() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return isFrozen_;
    }

    /** Comment.
     *
     *  A basic block may have a multi-line, plain-text comment.
     *
     *  Thread safety: This method is not thread safe.
     *
     * @{ */
    const std::string& comment() const { return comment_; }
    void comment(const std::string &s) { comment_ = s; }
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Instructions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Get the address for a basic block.
     *
     *  A basic block's address is also the starting address of its initial instruction.  The initial instruction need not be
     *  the instruction with the lowest address, but rather the instruction which is always executed first by the basic
     *  block.
     *
     *  Thread safety: This method is thread-safe. */
    rose_addr_t address() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return startVa_;
    }

    /** Get all instruction addresses.
     *
     *  The return value is the set of all virtual addresses for both instruction starting addresses and the internal addresses
     *  of instructions.
     *
     *  Thread safety: This method is not thread safe. */
    AddressIntervalSet insnAddresses() const;

    /** Get the address after the end of the final instruction.
     *
     *  This is the address that immediately follows the final byte of the instruction that is executed last by the basic
     *  block.  The final executed instruction need not be the instruction with the highest address.
     *
     *  Thread safety: This method is not thread safe. */
    rose_addr_t fallthroughVa() const;

    /** Get the number of instructions in this block.
     *
     *  Thread safety: This method is thread safe. */
    size_t nInstructions() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return insns_.size();
    }

    /** Return true if this basic block has no instructions.
     *
     *  A basic block is always expected to have at least one instruction whose address is the same as the basic block's
     *  address, and this method returns true if that instruction has not yet been discovered and appended to this basic
     *  block. A basic block may also own data blocks, but they are not counted by this method.
     *
     *  Thread safety: This method is thread-safe. */
    bool isEmpty() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return insns_.empty();
    }

    /** Determine if this basic block contains an instruction at a specific address.
     *
     *  Returns a non-null instruction pointer if this basic block contains an instruction that starts at the specified
     *  address, returns null otherwise.
     *
     *  Thread safety: This method is not thread safe. */
    SgAsmInstruction* instructionExists(rose_addr_t startVa) const;

    /** Determines if this basic block contains the specified instruction.
     *
     *  If the basic block contains the instruction then this method returns the index of this instruction within the
     *  block, otherwise it returns nothing.
     *
     *  Thread safety: This method is not thread safe. */
    Sawyer::Optional<size_t> instructionExists(SgAsmInstruction*) const;

    /** Get the instructions for this block.
     *
     *  Instructions are returned in the order they would be executed (i.e., the order they were added to the block).
     *  Blocks in the undiscovered and not-existing states never have instructions (they return an empty vector); blocks in
     *  the incomplete and complete states always return at least one instruction.
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const std::vector<SgAsmInstruction*>& instructions() const { return insns_; }

    /** Append an instruction to a basic block.
     *
     *  If this is the first instruction then the instruction address must match the block's starting address, otherwise
     *  the new instruction must not already be a member of this basic block.  No other attempt is made to verify the
     *  integrety of the intra-block control flow (i.e., we do not check that the previous instruction had a single
     *  successor which is the newly appended instruction).  It is an error to attempt to append to a frozen block.
     *
     *  The partitioner argument is only used for adjusting the instruction semantics framework for the block.
     *
     *  When adding multiple instructions:
     *
     * @code
     *  BasicBlock::Ptr bb = protoBlock->create(startingVa)
     *      ->append(insn1)->append(p, insn2)->append(p, insn3)
     *      ->freeze();
     * @endcode
     *
     *  Thread safety: This method is not thread safe. */
    void append(const Partitioner&, SgAsmInstruction*);

    /** Undo the latest append.
     *
     *  An append can be undone so that instructions can be appended, the result checked, and then undone.  Only one level
     *  of undo is available.
     *
     *  Thread safety: This method is thread safe. */
    void pop();

    /** Set of explicit constants.
     *
     *  Traverses all the instructions of this basic block and returns all explicit constants found in the instruction
     *  operands. Some architectures call these "immediate values".
     *
     *  Thread safety: This method is thread safe. */
    std::set<rose_addr_t> explicitConstants() const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Static data blocks
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Get the number of data blocks owned.
     *
     *  Thread safety: This method is thread safe. */
    size_t nDataBlocks() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return dblocks_.size();
    }

    /** Addresses that are part of static data.
     *
     *  Returns all addresses that are part of static data.
     *
     *  Thread safety: This method is not thread safe. */
    AddressIntervalSet dataAddresses() const;

    /** Determine if this basic block contains the specified data block or equivalent data block.
     *
     *  If the basic block owns the specified data block or an equivalent data block then this method returns a pointer to the
     *  existing data block, otherwise it returns the null pointer.
     *
     *  Thread safety: This method is not thread safe. */
    DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const;

    /** Make this basic block own the specified data block or equivalent data block.
     *
     *  If the specified data block is not yet owned by this basic block and the basic block contains no equivalent data block,
     *  then the specified data block is added as a member of this basic block and this method returns true. Otherwise, this
     *  basic block already contains the specified data block or an equivalent data block and the method returns false. A data
     *  block cannot be inserted when this basic block is frozen.
     *
     *  Thread safety: This method is not thread safe. */
    bool insertDataBlock(const DataBlock::Ptr&);

    /** Remove specified or equivalent data block from this basic block.
     *
     *  If this basic block is in a detached state (i.e., not part of the CFG/AUM) then the specified data block or equivalent
     *  data block is removed from this basic block. Returns the data block that was erased, or null if none was erased.
     *
     *  It is an error to invoke this method on basic block that is attached to the CFG/AUM, for which @ref isFrozen returns
     *  true. This method is a no-op if the specified data block is a null pointer. */
    DataBlock::Ptr eraseDataBlock(const DataBlock::Ptr&);

    /** Data blocks owned.
     *
     *  Returned vector is sorted according to data block starting address.
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const std::vector<DataBlock::Ptr>& dataBlocks() const { return dblocks_; }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Semantics
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Return information about semantics.
     *
     *  Although most of the returned information can also be obtained by calling individual accessor functions, this function
     *  returns all the information as a single atomic operation.
     *
     *  Thread safety: This method is thread safe. */
    BasicBlockSemantics semantics() const;

    /** Drops semantic information.
     *
     *  This function deletes semantic information for the basic block and can be used to save space.  The partitioner can be
     *  configured to drop semantic information when a basic block is attached to the CFG.  The partitioner is used only for
     *  adjusting the instruction semantics framework for the block.
     *
     *  Thread safety: This method is thread safe. */
    void dropSemantics(const Partitioner&);

    /** Undrop semantics.
     *
     *  This is the inverse of dropSemantics.  If semantics have been dropped then they will be recalculated if possible. If
     *  semantics have not been dropped then nothing happens.  The partitioner is used only for adjusting the instruction
     *  semantics framework for the block.
     *
     *  Thread safety: This method is thread safe. */
    BasicBlockSemantics undropSemantics(const Partitioner&);



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Control flow
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Control flow successors.
     *
     *  The control flow successors indicate how control leaves the end of a basic block. These successors should be the
     *  most basic level of information; e.g., a basic block that results in an unconditional function call should not have
     *  an edge representing the return from that call. The successors are typically computed in the partitioner and cached
     *  in the basic block.
     *
     *  Thread safety: The accessor is not thread safe since it returns a reference; The mutator is thread safe.
     *
     *  @{ */
    const Sawyer::Cached<Successors>& successors() const { return successors_; }
    void successors(const Successors&);
    /** @} */

    /** Ghost successors.
     *
     *  A ghost successor is a control flow successor that is present in an individual instruction, but not present in the
     *  broader scope of a basic block.  Ghost successors typically occur when a conditional branch instruction in the
     *  middle of a basic block has an opaque predicate, causing it to become an unconditional branch.  The return value is
     *  the union of the ghost successors for each instruction in the basic block, and is updated whenever the set of
     *  instructions in the basic block changes.  The ghost successors are typically computed in the partitioner and cached
     *  in the basic block.
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const Sawyer::Cached<std::set<rose_addr_t> >& ghostSuccessors() const { return ghostSuccessors_; }

    /** Insert a new successor.
     *
     *  Inserts a new successor into the cached successor list.  If the successor is already present then it is not added
     *  again (the comparison uses structural equivalance).  Both the expression and the edge type are significant when
     *  comparing. For instance, it is permissible to have a function call edge and a call-return edge that both point to the
     *  fall-through address.
     *
     *  Thread safety: This method is not thread safe.
     *
     *  @{ */
    void insertSuccessor(const BaseSemantics::SValuePtr&, EdgeType type=E_NORMAL, Confidence confidence=ASSUMED);
    void insertSuccessor(rose_addr_t va, size_t nBits, EdgeType type=E_NORMAL, Confidence confidence=ASSUMED);
    /** @} */

    /** Clear all successor information.
     *
     *  Thread safety: This method is thread safe. */
    void clearSuccessors();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Cached properties computed elsewhere
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Is a function call?
     *
     *  If the basic block appears to be a function call then this property is set to true.  A block is a function call if
     *  it appears to store a return value on the stack and then unconditionally branch to a function.  It need not end
     *  with a specific CALL instruction, nor are all CALL instructions actually function calls.  This property is
     *  typically computed in the partitioner and cached in the basic block.
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const Sawyer::Cached<bool>& isFunctionCall() const { return isFunctionCall_; }

    /** Is a function return?
     *
     *  This property indicates whether the basic block appears to be a return from a function call.  A block is a return from
     *  a function call if, after the block is executed, the instruction pointer contains the value stored in memory one past
     *  the top of the stack.
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const Sawyer::Cached<bool>& isFunctionReturn() const { return isFunctionReturn_; }

    /** May-return property.
     *
     *  This property holds a Boolean that indicates whether a function-return basic block is reachable from this basic
     *  block.  In other words, if control enters this basic block, might the top stack frame eventually be popped?
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const Sawyer::Cached<bool>& mayReturn() const { return mayReturn_; }

    /** Pops stack property.
     *
     *  This property holds a Boolean that indicates whether this basic block is known to have a net stack popping effect.
     *
     *  Thread safety: This method is not thread safe since it returns a reference. */
    const Sawyer::Cached<bool>& popsStack() const { return popsStack_; }

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Output
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** A printable name for this basic block.  Returns a string like 'basic block 0x10001234'.
     *
     *  Thread safety: This method is not thread safe. */
    std::string printableName() const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Private members for the partitioner
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    friend class Partitioner;
    void init(const Partitioner&);
    void freeze() { isFrozen_ = true; semantics_.optionalPenultimateState = Sawyer::Nothing(); }
    void thaw() { isFrozen_ = false; }
    BasicBlockSemantics undropSemanticsNS(const Partitioner&);

    // Find an equivalent data block and replace it with the specified data block, or insert the specified data block.
    void replaceOrInsertDataBlock(const DataBlock::Ptr&);
};

} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::BasicBlock, 1);

#endif
