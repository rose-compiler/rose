#ifndef ROSE_Partitioner2_Partitioner_H
#define ROSE_Partitioner2_Partitioner_H

#include <Partitioner2/AddressUsageMap.h>
#include <Partitioner2/Attribute.h>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/FunctionCallGraph.h>
#include <Partitioner2/InstructionProvider.h>
#include <Partitioner2/Modules.h>

#include <sawyer/Callbacks.h>
#include <sawyer/IntervalSet.h>
#include <sawyer/Map.h>
#include <sawyer/Message.h>
#include <sawyer/Optional.h>
#include <sawyer/SharedPointer.h>

#include <ostream>
#include <set>
#include <string>
#include <vector>

namespace rose {
namespace BinaryAnalysis {

/** Binary function detection.
 *
 *  @sa rose::BinaryAnalysis::Partitioner2::Partitioner */
namespace Partitioner2 {

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
 *  detached state. They are attached to the CFG/AUM by virtue of being owned by one or more basic blocks or functions that are
 *  attached to the CFG/AUM. Data blocks such as function alignment are typically attached to a function, while data blocks
 *  such as branch tables are typically attached to a basic block.  A data block may be attached to more than one function
 *  and/or basic block, and the CFG/AUM is able to support multiple data blocks having the same address.
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
class Partitioner: public Attribute::StoredValues {
public:
    typedef Sawyer::Callbacks<CfgAdjustmentCallback::Ptr> CfgAdjustmentCallbacks; /**< See @ref cfgAdjustmentCallbacks. */
    typedef Sawyer::Callbacks<BasicBlockCallback::Ptr> BasicBlockCallbacks; /**< See @ref basicBlockCallbacks. */
    typedef std::vector<FunctionPrologueMatcher::Ptr> FunctionPrologueMatchers; /**< See @ref functionPrologueMatchers. */
    typedef std::vector<FunctionPaddingMatcher::Ptr> FunctionPaddingMatchers; /**< See @ref functionPaddingMatchers. */

    /** Represents information about a thunk. */
    struct Thunk {
        BasicBlock::Ptr bblock;                         /**< The one and only basic block for the thunk. */
        rose_addr_t target;                             /**< The one and only successor for the basic block. */
        Thunk(const BasicBlock::Ptr &bblock, rose_addr_t target): bblock(bblock), target(target) {}
    };

    /** Information about may-return whitelist and blacklist.
     *
     *  The map keys are the function names that are present in the lists. The values are true if the function is whitelisted
     *  and false if blacklisted. */
    typedef Sawyer::Container::Map<std::string, bool> MayReturnList;
    
private:
    InstructionProvider::Ptr instructionProvider_;      // cache for all disassembled instructions
    MemoryMap memoryMap_;                               // description of memory, especially insns and non-writable
    ControlFlowGraph cfg_;                              // basic blocks that will become part of the ROSE AST
    VertexIndex vertexIndex_;                           // Vertex-by-address index for the CFG
    AddressUsageMap aum_;                               // How addresses are used for each address represented by the CFG
    SMTSolver *solver_;                                 // Satisfiable modulo theory solver used by semantic expressions
    mutable size_t progressTotal_;                      // Expected total for the progress bar; initialized at first report
    bool isReportingProgress_;                          // Emit automatic progress reports?
    Functions functions_;                               // List of all attached functions by entry address
    bool useSemantics_;                                 // If true, then use symbolic semantics to reason about things
    MayReturnList mayReturnList_;                       // White/black list by name for whether functions may return to caller
    bool autoAddCallReturnEdges_;                       // Add E_CALL_RETURN edges when blocks are attached to CFG?
    bool assumeFunctionsReturn_;                        // Assume that unproven functions return to caller?

    // Callback lists
    CfgAdjustmentCallbacks cfgAdjustmentCallbacks_;
    BasicBlockCallbacks basicBlockCallbacks_;
    FunctionPrologueMatchers functionPrologueMatchers_;
    FunctionPaddingMatchers functionPaddingMatchers_;

    // Special CFG vertices.
    ControlFlowGraph::VertexNodeIterator undiscoveredVertex_;
    ControlFlowGraph::VertexNodeIterator indeterminateVertex_;
    ControlFlowGraph::VertexNodeIterator nonexistingVertex_;
    static const size_t nSpecialVertices = 3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    Partitioner(Disassembler *disassembler, const MemoryMap &map)
        : memoryMap_(map), solver_(NULL), progressTotal_(0), isReportingProgress_(true), useSemantics_(false),
          autoAddCallReturnEdges_(false), assumeFunctionsReturn_(true) {
        init(disassembler, map);
    }

    // FIXME[Robb P. Matzke 2014-11-08]: This is not ready for use yet.  The problem is that because of the shallow copy, both
    // partitioners are pointing to the same basic blocks, data blocks, and functions.  This is okay by itself since these
    // things are reference counted, but the paradigm of locked/unlocked blocks and functions breaks down somewhat -- does
    // unlocking a basic block from one partitioner make it modifiable even though it's still locked in the other partitioner?
    Partitioner(const Partitioner &other)
        : instructionProvider_(other.instructionProvider_), memoryMap_(other.memoryMap_), cfg_(other.cfg_),
          aum_(other.aum_), solver_(other.solver_), progressTotal_(other.progressTotal_),
          isReportingProgress_(other.isReportingProgress_), functions_(other.functions_), useSemantics_(other.useSemantics_),
          mayReturnList_(other.mayReturnList_), autoAddCallReturnEdges_(other.autoAddCallReturnEdges_),
          assumeFunctionsReturn_(other.assumeFunctionsReturn_), cfgAdjustmentCallbacks_(other.cfgAdjustmentCallbacks_),
          basicBlockCallbacks_(other.basicBlockCallbacks_), functionPrologueMatchers_(other.functionPrologueMatchers_),
          functionPaddingMatchers_(other.functionPaddingMatchers_) {
        init(other);                                    // copies graph iterators, etc.
    }

    static void initDiagnostics();

    /** Reset CFG/AUM to initial state. */
    void clear();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner CFG queries
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the instruction provider.
     *  @{ */
    InstructionProvider& instructionProvider() { return *instructionProvider_; }
    const InstructionProvider& instructionProvider() const { return *instructionProvider_; }
    /** @} */

    /** Returns the memory map.
     *
     *  It is generally unwise to make extensive changes to a memory map after the partitioner has started using it, because
     *  that means that any instructions or data obtained earlier from the map might not be an accurate representation of
     *  memory anymore.
     *
     *  @{ */
    const MemoryMap& memoryMap() const { return memoryMap_; }
    MemoryMap& memoryMap() { return memoryMap_; }
    /** @} */

    /** Returns true if address is executable. */
    bool addressIsExecutable(rose_addr_t va) const { return memoryMap_.at(va).require(MemoryMap::EXECUTABLE).exists(); }

    /** Returns the number of bytes represented by the CFG.  This is a constant time operation. */
    size_t nBytes() const { return aum_.size(); }

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

    /** Determine all ghost successors in the control flow graph.
     *
     *  The return value is a list of basic block ghost successors for which no basic block or basic block placeholder exists.
     *
     *  @sa basicBlockGhostSuccessors */
    std::set<rose_addr_t> ghostSuccessors() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner instruction operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of instructions attached to the CFG/AUM.
     *
     *  This statistic is computed in time linearly proportional to the number of basic blocks in the control flow graph. */
    size_t nInstructions() const;

    /** Determines whether an instruction is attached to the CFG/AUM.
     *
     *  If the CFG/AUM represents an instruction that starts at the specified address, then this method returns the
     *  instruction/block pair, otherwise it returns nothing. The initial instruction for a basic block does not exist if the
     *  basic block is only represented by a placeholder in the CFG; such a basic block is said to be "undiscovered".
     *
     *  @{ */
    Sawyer::Optional<AddressUser> instructionExists(rose_addr_t startVa) const {
        return aum_.instructionExists(startVa);
    }
    Sawyer::Optional<AddressUser> instructionExists(SgAsmInstruction *insn) const {
        return insn==NULL ? Sawyer::Nothing() : instructionExists(insn->get_address());
    }
    /** @} */

    /** Returns instructions that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct instructions that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval. An instruction overlaps the interval if any of its bytes are within the interval.
     *
     *  The returned list of instructions are sorted by their starting address. */
    std::vector<SgAsmInstruction*> instructionsOverlapping(const AddressInterval&) const;

    /** Returns instructions that span an entire address interval.
     *
     *  Returns a sorted list of distinct instructions that are attached to the CFG/AUM and which span the entire specified
     *  interval.  An instruction spans the interval if the set of addresses for all its bytes are a superset of the interval.
     *
     *  The returned list of instructions are sorted by their starting address. */
    std::vector<SgAsmInstruction*> instructionsSpanning(const AddressInterval&) const;

    /** Returns instructions that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct instructions that are attached to the CFG/AUM and which are fully contained within
     *  the specified interval.  In order to be fully contained in the interval, the set of addresses of the bytes in the
     *  instruction must be a subset of the specified interval.
     *
     *  The returned list of instructions are sorted by their starting address. */
    std::vector<SgAsmInstruction*> instructionsContainedIn(const AddressInterval&) const;

    /** Returns the address interval for an instruction.
     *
     *  Returns the minimal interval describing from where the instruction was disassembled.  An instruction always exists in a
     *  contiguous region of memory, therefore the return value is a single interval rather than a set of intervals. If a null
     *  pointer is specified then an empty interval is returned. */
    AddressInterval instructionExtent(SgAsmInstruction*) const;

    /** Discover an instruction.
     *
     *  Returns (and caches) the instruction at the specified address by invoking an InstructionProvider.  Unlike @ref
     *  instructionExists, the address does not need to be known by the CFG/AUM.
     *
     *  If the @p startVa is not mapped with execute permission or is improperly aligned for the architecture then a null
     *  pointer is returned.  If an instruction cannot be disassembled at the address (e.g., bad byte code or not implemented)
     *  then a special 1-byte "unknown" instruction is returned; such instructions have indeterminate control flow successors
     *  and no semantics.  If an instruction was previously returned for this address (including the "unknown" instruction)
     *  then that same instruction will be returned this time. */
    SgAsmInstruction* discoverInstruction(rose_addr_t startVa) const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner basic block placeholder operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of basic basic block placeholders in the CFG.
     *
     *  A placeholder optionally points to a basic block, and this method returns the number of placeholders in the CFG
     *  regardless of whether they point to a discovered basic block.  Note that vertices that are mere placeholders and don't
     *  point to a discovered basic block are not represented in the AUM since a placeholder has no instructions.
     *
     *  This is a constant-time operation. */
    size_t nPlaceholders() const;

    /** Determines whether a basic block placeholder exists in the CFG.
     *
     *  Returns true if the CFG contains a placeholder at the specified address, and false if no such placeholder exists.  The
     *  placeholder may or may not point to a discovered basic block.
     *
     *  @sa findPlaceholder */
    bool placeholderExists(rose_addr_t startVa) const;

    /** Find the CFG vertex for a basic block placeholder.
     *  
     *  If the CFG contains a basic block placeholder at the specified address then that CFG vertex is returned, otherwise the
     *  end vertex (<code>partitioner.cfg().vertices().end()</code>) is returned.
     *
     *  @sa placeholderExists
     *
     *  @{ */
    ControlFlowGraph::VertexNodeIterator findPlaceholder(rose_addr_t startVa) {
        if (Sawyer::Optional<ControlFlowGraph::VertexNodeIterator> found = vertexIndex_.getOptional(startVa))
            return *found;
        return cfg_.vertices().end();
    }
    ControlFlowGraph::ConstVertexNodeIterator findPlaceholder(rose_addr_t startVa) const {
        if (Sawyer::Optional<ControlFlowGraph::VertexNodeIterator> found = vertexIndex_.getOptional(startVa))
            return *found;
        return cfg_.vertices().end();
    }
    /** @} */

    /** Insert a basic-block placeholder.
     *
     *  Inserts a basic block placeholder into the CFG if it does not already exist.
     *
     *  If a new placeholder is inserted, then it represents the starting address of a not-yet-discovered basic block (as far
     *  as the CFG/AUM is concerned), and will contain a single incident edge which goes to the special "undiscovered"
     *  vertex. The new placeholder does not point to a basic block yet.
     *
     *  If the specified address is the starting address of an instruction that's already attached to the CFG/AUM (but not the
     *  start of a basic block) then the existing basic block that owns that instruction is truncated (see @ref
     *  truncateBasicBlock), thereby inserting a new placeholder.
     *
     *  This method returns a pointer to either the existing placeholder (which may already point to an attached basic block)
     *  or the new placeholder. */
    ControlFlowGraph::VertexNodeIterator insertPlaceholder(rose_addr_t startVa);

    /** Remove a basic block placeholder from the CFG/AUM.
     *
     *  The specified placeholder (basic block starting address) is removed from the CFG along with its outgoing edges. If the
     *  placeholder pointed to a basic block then the basic block is detached from the CFG as if @ref detachBasicBlock had been
     *  called.  It is an error to attempt to remove a placeholder that has incoming edges that are not self edges (doing so
     *  will detach the basic block from the CFG/AUM before throwing an exception).
     *
     *  If the placeholder pointed to a discovered basic block then that basic block is returned, otherwise the null pointer is
     *  returned.
     *
     *  @{ */
    BasicBlock::Ptr erasePlaceholder(const ControlFlowGraph::VertexNodeIterator &placeholder);
    BasicBlock::Ptr erasePlaceholder(rose_addr_t startVa);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner basic block operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of basic blocks attached to the CFG/AUM.
     *
     *  This method returns the number of CFG vertices that are more than mere placeholders in that they point to an actual,
     *  discovered basic block.
     *
     *  This operation is linear in the number of vertices in the CFG.  Consider using @ref nPlaceholders instead. */
    size_t nBasicBlocks() const;

    /** Returns all basic blocks attached to the CFG.
     *
     *  The returned list contains distinct basic blocks sorted by their starting address.
     *
     * @sa basicBlocksOverlapping, @ref basicBlocksSpanning, @ref basicBlocksContainedIn */
    std::vector<BasicBlock::Ptr> basicBlocks() const;

    /** Determines whether a discovered basic block exists in the CFG/AUM.
     *
     *  If the CFG/AUM contains a basic block that starts at the specified address then a pointer to that basic block is
     *  returned, otherwise a null pointer is returned.  A null pointer is returned if the CFG contains only a placeholder
     *  vertex for a basic block at the specified address.
     *
     *  If a basic block pointer is specified instead of an address, the return value will be the same pointer if the specified
     *  basic block is attached to the CFG/AUM, otherwise the null pointer is returned.  It is not sufficient for the CFG/AUM
     *  to contain a basic block at the same starting address -- it must be the same actual basic block object.  If you're only
     *  looking for a similar (i.e., starting at the same address) basic block then use the version that takes an address:
     *
     * @code
     *  BasicBlock::Ptr original = ...;
     *  BasicBlock::Ptr similar = basicBlockExists(original->address());
     * @endcode
     *
     *  @sa placeholderExists
     *
     *  @{ */
    BasicBlock::Ptr basicBlockExists(rose_addr_t startVa) const;
    BasicBlock::Ptr basicBlockExists(const BasicBlock::Ptr&) const;
    /** @} */

    /** Returns basic blocks that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct basic blocks that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval.  By "overlap" we mean that the basic block has at least one instruction that overlaps
     *  with the specified interval.  An instruction overlaps the interval if any of its bytes are within the interval.
     *
     *  The returned list of basic blocks are sorted by their starting address. */
    std::vector<BasicBlock::Ptr> basicBlocksOverlapping(const AddressInterval&) const;

    /** Returns basic blocks that span an entire address interval.
     *
     *  Returns a sorted list of distinct basic blocks that are attached to the CFG/AUM and which span the entire specified
     *  interval. In order for a basic block to span an interval its set of instructions must span the interval.  In other
     *  words, the union of the addresses of the bytes contained in all the basic block's instructions is a superset of the
     *  specified interval.
     *
     *  The returned list of basic blocks are sorted by their starting address. */
    std::vector<BasicBlock::Ptr> basicBlocksSpanning(const AddressInterval&) const;

    /** Returns basic blocks that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct basic blocks that are attached to the CFG/AUM and which are fully contained within
     *  the specified interval.  In order to be fully contained in the interval, the union of the addresses of the bytes in the
     *  basic block's instructions must be a subset of the specified interval.
     *
     *  The returned list of basic blocks are sorted by their starting address. */
    std::vector<BasicBlock::Ptr> basicBlocksContainedIn(const AddressInterval&) const;

    /** Returns the basic block that contains a specific instruction address.
     *
     *  Returns the basic block that contains an instruction that starts at the specified address, or null if no such
     *  instruction or basic block exists in the CFG/AUM. */
    BasicBlock::Ptr basicBlockContainingInstruction(rose_addr_t insnVa) const;

    /** Returns the addresses used by basic block instructions.
     *
     *  Returns an interval set which is the union of the addresses of the bytes in the basic block's instructions.  Most
     *  basic blocks are contiguous in memory and can be represented by a single address interval, but this is not a
     *  requirement in ROSE.  ROSE only requires that the global control flow graph has edges that enter at only the initial
     *  instruction of the basic block and exit only from its final instruction.  The instructions need not be contiguous or
     *  non-overlapping. */
    AddressIntervalSet basicBlockInstructionExtent(const BasicBlock::Ptr&) const;

    /** Returns the addresses used by basic block data.
     *
     *  Returns an interval set which is the union of the extents for each data block referenced by this basic block. */
    AddressIntervalSet basicBlockDataExtent(const BasicBlock::Ptr&) const;

    /** Detach a basic block from the CFG/AUM.
     *
     *  The specified basic block is detached from the CFG/AUM, leaving only a placeholder in its place.  The original outgoing
     *  edges in the CFG are replaced by a single edge from the placeholder to the special "undiscovered" vertex.  The
     *  instructions that had been attached to the CFG/AUM on behalf of the basic block are also detached from the CFG/AUM.
     *
     *  Any data blocks owned by this attached basic block will have their ownership counts decremented, and those data blocks
     *  whose attached owner counts reach zero are detached from the CFG/AUM.
     *
     *  This function does not modify the basic block itself; it only detaches it from the CFG/AUM.  A basic block that is
     *  attached to the CFG/AUM is in a frozen state and cannot be modified directly, so one use of this function is to allow
     *  the user to modify a basic block and then re-attach it to the CFG/AUM.  Detaching an already-detached basic block is a
     *  no-op.
     *
     *  This method returns a pointer to the basic block so it can be manipulated by the user after it is detached.  If the
     *  user specified a basic block pointer to start with, then the return value is this same pointer; this function does
     *  nothing if the basic block was already detached. If the basic block was specified by its starting address and the
     *  CFG/AUM has no record of such a block then a null pointer is returned.
     *
     *  In order to completely remove a basic block, including its placeholder, use @ref eraseBasicBlock.
     *
     *  @{ */
    BasicBlock::Ptr detachBasicBlock(rose_addr_t startVa);
    BasicBlock::Ptr detachBasicBlock(const BasicBlock::Ptr &basicBlock);
    BasicBlock::Ptr detachBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder);
    /** @} */

    /** Truncate an attached basic-block.
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

    /** Attach a basic block to the CFG/AUM.
     *
     *  The specified basic block is inserted into the CFG/AUM.  If the CFG already has a placeholder for the block then the
     *  specified block is stored at that placeholder, otherwise a new placeholder is created first.  A basic block cannot be
     *  attached if the CFG/AUM already knows about a different basic block at the same address.  Attempting to attach a block
     *  which is already attached is allowed, and is a no-op. It is an error to specify a null pointer for the basic block.
     *
     *  The basic block's cached successors are consulted when creating the new edges in the CFG.  The block's successor types
     *  are used as-is except for the following modifications:
     *
     *  @li If the basic block is a function call and none of the successors are labeled as function calls (@ref
     *      E_FUNCTION_CALL) then all normal successors (@ref E_NORMAL) create function call edges in the CFG.
     *
     *  @li If the basic block is a function call and it has no call-return successor (@ref E_CALL_RETURN) and this
     *      partitioner's autoAddCallReturnEdges property is true then a may-return analysis is performed on each callee and a
     *      call-return edge is added if any callee could return.  If the analysis is indeterminate then an edge is added if
     *      this partitioner's assumeCallsReturn property is true.
     *
     *  @li If the basic block is a function return and has no function return successor (@ref E_FUNCTION_RETURN, which
     *      normally is a symbolic address) then all normal successors (@ref E_NORMAL) create function return edges in the
     *      CFG.
     *
     *  New placeholder vertices will be created automatically for new CFG edges that don't target an existing vertex.
     *
     *  A placeholder can be specified for better efficiency, in which case the placeholder must have the same address as the
     *  basic block.
     *
     *  @{ */
    void attachBasicBlock(const BasicBlock::Ptr&);
    void attachBasicBlock(const ControlFlowGraph::VertexNodeIterator &placeholder, const BasicBlock::Ptr&);
    /** @} */

    /** Discover instructions for a detached basic block.
     *
     *  Obtains a basic block and its instructions without modifying the control flow graph.  If the basic block already exists
     *  in the CFG/AUM then that block is returned, otherwise a new block is created but not added to the CFG/AUM. A basic
     *  block is created by adding one instruction at a time until one of the following conditions is met (tested in this
     *  order):
     *
     *  @li An instruction could not be obtained from the instruction provider via @ref discoverInstruction. The instruction
     *      provider should return null only if the address is not mapped with execute permission or is improperly aligned for
     *      the architecture.  The basic block's final instruction is the previous instruction, if any.  If the block is empty
     *      then it is said to be non-existing, and will have a special successor when added to the CFG.
     *
     *  @li The instruction is an "unknown" instruction. The instruction provider returns an unknown instruction if it isn't
     *      able to disassemble an instruction at the specified address but the address is mapped with execute permission and
     *      the address was properly aligned.  The partitioner treats this "unknown" instruction as a valid instruction with
     *      indeterminate successors and no semantics.
     *
     *  @li Note: at this point the user-defined basic block successors callbacks are invoked. They can query characteristics
     *      of the basic block, adjust the basic block successor cache and other characteristics of the block, and write values
     *      into a results structure that is used by some of the subsequent block termination conditions.
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
     *  @li The user-defined basic block callbacks indicated that the block should be terminated.  If the successors set the @c
     *      terminate member of the @c results output argument to @ref BasicBlockCallback::TERMINATE_NOW or @ref
     *      BasicBlockCallback::TERMINATE_PRIOR, then the current instruction either becomes the final instruction of the basic
     *      block, or the prior instruction becomes the final instruction.
     *
     *  @li The instruction causes this basic block to look like a function call.  This instruction becomes the final
     *      instruction of the basic block and when the block is inserted into the CFG/AUM the edge will be marked as a
     *      function call edge.  Function call instructions typically have one successor (the target function, usually
     *      concrete, but sometimes indeterminate), but the partitioner may eventually insert a "return" edge into the CFG when
     *      this basic block is attached.
     *
     *  @li The instruction doesn't have exactly one successor. Basic blocks cannot have a non-final instruction that branches,
     *      so this instruction becomes the final instruction.
     *
     *  @li The instruction successor is not a constant. If the successor cannot be resolved to a constant then this
     *      instruction becomes the final instruction.  If this basic block is eventually attached to the CFG/AUM then an edge
     *      to the special "indeterminate" vertex will be created.
     *
     *  @li The instruction successor is the starting address for the block on which we're working. A basic block's
     *      instructions are distinct by definition, so this instruction becomes the final instruction for the block.
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
     *      longer opaque in the new block.  Eventually if the new block is attached to the CFG/AUM then the conflict block
     *      will be truncated.  When there is no conflict block then this instruction becomes the final instruction of the
     *      basic block.
     *
     *  @{ */
    BasicBlock::Ptr discoverBasicBlock(rose_addr_t startVa) const;
    BasicBlock::Ptr discoverBasicBlock(const ControlFlowGraph::ConstVertexNodeIterator &placeholder) const;
    /** @} */

    /** Determine successors for a basic block.
     *
     *  Basic block successors are returned as a vector in no particular order.  This method returns the most basic successors;
     *  for instance, function call instructions will have an edge for the called function but no edge for the return.  The
     *  basic block holds a successor cache which is consulted/updated by this method.
     *
     *  The basic block need not be complete or attached to the CFG/AUM. A basic block that has no instructions has no
     *  successors. */
    BasicBlock::Successors basicBlockSuccessors(const BasicBlock::Ptr&) const;

    /** Determines concrete successors for a basic block.
     *
     *  Returns a vector of distinct, concrete successor addresses.  Semantics is identical to @ref bblockSuccessors except
     *  non-concrete values are removed from the list.  The optional @p isComplete argument is set to true or false depending
     *  on whether the set of returned concrete successors represents the complete set of successors (true) or some member in
     *  the complete set is not concrete (false). */
    std::vector<rose_addr_t> basicBlockConcreteSuccessors(const BasicBlock::Ptr&, bool *isComplete=NULL) const;

    /** Determine ghost successors for a basic block.
     *
     *  The ghost successors of a basic block are those addresses where control could have naively flowed had we looked only at
     *  individual instructions rather than entire basic blocks.  When a whole basic block is examined, the predicate of a
     *  conditional branch instruction might be determined to be constant, in which case the branch becomes unconditional, and
     *  the non-taken side of the branch becomes a ghost successor.  Ghost successors are addresses rather than basic blocks
     *  (although they can be easily turned into basic blocks if desired), and can originate from any instruction within a
     *  basic block.
     *
     *  The basic block need not be complete and need not be attached to a CFG/AUM, although the specified pointer must not be
     *  null.  A basic block that has no instructions has no ghost successors.  The true successors are not included in the
     *  list of ghost successors.  The basic block holds a ghost successor cache which is consulted/updated by this method.
     *
     *  @todo Perhaps we need to represent these as edges rather than successors so that we also know which instruction they're
     *  originating from since they can originate from anywhere in the basic block. */
    std::set<rose_addr_t> basicBlockGhostSuccessors(const BasicBlock::Ptr&) const;

    /** Determine if a basic block looks like a function call.
     *
     *  If the basic block appears to be a function call by some analysis then this function returns true.  The analysis may
     *  use instruction semantics to look at the stack, it may look at the kind of instructions in the block, it may look for
     *  patterns at the callee address if known, etc. The basic block caches the result of this analysis. */
    bool basicBlockIsFunctionCall(const BasicBlock::Ptr&) const;

    /** Determine if a basic block looks like a function return.
     *
     *  If the basic block appears by some analysis to be a return from a function call, then this function returns true.  The
     *  anlaysis may use instruction semantics to look at the stack, it may look at the kind of instructions in the lbock, it
     *  may look for patterns, etc.  The basic block caches the result of this analysis.
     *
     *  @todo Partitioner::basicBlockIsFunctionReturn does not currently detect callee-cleanup returns because the return
     *  address is not the last thing popped from the stack. FIXME[Robb P. Matzke 2014-09-15] */
    bool basicBlockIsFunctionReturn(const BasicBlock::Ptr&) const;

    /** Return the stack delta expression.
     *
     *  The stack delta is the value of the stack pointer register at the entrance to the specified block minus the stack delta
     *  at the entry point of the function.  The function entry point stack delta is zero; the return address pushed onto the
     *  stack by the caller is attributed to the caller, not the callee, although the callee pops it from the stack when
     *  returning. The stack delta can be four different kinds of values:
     *
     *  @li Never-computed is indicated by the basic block not caching any value for the stack delta.  This method will always
     *      attempt to compute a stack delta if none is cached in the basic block.
     *
     *  @li Error is indicated by a cached null expression. Errors are usually due to a reachable basic block that contains an
     *      instruction for which semantics are not known.
     *
     *  @li Constant offset, for which the is_number predicate applied to the return value is true.
     *
     *  @li Top, indicated by a non-null return value for which is_number is false.  This results when two or more paths
     *      through the control flow graph result in different constant offsets. It can also occur when the algebraic
     *      simplifications that are built into ROSE fail to simplify a constant expression.
     *
     *  Two stack deltas are computed for each basic block: the stack delta at the start of the block and the start delta at
     *  the end of the block, returned by the "in" and "out" variants of this method, respectively.
     *
     *  Since stack deltas use the control flow graph during the analysis, the specified basic block must be attached to the
     *  CFG/AUM before calling this method.
     *
     * @{ */
    BaseSemantics::SValuePtr basicBlockStackDeltaIn(const BasicBlock::Ptr&) const;
    BaseSemantics::SValuePtr basicBlockStackDeltaOut(const BasicBlock::Ptr&) const;
    /** @} */

    /** Determine if part of the CFG can pop the top stack frame.
     *
     *  This analysis enters the CFG at the specified basic block and follows certain edges looking for a basic block where
     *  @ref basicBlockIsFunctionReturn returns true.  The analysis caches results in the @ref BasicBlock::mayReturn
     *  "mayReturn" property of the reachable basic blocks.
     *
     *  Since the analysis results might change if edges are inserted or erased even on distant basic blocks, the @p recompute
     *  argument can be used to force recomputation of the value. If @p recompute is true then the cached value at reachable
     *  nodes is recomputed.  The @ref basicBlockMayReturnReset method can be used to "forget" the property for all basic
     *  blocks in the CFG/AUM.
     *
     *  A basic block's may-return property is computed as follows (the first applicable rule wins):
     *
     *  @li If the block is owned by a function and the function's name is present on a whitelist or blacklist
     *      then the block's may-return is positive if whitelisted or negative if blacklisted.
     *
     *  @li If the block is a non-existing placeholder (i.e. it's address is not mapped with execute permission) then
     *      its may-return is positive or negative depending on the value of this partitioner's @ref assumeFunctionsReturn
     *      property.
     *
     *  @li If the block is a function return then the block's may-return is positive.
     *
     *  @li If any significant successor (defined below) of the block has a positive may-return value then the block's
     *      may-return is also positive.
     *
     *  @li If any significant succesor has an indeterminate may-return value, then the block's may-return is also
     *      indeterminate.
     *
     *  @li The block's may-return is negative.
     *
     *  A successor vertex is significant if there exists a significant edge to that vertex using these rules (first rule that
     *  applies wins):
     *
     *  @li A self-edge is never signiciant.  A self edge does not affect the outcome of the analysis, so they're excluded.
     *
     *  @li A function-call edge (@ref E_FUNCTION_CALL) is never significant. Even if the called function may return, it
     *      doesn't affect whether the block in question may return.
     *
     *  @li A call-return edge (@ref E_CALL_RETURN) is significant if at least one of its sibling function call edges (@ref
     *      E_FUNCTION_CALL) points to a vertex with a positive may-return, or if any sibling function call edge points to a
     *      vertex with an indeterminate may-return.
     *
     *  @li The edge is significant. This includes edges to the indeterminate and undiscovered vertices, whose may-return
     *      is always indeterminate.
     *
     *  The algorithm uses a combination of depth-first traversal and recursive calls.  If any vertex's may-return is requested
     *  recursively while it is being computed, the recursive call returns an indeterminate may-return.  Indeterminate results
     *  are indicated by returning nothing.
     *
     * @{ */
    Sawyer::Optional<bool>
    basicBlockOptionalMayReturn(const BasicBlock::Ptr&, bool recompute=false) const;

    Sawyer::Optional<bool>
    basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexNodeIterator&, bool recompute=false) const;
    /** @} */

    /** Clear all may-return properties.
     *
     *  This function is const because it doesn't modify the CFG/AUM; it only removes the may-return property from all the
     *  CFG/AUM basic blocks. */
    void basicBlockMayReturnReset() const;

    /** Adjust may-return blacklist and whitelist.
     *
     *  The @ref setMayReturnWhitelisted will add (or remove if @p state is false) the function name from the whitelist. It
     *  will erase the name from the blacklist if added to the whitelist, but will never add the name to the blacklist.
     *
     *  The @ref setMayReturnBlacklisted will add (or remove if @p state if false) the function name from the blacklist. It
     *  will erase the name from the whitelist if added to the blacklist, but will never add the name to the whitelist.
     *
     *  The @ref adjustMayReturnList will add the name to the whitelist when @p state is true, or to the blacklist when @p
     *  state if false, or to neither list when @p state is <code>boost::indeterminate</code>. It erases the name from those
     *  lists to which it wasn't added.
     *
     * @{ */
    void setMayReturnWhitelisted(const std::string &functionName, bool state=true);
    void setMayReturnBlacklisted(const std::string &functionName, bool state=true);
    void adjustMayReturnList(const std::string &functionName, boost::tribool state);
    /** @} */

    /** Query may-return blacklist and whitelist.
     *
     *  The @ref isMayReturnWhitelisted returns true if and only if @p functionName is present in the whitelist.
     *
     *  The @ref isMayReturnBlacklisted returns true if and only if @p functionName is present in the blacklist.
     *
     *  The @ref isMayReturnListed consults either the whitelist or blacklist depending on the value of @p dflt.  If @p dflt is
     *  true then this method returns false if @p functionName is blacklisted and true otherwise. If @p dflt is false then this
     *  method returns true if @p functionName is whitelisted and false otherwise.  If @p dflt is indeterminate then returns
     *  true if whitelisted, false if blacklisted, or indeterminate if not listed at all.
     *
     * @{ */
    bool isMayReturnWhitelisted(const std::string &functionName) const;
    bool isMayReturnBlacklisted(const std::string &functionName) const;
    boost::logic::tribool isMayReturnListed(const std::string &functionName, boost::logic::tribool dflt=true) const;
    /** @} */

    /** The may-return listed names.
     *
     *  Returns a map indicating whether names are whitelisted or blacklisted.  The map keys are the registered function names
     *  and the map values are true for whitelisted and false for blacklisted. */
    const MayReturnList& mayReturnList() const { return mayReturnList_; }

private:
    // Per-vertex data used during may-return analysis
    struct MayReturnVertexInfo {
        enum State {INIT, CALCULATING, FINISHED};
        State state;                                        // current state of vertex
        bool processedCallees;                              // have we processed BBs this vertex calls?
        boost::logic::tribool anyCalleesReturn;             // do any of those called BBs have a true may-return value?
        boost::logic::tribool result;                       // final result (eventually cached in BB)
        MayReturnVertexInfo(): state(INIT), processedCallees(false), anyCalleesReturn(false), result(boost::indeterminate) {}
    };

    // Is edge significant for analysis? See .C file for full documentation.
    bool mayReturnIsSignificantEdge(const ControlFlowGraph::ConstEdgeNodeIterator &edge, bool recompute,
                                    std::vector<MayReturnVertexInfo> &vertexInfo) const;

    // Determine (and cache in vertexInfo) whether any callees return.
    boost::logic::tribool mayReturnDoesCalleeReturn(const ControlFlowGraph::ConstVertexNodeIterator &vertex, bool recompute,
                                                    std::vector<MayReturnVertexInfo> &vertexInfo) const;

    // Maximum may-return result from significant successors including phantom call-return edge.
    boost::logic::tribool mayReturnDoesSuccessorReturn(const ControlFlowGraph::ConstVertexNodeIterator &vertex, bool recompute,
                                                       std::vector<MayReturnVertexInfo> &vertexInfo) const;

    // The guts of the may-return analysis
    Sawyer::Optional<bool> basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexNodeIterator &start, bool recompute,
                                                       std::vector<MayReturnVertexInfo> &vertexInfo) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner data block operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of data blocks attached to the CFG/AUM.
     *
     *  This is a relatively expensive operation compared to querying the number of basic blocks or functions. */
    size_t nDataBlocks() const;

    /** Determine if a data block is attached to the CFG/AUM.
     *
     *  Returns true if this data block is attached to the CFG/AUM and false if not attached. */
    bool dataBlockExists(const DataBlock::Ptr&) const;

    /** Find an existing data block.
     *
     *  Finds a data block that spans the specified address interval or which can be extended to span the address interval.
     *  The first choice is to return the smallest data block that spans the entire interval; second choice is the largest
     *  block that contains the first byte of the interval.  If there is a tie in sizes then the block with the highest
     *  starting address wins.  If no suitable data block can be found then the null pointer is returned. */
    DataBlock::Ptr findBestDataBlock(const AddressInterval&) const;

    /** Attach a data block to the CFG/AUM.
     *
     *  Attaches the data block to the CFG/AUM if it is not already attached.  A newly attached data block will have a
     *  ownership count of zero since none of its owners are attached (otherwise the data block would also have been
     *  already attached). Multiple data blocks having the same address can be attached. It is an error to supply a null
     *  pointer. */
    void attachDataBlock(const DataBlock::Ptr&);

    /** Detaches a data block from the CFG/AUM.
     *
     *  The specified data block is detached from the CFG/AUM and thawed, and returned so it can be modified.  It is an error
     *  to attempt to detach a data block which is owned by attached basic blocks or attached functions. */
    DataBlock::Ptr detachDataBlock(const DataBlock::Ptr&);

    /** Returns data blocks that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval.  All bytes represented by the data block are returned, even if they are unused or
     *  marked as padding in the data block type.
     *
     *  The returned list of data blocks are sorted by their starting address. */
    std::vector<DataBlock::Ptr> dataBlocksOverlapping(const AddressInterval&) const;

    /** Returns data blocks that span an entire address interval.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM and which span the entire specified
     *  interval. All bytes represented by the data block are returned, even if they are unused or marked as padding in the
     *  data block type.
     *
     *  The returned list of data blocks are sorted by their starting address. */
    std::vector<DataBlock::Ptr> dataBlocksSpanning(const AddressInterval&) const;

    /** Returns data blocks that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM and which are fully contained within the
     *  specified interval.  All bytes represented by the data block are returned, even if they are unused or marked as padding
     *  in the data block type.
     *
     *  The returned list of data blocks are sorted by their starting address. */
    std::vector<DataBlock::Ptr> dataBlocksContainedIn(const AddressInterval&) const;

    /** Returns the addresses used by a data block.
     *
     *  Returns an address interval describing all addresses of the data block, even if they are unused or marked as padding
     *  in the data block type.  Since all addresses are returned, the extent of a data block is always contiguous. */
    AddressInterval dataBlockExtent(const DataBlock::Ptr&) const;

    /** Returns the list of all attached data blocks.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM. */
    std::vector<DataBlock::Ptr> dataBlocks() const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner function operations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of functions attached to the CFG/AUM.
     *
     *  This is a constant-time operation. */
    size_t nFunctions() const { return functions_.size(); }

    /** Determines whether a function exists in the CFG/AUM.
     *
     *  If the CFG/AUM knows about the specified function then this method returns a pointer to that function, otherwise it
     *  returns the null pointer.
     *
     *  The query can supply either a function entry address or a function pointer.  If a pointer is specified then the return
     *  value will be the same pointer if and only if the function exists in the CFG/AUM, otherwise the null pointer is
     *  returned. It is not sufficient for the CFG/AUM to contain a function with the same entry address -- it must be the same
     *  actual function object.
     *
     *  @{ */
    Function::Ptr functionExists(rose_addr_t startVa) const {
        return functions_.getOptional(startVa).orDefault();
    }
    Function::Ptr functionExists(const Function::Ptr &function) const {
        if (function!=NULL) {
            Function::Ptr found = functionExists(function->address());
            if (found==function)
                return function;
        }
        return Function::Ptr();
    }
    /** @} */

    /** All functions attached to the CFG/AUM.
     *
     *  Returns a vector of distinct functions sorted by their entry address. */
    std::vector<Function::Ptr> functions() const;

    /** Returns functions that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct functions that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval.  By "overlap" we mean that the function owns at least one basic block or data block
     *  that overlaps with the interval.
     *
     *  The returned list of funtions are sorted by their entry address. */
    std::vector<Function::Ptr> functionsOverlapping(const AddressInterval&) const;

    /** Returns functions that span an entire address interval.
     *
     *  Returns a sorted list of distinct functions that are attached to the CFG/AUM and which span the entire specified
     *  interval. In order for a function to span the interval its extent must be a superset of the interval. See @ref
     *  functionExtent.  In other words, the union of all the addresseses represented by the function's basic blocks and data
     *  blocks is a superset of the specified interval.
     *
     *  The returned list of functions are sorted by their starting address. */
    std::vector<Function::Ptr> functionsSpanning(const AddressInterval&) const;

    /** Returns functions that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct functions that are attached to the CFG/AUM and which are fully contained within
     *  the specified interval.  In order to be fully contained in the interval, the addresses represented by the function's
     *  basic blocks and data blocks must be a subset of the specified interval.
     *
     *  The returned list of functions are sorted by their starting address. */
    std::vector<Function::Ptr> functionsContainedIn(const AddressInterval&) const;

    /** Returns the addresses used by a function.
     *
     *  Returns an interval set which is the union of the addresses of the function's basic blocks and data blocks.  Most
     *  functions are contiguous in memory and can be represented by a single address interval, but this is not a
     *  requirement in ROSE. */
    AddressIntervalSet functionExtent(const Function::Ptr&) const;
    
    /** Attaches a function to the CFG/AUM.
     *
     *  The indicated function(s) is inserted into the control flow graph.  Basic blocks (or at least placeholders) are
     *  inserted into the CFG for the function entry address and any basic block addresses the function might already contain.
     *  This method returns the number of new basic block placeholders that were created.  If any data blocks are associated
     *  with the function then they are inserted into the AUM.
     *
     *  It is permissible to insert the same function multiple times at the same address (subsequent insertions are no-ops),
     *  but it is an error to insert a different function at the same address as an existing function.  The CFG/AUM is capable
     *  of representing at most one function per function entry address.
     *
     *  All functions that are attached to the CFG/AUM are marked as frozen and the user is prevented from directly
     *  manipulating the function's basic block and data block ownership lists. The connectivity of frozen functions can only
     *  be changed by using the partitioner's API, not the function's API.  This allows the partitioner to keep the CFG in a
     *  consistent state.
     *
     *  @{ */
    size_t attachFunction(const Function::Ptr&);
    size_t attachFunctions(const Functions&);
    /** @} */

    /** Attaches or merges a function into the CFG/AUM.
     *
     *  If no other function exists at the specified function's entry address, then this method behaves identically to @ref
     *  attachFunction.  Otherwise, this method attempts to merge the specified function into an existing function.  In any
     *  case, it returns a pointer to the function in the CFG/AUM (the existing one, or the new one). If the merge is not
     *  possible, then an exception is thrown. */
    Function::Ptr attachOrMergeFunction(const Function::Ptr&);

    /** Create placeholders for function basic blocks.
     *
     *  Ensures that a basic block placeholder (or basic block) exists for each function entry address and each function basic
     *  block address.  If a placeholder is absent then one is created by calling @ref insertPlaceholder.  The return value is
     *  the number of new placeholders created.  A function that is attached to the CFG/AUM cannot have its basic block and
     *  data block membership lists manipulated directly by the user, but only through the Partitioner API.
     *
     *  If the function is attached to the CFG/AUM then additional actions occur: any placeholders (or basic blocks) owned by
     *  this function are verified to not be owned by some other function, and they are marked as owned by this function.
     *
     *  @todo Does it make sense to call insertFunctionBasicBlocks on a function that's already attached to the CFG/AUM?
     *  Wouldn't its basic blocks already also be attached? [Robb P. Matzke 2014-08-15]
     *
     *  @{ */
    size_t attachFunctionBasicBlocks(const Functions&);
    size_t attachFunctionBasicBlocks(const Function::Ptr&);
    /** @} */

    /** Detaches a function from the CFG/AUM.
     *
     *  The indicated function is detached from the control flow graph. Although this function's basic blocks remain attached
     *  to the CFG/AUM, they are no longer considered to be owned by this function even though this function will continue to
     *  list the addresses of those blocks as its members.  Any data blocks that were owned by only this function become
     *  detached from the CFG/AUM, but this function continues to point to them; other multiply-owned data blocks will remain
     *  attached to the CFG/AUM and will continue to be pointed to by this function, but the CFG/AUM will no longer list this
     *  function as one of their owners.
     *
     *  Detaching a function from the CFG/AUM does not change the function other than thawing it so it can be modified by the
     *  user directly through its API. Attempting to detach a function that is already detached has no effect. */
    void detachFunction(const Function::Ptr&);

    /** Attach a data block into an attached or detached function.
     *
     *  @todo This is certainly not the final API.  The final API will likely describe data as an address and type rather than
     *  an address and size.  It is also likely that attaching data to a function will try to adjust an existing data block's
     *  type rather than creating a new data block -- this will allow a data block's type to become more and more constrained
     *  as we learn more about how it is accessed.
     *
     *  Returns the data block that has been attached to the function. */
    DataBlock::Ptr attachFunctionDataBlock(const Function::Ptr&, rose_addr_t startVa, size_t nBytes);

    /** Attach a data block to an attached or detached function.
     *
     *  Causes the specified function to become an owner of the specified data block. */
    void attachFunctionDataBlock(const Function::Ptr&, const DataBlock::Ptr&);

    /** Finds the function that owns the specified basic block.
     *
     *  If @p bblockVa is a starting address for a basic block that is in the CFG/AUM then this method returns the pointer to
     *  the function that owns that block.  If the CFG/AUM does not contain a basic block that starts at the specified address,
     *  or if no function owns that basic block, then a null function pointer is returned.
     *
     *  If a basic block pointer is supplied instead of a basic block starting address, then the starting address of the
     *  specified basic block is used.  That is, the returned function might not own the exact specified basic block, but owns
     *  a different basic block that starts at the same address.  This can only happen when the specified basic block is
     *  detached from the CFG and the CFG contains a different (attached) basic block at the same starting address.
     *
     *  The returned function will be a function that is attached to the CFG/AUM; detached functions are never returned since
     *  the partitioner does not necessarily know about them.
     *
     *  @{ */
    Function::Ptr findFunctionOwningBasicBlock(rose_addr_t bblockVa) const;
    Function::Ptr findFunctionOwningBasicBlock(const BasicBlock::Ptr&) const;
    /** @} */

    /** Finds functions that own specified basic blocks.
     *
     *  Finds the set of distinct functions that own the specified basic blocks and returns a list of such functions in entry
     *  address order.  This is similar to @ref findFunctionOwningBasicBlock except it operates on a collection of basic blocks
     *  and returns a collection of distinct function pointers.
     *
     *  @{ */
    std::vector<Function::Ptr> findFunctionsOwningBasicBlocks(const std::vector<rose_addr_t>&) const;
    std::vector<Function::Ptr> findFunctionsOwningBasicBlocks(const std::vector<BasicBlock::Ptr>&) const;
    /** @} */

    /** Scans the CFG to find function calls.
     *
     *  Scans the CFG without modifying it and looks for edges that are marked as being function calls.  A function is created
     *  at each call if one doesn't already exist in the CFG/AUM, and the list of created functions is returned.  None of the
     *  created functions are added to the CFG/AUM.
     *
     *  See also @ref discoverFunctionEntryVertices which returns a superset of the functions returned by this method. */
    std::vector<Function::Ptr> discoverCalledFunctions() const;

    /** Scans the CFG to find function entry basic blocks.
     *
     *  Scans the CFG without modifying it in order to find vertices (basic blocks and basic block placeholders) that are the
     *  entry points of functions.  A vertex is a function entry point if it has an incoming edge that is a function call or if
     *  it is the entry block of a function that already exists.
     *
     *  The returned function pointers are sorted by function entry address.
     *
     *  See also @ref discoverFunctionCalls which returns a subset of the functions returned by this method. */
    std::vector<Function::Ptr> discoverFunctionEntryVertices() const;

    /** True if function is a thunk.
     *
     *  If the function is non-null and a thunk then some information about the thunk is returned, otherwise nothing is
     *  returned.  A function is a thunk if it has the @ref SgAsmFunction::FUNC_THUNK bit set in its reason mask, and it has
     *  exactly one basic block, and the basic block has exactly one successor, and the successor is concrete.
     *
     *  As a side effect, the basic block's outgoing edge type is changed to E_FUNCTION_XFER. */
    Sawyer::Optional<Thunk> functionIsThunk(const Function::Ptr&) const;

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

    /** Returns ghost successors for a single function.
     *
     *  Returns the set of basic block starting addresses that are naive successors for the basic blocks of a function but
     *  which are not actual control flow successors due to the presence of opaque predicates. */
    std::set<rose_addr_t> functionGhostSuccessors(const Function::Ptr&) const;

    /** Returns a function call graph.
     *
     *  If @p allowParallelEdges is true then the returned call graph will have one edge for each function call and each edge
     *  will have a count of one.  Otherwise multiple calls between the same pair of functions are coalesced into single edges
     *  with non-unit counts in the call graph. */
    FunctionCallGraph functionCallGraph(bool allowParallelEdges = true) const;

    /** Stack delta analysis for one function.
     *
     *  Computes stack deltas if possible at each basic block within the specified function.  The algorithm starts at the
     *  function's entry block with an incoming stack delta of zero, and performs a dataflow anysis following a control flow
     *  graph that contains those vertices reachable from the entry block by following only intra-function edges.  Each such
     *  vertex is given an incoming and outgoing stack delta.
     *
     *  If the function appears to make reasonable use of the stack then an overall stack delta is returned. This is the delta
     *  resulting from the final function return blocks.  Otherwise a null expression is returned.  The result is cached in the
     *  Function::stackDelta property.
     *
     *  Since this analysis is based on data flow, which is based on a control flow graph, the function must be attached to the
     *  CFG/AUM and all its basic blocks must also exist in the CFG/AUM.  Also, the @ref basicBlockStackDelta method must be
     *  non-null for each reachable block in the function. */
    BaseSemantics::SValuePtr functionStackDelta(const Function::Ptr &function) const;

    /** Stack delta analysis for all functions. */
    void allFunctionStackDelta() const;

    /** May-return analysis for one function.
     *
     *  Determines if a function can possibly return to its caller. This is a simple wrapper around @ref
     *  basicBlockOptionalMayReturn invoked on the function's entry block. See that method for details. */
    Sawyer::Optional<bool> functionOptionalMayReturn(const Function::Ptr &function) const;

    /** May-return analysis for all functions. */
    void allFunctionMayReturn() const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Callbacks
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** List of all callbacks invoked when the CFG is adjusted.
     *
     *  Inserting a new callback goes something like this:
     *
     * @code
     *  struct MyCallback: Partitioner::CfgAdjustmentCallback {
     *      static Ptr instance() { return Ptr(new MyCallback); }
     *      virtual bool operator()(bool chain, const AttachedBasicBlock &args) { ... }
     *      virtual bool operator()(bool chain, const DetachedBasicBlock &args) { ... }
     *  };
     *
     *  partitioner.cfgAdjustmentCallbacks().append(MyCallback::instance());
     * @endcode
     *
     *  @{ */
    CfgAdjustmentCallbacks& cfgAdjustmentCallbacks() { return cfgAdjustmentCallbacks_; }
    const CfgAdjustmentCallbacks& cfgAdjustmentCallbacks() const { return cfgAdjustmentCallbacks_; }
    /** @} */

    /** Callbacks for adjusting basic block during discovery.
     *
     *  Each time an instruction is appended to a basic block these callbacks are invoked to make adjustments to the block.
     *  See @ref BasicBlockCallback and @ref discoverBasicBlock for details.
     *
     *  @{ */
    BasicBlockCallbacks& basicBlockCallbacks() { return basicBlockCallbacks_; }
    const BasicBlockCallbacks& basicBlockCallbacks() const { return basicBlockCallbacks_; }
    /** @} */

public:
    /** Ordered list of function prologue matchers.
     *
     *  @sa nextFunctionPrologue
     *
     *  @{ */
    FunctionPrologueMatchers& functionPrologueMatchers() { return functionPrologueMatchers_; }
    const FunctionPrologueMatchers& functionPrologueMatchers() const { return functionPrologueMatchers_; }
    /** @} */

    /** Finds the next function by searching for a function prologue.
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
     *  Some function prologue matchers can return multiple functions. For instance, a matcher for a thunk might return the
     *  thunk and the function to which it points.  In any case, the first function is the primary one.
     *
     *  If no match is found then an empty vector is returned. */
    std::vector<Function::Ptr> nextFunctionPrologue(rose_addr_t startVa);

public:
    /** Ordered list of function padding matchers.
     *
     * @{ */
    FunctionPaddingMatchers& functionPaddingMatchers() { return functionPaddingMatchers_; }
    const FunctionPaddingMatchers& functionPaddingMatchers() const { return functionPaddingMatchers_; }
    /** @} */

    /** Finds function padding.
     *
     *  Scans backward from the specified function's entry address by invoking each function padding matcher in the order
     *  returned by @ref functionPaddingMatchers until one of them finds some padding.  Once found, a data block is created and
     *  returned.  If no padding is found then the null pointer is returned. */
    DataBlock::Ptr matchFunctionPadding(const Function::Ptr&);

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
     *  A @p prefix can be specified to be added to the beginning of each line of output. If @p showBlocks is set then the
     *  instructions are shown for each basic block. If @p computeProperties is set then various properties are computed and
     *  cached rather than only consulting the cache. */
    void dumpCfg(std::ostream&, const std::string &prefix="", bool showBlocks=true, bool computeProperties=true) const;

    /** Output CFG as GraphViz.
     *
     *  Emits all vertices whose starting address falls within the specified address interval, and all vertices that are
     *  reachable forward or backward by a single edge from those vertices.
     *
     *  If @p showNeighbors is false then only edges whose source and target are both selected vertices are shown. */
    void cfgGraphViz(std::ostream&, const AddressInterval &restrict = AddressInterval::whole(),
                     bool showNeighbors=true) const;

    /** Name of a vertex.
     *
     *  @{ */
    static std::string vertexName(const ControlFlowGraph::VertexNode&);
    std::string vertexName(const ControlFlowGraph::ConstVertexNodeIterator&) const;
    /** @} */

    /** Name of last instruction in vertex. */
    static std::string vertexNameEnd(const ControlFlowGraph::VertexNode&);

    /** Name of an incoming edge.
     *
     * @{ */
    static std::string edgeNameSrc(const ControlFlowGraph::EdgeNode&);
    std::string edgeNameSrc(const ControlFlowGraph::ConstEdgeNodeIterator&) const;
    /** @} */

    /** Name of an outgoing edge.
     *
     * @{ */
    static std::string edgeNameDst(const ControlFlowGraph::EdgeNode&);
    std::string edgeNameDst(const ControlFlowGraph::ConstEdgeNodeIterator&) const;
    /** @} */

    /** Name of an edge.
     *
     * @{ */
    static std::string edgeName(const ControlFlowGraph::EdgeNode&);
    std::string edgeName(const ControlFlowGraph::ConstEdgeNodeIterator&) const;
    /** @} */

    /** Name of a basic block. */
    static std::string basicBlockName(const BasicBlock::Ptr&);

    /** Name of a data block. */
    static std::string dataBlockName(const DataBlock::Ptr&);

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

    /** Use or not use symbolic semantics.
     *
     *  When true, a symbolic semantics domain will be used to reason about certain code properties such as successors for a
     *  basic block.  When false, more naive but faster methods are used.
     *
     *  @{ */
    void enableSymbolicSemantics(bool b=true) { useSemantics_ = b; }
    void disableSymbolicSemantics() { useSemantics_ = false; }
    bool usingSymbolicSemantics() const { return useSemantics_; }
    /** @} */

    /** Property: Insert (or not) function call return edges.
     *
     *  When true, attaching a function call basic block to the CFG will create a call-return edge (@ref E_CALL_RETURN) in the
     *  CFG even if the basic block has no explicit call-return edge and an analysis indicates that the callee (or at least one
     *  callee if there are more than one) may return. Call-return edges are typically the edge from a @c CALL instruction to
     *  the instruction immediately following in the address space.
     *
     *  If true, then the decision to add a call-return edge is made at the time the function call site is attached to the
     *  basic block, but the may-return analysis for the callees might be indeterminate at that time (such as if the callee
     *  instructions have not been discovered).  The assumeCallsReturn partitioner property can be used to guess whether
     *  indeterminate may-return analysis should be assumed as a positive or negative result.
     *
     *  Deciding whether to add a call-return edge at the time of the call-site insertion can result in two kinds of errors: an
     *  extra CFG edge where there shouldn't be one, or a missing CFG edge where there should be one.  An alternative is to
     *  turn off the autoAddCallReturnEdges property and delay the decision to a time when more information is available.  This
     *  is the approach taken by the default @ref Engine -- it maintains a list of basic blocks that need to be investigated at
     *  a later time to determine if a call-return edge should be inserted, and it delays the decision as long as possible.
     *
     * @{ */
    void autoAddCallReturnEdges(bool b) { autoAddCallReturnEdges_ = b; }
    bool autoAddCallReturnEdges() const { return autoAddCallReturnEdges_; }
    /** @} */

    /** Property: Assume (or not) that function calls return.
     *
     *  If the may-return analysis is indeterminate and the partitioner needs to make an immediate decision about whether a
     *  function might return to its caller then this property is used.  This property also determines whether the may-return
     *  analysis uses a whitelist or blacklist.
     *
     *  If this property is true, then functions are assumed to return unless it can be proven that they cannot. A blacklist is
     *  one way to prove that a function does not return.  On the other hand, if this property is false then functions are
     *  assumed to not return unless it can be proven that they can.  A whitelist is one way to prove that a function can
     *  return.
     *
     * @{ */
    void assumeFunctionsReturn(bool b) { assumeFunctionsReturn_ = b; }
    bool assumeFunctionsReturn() const { return assumeFunctionsReturn_; }
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Partitioner internal utilities
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init(Disassembler*, const MemoryMap&);
    void init(const Partitioner&);
    void reportProgress() const;

public:
    // Obtain a new RiscOperators along with new states
    BaseSemantics::RiscOperatorsPtr newOperators() const;

    // Obtain a new instruction semantics dispatcher initialized with the partitioner's semantic domain and a fresh state.
    BaseSemantics::DispatcherPtr newDispatcher(const BaseSemantics::RiscOperatorsPtr&) const;

private:
    // Convert a CFG vertex iterator from one partitioner to another.  This is called during copy construction when the source
    // and destination CFGs are identical.
    ControlFlowGraph::VertexNodeIterator convertFrom(const Partitioner &other,
                                                     ControlFlowGraph::ConstVertexNodeIterator otherIter);
    
    // Adjusts edges for a placeholder vertex. This method erases all outgoing edges for the specified placeholder vertex and
    // then inserts a single edge from the placeholder to the special "undiscovered" vertex. */
    ControlFlowGraph::EdgeNodeIterator adjustPlaceholderEdges(const ControlFlowGraph::VertexNodeIterator &placeholder);

    // Adjusts edges for a non-existing basic block.  This method erases all outgoing edges for the specified vertex and
    // then inserts a single edge from the vertex to the special "non-existing" vertex. */
    ControlFlowGraph::EdgeNodeIterator adjustNonexistingEdges(const ControlFlowGraph::VertexNodeIterator &vertex);

    // Implementation for the discoverBasicBlock methods.  The startVa must not be the address of an existing placeholder.
    BasicBlock::Ptr discoverBasicBlockInternal(rose_addr_t startVa) const;

    // Checks consistency of internal data structures when debugging is enable (when NDEBUG is not defined).
    void checkConsistency() const;

    // This method is called whenever a new placeholder is inserted into the CFG or a new basic block is attached to the
    // CFG/AUM. The call happens immediately after the CFG/AUM are updated.
    virtual void bblockAttached(const ControlFlowGraph::VertexNodeIterator &newVertex);

    // This method is called whenever a basic block is detached from the CFG/AUM or when a placeholder is erased from the CFG.
    // The call happens immediately after the CFG/AUM are updated.
    virtual void bblockDetached(rose_addr_t startVa, const BasicBlock::Ptr &removedBlock);

    // String for a vertex in a GraphViz file. The attrs are only added for basic block vertices.
    static std::string cfgGraphVizVertex(const ControlFlowGraph::VertexNode&, const std::string &attrs="");
};

} // namespace
} // namespace
} // namespace

#endif
