#ifndef ROSE_Partitioner2_Partitioner_H
#define ROSE_Partitioner2_Partitioner_H

#include "sage3basic.h"
#include "InstructionProvider.h"
#include "PartitionerSemantics.h"

#include <sawyer/Callbacks.h>
#include <sawyer/Graph.h>
#include <sawyer/IntervalMap.h>
#include <sawyer/IntervalSet.h>
#include <sawyer/Optional.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace BaseSemantics = rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;


/** Partitions instructions into basic blocks and functions.
 *
 *  A partitioner is responsible for driving a disassembler to obtain instructions, grouping those instructions into basic
 *  blocks, grouping the basic blocks into functions, and building an abstract syntax tree.
 *
 *  The following objects are needed as input:
 *
 * @li A memory map containing the memory for the specimen being analyzed.  Parts of memory that contain instructions must be
 *     mapped with execute permission.  Parts of memory that are readable and non-writable will be considered constant for the
 *     purpose of disassembly and partitioning and can contain things like dynamic linking tables that have been initialized
 *     prior to calling the partitioner.
 *
 * @li A disassembler which is canonical for the specimen architecture and which will return an instruction (possibly an
 *     "unknown" instruction) whenever it is asked to disassemble an address that is mapped with execute permission.  The
 *     partitioner wraps the disassembler and memory map into an InstructionProvider that caches disassembled instructions.
 *
 *  The following data structures are maintained consistently by the partitioner (described in detail later):
 *
 *  @li A control flow graph (CFG) indicating the basic blocks that will become part of the final abstract syntax tree (AST).
 *      The CFG is highly fluid during partitioning, with basic blocks and control flow edges being added and removed.  Since
 *      basic blocks are composed of instructions, the CFG indirectly represents the instructions that will become the AST.
 *
 *  @li An address usage map (AUM), which is a mapping from every address represented in the CFG to the instruction(s) and
 *      their basic blocks.  A single address may have multiple overlapping instructions (although this isn't the usual case),
 *      and every instruction represented by the map belongs to exactly one basic block that belongs to the CFG.
 *
 *  @li Various work lists.  Most built-in work lists are represented by special vertices in the CFG.  For instance, the
 *      "nonexisting" vertex has incoming edges from all basic blocks whose first instruction is not in executable-mapped
 *      memory.  The built-in worklists are unordered, but users can maintain their own worklists that are notified whenever
 *      instructions are added to or erased from the CFG.
 *
 * @section basic_block Basic Blocks
 *
 *  A basic block (BB) is a sequence of distinct instructions that are always executed linearly from beginning to end with no
 *  branching into or out of the middle of the BB.  The semantics of a BB are the composition of the semantics of each
 *  instruction in the order they would be executed.  The instructions of a BB are not required to be contiguous in memory,
 *  although they usually are.
 *
 *  A basic block has a starting address (equivalent to the starting address of its first instruction when its first
 *  instruction is known), and a size measured in instructions.  A basic block's size in bytes is generally not useful since
 *  there is no requirement that the instructions be contiguous in memory.  Basic blocks also store the results of various
 *  analyses that are run when the block is created.
 *
 *  Basic blocks can either be represented in a partitioner's CFG/AUM, or they can exist in a detached state.  Basic blocks in
 *  a detached state can be modified directly via BasicBlock methods, but blocks that are attached to the CFG/AUM are
 *  frozen. Frozen blocks can still be modified in certain ways, but usually only by going through the Partitioner API that
 *  ensures that the CFG/AUM are kept up-to-date.  The CFG/AUM will contain at most one basic block per basic block starting
 *  address.
 *
 *  If the first instruction of a basic block is unmapped or mapped without execute permission then the basic block is said to
 *  be non-existing and will have no instructions.  Such blocks point to the special "nonexisting" CFG vertex when they are
 *  attached to the control flow graph. If a non-initial instruction of a basic block is unmapped or not executable then the
 *  prior instruction becomes the final instruction of the block and the block's successor will be a vertex for a non-existing
 *  basic block which in turn points to the special "nonexisting" CFG vertex.  In other words, a basic block will either
 *  entirely exist or entirely not exist (there are no basic blocks containing instructions that just run off the end of
 *  memory).
 *
 *  If a basic block encounters an address which is mapped with execute permission and properly aligned but the instruction
 *  provider is unable to disassemble an instruction at that address, then the instruction provider must provide an "unknown"
 *  instruction. Since an "unknown" instruction always has indeterminate edges it becomes the final instruction of the basic
 *  block, and the CFG will contain an edge to the special "indeterminate" vertex.  Blocks that have improper alignment are
 *  treated as if they started at an unmapped or non-executable address.
 *
 * @section data_block Data Blocks
 *
 *  A data block is an address and data type anywhere in memory.  A data block can be attached to a CFG/AUM, or exist in a
 *  detached state. The CFG/AUM will contain at most one data block per starting address.  A data block that is attached to the
 *  CFG/AUM is frozen and its address and size cannot be modified directly, although it may still be possible to do so through
 *  the Partitioner API.  A data block is attached to the CFG/AUM by virtue of being owned by a function which is attached to
 *  the CFG/AUM.  A data block may be owned by any number of attached or detached functions. When owned by multiple attached
 *  functions, the resulting ROSE AST will contain multiple SgAsmStaticData IR nodes each having a copy of the same data and
 *  being a child of one of the functions.
 *
 * @section functions Functions
 *
 *  A function is a collection of one or more basic blocks related by control flow edges.  One basic block is special in that
 *  it serves as the only entry point to this function for inter-function edges (usually function calls).  Any edge that leaves
 *  the function must enter a different function's entry block.  These two rules can be relaxed, but result in a control flow
 *  graph that is not proper for a function--most of ROSE's analyses work only on proper control flow graphs.
 *
 *  Functions can either be represented in a partitioner's CFG/AUM, or they can exist in a detached state.  Functions in a
 *  detached state can have their basic block and data block ownership adjusted, otherwise the function exists in a frozen
 *  state to prevent the CFG/AUM from becoming out of date with respect to the function.  Frozen functions can only be modified
 *  through the Partitioner API so that the CFG/AUM can be updated.  When a function becomes detached from the CFG it thaws out
 *  again and can be modified.  The CFG/AUM will contain at most one function per function starting address.
 *
 * @section cfg Control Flow Graph
 *
 *  At any point in time, the partitioner's control flow graph represents those basic blocks (and indirectly the instructions)
 *  that have been selected to appear in the final abstract syntax tree (AST).  This is a subset of all basic blocks ever
 *  created, and a subset of the instructions known to the instruction provider. Note: a final pass during AST construction
 *  might join certain CFG vertices into a single SgAsmBlock under certain circumstances.
 *
 *  Most CFG vertices are either basic block placeholders, or the basic blocks themselves (pointers to BasicBlock objects).  A
 *  placeholder is a basic block starting address without a pointer to an object, and always has exactly one outgoing edge to
 *  the special "undiscovered" vertex.
 *
 *  The CFG has a number of special vertices that don't correspond to a particular address or basic block:
 *
 *  @li "Undiscovered" is a unique, special vertex whose incoming edges originate from placeholder vertices.
 *
 *  @li "Nonexisting" is a unique, special vertex whose incoming edges originate from basic blocks that were discovered to have
 *      an unmapped or non-executable starting address.
 *
 *  @li "Function return" is a unique, special vertex whose incoming edges represent a basic block that is a
 *      return-from-function. Such vertices do not have an edge to the special "indeterminate" vertex.
 *
 *  @li "Indeterminate" is a unique, special vertex whose incoming edges originate from basic blocks whose successors
 *      are not completely known (excluding function returns). Vertices that point to the "indeterminate" vertex might also
 *      point to basic block vertices. For instance, an indirect branch through a memory location which is not mapped or is
 *      mapped with write permission will have an edge to the "indeterminate" vertex.  Unknown instructions (which indicate
 *      that the memory is executable but where the instruction provider could not disassemble anything) have only one edge,
 *      and it points to the "indeterminate" vertex.
 *
 *  CFG vertices representing function calls (i.e., the basic block is marked as being a function call) have an outgoing edge
 *  to the called function if known, and also an outgoing edge to the return point if known and reachable. These edges are
 *  labeled as calls and returns.  CFG vertices representing a function return have a single outgoing edge to the "function
 *  return" CFG vertex. Other vertices with an outgoing inter-function branch are not special (e.g., thunks).
 *
 *  @section fix FIXME[Robb P. Matzke 2014-08-03] 
 *
 *  The partitioner operates in three major phases: CFG-discovery, where basic blocks are discovered and added to the CFG;
 *  function-discovery, where the CFG is partitioned into functions; and AST-building, where the final ROSE abstract syntax
 *  tree is constructed.  The paritioner exposes a low-level API for users that need fine-grained control, and a high-level API
 *  where more things are automated.
 *
 *  During the CFG-discovery phase
 *
 * @section recursion Recursive Disassembly
 *
 *  Recursive disassembly is implemented by processing the "undiscovered" worklist (the vertices with edges to the special
 *  "undiscovered" vertex) until it is empty.  Each iteration obtains a basic block starting address from a placeholder vertex,
 *  creates a BasicBlock object and appends instructions to it until some block termination condition is reached, and inserts
 *  the new basic block into the CFG.  The worklist becoming empty is an indication that the recursion is complete.
 *
 *  The CFG may have orphaned basic blocks (blocks with no incoming edges) which can be recursively removed if desired.
 *  Orphans are created from the addresses that were manually placed on the "undiscovered" worklist and which are not the
 *  target of any known branch.  Orphans can also be created as the CFG evolves.
 *
 * @section linear Linear Disassembly
 *
 *  Linear disassembly can be approximated by running recursive disassembly repeatedly.  Each iteration adds the lowest unused
 *  executable address as a placeholder in the CFG and then runs the recursive disassembly.  Pure linear disassembly does not
 *  use control flow graphs, does not build basic blocks or functions, and is best done by calling the instruction provider or
 *  disassembler directly--it is trivial, there is no need to use a partitioner for this (see linearDisassemble.C in the
 *  projects/BinaryAnalysisTools directory).
 *
 * @section prioritizing Prioritizing Work
 *
 *  A prioritized worklist can be created by using any criteria available to the user.  Such worklists can be created from a
 *  combination of the special vertices (e.g., "undiscovered"), user-defined worklists, searching through the instruction
 *  address map, searching through the memory map, searching through the instruction provider, etc.  The partitioner provides
 *  hooks for tracking when basic blocks and edges are added to or erased from the CFG if the user needs this information to
 *  keep his worklists updated.
 *
 * @section provisional Provisional Detection
 *
 *  Sometimes one wants to ask the question "does a recursive disassembly starting at some particular address look reasonable?"
 *  and avoid making any changes if it doesn't.  This can be accomplished by creating a second "provisional" partitioner which
 *  is either in its initial empty state or a copy of the current partitioner, running the query, and examining the result.
 *  If the result looks reasonable, then the provisional partitioner can be assigned to the current partitioner.
 *
 *  When a partitioner is copied (by the copy constructor or by assignment) it makes a new copy of the CFG and the address
 *  mapping.  The new copy points to the same instructions and basic blocks as the original, but since both of these items are
 *  constant (other than basic block analysis results) they are sharing read-only information.
 *
 *  The cost of copying the CFG is linear in the number of vertices and edges.  The cost of copying the address map is linear
 *  in the number of instructions (or slightly more if instructions overlap).
 *
 *  A more efficient mechanism might be developed in the future.
 *
 * @section functions Function Boundary Determination
 *
 *  Eventually the CFG construction phase of the partitioner will complete, and then the task of partitioning the basic blocks
 *  into functions begins. During function partitioning phase, the CFG is static -- basic blocks, instructions, and edges are
 *  neither inserted nor removed. [FIXME[Robb P. Matzke 2014-07-30]: to be written later] */
class Partitioner {

public:
    /** Type of CFG vertex. */
    enum VertexType {
        V_BASICBLOCK,                                   /**< A basic block or placeholder for a basic block. */
        V_UNDISCOVERED,                                 /**< The special "undiscovered" vertex. */
        V_INDETERMINATE,                                /**< Special vertex destination for indeterminate edges. */
        V_NONEXISTING,                                  /**< Special vertex destination for non-existing basic blocks. */
    };

    /** Type of CFG edge. */
    enum EdgeType {
        E_NORMAL,                                       /**< Normal control flow edge, nothing special. */
        E_FCALL,                                        /**< Edge is a function call. */
        E_FRET,                                         /**< Edge is a function return from the call site. */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Basic blocks (BB)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Basic block information.
     *
     *  A basic block is a sequence of distinct instructions with linear control flow from the first instruction to the last.
     *  No edges are permitted to enter or leave the basic block except to the first instruction and from the last instruction,
     *  respectively.  The instructions of a basic block are not required to be contiguous or non-overlapping.
     *
     *  A basic block is a read-only object once it reaches the BB_COMPLETE state, and can thus be shared between partitioners
     *  and threads.  The memory for these objects is shared and managed by a shared pointer implementation. */
    class BasicBlock: public Sawyer::SharedObject {
    public:
        /** Shared pointer to a basic block. */
        typedef Sawyer::SharedPointer<BasicBlock> Ptr;

        /** Basic block successor. */
        class Successor {
        private:
            Semantics::SValuePtr expr_;
            EdgeType type_;
        public:
            explicit Successor(const Semantics::SValuePtr &expr, EdgeType type=E_NORMAL)
                : expr_(expr), type_(type) {}
            const Semantics::SValuePtr& expr() const { return expr_; }
            EdgeType type() const { return type_; }
        };

        /** All successors in no particular order. */
        typedef std::vector<Successor> Successors;

    private:
        bool isFrozen_;                                 // True when the object becomes read-only
        rose_addr_t startVa_;                           // Starting address, perhaps redundant with insns_[0]->p_address
        std::vector<SgAsmInstruction*> insns_;          // Instructions in the order they're executed
        BaseSemantics::DispatcherPtr dispatcher_;       // How instructions are dispatched (null if no instructions)
        BaseSemantics::StatePtr initialState_;          // Initial state for semantics (null if no instructions)
        bool usingDispatcher_;                          // True if dispatcher's state is up-to-date for the final instruction
        Sawyer::Optional<BaseSemantics::StatePtr> optionalPenultimateState_; // One level of undo information

        // The following members are caches. Make sure clearCache() resets these to initial values.
        mutable Sawyer::Optional<Successors> cachedSuccessors_;
        mutable Sawyer::Optional<bool> cachedIsFunctionCall_;
        mutable BaseSemantics::SValuePtr stackDelta_;   // change in stack pointer if known

    protected:
        // use instance() instead
        BasicBlock(rose_addr_t startVa, const Partitioner *partitioner)
            : isFrozen_(false), startVa_(startVa), usingDispatcher_(true) { init(partitioner); }

    public:
        /** Static allocating constructor.
         *
         *  The @p startVa is the starting address for this basic block.  The @p partitioner is the partitioner on whose behalf
         *  this basic block is created.  The partitioner is not stored in the basic block, but is only used to initialize
         *  certain data members of the block (such as its instruction dispatcher). */
        static Ptr instance(rose_addr_t startVa, const Partitioner *partitioner) {
            return Ptr(new BasicBlock(startVa, partitioner));
        }

        /** Virtual constructor.
         *
         *  The @p startVa is the starting address for this basic block.  The @p partitioner is the partitioner on whose behalf
         *  this basic block is created.  The partitioner is not stored in the basic block, but is only used to initialize
         *  certain data members of the block (such as its instruction dispatcher). */
        virtual Ptr create(rose_addr_t startVa, const Partitioner *partitioner) const {
            return instance(startVa, partitioner);
        }

        /** Mark as read-only. */
        void freeze() {
            isFrozen_ = true;
        }

        /** Determine if basic block is read-only.
         *
         *  Returns true if read-only, false otherwise. */
        bool isFrozen() const { return isFrozen_; }

        /** Get the address for a basic block. */
        rose_addr_t address() const { return startVa_; }

        /** Get the address after the end of the last instruction. */
        rose_addr_t fallthroughVa() const;

        /** Get the number of instructions in this block. */
        size_t nInsns() const { return insns_.size(); }

        /** Return true if this block has no instructions. */
        bool isEmpty() const { return insns_.empty(); }

        /** Append an instruction to a basic block.
         *
         *  If this is the first instruction then the instruction address must match the block's starting address, otherwise
         *  the new instruction must not already be a member of this basic block.  No other attempt is made to verify the
         *  integrety of the intra-block control flow (i.e., we do not check that the previous instruction had a single
         *  successor which is the newly appended instruction).  It is an error to attempt to append to a frozen block.
         *
         *  When adding multiple instructions:
         *
         * @code
         *  BasicBlock::Ptr bb = protoBlock->create(startingVa)
         *      ->append(insn1)->append(insn2)->append(insn3)
         *      ->freeze();
         * @endcode */
        void append(SgAsmInstruction*);

        /** Undo the latest append.
         *
         *  An append can be undone so that instructions can be appended, the result checked, and then undone.  Only one level
         *  of undo is available. */
        void pop();

        /** Get the instructions for this block.
         *
         *  Instructions are returned in the order they would be executed (i.e., the order they were added to the block).
         *  Blocks in the undiscovered and not-existing states never have instructions (they return an empty vector); blocks in
         *  the incomplete and complete states always return at least one instruction. */
        const std::vector<SgAsmInstruction*> instructions() const { return insns_; }

        /** Determine if the basic block contains an instruction at a specific address.
         *
         *  Returns a non-null instruction pointer if this basic block contains an instruction that starts at the specified
         *  address, returns null otherwise. */
        SgAsmInstruction* instructionExists(rose_addr_t startVa) const;

        /** Determines if the basic block contains the specified instruction.
         *
         *  If the basic block contains the instruction then this function returns the index of this instruction within the
         *  block, otherwise it returns nothing. */
        Sawyer::Optional<size_t> instructionExists(SgAsmInstruction*) const;

        /** Return the initial semantic state.
         *
         *  A null pointer is returned if this basic block has no instructions. */
        const BaseSemantics::StatePtr& initialState() const { return initialState_; }

        /** Return the final semantic state.
         *
         *  The returned state is equivalent to starting with the initial state and processing each instruction.  If a semantic
         *  error occurs during processing then the null pointer is returned.  The null pointer is also returned if this basic
         *  block is empty. */
        BaseSemantics::StatePtr finalState();

        /** Return the dispatcher that was used for the semantics.
         *
         *  Dispatchers are specific to the instruction architecture, and also contain a pointer to the register dictionary
         *  that was used.  The register dictionary can be employed to obtain names for the registers in the semantic
         *  states. A null dispatcher is returned if this basic block is empty. */
        const BaseSemantics::DispatcherPtr& dispatcher() const { return dispatcher_; }

        /** Clear analysis cache.
         *
         *  The cache is cleared automatically whenever a new instruction is inserted. */
        void clearCache();

        /** Accessor for the successor cache.
         *  @{ */
        const Sawyer::Optional<Successors>& cachedSuccessors() const { return cachedSuccessors_; }
        const Successors& cacheSuccessors(const Successors &x) const { cachedSuccessors_ = x; return x; }
        void uncacheSuccessors() const { cachedSuccessors_ = Sawyer::Nothing(); }
        bool isCachedSuccessors() const { return bool(cachedSuccessors_); }
        /** @} */

        /** Accessor for isFunctionCall cache.
         *  @{ */
        const Sawyer::Optional<bool>& cachedIsFunctionCall() const { return cachedIsFunctionCall_; }
        bool cacheIsFunctionCall(bool x) const { cachedIsFunctionCall_ = x; return x; }
        void uncacheIsFunctionCall() const { cachedIsFunctionCall_ = Sawyer::Nothing(); }
        bool isCachedIsFunctionCall() const { return bool(cachedIsFunctionCall_); }
        /** @} */

        /** Accessor for the stack delta cache.
         *  @{ */
        const BaseSemantics::SValuePtr cachedStackDelta() const { return stackDelta_; }
        const BaseSemantics::SValuePtr& cacheStackDelta(const BaseSemantics::SValuePtr &d) const { stackDelta_ = d; return d; }
        void uncacheStackDelta() const { stackDelta_ = BaseSemantics::SValuePtr(); }
        bool isCachedStackDelta() const { return stackDelta_ != NULL; }
        /** @} */
        
    private:
        void init(const Partitioner*);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data blocks (DB)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Data block information.
     *
     *  A data block represents data with a type. */
    class DataBlock: public Sawyer::SharedObject {
    public:
        /** Shared pointer to a data block. */
        typedef Sawyer::SharedPointer<DataBlock> Ptr;

    private:
        bool isFrozen_;                                 // true if object is read-only because it's in the CFG
        rose_addr_t startVa_;                           // starting address
        size_t size_;                                   // size in bytes; FIXME[Robb P. Matzke 2014-08-12]: replace with type

    protected:
        // use instance() instead
        DataBlock(rose_addr_t startVa, size_t size): startVa_(startVa), size_(size) {
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
         *  is, when this object is not in a frozen state. */
        void size(size_t nBytes) {
            ASSERT_forbid(isFrozen_);
            ASSERT_require(nBytes > 0);
            size_ = nBytes;
        }

    private:
        friend class Partitioner;
        void freeze() { isFrozen_ = true; }
        void thaw() { isFrozen_ = false; }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Function descriptors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Describes one function.
     *
     *  A function consists of one or more basic blocks.  Exactly one block is special in that it serves as the entry point
     *  when this function is invoked from elsewhere; the only incoming inter-function edges are to this entry block.  This
     *  function may have outgoing inter-function edges that represent invocations of other functions, and the targets of all
     *  such edges will be the entry block of another function.  A function may also own zero or more data blocks consisting of
     *  a base address and size (type).
     *
     *  A function may exist as part of the partitioner's control flow graph, or in a detached state.  When a function is
     *  represented by the control flow graph then it is in a frozen state, meaning that its basic blocks and data blocks
     *  cannot be adjusted adjusted; one must use the partitioner interface to do so. */
    class Function: public Sawyer::SharedObject {
    public:
        /** Manner in which a function owns a block. */
        enum Ownership { OWN_UNOWNED=0,                 /**< Function does not own the block. */
                         OWN_EXPLICIT,                  /**< Function owns the block explicitly, the normal ownership. */
                         OWN_PROVISIONAL,               /**< Function might own the block in the future. */
        };
        typedef Sawyer::SharedPointer<Function> Ptr;
    private:
        rose_addr_t entryVa_;                           // entry address; destination for calls to this function
        std::set<rose_addr_t> bblockVas_;               // addresses of basic blocks
        std::vector<DataBlock::Ptr> dblocks_;           // data blocks owned by this function, sorted by starting address
        bool isFrozen_;                                 // true if function is represented by the CFG
    protected:
        // Use instance() instead
        Function(rose_addr_t entryVa): entryVa_(entryVa), isFrozen_(false) {
            bblockVas_.insert(entryVa);
        }
    public:
        /** Static allocating constructor.  Creates a new function having the specified entry address. */
        static Ptr instance(rose_addr_t entryVa) { return Ptr(new Function(entryVa)); }

        /** Return the entry address.  The entry address also serves as an identifier for the function since the CFG can only
         *  hold one function per entry address.  Detached functions need not have unique entry addresses. */
        rose_addr_t address() const { return entryVa_; }

        /** Returns basic block addresses.  Because functions can exist in a detatched state, a function stores basic block
         *  addresses rather than basic blocks.  This allows a function to indicate which blocks will be ultimately part of its
         *  definition without requiring that the blocks actually exist.  When a detached function is inserted into the CFG
         *  then basic block placeholders will be created for any basic blocks that don't exist in the CFG (see @ref
         *  Partitioner::insertFunction). */
        const std::set<rose_addr_t>& basicBlockAddresses() const { return bblockVas_; }

        /** Add a basic block to this function.  This method does not adjust the partitioner CFG. Basic blocks cannot be added
         *  by this method when this function is attached to the CFG since it would cause the CFG to become outdated with
         *  respect to this function, but as long as the function is detached blocks can be inserted and removed arbitrarily.
         *  If the specified address is already part of the function then it is not added a second time. */
        void insertBasicBlock(rose_addr_t bblockVa) { // no-op if exists
            ASSERT_forbid(isFrozen_);
            bblockVas_.insert(bblockVa);
        }

        /** Remove a basic block from this function.  This method does not adjust the partitioner CFG.  Basic blocks cannot be
         * removed by this method when this function is attached to the CFG since it would cause the CFG to become outdated
         * with respect to this function, but as long as the function is detached blocks can be inserted and removed
         * arbitrarily.  If the specified address is not a basic block address for this function then this is a no-op.
         * Removing the function's entry address is never permitted. */
        void eraseBasicBlock(rose_addr_t bblockVa) {              // no-op if not existing
            ASSERT_forbid(isFrozen_);
            ASSERT_forbid2(bblockVa==entryVa_, "function entry block cannot be removed");
            bblockVas_.erase(bblockVa);
        }

        /** Returns data blocks owned by this function.  Returns the data blocks that are owned by this function in order of
         *  their starting address. */
        const std::vector<DataBlock::Ptr>& dataBlocks() const { return dblocks_; }

        /** Add a data block to this function.  This method does not adjust the partitioner CFG.  Data blocks cannot be added
         *  by this method when this function is attached to the CFG since it would cause the CFG to become outdated with
         *  respect to this function, but as long as the function is detached blocks can be inserted and removed arbitrarily.
         *  The specified data block cannot be a null pointer.  If a data block is already present at the same address then the
         *  specified data block replaces it. */
        void insertDataBlock(const DataBlock::Ptr&);

        /** Remove a data block from this function.  This method does not adjust the partitioner CFG.  Data blocks cannot be
         *  removed by this method when this function is attached to the CFG since it would cause the CFG to become outdated
         *  with respect to this function, but as long as the function is detached blocks can be inserted and removed
         *  arbitrarily.  If the specified pointer is null or the data block does not exist in this function then this method
         *  is a no-op. */
        void eraseDataBlock(const DataBlock::Ptr&);

        /** Determines whether a function is frozen.  The ownership relations (instructions, basic blocks, and data blocks)
         *  cannot be adjusted while a function is in a frozen state.  All functions that are represented in the control flow
         *  graph are in a frozen state; detaching a function from the CFG thaws it. */
        bool isFrozen() const { return isFrozen_; }

        /** Number of basic blocks in the function. */
        size_t nBasicBlocks() const { return bblockVas_.size(); }
    private:
        friend class Partitioner;
        void freeze() { isFrozen_ = true; }
        void thaw() { isFrozen_ = false; }
    };

    typedef Sawyer::Container::Map<rose_addr_t, Function::Ptr> Functions;

    /** Shared reference to data block.  Data blocks can be owned by multiple functions, which is handled by using the @ref
     *  DataBlock::Ptr shared ownership pointers.  However, data blocks can also be owned by multiple functions that are
     *  attached to a CFG, in which case we need to keep track of the number of such owners so that the data block can be
     *  removed from the partitioner's bookkeeping when its last function is detached from the CFG.  The DataBlockReference
     *  objects associate a data block pointer with its owning, CFG-attached functions */
    class OwnedDataBlock {
        DataBlock::Ptr dblock_;                         // the data block, non-null
        std::vector<Function::Ptr> owners_;             // CFG-attached functions that own this block, sorted by address
    public:
        /** Construct a new data block ownership record.  The data block is not owned by any function, so the @ref insert
         *  method must be called soon. The data block must not be null. */
        explicit OwnedDataBlock(const DataBlock::Ptr &dblock): dblock_(dblock) {
            ASSERT_not_null(dblock);
        }

        /** Construct a new data block ownership record.  The data block is marked so that it is owned by this one function.
         *  The data block and owning function must neither be null pointers. */
        OwnedDataBlock(const DataBlock::Ptr &dblock, const Function::Ptr &owner)
            : dblock_(dblock), owners_(1, owner) {
            ASSERT_not_null(dblock);
            ASSERT_not_null(owner);
        }

        /** Add a function owner for this data block.  The specified function must not be null. If the function is already a
         *  member of the block owner list then this method does nothing. Returns the number of owners after adding the
         *  specified function. */
        size_t insert(const Function::Ptr&);

        /** Remove a function owner for this data block.  If the function is a null pointer or the function is not an owner of
         *  the data block then this method does nothing. Returns the number of owners after removing the specified function. */
        size_t erase(const Function::Ptr&);

        /** Returns the list of functions that own this data block. */
        const std::vector<Function::Ptr>& owningFunctions() const { return owners_; }

        /** Returns the number of functions that own this data block. */
        size_t nOwners() const { return owners_.size(); }

        /** Returns the data block for this ownership record. */
        DataBlock::Ptr dblock() const { return dblock_; }
    };

    /** Data blocks by starting address. */
    typedef Sawyer::Container::Map<rose_addr_t, OwnedDataBlock> DataBlocks;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Address usage map (AUM)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Address usage item.
     *
     *  This struct represents one user for an address interval.  The user can be either an instruction with a valid basic
     *  block (since every instruction in the CFG belongs to exactly one basic block in the CFG), or a data block.  Address
     *  usage items are usually ordered by their starting address. */
    class AddressUser {
        SgAsmInstruction *insn_;
        BasicBlock::Ptr bblock_;
        DataBlock::Ptr dblock_;
    public:
        AddressUser(): insn_(NULL) {}                 // needed by std::vector<AddressUser>, but otherwise unused

        /** Constructs new user which is an instruction and its basic block. The instruction must not be the null pointer, but
         *  the basic block may. A null basic block is generally only useful when searching for a particular instruction in an
         *  AddressUsers object. */
        AddressUser(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock): insn_(insn), bblock_(bblock) {
            ASSERT_not_null(insn_);
        }

        /** Constructs a new user which is a data block. The data block must not be the null pointer. */
        AddressUser(const DataBlock::Ptr &dblock): insn_(NULL), dblock_(dblock) {
            ASSERT_not_null(dblock);
        }

        /** Return the non-null pointer to the instruction. */
        SgAsmInstruction* insn() const {
            return insn_;
        }

        /** Change the instruction pointer.  The new pointer cannot be null. */
        void insn(SgAsmInstruction *insn) {
            ASSERT_not_null2(insn_, "this is not an instruction address user");
            ASSERT_not_null(insn);
            insn_ = insn;
        }

        /** Return the non-null pointer to the basic block. */
        BasicBlock::Ptr bblock() const {
            return bblock_;
        }

        /** Change the basic block pointer.  The new pointer cannot be null. */
        void bblock(const BasicBlock::Ptr &bblock) {
            ASSERT_not_null2(insn_, "this is not an instruction address user");
            ASSERT_not_null(bblock);
            bblock_ = bblock;
        }

        /** Return the non-null pointer to the data block. */
        DataBlock::Ptr dblock() const {
            return dblock_;
        }

        /** Change the data block pointer. The new pointer cannot be null. */
        void dblock(const DataBlock::Ptr &dblock) {
            ASSERT_not_null2(dblock_, "this is not a data block address user");
            ASSERT_not_null(dblock);
            dblock_ = dblock;
        }

        /** Determines if this user is a first instruction of a basic block. */
        bool isBlockEntry() const {
            return insn_ && bblock_ && insn_->get_address() == bblock_->address();
        }

        /** Compare two users for equality.  Two pairs are equal if and only if they point to the same instruction and the same
         *  basic block, or they point to the same data block. */
        bool operator==(const AddressUser &other) const {
            return insn_==other.insn_ && bblock_==other.bblock_ && dblock_==other.dblock_;
        }

        /** Compare two users for sorting.  Two users are compared according to their starting addresses.  If two instruction
         *  users have the same starting address then they are necessarily the same instruction (i.e., instruction pointers are
         *  equal), and they necessarily belong to the same basic block (basic block pointers are equal).  However, one or both
         *  of the basic block pointers may be null, which happens when performing a binary search for an instruction when its
         *  basic block is unknown. */
        bool operator<(const AddressUser &other) const { // hot
            if (insn_!=NULL && other.insn_!=NULL) {
                ASSERT_require((insn_!=other.insn_) ^ (insn_->get_address()==other.insn_->get_address()));
                ASSERT_require(insn_!=other.insn_ || bblock_==NULL || other.bblock_==NULL || bblock_==other.bblock_);
                return insn_->get_address() < other.insn_->get_address();
            } else if (insn_!=NULL || other.insn_!=NULL) {
                return insn_==NULL;                     // instructions come before data blocks
            } else {
                ASSERT_not_null(dblock_);
                ASSERT_not_null(other.dblock_);
                return dblock_->address() < other.dblock_->address();
            }
        }

        /** Print the pair on one line. */
        void print(std::ostream&) const;
    };

    /** List of virtual address users.
     *
     *  This is a list of users of virtual addresses.  A user is either an instruction/block pair or a data block pointer. The
     *  list is maintained in a sorted order by increasing instruction and/or data block starting address.  The class ensures
     *  that all users in the list have valid pointers and that the list contains no duplicates. */
    class AddressUsers {
        std::vector<AddressUser> users_;
    public:
        /** Constructs an empty list. */
        AddressUsers() {}

        /** Constructs a list having one instruction user. */
        explicit AddressUsers(SgAsmInstruction *insn, const BasicBlock::Ptr &bb) { insertInstruction(insn, bb); }

        /** Constructs a list having one data block user. */
        explicit AddressUsers(const DataBlock::Ptr &db) { insertDataBlock(db); }

        /** Determines if an instruction exists in the list.
         *
         *  If the instruciton exists then its basic block pointer is returned, otherwise null. */
        BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;

        /** Determines if an instruction exists in the list.
         *
         *  If an instruction with the specified starting address exists in the list then the address user information is
         *  returned, otherwise nothing is returned. */
        Sawyer::Optional<AddressUser> instructionExists(rose_addr_t insnStart) const;

        /** Insert an instruction/basic block pair.
         *
         *  Neither the instruction nor the basic block may be null.  The instruction must not already exist in the
         *  list.  Returns a reference to @p this so that the method call can be chained. */
        AddressUsers& insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

        /** Erase an instruction user.
         *
         *  Erases the specified instruction from the list.  If the instruction is null or the list does not contain the
         *  instruction then this is a no-op. */
        AddressUsers& eraseInstruction(SgAsmInstruction*);

        /** Determines if a data block exists in the list.
         *
         *  Returns true if the specified data block, or a data block with the same starting address, exists in the list. If
         *  one exists then its pointer is returned, otherwise a null pointer is returned. */
        DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const;

        /** Determines if a data block exists in the list.
         *
         *  If a data block with the specified starting address exists in the list then the address user information is
         *  returned, otherwise nothing is returned. */
        Sawyer::Optional<AddressUser> dataBlockExists(rose_addr_t dbStart) const;

        /** Insert a new data block.
         *
         *  The data block must not be null and must not already exist in the list.  Returns a reference to @p this so that the
         *  method can be chained. */
        AddressUsers& insertDataBlock(const DataBlock::Ptr&);

        /** Erase a data block user.
         *
         *  Erases the specified data block from the list.  If the data block is null or the list does not contain the data
         *  block then this is a no-op. */
        AddressUsers& eraseDataBlock(const DataBlock::Ptr&);

        /** Return all address users.
         *
         *  Returns all address users as a vector sorted by starting address. */
        const std::vector<AddressUser>& addressUsers() const { return users_; }

        /** Number of address users. */
        size_t size() const { return users_.size(); }

        /** Determines whether this address user list is empty.
         *
         *  Returns true if empty, false otherwise. */
        bool isEmpty() const { return users_.empty(); }

        /** Computes the intersection of this list with another. */
        AddressUsers intersection(const AddressUsers&) const;

        /** Computes the union of this list with another. */
        AddressUsers union_(const AddressUsers&) const;

        /** True if two lists are equal. */
        bool operator==(const AddressUsers &other) const {
            return users_.size()==other.users_.size() && std::equal(users_.begin(), users_.end(), other.users_.begin());
        }

        /** Prints pairs space separated on a single line. */
        void print(std::ostream&) const;

    protected:
        /** Checks whether the list satisfies all invariants.  This is used in pre- and post-conditions. */
        bool isConsistent() const;
    };

    /** Address usage map.
     *
     *  Keeps track of which instructions and data span each virtual address and are represented by the control flow graph. */
    class AddressUsageMap {
        typedef Sawyer::Container::IntervalMap<AddressInterval, AddressUsers> Map;
        Map map_;
    public:
        /** Determines whether a map is empty.
         *
         *  Returns true if the map contains no instructions or data, false if it contains at least one instruction or at least
         *  one data block. */
        bool isEmpty() const { return map_.isEmpty(); }

        /** Number of addresses represented by the map.
         *
         *  Returns the number of addresses that have at least one user.  This is a constant-time operation. */
        size_t size() const { return map_.size(); }

        /** Minimum and maximum used addresses.
         *
         *  Returns minimum and maximum addresses that exist in this address usage map.  If the map is empty then the returned
         *  interval is empty, containing neither a minimum nor maximum address. */
        AddressInterval hull() const { return map_.hull(); }

        /** Addresses represented.
         *
         *  Returns the set of addresses that are represented. */
        Sawyer::Container::IntervalSet<AddressInterval> extent() const;

        /** Addresses not represented.
         *
         *  Returns the set of addresses that are not represented.  The nBits argument is the number of bits in the virtual
         *  address space, usually 32 or 64, and must be between 1 and 64, inclusive; or an interval can be supplied.
         *
         *  @{ */
        Sawyer::Container::IntervalSet<AddressInterval> unusedExtent(size_t nBits) const;
        Sawyer::Container::IntervalSet<AddressInterval> unusedExtent(const AddressInterval&) const;
        /** @} */

        /** Insert an instruction/block pair into the map.
         *
         *  The specified instruction/block pair is added to the map. The instruction must not already be present in the map. */
        void insertInstruction(SgAsmInstruction*, const BasicBlock::Ptr&);

        /** Remove an instruction from the map.
         *
         *  The specified instruction is removed from the map.  If the pointer is null or the instruction does not exist in the
         *  map, then this is a no-op. */
        void eraseInstruction(SgAsmInstruction*);

        /** Determines whether an instruction exists in the map.
         *
         *  If the instruction exists in the map then a pointer to its basic block is returned, otherwise a null pointer is
         *  returned. */
        BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;

        /** Determines if an address is the start of an instruction.
         *
         *  If the specified address is the starting address of an instruction then the address user information is returned,
         *  otherwise nothing is returned. */
        Sawyer::Optional<AddressUser> instructionExists(rose_addr_t startOfInsn) const;

        /** Determines if an address is the start of a basic block.
         *
         *  If the specified address is the starting address of a basic block then the basic block pointer is returned,
         *  otherwise the null pointer is returned.  A basic block exists only when it has at least one instruction; this is
         *  contrary to the CFG, where a basic block can be represented by a placeholder with no instructions. */
        BasicBlock::Ptr basicBlockExists(rose_addr_t startOfBlock) const;

        /** Insert a data block into the map.  The data block must not be a null pointer and must not already exist in the
         *  map. */
        void insertDataBlock(const DataBlock::Ptr&);

        /** Remove a data block from the map.
         *
         *  The specified data block is removed from the map.  If the pointer is null or the data block does not exist in the
         *  map, then this is a no-op. */
        void eraseDataBlock(const DataBlock::Ptr&);

        /** Determines whether a data block exists in the map.
         *
         *  If a data block exists in the map, or a data block at the same address exists, then returns a pointer to the
         *  existing data block, otherwise returns null. */
        DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const;

        /** Determines if an address is the start of a data block.
         *
         *  If the specified address is the starting address of a data block then the address user information is returned,
         *  otherwise nothing is returned. */
        Sawyer::Optional<AddressUser> dataBlockExists(rose_addr_t startOfBlock) const;

        /** Users that span the entire interval.
         *
         *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where
         *  each user starts at or before the beginning of the interval and ends at or after the end of the interval. */
        AddressUsers spanning(const AddressInterval&) const;

        /** Users that overlap the interval.
         *
         *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where
         *  each user overlaps with the interval.  That is, at least one byte of the instruction or data block came from the
         *  specified interval of byte addresses. */
        AddressUsers overlapping(const AddressInterval&) const;

        /** Users that are fully contained in the interval.
         *
         *  The return value is a vector of address users (instructions and/or data blocks) sorted by starting address where
         *  each user is fully contained within the specified interval.  That is, each user starts at or after the beginning of
         *  the interval and ends at or before the end of the interval. */
        AddressUsers containedIn(const AddressInterval&) const;

        /** Returns the least unmapped address with specified lower limit.
         *
         *  Returns the smallest unmapped address that is greater than or equal to @p startVa.  If no such address exists then
         *  nothing is returned. */
        Sawyer::Optional<rose_addr_t> leastUnmapped(rose_addr_t startVa) const {
            return map_.leastUnmapped(startVa);
        }

        /** Dump the contents of this AUM to a stream.
         *
         *  The output contains one entry per line and the last line is terminated with a linefeed. */
        void print(std::ostream&, const std::string &prefix="") const;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Control flow graph (CFG)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Control flow graph vertex. */
    class CfgVertex {
        friend class Partitioner;
    private:
        VertexType type_;                               // type of vertex, special or not
        rose_addr_t startVa_;                           // address of start of basic block
        BasicBlock::Ptr bblock_;                        // basic block, or null if only a place holder
        Function::Ptr function_;                        // function to which vertex belongs, if any

    public:
        /** Construct a basic block placeholder vertex. */
        explicit CfgVertex(rose_addr_t startVa): type_(V_BASICBLOCK), startVa_(startVa) {}

        /** Construct a basic block vertex. */
        explicit CfgVertex(const BasicBlock::Ptr &bb): type_(V_BASICBLOCK), bblock_(bb) {
            ASSERT_not_null(bb);
            startVa_ = bb->address();
        }

        /** Construct a special vertex. */
        explicit CfgVertex(VertexType type): type_(type), startVa_(0) {
            ASSERT_forbid2(type==V_BASICBLOCK, "this constructor does not create basic block or placeholder vertices");
        }

        /** Returns the vertex type. */
        VertexType type() const { return type_; }

        /** Return the starting address of a placeholder or basic block. */
        rose_addr_t address() const {
            ASSERT_require(V_BASICBLOCK==type_);
            return startVa_;
        }

        /** Return the basic block pointer.  A null pointer is returned when the vertex is only a basic block placeholder. */
        const BasicBlock::Ptr& bblock() const {
            ASSERT_require(V_BASICBLOCK==type_);
            return bblock_;
        }

        /** Return the function pointer.  A basic block may belong to a function, in which case the function pointer is
         * returned. Otherwise the null pointer is returned. */
        const Function::Ptr& function() const {
            ASSERT_require(V_BASICBLOCK==type_);
            return function_;
        }

        /** Turns a basic block vertex into a placeholder.  The basic block pointer is reset to null. */
        void nullify() {
            ASSERT_require(V_BASICBLOCK==type_);
            bblock_ = BasicBlock::Ptr();
        }

    private:
        // Change the basic block pointer.  Users are not allowed to do this directly; they must go through the Partitioner API.
        void bblock(const BasicBlock::Ptr &bb) {
            bblock_ = bb;
        }

        // Change the function pointer.  Users are not allowed to do this directly; they must go through the Partitioner API.
        void function(const Function::Ptr &f) {
            function_ = f;
        }
    };

    /** Control flow graph edge. */
    class CfgEdge {
    private:
        EdgeType type_;
    public:
        CfgEdge(): type_(E_NORMAL) {}
        explicit CfgEdge(EdgeType type): type_(type) {}
        EdgeType type() const { return type_; }
    };

    /** Control flow graph. */
    typedef Sawyer::Container::Graph<CfgVertex, CfgEdge> ControlFlowGraph;

    /** Mapping from basic block starting address to CFG vertex. */
    typedef Sawyer::Container::Map<rose_addr_t, ControlFlowGraph::VertexNodeIterator> VertexIndex;

    /** List of CFG vertex pointers.
     *
     * @{ */
    typedef std::list<ControlFlowGraph::VertexNodeIterator> VertexList;
    typedef std::list<ControlFlowGraph::ConstVertexNodeIterator> ConstVertexList;
    /** @} */

    /** List of CFG edge pointers.
     *
     * @{ */
    typedef std::list<ControlFlowGraph::EdgeNodeIterator> EdgeList;
    typedef std::list<ControlFlowGraph::ConstEdgeNodeIterator> ConstEdgeList;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    InstructionProvider instructionProvider_;           // cache for all disassembled instructions
    const MemoryMap &memoryMap_;                        // description of memory, especially insns and non-writable
    ControlFlowGraph cfg_;                              // basic blocks that will become part of the ROSE AST
    VertexIndex vertexIndex_;                           // Vertex-by-address index for the CFG
    AddressUsageMap aum_;                               // How addresses are used for each address represented by the CFG
    SMTSolver *solver_;                                 // Satisfiable modulo theory solver used by semantic expressions
    mutable size_t progressTotal_;                      // Expected total for the progress bar; initialized at first report
    bool isReportingProgress_;                          // Emit automatic progress reports?
    Functions functions_;                               // List of all attached functions by entry address
    DataBlocks dblocks_;                                // List of all attached data blocks by starting address

    // Special CFG vertices
    ControlFlowGraph::VertexNodeIterator undiscoveredVertex_;
    ControlFlowGraph::VertexNodeIterator indeterminateVertex_;
    ControlFlowGraph::VertexNodeIterator nonexistingVertex_;

public:
    static Sawyer::Message::Facility mlog;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    Partitioner(Disassembler *disassembler, const MemoryMap &map)
        : instructionProvider_(InstructionProvider(disassembler, map)), memoryMap_(map), solver_(NULL),
          progressTotal_(0), isReportingProgress_(true) {
        init();
    }

    static void initDiagnostics();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner CFG queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the instruction provider.
     *  @{ */
    InstructionProvider& instructionProvider() { return instructionProvider_; }
    const InstructionProvider& instructionProvider() const { return instructionProvider_; }
    /** @} */

    /** Returns the memory map. */
    const MemoryMap& memoryMap() const { return memoryMap_; }

    /** Returns the number of bytes represented by the CFG.  This is a constant time operation. */
    size_t nBytes() const { return aum_.size(); }

    /** Returns the number of basic blocks in the CFG. This is a constant-time operation. */
    size_t nBasicBlocks() const { return cfg_.nVertices(); }

    /** Returns the number of data blocks in the CFG.  Data blocks don't belong directly to the CFG in that they're not
     *  vertices or edges, but rather they belong to one or more functions whose basic blocks are CFG vertices.  Regardless of
     *  the indirection, this function returns in constant time. */
    size_t nDataBlocks() const { return dblocks_.size(); }

    /** Returns the number of functions in the CFG.  This is a constant-time operation. */
    size_t nFunctions() const { return functions_.size(); }

    /** Returns the number of instructions in the CFG.  This statistic is computed in time linearly proportional to the number
     *  of basic blocks in the control flow graph. */
    size_t nInstructions() const;

    /** Determines whether an instruction is represented in the CFG.
     *
     *  If the CFG represents an instruction that starts at the specified address, then this method returns the
     *  instruction/block pair, otherwise it returns nothing. The initial instruction for a basic block does not exist if the
     *  basic block is only represented by a placeholder in the CFG. */
    Sawyer::Optional<AddressUser> instructionExists(rose_addr_t startVa) const {
        return aum_.instructionExists(startVa);
    }

    /** Determines whether a basic block or basic block placeholder exists in the CFG.
     *
     *  If the CFG contains a basic block or a placeholder for a basic block that begins at the specified address then the CFG
     *  vertex is returned, otherwise the end vertex is returned.
     *
     *  @{ */
    ControlFlowGraph::VertexNodeIterator placeholderExists(rose_addr_t startVa) {
        if (Sawyer::Optional<ControlFlowGraph::VertexNodeIterator> found = vertexIndex_.getOptional(startVa))
            return *found;
        return cfg_.vertices().end();
    }
    ControlFlowGraph::ConstVertexNodeIterator placeholderExists(rose_addr_t startVa) const {
        if (Sawyer::Optional<ControlFlowGraph::VertexNodeIterator> found = vertexIndex_.getOptional(startVa))
            return *found;
        return cfg_.vertices().end();
    }
    /** @} */

    /** Determines whether a basic block (but not just a placeholder) exists in the CFG.
     *
     *  If the CFG contains a basic block that starts at the specified address then a pointer to the basic block is returned,
     *  otherwise a null pointer is returned.  A null pointer is returned if the CFG contains only a placeholder vertex for a
     *  basic block at the specified address. */
    BasicBlock::Ptr basicBlockExists(rose_addr_t startVa) const {
        ControlFlowGraph::ConstVertexNodeIterator vertex = placeholderExists(startVa);
        if (vertex!=cfg_.vertices().end())
            return vertex->value().bblock();
        return BasicBlock::Ptr();
    }

    /** Determines whether a function exists in the function table.
     *
     *  The function table holds the entry addresses of all known functions.  If the table contains a function for 
     *  specified address then a pointer to the function is returned, otherwise the null function pointer is returned.
     *
     *  @{ */
    Function::Ptr functionExists(rose_addr_t startVa) const {
        return functions_.getOptional(startVa).orDefault();
    }
    Function::Ptr functionExists(const Function::Ptr &function) const {
        return function!=NULL && functions_.exists(function->address()) ? function : Function::Ptr();
    }
    /** @} */

    /** Determines whether a data block exists in the partitioner.  Data blocks are either attached to the CFG (indirectly via
     *  functions), or detached; this method returns only those data blocks that are attached.  If a data block starts at the
     *  specified address then a data block ownership record is returned. The ownership record has a non-null pointer to the
     *  data block along with a list of functions that own the block.  If no attached data block starts at the specified
     *  address then nothing is returned. */
    Sawyer::Optional<OwnedDataBlock> dataBlockExists(rose_addr_t startVa) const {
        return dblocks_.getOptional(startVa);
    }

    /** Returns the special "undiscovered" vertex.
     *
     *  The incoming edges for this vertex originate from the basic block placeholder vertices.
     *
     * @{ */
    ControlFlowGraph::VertexNodeIterator undiscoveredVertex() {
        return undiscoveredVertex_;
    }
    ControlFlowGraph::ConstVertexNodeIterator undiscoveredVertex() const {
        return undiscoveredVertex_;
    }
    /** @} */

    /** Returns the special "indeterminate" vertex.
     *
     *  The incoming edges for this vertex originate from basic blocks whose successors are not all concrete values.  Each such
     *  basic block has only one edge from that block to this vertex.
     *
     *  Indeterminate successors result from, among other things, indirect jump instructions, like x86 "JMP [EAX]".
     *
     * @{ */
    ControlFlowGraph::VertexNodeIterator indeterminateVertex() {
        return indeterminateVertex_;
    }
    ControlFlowGraph::ConstVertexNodeIterator indeterminateVertex() const {
        return indeterminateVertex_;
    }
    /** @} */

    /** Returns the special "non-existing" vertex.
     *
     *  The incoming edges for this vertex originate from basic blocks that have no instructions but which aren't merely
     *  placeholders.  Such basic blocks exist when an attempt is made to discover a basic block but its starting address is
     *  memory which is not mapped or memory which is mapped without execute permission.
     *
     *  @{ */
    ControlFlowGraph::VertexNodeIterator nonexistingVertex() {
        return nonexistingVertex_;
    }
    ControlFlowGraph::ConstVertexNodeIterator nonexistingVertex() const {
        return nonexistingVertex_;
    }
    /** @} */

    /** Returns the control flow graph.
     *
     *  Returns the global control flow graph. The CFG should not be modified by the caller except through the partitioner's
     *  own API. */
    const ControlFlowGraph& cfg() const { return cfg_; }

    /** Returns the address usage map.
     *
     *  Returns the global address usage map.  The AUM should not be modified by the caller except through the paritioner's own
     *  API. */
    const AddressUsageMap& aum() const { return aum_; }

    /** Returns the address usage map for a single function. */
    AddressUsageMap aum(const Function::Ptr&) const;

    /** Returns the list of all attached functions.  Returns a map which maps function entry address to function pointer for
     *  the functions that are part of the control flow graph. */
    const Functions& functions() const { return functions_; }

    /** Returns the list of all attached data blocks.  Returns a map from data block starting address to data block ownership
     *  information for each data block that is represented in the control flow graph.  The ownership information associates
     *  each block with a list of functions that own the block since more than one function can own the same data. */
    const DataBlocks& dataBlocks() const { return dblocks_; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner attached basic block operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Remove basic block information from the CFG.
     *
     *  The basic block (specified by its starting address or CFG vertex) is turned into a placeholder vertex.  That is, the
     *  CFG vertex no longer points to a basic block object but still contains the starting address of the basic block.  The
     *  outgoing edges are modified so that the only outgoing edge is an edge to the special "undiscovered" vertex.  The
     *  instructions that had been pointed to by the basic block are no longer represented in the CFG.
     *
     *  If the CFG does not have a vertex for the specified address, or if the vertex iterator is the end iterator, or if the
     *  vertex is only a placeholder, then this method is a no-op.
     *
     *  In order to completely remove a basic block, including its placeholder, use @ref eraseBasicBlock.
     *
     *  @{ */
    void nullifyBasicBlock(const ControlFlowGraph::VertexNodeIterator&);
    void nullifyBasicBlock(rose_addr_t startVa) {
        nullifyBasicBlock(placeholderExists(startVa));
    }
    /** @} */

    /** Erase all trace of a basic block from the CFG.
     *
     *  The basic block (specified by its starting address or CFG vertex) is entirely removed from the CFG, including its
     *  placeholder.  If the CFG does not have a vertex for the specified address then this method is a no-op.  It is an error
     *  to specify a basic block that has incoming edges.
     *
     *  @{ */
    void eraseBasicBlock(const ControlFlowGraph::VertexNodeIterator&);
    void eraseBasicBlock(rose_addr_t startVa) {
        eraseBasicBlock(placeholderExists(startVa));
    }
    /** @} */

    /** Truncate an existing basic-block.
     *
     *  The specified block is modified so that its final instruction is the instruction immediately prior to the specified
     *  instruction, a new placeholder vertex is created with the address of the specified instruction, and an edge is created
     *  from the truncated block to the new placeholder.  All other outgoing edges of the truncated block are erased.
     *
     *  The specified block must exist and must have the specified instruction as a member.  The instruction must not be the
     *  first instruction of the block.
     *
     *  The return value is the vertex for the new placeholder. */
    ControlFlowGraph::VertexNodeIterator truncateBasicBlock(const ControlFlowGraph::VertexNodeIterator &basicBlock,
                                                            SgAsmInstruction *insn);

    /** Insert a basic-block placeholder.
     *
     *  Inserts a basic block placeholder into the CFG.  A placeholder is the starting address of a basic block and an
     *  outgoing edge to the special "undiscovered" vertex, but no pointer to a basic block object.  If the CFG already has a
     *  vertex with the specified address (discovered or not) then nothing happens.  If the specified address is the starting
     *  address of an instruction that's already in the CFG (but not the start of a basic block) then the existing basic block
     *  is truncated before the placeholder is inserted (see @ref truncateBasicBlock). In any case, the return value is the
     *  new CFG vertex. */
    ControlFlowGraph::VertexNodeIterator insertPlaceholder(rose_addr_t startVa);

    /** Insert a basic block information into the control flow graph.
     *
     *  The specified basic block is inserted into the CFG.  If the CFG already has a placeholder for the block then the
     *  specified block is stored at that placeholder, otherwise a new placeholder is created first.  Once the block is added
     *  to the CFG its outgoing edges are adjusted, which may introduce new placeholders.
     *
     *  @{ */
    void insertBasicBlock(const BasicBlock::Ptr&);
    void insertBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder, const BasicBlock::Ptr&);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner instruction operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Discover an instruction.
     *
     *  Returns (and caches) the instruction at the specified address by invoking an InstructionProvider. */
    SgAsmInstruction* discoverInstruction(rose_addr_t startVa);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner detached basic block operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Discover instructions for a basic block.
     *
     *  Obtain a basic block and its instructions without modifying the control flow graph.  If the basic block already exists
     *  in the CFG then that block is returned, otherwise a new block is created but not added to the CFG. A basic block is
     *  created by adding one instruction at a time until one of the following conditions is met (tested in this order):
     *
     *  @li An instruction could not be obtained from the instruction provider. The instruction provider should only return
     *      null if the address is not mapped or is not mapped with execute permission.  The basic block's final instruction is
     *      the previous instruction, if any.  If the block is empty then it is said to be non-existing, and will have a
     *      special successor when added to the CFG.
     *
     *  @li The instruction is an "unknown" instruction. The instruction provider returns an unknown instruction if it isn't
     *      able to disassemble an instruction at the specified address but the address is mapped with execute permission.  The
     *      partitioner treats this "unknown" instruction as a valid instruction with indeterminate successors.
     *
     *  @li The instruction has a concrete successor address that is an address of a non-initial instruction in this
     *      block. Basic blocks cannot have a non-initial instruction with more than one incoming edge, therefore we've already
     *      added too many instructions to this block.  We could proceed two ways: (A) We could throw away this instruction
     *      with the back-edge successor and make the block terminate at the previous instruction. This causes the basic block
     *      to be as big as possible for as long as possible, which is a good thing if it is determined later that the
     *      instruction with the back-edge is not reachable anyway. (B) We could truncate the basic block at the back-edge
     *      target so that the instruction prior to that is the final instruction. This is good because it converges to a
     *      steady state faster, but could result in basic blocks that are smaller than optimal. (The current algorithm uses
     *      method A.)
     *
     *  @li The instruction causes this basic block to look like a function call.  This instruction becomes the final
     *      instruction of the basic block and when the block is inserted into the CFG the edge will be marked as a function
     *      call edge.
     *
     *  @li The instruction doesn't have exactly one successor. Basic blocks cannot have a non-final instruction that branches,
     *      so this instruction becomes the final instruction.  An additional return-point successor is added.
     *
     *  @li The instruction successor is not a constant. If the successor cannot be resolved to a constant then this
     *      instruction becomes the final instruction.  When this basic block is added to the CFG an edge to the special
     *      "indeterminate" vertex will be created.
     *
     *  @li The instruction successor is the starting address for the block on which we're working. A basic block's
     *      instructions are unique by definition, so this instruction becomes the final instruction for the block.
     *
     *  @li The instruction successor is the starting address of a basic block already in the CFG. This is a common case and
     *      probably means that what we discovered earlier is correct.
     *
     *  @li The instruction successor is an instruction already in the CFG other than in the conflict block.  A "conflict
     *      block" is the basic block, if any, that contains as a non-first instruction the first instruction of this block. If
     *      the first instruction of the block being discovered is an instruction in the middle of some other basic block in
     *      the CFG, then we allow this block to use some of the same instructions as in the conflict block and we do not
     *      terminate construction of this block at this time. Usually what happens is the block being discovered uses all the
     *      final instructions from the conflict block; an exception is when an opaque predicate in the conflicting block is no
     *      longer opaque in the new block.  Eventually when the new block is added to the CFG the conflict block will be
     *      truncated.  When there is no conflict block then this instruction becomes the final instruction of the basic block.
     *
     *  When a basic block is created, various analysis algorithms are run on the block to characterize it.
     *
     *  @{ */
    BasicBlock::Ptr discoverBasicBlock(rose_addr_t startVa);
    BasicBlock::Ptr discoverBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder);
    /** @} */

    /** Determine successors for a basic block.
     *
     *  Basic block successors are returned as a vector in no particular order.  This method returns the most basic successors;
     *  for instance, function call instructions will have an edge for the called function but no edge for the return.  The
     *  basic block holds a successor cache which is consulted/updated by this method.
     *
     *  The basic block need not be complete (this is used during basic block discovery). A basic block that has no
     *  instructions has no successors. */
    BasicBlock::Successors bblockSuccessors(const BasicBlock::Ptr&) const;

    /** Determines concrete successors for a basic block.
     *
     *  Returns a vector of distinct, concrete successor addresses.  Semantics is identical to @ref bblockSuccessors except
     *  non-concrete values are removed from the list. */
    std::vector<rose_addr_t> bblockConcreteSuccessors(const BasicBlock::Ptr &bb) const;

    /** Determine if a basic block looks like a function call.
     *
     *  If the basic block appears to be a function call by some analysis then this function returns true.  The analysis may
     *  use instruction semantics to look at the stack, it may look at the kind of instructions in the block, it may look for
     *  patterns at the callee address if known, etc. The basic block caches the result of this analysis. */
    bool bblockIsFunctionCall(const BasicBlock::Ptr&) const;

    /** Return the stack delta expression.
     *
     *  The stack delta is the difference between the stack pointer register at the end of the block and the stack pointer
     *  register at the beginning of the block.  Returns a null pointer if the information is not available. */
    BaseSemantics::SValuePtr bblockStackDelta(const BasicBlock::Ptr&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner data block methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void insertDataBlock(const Function::Ptr&, rose_addr_t startVa, size_t nBytes);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner attached function methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Inserts functions into the CFG.
     *
     *  The indicated function(s) is inserted into the control flow graph.  Basic blocks (or at least placeholders) are
     *  inserted into the CFG for the function entry address and any basic block addresses the function might already contain.
     *  Returns the number of new basic block placeholders that were created.  If any data blocks are associated with the
     *  function then they are inserted into the AUM.
     *
     *  It is permissible to insert the same function multiple times at the same address (subsequent insertions are no-ops),
     *  but it is an error to insert a different function at the same address as an existing function.
     *
     *  All functions that exist in the function table are marked as frozen. The connectivity of frozen functions can only be
     *  changed by using the partitioner's API, not the function's API.  This allows the partitioner to keep the CFG in a
     *  consistent state.
     *
     *  @{ */
    size_t insertFunction(const Function::Ptr&);
    size_t insertFunctions(const Functions&);
    /** @} */

    /** Create CFG placeholders for functions.
     *
     *  Ensures that a CFG placeholder (or basic block) exists for each function entry address and each function basic block
     *  address.  If a placeholder is absent then one is created by calling @ref insertPlaceholder.  The return value is the
     *  number of new placeholders created.
     *
     *  If the function exists in the CFG (i.e., it is a function that we think is real versus a function that we're
     *  only investigating), then additional actions occur:  any placeholders (or basic blocks) owned by this function are
     *  verified to not be owned by some other function, and they are marked as owned by this function.
     *
     *  @{ */
    size_t insertFunctionBasicBlocks(const Functions&);
    size_t insertFunctionBasicBlocks(const Function::Ptr&);
    /** @} */

    /** Removes a function from the CFG.
     *
     *  The indicated function is removed from the control flow graph and all its basic blocks and data blocks are reset so
     *  they no longer point back to this function.  The function itself is not affected; it still contains the its original
     *  blocks. The function is thawed so that its connectivity is modifiable again with the function's API. */
    void eraseFunction(const Function::Ptr&);

    /** Scans the CFG to find function entry basic blocks.
     *
     *  Scans the CFG to find placeholders (or basic blocks) that are the entry points of functions.  A placeholder is a
     *  function entry if it has an incoming edge that is a function call or if it is the entry block of a known function.
     *  This method does not modify the CFG.  It returns the functions in a map indexed by function entry address. */
    Functions discoverFunctionEntryVertices() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner detached function methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Adds basic blocks to a function.
     *
     *  Attempts to discover the basic blocks that should belong to the specified function.  This is done as follows:
     *
     *  @li An initial CFG traversal follows the non-function-call edges starting at the function's already-owned basic
     *      blocks.  It makes note of any newly encountered blocks, and considers them to be "provisionally owned" by the
     *      function.  If it encounters a vertex already owned by some other function then the ID number for the edge leading
     *      to that vertex is appended to the @p outwardInterFunctionEdges list (if not null), that vertex is not marked as
     *      provisionally owned by this function, and that vertex's outgoing edges are not traversed.
     *
     *  @li A second traversal of the new provisionally-owned vertices (excluding the entry vertex) verifies that all
     *      incoming edges originate from this same function.  If an edge is detected coming from a vertex that is not owned by
     *      this function (explicitly or provisionally) then that edge is appended to the @ref inwardInterFunctionEdges list
     *      (if not null).
     *
     *  @li If there were no conflicts (nothing appended to @p outwardInterFunctionEdges or @p inwardInterFunctionEdges) then a
     *      final traversal of the provisionally-owned vertices adds them to the specified function.
     *
     *  The CFG is not modified by this method, and therefore the function must not exist in the CFG; the function must be in a
     *  thawed state.
     *
     *  The return value is the number of edges inserted (or that would have been inerted) into the two edge list arguments. A
     *  return value other than zero means that conflicts were encountered and the function was not modified.  If a conflict
     *  occurs, the user is permitted to insert the vertices explicitly since this algorithm does not check consistency for
     *  vertices already owned by the function.
     *
     *  @{ */
    size_t discoverFunctionBasicBlocks(const Function::Ptr&,
                                       EdgeList *inwardInterFunctionEdges /*out*/,
                                       EdgeList *outwardInterFunctionEdges /*out*/);
    size_t discoverFunctionBasicBlocks(const Function::Ptr&,
                                       ConstEdgeList *inwardInterFunctionEdges /*out*/,
                                       ConstEdgeList *outwardInterFunctionEdges /*out*/) const;
    size_t discoverFunctionBasicBlocks(const Function::Ptr &function,
                                       std::vector<size_t> &inwardInterFunctionEdges /*out*/,
                                       std::vector<size_t> &outwardInterFunctionEdges /*out*/) const;
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  CFG change callbacks
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Base class for CFG-adjustment callbacks.
     *
     *  Users may create subclass objects from this class and pass their shared-ownership pointers to the partitioner, in which
     *  case the partitioner will invoke one of the callback's virtual function operators every time the control flow graph
     *  changes (the call occurs after the CFG has been adjusted).  Multiple callbacks are allowed; the list is obtained with
     *  the @ref cfgAdjustmentCallbacks method. */
    class CfgAdjustmentCallback: public Sawyer::SharedObject {
    public:
        typedef Sawyer::SharedPointer<CfgAdjustmentCallback> Ptr;

        /** Arguments for inserting a new basic block. */
        struct InsertionArgs {
            Partitioner *partitioner;                                   /**< This partitioner. */
            ControlFlowGraph::VertexNodeIterator insertedVertex;        /**< Vertex that was recently inserted. */
            InsertionArgs(Partitioner *partitioner, const ControlFlowGraph::VertexNodeIterator &insertedVertex)
                : partitioner(partitioner), insertedVertex(insertedVertex) {}
        };

        /** Arguments for erasing a basic block. */
        struct ErasureArgs {
            Partitioner *partitioner;                                   /**< This partitioner. */
            BasicBlock::Ptr erasedBlock;                                /**< Basic block that was recently erased. */
            ErasureArgs(Partitioner *partitioner, const BasicBlock::Ptr &erasedBlock)
                : partitioner(partitioner), erasedBlock(erasedBlock) {}
        };

        /** Insertion callback. This method is invoked after each CFG vertex is inserted (except for special vertices). */
        virtual bool operator()(bool enabled, const InsertionArgs&) = 0;

        /** Erasure callback. This method is invoked after each basic block is removed from the CFG. */
        virtual bool operator()(bool enabled, const ErasureArgs&) = 0;
    };

    /** List of all callbacks invoked when the CFG is adjusted.
     *
     *  @{ */
    typedef Sawyer::Callbacks<CfgAdjustmentCallback::Ptr> CfgAdjustmentCallbacks;
    CfgAdjustmentCallbacks& cfgAdjustmentCallbacks() { return cfgAdjustmentCallbacks_; }
    const CfgAdjustmentCallbacks& cfgAdjustmentCallbacks() const { return cfgAdjustmentCallbacks_; }
    /** @} */

private:
    CfgAdjustmentCallbacks cfgAdjustmentCallbacks_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Instruction/byte pattern matching
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Base class for matching an instruction pattern.
     *
     *  Instruction matchers are generally reference from the partitioner via shared-ownership pointers.  Subclasses must
     *  implement a @ref match method that performs the actual matching. */
    class InstructionMatcher: public Sawyer::SharedObject {
    public:
        /** Shared-ownership pointer. The partitioner never explicitly frees matchers. Their pointers are copied when
         *  partitioners are copied. */
        typedef Sawyer::SharedPointer<InstructionMatcher> Ptr;

        /** Attempt to match an instruction pattern.
         *
         *  If the subclass implementation is able to match instructions, bytes, etc. anchored at the @p anchor address then it
         *  should return true, otherwise false.  The anchor address will always be valid for the situation (e.g., if the
         *  partitioner is trying to match something anchored at an address that is not in the CFG, then the @p anchor will be
         *  such an address; if it is trying to match something that is definitely an instruction then the address will be
         *  mapped with execute permission; etc.).  This precondition makes writing matchers that match against a single
         *  address easier to write, but matchers that match at additional locations must explicitly check those other
         *  locations with the same conditions (FIXME[Robb P. Matzke 2014-08-04]: perhaps we should pass those conditions as an
         *  argument). */
        virtual bool match(Partitioner*, rose_addr_t anchor) = 0;
    };

    /** Base class for matching function prologues.
     *
     *  A function prologue is a pattern of bytes or instructions that typically mark the beginning of a function.  For
     *  instance, many x86-based functions start with "PUSH EBX; MOV EBX, ESP" while many M68k functions begin with a single
     *  LINK instruction affecting the A6 register.  A subclass must implement the @ref match method that does the actual
     *  pattern matching.  If the @ref match method returns true, then the partitioner will call the @ref function method to
     *  obtain a function object.
     *
     *  The matcher will be called only with anchor addresses that are mapped with execute permission and which are not a
     *  starting address of any instruction in the CFG.  The matcher should ensure similar conditions are met for any
     *  additional addresses, especially the address returned by @ref functionVa. */
    class FunctionPrologueMatcher: public InstructionMatcher {
    public:
        /** Shared-ownership pointer. The partitioner never explicitly frees matchers. Their pointers are copied when
         *  partitioners are copied. */
        typedef Sawyer::SharedPointer<FunctionPrologueMatcher> Ptr;

        /** Returns the function for the previous successful match.  If the previous call to @ref match returned true then this
         *  method should return a function for the matched function prologue.  Although the function returned by this method
         *  is often at the same address as the anchor for the match, it need not be.  For instance, a matcher could match
         *  against some amount of padding followed the instructions for setting up the stack frame, in which case it might
         *  choose to return a function that starts at the stack frame setup instructions and includes the padding as static
         *  data. The partitioner will never call @ref function without first having called @ref match. */
        virtual Function::Ptr function() const = 0;
    };

    /** Ordered list of function prologue matchers.
     *
     *  @{ */
    typedef std::vector<FunctionPrologueMatcher::Ptr> FunctionPrologueMatchers;
    FunctionPrologueMatchers& functionPrologueMatchers() { return functionPrologueMatchers_; }
    const FunctionPrologueMatchers& functionPrologueMatchers() const { return functionPrologueMatchers_; }
    /** @} */

    /** Finds the next function by search for a function prologue.
     *
     *  Scans executable memory starting at @p startVa and tries to match a function prologue pattern.  The patterns are
     *  represented by matchers that have been inserted into the vector reference returned by @ref functionPrologueMatchers.
     *  The first matcher that finds an instruction anchored at a supplied starting address wins.  The starting address is
     *  incremented at each step so that it is always an address that is mapped with execute permission and is not an address
     *  that is the start of an instruction that's in the CFG.
     *
     *  If a matcher matches a function prologue then a detached function is created and returned. The starting address
     *  need not be the same as the anchor address for the match.  For instance, a matcher might match one or more no-op
     *  instructions followed by the function prologue, in which case the address after the no-ops is the one used as the
     *  entry point of the returned function.
     *
     *  If no match is found then a null pointer is returned. */
    Function::Ptr nextFunctionPrologue(rose_addr_t startVa);

private:
    FunctionPrologueMatchers functionPrologueMatchers_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner conversion to AST
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Build AST for basic block.
     *
     *  Builds and returns an AST for the specified basic block. The basic block must not be a null pointer, but it need not be
     *  in the CFG.  If the basic block has no instructions then it would violate ROSE's invariants, so a null pointer is
     *  returned instead; however, if @p relaxed is true then an IR node is returned anyway. */
    SgAsmBlock* buildBasicBlockAst(const BasicBlock::Ptr&, bool relaxed=false) const;

    /** Build AST for data block.
     *
     *  Builds and returns an AST for the specified data block.  The data block must not be a null pointer, but it need not be
     *  in the CFG.  If @p relaxed is true then IR nodes are created even if they would violate some ROSE invariant, otherwise
     *  invalid data blocks are ignored and a null pointer is returned for them. */
    SgAsmBlock* buildDataBlockAst(const DataBlock::Ptr&, bool relaxed=false) const;

    /** Build AST for function.
     *
     *  Builds and returns an AST for the specified function.  The function must not be a null pointer, but it need not be in
     *  the CFG.  The function will have children created only for its basic blocks that exist in the CFG (otherwise the
     *  partitioner doesn't know about them).  If no children were created then the returned function IR node violates
     *  ROSE's invariants, so a null pointer is returned instead; however, if @p relaxed is true then an IR node is returned
     *  anyway. */
    SgAsmFunction* buildFunctionAst(const Function::Ptr&, bool relaxed=false) const;

    /** Builds the global block AST.
     *
     *  A global block's children are all the functions contained in the AST, which in turn contain SgAsmBlock IR nodes for the
     *  basic blocks, which in turn contain instructions.  If no functions exist in the CFG then the returned node would
     *  violate ROSE's invariants, so a null pointer is returned instead; however, if @p relaxed is true then the IR node is
     *  returned anyway. */
    SgAsmBlock* buildGlobalBlockAst(bool relaxed=false) const;

    /** Builds an AST from the CFG.
     *
     *  Builds an abstract syntax tree from the control flow graph.  The returned SgAsmBlock will have child functions; each
     *  function (SgAsmFunction) will have child basic blocks; each basic block (SgAsmBlock) will have child instructions.  If
     *  @p relaxed is true then all IR nodes in the returned tree will satisfy ROSE's invariants concerning them at the expense
     *  of not including certain things in the AST; otherwise, when @p relaxed is true, the AST will be as complete as possible
     *  but may violate some invariants.
     *
     *  This function is the same as @ref buildGlobalBlockAst except it also calls various AST fixup functions. Providing an
     *  interpretation allows more fixups to occur. */
    SgAsmBlock* buildAst(SgAsmInterpretation *interp=NULL, bool relaxed=false) const;

    /** Fixes pointers in the AST.
     *
     *  Traverses the AST to find SgAsmIntegerValueExpressions and changes absolute values to relative values.  If such an
     *  expression is the starting address of a function then the expression will point to that function; else if the
     *  expression is the starting address of a basic block then the expression will point to that basic block; else if the
     *  expression is the starting address of an instruction then the expression will point to that instruction; else if the
     *  expression evaluates to an address inside a mapped section, then the expression will become relative to the start of
     *  the best section. Pointers into sections are only created if an interpretation is specified. */
    void fixupAstPointers(SgNode *ast, SgAsmInterpretation *interp=NULL) const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner miscellaneous
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Output the control flow graph.
     *
     *  Emits the control flow graph, basic blocks, and their instructions to the specified stream.  The addresses are starting
     *  addresses, and the suffix "[P]" means the address is a basic block placeholder, and the suffix "[X]" means the basic
     *  block was discovered to be non-existing (i.e., no executable memory for the first instruction).
     *
     *  A @p prefix can be specified to be added to the beginning of each line of output. */
    void dumpCfg(std::ostream&, const std::string &prefix="", bool showBlocks=true) const;

    /** Name of a vertex. */
    static std::string vertexName(const ControlFlowGraph::VertexNode&);

    /** Name of last instruction in vertex. */
    static std::string vertexNameEnd(const ControlFlowGraph::VertexNode&);

    /** Name of an incoming edge. */
    static std::string edgeNameSrc(const ControlFlowGraph::EdgeNode&);

    /** Name of an outgoing edge. */
    static std::string edgeNameDst(const ControlFlowGraph::EdgeNode&);

    /** Name of an edge. */
    static std::string edgeName(const ControlFlowGraph::EdgeNode&);

    /** Name of a function */
    static std::string functionName(const Function::Ptr&);

    /** Enable or disable progress reports.
     *
     *  This controls the automatic progress reports, but the @ref reportProgress method can still be invoked explicitly by the
     *  user to create a report nonetheless.
     *
     *  @{ */
    void enableProgressReports(bool b=true) { isReportingProgress_ = b; }
    void disableProgressReports() { isReportingProgress_ = false; }
    bool isReportingProgress() const { return isReportingProgress_; }
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner internal utilities
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init();
    void reportProgress() const;

    // Obtain a new instruction semantics dispatcher initialized with the partitioner's semantic domain and a fresh state.
    BaseSemantics::DispatcherPtr newDispatcher() const;

    // Adjusts edges for a placeholder vertex. This method erases all outgoing edges for the specified placeholder vertex and
    // then inserts a single edge from the placeholder to the special "undiscovered" vertex. */
    ControlFlowGraph::EdgeNodeIterator adjustPlaceholderEdges(const ControlFlowGraph::VertexNodeIterator &placeholder);

    // Adjusts edges for a non-existing basic block.  This method erases all outgoing edges for the specified vertex and
    // then inserts a single edge from the vertex to the special "non-existing" vertex. */
    ControlFlowGraph::EdgeNodeIterator adjustNonexistingEdges(const ControlFlowGraph::VertexNodeIterator &vertex);

    // Implementation for the discoverBasicBlock methods.  The startVa must not be the address of an existing placeholder.
    BasicBlock::Ptr discoverBasicBlockInternal(rose_addr_t startVa);

    // Checks consistency of internal data structures when debugging is enable (when NDEBUG is not defined).
    void checkConsistency() const;

    // This method is called whenever a new basic block is inserted into the control flow graph. The call happens immediately
    // after the partitioner internal data structures are updated to reflect the insertion.  This call occurs whether a basic
    // block or only a placeholder was inserted.
    virtual void bblockInserted(const ControlFlowGraph::VertexNodeIterator &newVertex);

    // This method is called whenever a non-placeholder basic block is erased from the control flow graph.  The call happens
    // immediately after the partitioner internal data structures are updated to reflect the erasure. The call occurs whether
    // or not a basic block placeholder is left in the graph. */
    virtual void bblockErased(const BasicBlock::Ptr &removedBlock);
    
};

std::ostream& operator<<(std::ostream&, const Partitioner::AddressUser&);
std::ostream& operator<<(std::ostream&, const Partitioner::AddressUsers&);
std::ostream& operator<<(std::ostream&, const Partitioner::AddressUsageMap&);
std::ostream& operator<<(std::ostream&, const Partitioner::ControlFlowGraph::VertexNode&);
std::ostream& operator<<(std::ostream&, const Partitioner::ControlFlowGraph::EdgeNode&);

} // namespace
} // namespace
} // namespace

#endif
