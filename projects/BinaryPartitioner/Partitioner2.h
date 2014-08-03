#ifndef ROSE_Partitioner2_Partitioner_H
#define ROSE_Partitioner2_Partitioner_H

#include "sage3basic.h"
#include "InstructionProvider.h"
#include "SymbolicSemantics2.h"

#include <sawyer/Graph.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Semantics = rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
namespace BaseSemantics = rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;


/** Partitions instructions into basic blocks and functions.
 *
 *  The instruction partitioner uses the following objects as input:
 *
 * @li An instruction provider (InstructionProvider) that associates a single instruction (or lack of instruction) per
 *     instruction starting address.  The instruction provider can either be pre-populated with all known instructions or it
 *     can disassemble one instruction at a time each time a new starting address is queried.  In either case, the same
 *     instruction pointer must be returned each time its starting address is queried (that is, the provider caches
 *     instructions).
 *
 * @li A memory map whose non-writable segments are used as initial memory values during semantic analysis.  The memory map
 *     includes the data that was used to disassemble instructions, and which must be marked as executable (the partitioner
 *     assumes that non-executable memory does not contain instructions).
 *
 *  The following data structures are maintained consistently by the partitioner (described in detail later):
 *
 *  @li A control flow graph (CFG) indicating the basic blocks that will become part of the final abstract syntax tree (AST).
 *      The CFG is highly fluid during partitioning, with basic blocks and control flow edges being added and removed.  Since
 *      basic blocks are composed of instructions, the CFG indirectly represents the instructions that will become the AST.
 *
 *  @li An address usage map (AUM), which is a mapping from every address represented in the CFG to the instruction(s) and
 *      their basic blocks.  A single address may have multiple overlapping instructions (although this isn't the usual case),
 *      and every instruction represented by the map belongs to exactly one basic block that belongs to the CFG.  This is
 *      different than the instruction provider's map because this represents only those instructions that are represented by
 *      the CFG, whereas the instruction providor represents all instructions which have ever been disassembled.
 *
 *  @li Various work lists.  Most work lists are represented by the control flow edges incoming to certain special CFG
 *      vertices.  For instance, the set of all basic block placeholders (vertices where a basic block starting address is all
 *      that's known) can be found by looking at the incoming edges for the special "undiscovered" vertex.   Worklists
 *      implemented this way are unordered, but vertices can be added and removed from them in constant time with no memory
 *      allocation or deallocation.
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
 *  If the first instruction of a basic block is unmapped or mapped without execute permission then the basic block is said to
 *  be non-existing and will have no instructions.  Such blocks always point to the special "nonexisting" CFG vertex (see
 *  below).  If a non-initial instruction of a basic block is unmapped or not executable then the prior instruction becomes the
 *  final instruction of the block and the block's successor will be a vertex for a non-existing basic block which in turn
 *  points to the special "nonexisting" CFG vertex.  In other words, a basic block will either entirely exist or entirely not
 *  exist (there are no basic blocks containing instructions that just run off the end of memory).
 *
 *  If a basic block encounters an address which is mapped with execute permission but the instruction provider is unable to
 *  disassemble an instruction at that address, then the instruction provider must provide an "unknown" instruction. Since an
 *  "unknown" instruction always has indeterminate edges it becomes the final instruction of the basic block.  The CFG will
 *  contain an edge to the special "indeterminate" vertex.
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


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Basic blocks
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

    private:
        bool isFrozen_;                                 // True when the object becomes read-only
        rose_addr_t startVa_;                           // Starting address, perhaps redundant with insns_[0]->p_address
        std::vector<SgAsmInstruction*> insns_;          // Instructions in the order they're executed
        BaseSemantics::DispatcherPtr dispatcher_;       // How instructions are dispatched (null if no instructions)
        BaseSemantics::StatePtr initialState_;          // Initial state for semantics (null if no instructions)
        BaseSemantics::StatePtr finalState_;            // Semantic state after the final instruction (null if invalid)

    protected:
        // use instance() instead
        BasicBlock(rose_addr_t startVa, const Partitioner *partitioner)
            : isFrozen_(false), startVa_(startVa) { init(partitioner); }

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
        virtual rose_addr_t address() const { return startVa_; }

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
        const BaseSemantics::StatePtr& finalState() const { return finalState_; }

        /** Return the dispatcher that was used for the semantics.
         *
         *  Dispatchers are specific to the instruction architecture, and also contain a pointer to the register dictionary
         *  that was used.  The register dictionary can be employed to obtain names for the registers in the semantic
         *  states. A null dispatcher is returned if this basic block is empty. */
        const BaseSemantics::DispatcherPtr& dispatcher() const { return dispatcher_; }

    private:
        void init(const Partitioner*);
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Address usage map
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Instruction/Block pair.
     *
     *  A pointer to an instruction and the basic block to which the instruction belongs.  When an instruction is represented by
     *  the control flow graph, the instruction belongs to exactly one basic block.
     *
     *  Instruction/block pairs are generally sorted by the starting address of the instruction. */
    class InsnBlockPair {
        SgAsmInstruction *insn_;
        BasicBlock::Ptr bblock_;
    public:
        InsnBlockPair(): insn_(NULL) {}                 // needed by std::vector<InsnBlockPair>, but otherwise unused

        /** Constructs new pair with instruction and basic block. The instruction must not be the null pointer, but the basic
         *  block may. A null basic block is generally only useful when searching for a particular instruction in an
         *  InsnBlockPairs object. */
        InsnBlockPair(SgAsmInstruction *insn, const BasicBlock::Ptr &bblock): insn_(insn), bblock_(bblock) {
            ASSERT_not_null(insn_);
        }

        /** Return the non-null pointer to the instruction. */
        SgAsmInstruction* insn() const {
            return insn_;
        }

        /** Change the instruction pointer.  The new pointer cannot be null. */
        void insn(SgAsmInstruction *insn) {
            ASSERT_not_null(insn);
            insn_ = insn;
        }

        /** Return the non-null pointer to the basic block. */
        BasicBlock::Ptr bblock() const {
            return bblock_;
        }

        /** Change the basic block pointer.  The new pointer cannot be null. */
        void bblock(const BasicBlock::Ptr &bblock) {
            ASSERT_not_null(bblock);
            bblock_ = bblock;
        }

        /** Compare two pairs for equality.  Two pairs are equal if and only if they point to the same instruction and the same
         * basic block. */
        bool operator==(const InsnBlockPair &other) const {
            return insn_==other.insn_ && bblock_==other.bblock_;
        }

        /** Compare two pairs for sorting.  Two pairs are compared according to the starting address of their instructions.  If
         *  two instructions have the same starting address then they are necessarily the same instruction (i.e., instruction
         *  pointers are equal), and they necessarily belong to the same basic block (basic block pointers are equal).
         *  However, one or both of the basic block pointers may be null, which happens when performing a binary search for an
         *  instruction when its basic block is unknown. */
        bool operator<(const InsnBlockPair &other) const { // hot
            ASSERT_not_null(insn_);
            ASSERT_not_null(other.insn_);
            ASSERT_require((insn_!=other.insn_) ^ (insn_->get_address()==other.insn_->get_address()));
            ASSERT_require(insn_!=other.insn_ || bblock_==NULL || other.bblock_==NULL || bblock_==other.bblock_);
            return insn_->get_address() < other.insn_->get_address();
        }
    };

    /** List of instruction/block pairs.
     *
     *  This is a list of instruction/block pairs which is maintained in a sorted order (by increasing instruction starting
     *  address).  The class ensures that all pairs in the list have a valid instruction and basic block pointer and that the
     *  list contains no duplicate instructions. */
    class InsnBlockPairs {
        std::vector<InsnBlockPair> pairs_;
    public:
        /** Constructs an empty list. */
        InsnBlockPairs() {}

        /** Constructs a list having one pair. */
        explicit InsnBlockPairs(const InsnBlockPair &pair) { insert(pair); }

        /** Determines if an instruction exists in the list.
         *
         *  If the instruciton exists then its basic block pointer is returned, otherwise null. */
        BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;

        /** Determines if an instruction exists in the list.
         *
         *  If an instruction with the specified address exists in the list then the instruction/block pair is returned,
         *  otherwise nothing is returned. */
        Sawyer::Optional<InsnBlockPair> instructionExists(rose_addr_t insnStart) const;

        /** Insert an instruction/block pair.
         *
         *  The pair must have a valid instruction and a valid block.  The instruction must not already exist in the list.
         *  Returns a reference to this so that the method call can be chained. */
        InsnBlockPairs& insert(const InsnBlockPair&);

        /** Erase an instruction/block pair.
         *
         *  Erases the indicated instruction from the list.  If the instruction is null or the list does not contain the
         *  instruction then this is a no-op. */
        InsnBlockPairs& erase(SgAsmInstruction*);

        /** Return all instruction/block pairs.
         *
         *  Returns all instruction/block pairs as a vector sorted by instruction starting address. */
        const std::vector<InsnBlockPair>& pairs() const { return pairs_; }

        /** Number of instruction/block pairs. */
        size_t size() const { return pairs_.size(); }

        /** Determines whether the instruction/block list is empty.
         *
         *  Returns true if empty, false otherwise. */
        bool isEmpty() const { return pairs_.empty(); }

        /** Computes the intersection of this list with another. */
        InsnBlockPairs intersection(const InsnBlockPairs&) const;

        /** Computes the union of this list with another. */
        InsnBlockPairs union_(const InsnBlockPairs&) const;

        /** True if two lists are equal. */
        bool operator==(const InsnBlockPairs &other) const {
            return pairs_.size()==other.pairs_.size() && std::equal(pairs_.begin(), pairs_.end(), other.pairs_.begin());
        }

    protected:
        /** Checks whether the list satisfies all invariants.  This is used in pre- and post-conditions. */
        bool isConsistent() const;
    };

    /** Address usage map.
     *
     *  Keeps track of which instructions span each virtual address.  This is similar to an @ref InstructionProvider, except
     *  the InstructionProvider keeps track only of instruction starting addresses (there is only one instruction per starting
     *  address). This class on the other hand keeps track of all instructions that cover a particular address regardless of
     *  where the instruction started.  This is especially useful on variable-width instruction architectures since finding the
     *  instructions that overlap a particular address would otherwise entail scanning backward through memory to find all
     *  instructions that are large enough to cover the address in question. */
    class AddressUsageMap {
        typedef Sawyer::Container::IntervalMap<AddressInterval, InsnBlockPairs> Map;
        Map map_;
    public:
        /** Determines whether a map is empty.
         *
         *  Returns true if the map contains no instructions, false if it contains at least one instruction.  An alternative
         *  way to determine if the map is empty is by calling @ref hull and asking if the hull is empty. */
        bool isEmpty() const { return map_.isEmpty(); }

        /** Minimum and maximum instruction addresses.
         *
         *  Returns minimum and maximum addresses that have instructions.  If the map is empty then the returned interval is
         *  empty, containing neither a minimum nor maximum address. */
        AddressInterval hull() const { return map_.hull(); }

        /** Insert an instruction/block pair into the map.
         *
         *  The specified instruction/block pair is added to the map. The instruction must not already be present in the map. */
        void insert(const InsnBlockPair&);

        /** Remove an instruction from the map.
         *
         *  The specified instruction is removed from the map.  If the pointer is null or the instruction does not exist in the
         *  map, then this is a no-op. */
        void erase(SgAsmInstruction*);

        /** Instructions/blocks that span the entire interval.
         *
         *  The return value is a vector of instruction/block pairs sorted by instruction starting address where each instruction
         *  starts at or before the beginning of the interval and ends at or after the end of the interval. */
        InsnBlockPairs spanning(const AddressInterval&) const;

        /** Instructions/blocks that overlap the interval.
         *
         *  The return value is a vector of instruction/block pairs sorted by instruction starting address where each
         *  instruction overlaps with the interval.  That is, at least one byte of the instruction (most instructions are
         *  multiple bytes) came from the specified interval of byte addresses. */
        InsnBlockPairs overlapping(const AddressInterval&) const;

        /** Determines whether an instruction exists in the map.
         *
         *  If the instruction exists in the map then a pointer to its basic block is returned, otherwise a null pointer is
         *  returned. */
        BasicBlock::Ptr instructionExists(SgAsmInstruction*) const;

        /** Determines if an address is the start of an instruction.
         *
         *  If the specified address is the starting address of an instruction then the instruction/block pair is returned,
         *  otherwise nothing is returned. */
        Sawyer::Optional<InsnBlockPair> instructionExists(rose_addr_t startOfInsn) const;

        /** Determines if an address is the start of a basic block.
         *
         *  If the specified address is the starting address of a basic block then the basic block pointer is returned,
         *  otherwise the null pointer is returned.  A basic block exists only when it has at least one instruction; this is
         *  contrary to the CFG, where a basic block can be represented by a placeholder with no instructions. */
        BasicBlock::Ptr bblockExists(rose_addr_t startOfBlock) const;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Control flow graph
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Control flow graph vertex. */
    class CFGVertex {
        friend class Partitioner;
    public:
        enum Type {
            VERTEX_BASICBLOCK,                          /**< A basic block or placeholder for a basic block. */
            VERTEX_UNDISCOVERED,                        /**< The special "undiscovered" vertex. */
            VERTEX_INDETERMINATE,                       /**< Special vertex serving as destination for indeterminate edges. */
        };

    private:
        Type type_;                                     // type of vertex, special or not
        rose_addr_t startVa_;                           // address of start of basic block
        BasicBlock::Ptr bblock_;                        // basic block, or null if only a place holder

    public:
        /** Construct a basic block placeholder vertex. */
        explicit CFGVertex(rose_addr_t startVa): type_(VERTEX_BASICBLOCK), startVa_(startVa) {}

        /** Construct a basic block vertex. */
        explicit CFGVertex(const BasicBlock::Ptr &bb): type_(VERTEX_BASICBLOCK), bblock_(bb) {
            ASSERT_not_null(bb);
            startVa_ = bb->address();
        }

        /** Construct a special vertex. */
        explicit CFGVertex(Type type): type_(type), startVa_(0) {
            ASSERT_forbid2(type==VERTEX_BASICBLOCK, "this constructor does not create basic block or placeholder vertices");
        }

        /** Returns the vertex type. */
        Type type() const { return type_; }

        /** Return the starting address of a placeholder or basic block. */
        rose_addr_t address() const {
            ASSERT_require(VERTEX_BASICBLOCK==type_);
            return startVa_;
        }

        /** Return the basic block pointer.  A null pointer is returned when the vertex is only a basic block placeholder. */
        const BasicBlock::Ptr& bblock() const {
            ASSERT_require(VERTEX_BASICBLOCK==type_);
            return bblock_;
        }

        /** Turns a basic block vertex into a placeholder.  The basic block pointer is reset to null. */
        void nullify() {
            ASSERT_require(VERTEX_BASICBLOCK==type_);
            bblock_ = BasicBlock::Ptr();
        }

    private:
        // Change the basic block pointer.  Users are not allowed to do this directly; they must go through the Partitioner API.
        void bblock(const BasicBlock::Ptr &bb) {
            bblock_ = bb;
        }
    };

    /** Control flow graph edge. */
    struct CFGEdge {};

    /** Control flow graph. */
    typedef Sawyer::Container::Graph<CFGVertex, CFGEdge> ControlFlowGraph;

    /** Mapping from basic block starting address to CFG vertex. */
    typedef Sawyer::Container::Map<rose_addr_t, ControlFlowGraph::VertexNodeIterator> VertexIndex;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    InstructionProvider instructionProvider_;           // cache for all disassembled instructions
    const MemoryMap &memoryMap_;                        // description of memory, especially insns and non-writable
    AddressUsageMap addrUsageMap_;                      // maps addresses to insn/block pairs
    ControlFlowGraph cfg_;                              // basic blocks that will become part of the ROSE AST
    VertexIndex vertexIndex_;                           // vertex-by-address index for the CFG
    SMTSolver *solver_;                                 // Satisfiable modulo theory solver used by semantic expressions

    // Special CFG vertices
    ControlFlowGraph::VertexNodeIterator undiscoveredVertex_;
    ControlFlowGraph::VertexNodeIterator indeterminateVertex_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    Partitioner(Disassembler *disassembler, const MemoryMap &map)
        : instructionProvider_(InstructionProvider(disassembler, map)), memoryMap_(map), solver_(NULL) { init(); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner CFG queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Determines whether an instruction is represented in the CFG.
     *
     *  If the CFG represents an instruction that starts at the specified address, then this method returns the
     *  instruction/block pair, otherwise it returns nothing. The initial instruction for a basic block does not exist if the
     *  basic block is only represented by a placeholder in the CFG. */
    Sawyer::Optional<InsnBlockPair> instructionExists(rose_addr_t startVa) const {
        return addrUsageMap_.instructionExists(startVa);
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
    BasicBlock::Ptr bblockExists(rose_addr_t startVa) const {
        ControlFlowGraph::ConstVertexNodeIterator vertex = placeholderExists(startVa);
        if (vertex!=cfg_.vertices().end())
            return vertex->value().bblock();
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner CFG operations
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
    //                                  Partitioner basic block operations
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
     *  @li The successor address is the starting address for the block on which we're working. A basic block's instructions
     *      are unique by definition, so this instruction becomes the final instruction for the block.
     *
     *  @li The successor address is an address of a (non-initial) instruction in this block. Basic blocks cannot have a
     *      non-initial instruction with more than one incoming edge, therefore we've already added too many instructions to
     *      this block.  We could proceed two ways: (A) We could throw away this instruction with the back-edge successor and
     *      make the block terminate at the previous instruction. This causes the basic block to be as big as possible for as
     *      long as possible, which is a good thing if it is determined later that the instruction with the back-edge is not
     *      reachable anyway. (B) We could truncate the basic block at the back-edge target so that the instruction prior to
     *      that is the final instruction. This is good because it converges to a steady state faster, but could result in
     *      basic blocks that are smaller than optimal. (The current algorithm uses method A.)
     *
     *  @li The successor address is the starting address of a basic block already in the CFG. This is a common case and
     *      probably means that what we discovered earlier is correct.
     *
     *  @li The successor address is an instruction already in the CFG other than in the conflict block.  A "conflict block" is
     *      the basic block, if any, that contains as a non-first instruction the first instruction of this block. If the first
     *      instruction of the block being discovered is an instruction in the middle of some other basic block in the CFG,
     *      then we allow this block to use some of the same instructions as in the conflict block and we do not terminate
     *      construction of this block at this time. Usually what happens is the block being discovered uses all the final
     *      instructions from the conflict block; an exception is when an opaque predicate in the conflicting block is no
     *      longer opaque in the new block.  Eventually when the new block is added to the CFG the conflict block will be
     *      truncated.  When there is no conflict block then this instruction becomes the final instruction of the basic
     *      block.
     *
     *  When a basic block is created, various analysis algorithms are run on the block to characterize it.
     *
     *  @{ */
    BasicBlock::Ptr discoverBasicBlock(rose_addr_t startVa);
    BasicBlock::Ptr discoverBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder);
    /** @} */

    /** Determine successors for a basic block.
     *
     *  Basic block successors are returned as a vector of symbolic expressions in no particular order. The basic block need
     *  not be complete (this is used during basic block discovery). A basic block that has no instructions has no successors. */
    std::vector<Semantics::SValuePtr> successorExpressions(const BasicBlock::Ptr&) const;

    /** Determine if a basic block looks like a function call.
     *
     *  If the basic block appears to be a function call by some analysis then this function returns true.  The analysis may
     *  use instruction semantics to look at the stack, it may look at the kind of instructions in the block, it may look for
     *  patterns at the callee address if known, etc. */
    bool isFunctionCall(const BasicBlock::Ptr&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner worklist adjusters
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** CFG insertion notifier.
     *
     *  This method is called whenever a new basic block is inserted into the control flow graph. The call happens immediately
     *  after the partitioner internal data structures are updated to reflect the insertion.  This call occurs whether a basic
     *  block or only a placeholder was inserted. */
    virtual void bblockInserted(const ControlFlowGraph::VertexNodeIterator &newVertex) {}

    /** CFG erasure notifier.
     *
     *  This method is called whenever a non-placeholder basic block is erased from the control flow graph.  The call happens
     *  immediately after the partitioner internal data structures are updated to reflect the erasure. The call occurs whether
     *  or not a basic block placeholder is left in the graph. */
    virtual void bblockErased(const BasicBlock::Ptr &removedBlock) {}


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner conversion to AST
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmBlock* toAst();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner internal utilities
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init();

    // Obtain a new instruction semantics dispatcher initialized with the partitioner's semantic domain and a fresh state.
    BaseSemantics::DispatcherPtr newDispatcher() const;

    // Adjusts edges for a placeholder vertex. This method erases all outgoing edges for the specified placeholder vertex and
    // then inserts a single edge from the placeholder to the special "undiscovered" vertex. */
    ControlFlowGraph::EdgeNodeIterator adjustPlaceholderEdges(const ControlFlowGraph::VertexNodeIterator &placeholder);

    // Implementation for the discoverBasicBlock methods.  The startVa must not be the address of an existing placeholder.
    BasicBlock::Ptr discoverBasicBlockInternal(rose_addr_t startVa);
};


} // namespace
} // namespace
} // namespace

#endif
