#ifndef ROSE_Partitioner2_Partitioner_H
#define ROSE_Partitioner2_Partitioner_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/AddressUsageMap.h>
#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/Config.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/FunctionCallGraph.h>
#include <Partitioner2/InstructionProvider.h>
#include <Partitioner2/Modules.h>
#include <Partitioner2/Reference.h>

#include <Sawyer/Attribute.h>
#include <Sawyer/Callbacks.h>
#include <Sawyer/IntervalSet.h>
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/Optional.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/SharedPointer.h>

#include <BinarySourceLocations.h>
#include <BinaryUnparser.h>
#include <Progress.h>

#include <boost/filesystem.hpp>
#include <boost/move/utility_core.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>

#include <ostream>
#include <set>
#include <string>
#include <vector>

// Derived classes needed for serialization
#include <BinaryYicesSolver.h>
#include <BinaryZ3Solver.h>
#include <DispatcherM68k.h>
#include <DispatcherPowerpc.h>
#include <DispatcherX86.h>

// Define ROSE_PARTITIONER_MOVE if boost::move works. Mainly this is to work around a GCC bug that reports this error:
//
//   prototype for
//   'Rose::BinaryAnalysis::Partitioner2::Partitioner::Partitioner(boost::rv<Rose::BinaryAnalysis::Partitioner2::Partitioner>&)'
//   does not match any in class 'Rose::BinaryAnalysis::Partitioner2::Partitioner'
//
// followed by saying that the exact same signature is one of the candidates:
//
//   candidates are:
//   Rose::BinaryAnalysis::Partitioner2::Partitioner::Partitioner(boost::rv<Rose::BinaryAnalysis::Partitioner2::Partitioner>&)
//
// This is apparently GCC issue 49377 [https://gcc.gnu.org/bugzilla/show_bug.cgi?id=49377] fixed in GCC-6.1.0.
#if __cplusplus >= 201103L
    #define ROSE_PARTITIONER_MOVE
#elif defined(__GNUC__)
    #if __GNUC__ > 5
       #define ROSE_PARTITIONER_MOVE
    #elif BOOST_VERSION >= 106900 // 1.68.0 might be okay too, but ROSE blacklists it for other reasons
       #define ROSE_PARTITIONER_MOVE
    #endif
#endif

namespace Rose {
namespace BinaryAnalysis {

/** Binary function detection.
 *
 *  This namespace consists of two major parts and a number of smaller parts.  The major parts are:
 *
 *  @li @ref Partitioner2::Partitioner "Partitioner": The partitioner is responsible for organizing instructions into basic
 *      blocks and basic blocks into functions. It has methods to discover new parts of the executable, and methods to control
 *      how those parts are organized into larger parts.  It queries a memory map and an instruction provider and updates a
 *      global control flow graph (CFG) and address usage map (AUM). Its operations are quite low-level and its behavior is
 *      customized primarily by callbacks.
 *
 *  @li @ref Partitioner2::Engine "Engine": The engine contains the higher-level functionality that drives the partitioner.
 *      Where the partitioner knows @em how to make a basic block or a function, the engine know @em where to make a basic
 *      block or function.  The engine is customized by subclassing it and overriding various methods.
 *
 *  Disassembly techniques fall into two broad categories: linear disassembly and recursive disassembly.  Linear disassembly
 *  progresses by starting at some low address in the specimen address space, disassembling one instruction, and then moving on
 *  to the next (fallthrough) address and repeating.  This approach is quite good at disassembling everything (especially for
 *  fixed length instructions) but makes no attempt to organize instructions according to flow of control.  On the other hand,
 *  recursive disassembly uses a work list containing known instruction addresses, disassembles an instruction from the
 *  worklist, determines its control flow successors, and adds those addresses to the work list. As a side effect, it produces
 *  a control flow graph.
 *
 *  ROSE supports both linear and recursive disassembly. Linear disassembly is trivial to implement: simply walk the memory map
 *  from beginning to end calling an @ref InstructionProvider at each address.  Recursive disassembly is where the rubber meets
 *  the road, so to speak, and the quality of the disassembly is intimately tied to the quality of the control flow graph.
 *  ROSE supports pure control flow reasoning, and combines that with heuristics to discover (or prevent discovery) where the
 *  control flow reasoning is deficient.  For example, a pure control flow approach will miss dead code, but heuristics might
 *  be able to find the dead code at which time the control flow approach can continue.  The trick is finding the right set of
 *  heuristics for a particular situation -- if to permissive, they'll find code where it doesn't exist and which might
 *  interfere with the control flow reasoning; to restrictive and they could easily miss large parts of a specimen.  Heuristics
 *  are mostly implemented as optional parts of an engine, or callbacks registered with the partitioner. */
namespace Partitioner2 {

/** Partitions instructions into basic blocks and functions.
 *
 *  A partitioner is responsible for driving a disassembler to obtain instructions, grouping those instructions into basic
 *  blocks, grouping the basic blocks into functions, and building an abstract syntax tree. The partitioner is the low-level
 *  part of the whole partitioning process (the @ref Engine is the high-level part) and is customized by registering
 *  callbacks.  The partitioner knows "how" to group small components (like instructions) into larger components (like basic
 *  blocks) whereas the engine knows "where" to look for things in the specimen address space.
 *
 *  The following objects are needed as input:
 *
 * @li A memory map containing the memory for the specimen being analyzed.  Parts of memory that contain instructions must be
 *     mapped with execute permission.  Parts of memory that are readable and non-writable will be considered constant for the
 *     purpose of disassembly and partitioning and can contain things like dynamic linking tables that have been initialized
 *     prior to calling the partitioner.  The memory map need not be the exact memory map for the specimen, and often isn't,
 *     because the specimen's mapping permissions might not be appropriate.
 *
 * @li A disassembler which is canonical for the specimen architecture and which will return an instruction (possibly an
 *     "unknown" instruction) whenever it is asked to disassemble an address that is mapped with execute permission.  The
 *     partitioner wraps the disassembler and memory map into an @ref InstructionProvider that caches disassembled
 *     instructions.
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
 * @section partitoner_basic_block Basic Blocks
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
 * @section partitioner_data_block Data Blocks
 *
 *  A data block is an address and data type anywhere in memory.  A data block can be attached to a CFG/AUM, or exist in a
 *  detached state. They are attached to the CFG/AUM by virtue of being owned by one or more basic blocks or functions that are
 *  attached to the CFG/AUM. Data blocks such as function alignment are typically attached to a function, while data blocks
 *  such as branch tables are typically attached to a basic block.  A data block may be attached to more than one function
 *  and/or basic block, and the CFG/AUM is able to support multiple data blocks having the same address.
 *
 * @section partitioner_functions Functions
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
 * @section partitioner_cfg Control Flow Graph
 *
 *  At any point in time, the partitioner's control flow graph represents those basic blocks (and indirectly the instructions)
 *  that have been selected to appear in the final abstract syntax tree (AST).  This is a subset of all basic blocks ever
 *  created, and a subset of the instructions known to the instruction provider. Note: a final pass during AST construction
 *  might join certain CFG vertices into a single SgAsmBlock under certain circumstances.  The CFG is of type @ref
 *  ControlFlowGraph, a subset of @ref Sawyer::Container::Graph whose vertices and edges both carry information: the vertices
 *  are of type @ref ControlFlowGraph::CfgVertex and the edges are of type @ref ControlFlowGraph::CfgEdge.
 *
 *  Most CFG vertices represent basic blocks (vertex type @ref V_BASIC_BLOCK) and either point to a non-null @ref BasicBlock
 *  having at least one discovered instruction, or to a null basic block.  A "placeholder" usually refers to a vertex with a
 *  null basic block pointer (but not necessarily), and "basic block" refers to a vertex with a non-null pointer. Therefore the
 *  set of placeholders is a superset of the set of basic blocks.  A "pure placeholder" is a basic block vertex with a null
 *  pointer. Any of the @ref V_BASIC_BLOCK vertices can also point to an optional function that owns the block.
 *
 *  Placeholders with null basic block pointers represent one of two situations: either the partitioner has not attempted to
 *  discover an instruction at the basic block starting address, or the partitioner has tried but failed due to the address
 *  being non-executable or not mapped.  The two cases are distinguished from each other by whether the vertex's only outgoing
 *  edge points to the special "undiscovered" or "nonexisting" vertex.  Pure placeholders always have exactly this one outgoing
 *  edge.
 *
 *  The CFG also has other special (non-basic block) vertices each represented by its own vertex type. Altogether, the special
 *  vertices, by their type, are:
 *
 *  @li @ref V_UNDISCOVERED: a unique, special vertex whose incoming edges originate only from pure placeholders, one edge per
 *      placeholder. This vertex is returned by @ref undiscoveredVertex and users can use its incoming edge list as a simple
 *      work-list of places where instructions need to be discovered.
 *
 *  @li @ref V_NONEXISTING: a unique, special vertex whose incoming edges originate from pure placeholders that were discovered
 *      to have an unmapped or non-executable starting address, one edge per placeholder.  In other words, this vertex's incoming
 *      edges point back to addresses where the partitioner thinks there should be a basic block but there isn't. An example is
 *      entry addresses for dynamically linked functions that have not been loaded and linked. This vertex is returned by @ref
 *      nonexistingVertex.
 *
 *  @li @ref V_INDETERMINATE: a unique, special vertex that serves as the destination for any basic block outgoing edge whose
 *      target address is not concrete.  For instance, an indirect jump through a register whose value is not known will have
 *      an edge to the indeterminate vertex.  An "unknown" instruction, which indicates that memory is executable but
 *      disassembly failed, also always point to the indeterminate vertex. This vertex is returned by @ref
 *      indeterminateVertex.
 *
 *  CFG edges are also labeled with type information:
 *
 *  @li @ref E_FUNCTION_CALL: represents an edge known to be a function call. A function call is defined as an inter-function
 *      branch that pushes a return address onto the stack.
 *
 *  @li @ref E_FUNCTION_XFER: represents an inter-function edge which is not a function call.  These edges represent a transfer
 *      of control from the caller to the callee (even though there is no CALL instruction) where the callee inherits the stack
 *      frame of the caller.  When the callee returns it will skip over the caller because the caller does not have a distinct
 *      stack frame.  An example is an edge from a thunk consisting of an indirect jump instruction that branches to another
 *      function.
 *
 *  @li @ref E_FUNCTION_RETURN: represents a function return-to-caller, such as from a RET instruction.  If a function can
 *      return to multiple locations, rather than store a list edges for each possible return address, the CFG stores only one
 *      edge with an abstract value, thus pointing to the indeterminate vertex. Of course if a function can only return to one
 *      address then the edge is concrete and points to a placeholder at that address, but how often does that happen?
 *
 *  @li @ref E_CALL_RETURN: represents a function return with respect to a call site.  These edges normally originate at
 *      CALL instructions and point to the address following the call.  Lack of such an edge when the CFG is finished usually
 *      indicates that the called function cannot return.
 *
 *  @li @ref E_NORMAL: represents any edge not in one of the above categories.
 *
 * @section partitioner_aum Address Usage Map
 *
 *  The address usage map (AUM) is similar to the control flow graph in that they both represent the same set of instructions,
 *  basic blocks, and functions. But where the CFG organizes them according to their control flow relationships, the AUM
 *  organizes them by the memory addresses they occupy.  The partitioner ensures that both data structures are always
 *  synchronized with each other from the perspective of an outside viewer (including user callbacks).
 *
 *  The AUM can answer queries about what instructions, basic blocks, data blocks, or functions are contained within, begin
 *  within, span, or overlap an address or address interval. For small intervals the results can usually be obtained in
 *  logorithmic time, but querying large intervals can be slower.
 *
 * @section partitioner_prioritizing Prioritizing Work
 *
 *  The partitioner itself does not prioritize work or perform work automatically--it must be told what to do, usually through
 *  an engine.  However, the partitioner does have certain features that facilitate prioritization at higher layers. Namely, it
 *  has the CFG with special vertices, and AUM and a whole set of queries, an address map that can be traversed, and
 *  user-defined callbacks that are notified for certain CFG changes.
 *
 *  @li The special @em undiscovered vertex is commonly used to obtain a list of addresses where a basic block needs to be
 *      discovered.  Unfortunately these addresses are in no particular order and will change continually as pure placeholders
 *      are inserted and erased from the CFG, or a pure placeholder becomes a discovered basic block.
 *
 *  @li The special @em indeterminate vertex can be queried to find basic blocks that might need more analysis. For instance,
 *      an indirect jump that results in an abstract successor might be a point where an inter-block data-flow analysis could
 *      nail down the successor to a concrete value or a set of values.  A common example is case-choosing code for a C
 *      "switch" statement.
 *
 *  @li The address usage map (AUM) can be queried to find addresses where instructions haven't been discovered yet. This
 *      technique is usually combined with a memory map traversal so that only executable mapped addresses need be considered
 *      rather than the entire 4GB (or larger) specimen address space.
 *
 *  @li The user can register a callback (or callbacks) that will be notified each time a basic block or placeholder is
 *      attached or detached from the CFG, thus giving the user a chance to discover new work areas and prioritize them.
 *
 *  The base implementation of @ref Engine uses these features to prioritize its work and can be consulted as an example.
 *
 * @section partitioner_provisional Provisional Detection
 *
 *  (This section is a placeholder for an idea that is not yet implemented.)
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
 * @section partitioner_function_boundaries Function Boundary Determination
 *
 *  (This section is a placeholder for future documentation).
 *
 * @section partitioner_faq Frequenctly Asked Questions
 *
 * Q. Why is this class final?
 *
 * A. This class represents the low-level operations for partitioning instructions and is responsible for ensuring that certain
 *    data structures such as the CFG and AUM are always consistent.  The class is final to guarantee these invariants. Its
 *    behavior can only be modified by registering callbacks.  High-level behavior is implemented above this class such as in
 *    module functions (various Module*.h files) or engines derived from the @ref Engine class.  Additional data can be
 *    attached to a partitioner via attributes (see @ref Attribute). */
class ROSE_DLL_API Partitioner: public Sawyer::Attribute::Storage<> {     // final
#ifdef ROSE_PARTITIONER_MOVE
    BOOST_MOVABLE_BUT_NOT_COPYABLE(Partitioner)
#endif

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

    /** Map address to name. */
    typedef Sawyer::Container::Map<rose_addr_t, std::string> AddressNameMap;
    
private:
    BasePartitionerSettings settings_;                  // settings adjustable from the command-line
    Configuration config_;                              // configuration information about functions, blocks, etc.
    InstructionProvider::Ptr instructionProvider_;      // cache for all disassembled instructions
    MemoryMap::Ptr memoryMap_;                          // description of memory, especially insns and non-writable
    ControlFlowGraph cfg_;                              // basic blocks that will become part of the ROSE AST
    CfgVertexIndex vertexIndex_;                        // Vertex-by-address index for the CFG
    AddressUsageMap aum_;                               // How addresses are used for each address represented by the CFG
    SmtSolverPtr solver_;                               // Satisfiable modulo theory solver used by semantic expressions
    Functions functions_;                               // List of all attached functions by entry address
    bool autoAddCallReturnEdges_;                       // Add E_CALL_RETURN edges when blocks are attached to CFG?
    bool assumeFunctionsReturn_;                        // Assume that unproven functions return to caller?
    size_t stackDeltaInterproceduralLimit_;             // Max depth of call stack when computing stack deltas
    AddressNameMap addressNames_;                       // Names for various addresses
    SourceLocations sourceLocations_;                   // Mapping between source locations and addresses
    SemanticMemoryParadigm semanticMemoryParadigm_;     // Slow and precise, or fast and imprecise?
    Unparser::BasePtr unparser_;                        // For unparsing things to pseudo-assembly
    Unparser::BasePtr insnUnparser_;                    // For unparsing single instructions in diagnostics

    // Callback lists
    CfgAdjustmentCallbacks cfgAdjustmentCallbacks_;
    BasicBlockCallbacks basicBlockCallbacks_;
    FunctionPrologueMatchers functionPrologueMatchers_;
    FunctionPaddingMatchers functionPaddingMatchers_;

    // Special CFG vertices.
    ControlFlowGraph::VertexIterator undiscoveredVertex_;
    ControlFlowGraph::VertexIterator indeterminateVertex_;
    ControlFlowGraph::VertexIterator nonexistingVertex_;
    static const size_t nSpecialVertices = 3;

    // Protects the following data members
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;
    Progress::Ptr progress_;                            // Progress reporter to update, or null
    mutable size_t cfgProgressTotal_;                   // Expected total for the CFG progress bar; initialized at first report


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Serialization
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serializeCommon(S &s, const unsigned version) {
        s.template register_type<InstructionSemantics2::SymbolicSemantics::SValue>();
        s.template register_type<InstructionSemantics2::SymbolicSemantics::RiscOperators>();
        s.template register_type<InstructionSemantics2::DispatcherX86>();
        s.template register_type<InstructionSemantics2::DispatcherM68k>();
        s.template register_type<InstructionSemantics2::DispatcherPowerpc>();
        s.template register_type<SymbolicExpr::Interior>();
        s.template register_type<SymbolicExpr::Leaf>();
        s.template register_type<YicesSolver>();
        s.template register_type<Z3Solver>();
        s.template register_type<Semantics::SValue>();
        s.template register_type<Semantics::MemoryListState>();
        s.template register_type<Semantics::MemoryMapState>();
        s.template register_type<Semantics::RegisterState>();
        s.template register_type<Semantics::State>();
        s.template register_type<Semantics::RiscOperators>();
        s & BOOST_SERIALIZATION_NVP(settings_);
        // s & config_;                         -- FIXME[Robb P Matzke 2016-11-08]
        s & BOOST_SERIALIZATION_NVP(instructionProvider_);
        s & BOOST_SERIALIZATION_NVP(memoryMap_);
        s & BOOST_SERIALIZATION_NVP(cfg_);
        // s & vertexIndex_;                    -- initialized by rebuildVertexIndices
        s & BOOST_SERIALIZATION_NVP(aum_);
        // s & BOOST_SERIALIZATION_NVP(solver_); -- not saved/restored in order to override from command-line
        s & BOOST_SERIALIZATION_NVP(functions_);
        s & BOOST_SERIALIZATION_NVP(autoAddCallReturnEdges_);
        s & BOOST_SERIALIZATION_NVP(assumeFunctionsReturn_);
        s & BOOST_SERIALIZATION_NVP(stackDeltaInterproceduralLimit_);
        s & BOOST_SERIALIZATION_NVP(addressNames_);
        if (version >= 1)
            s & BOOST_SERIALIZATION_NVP(sourceLocations_);
        s & BOOST_SERIALIZATION_NVP(semanticMemoryParadigm_);
        // s & unparser_;                       -- not saved; restored from disassembler
        // s & cfgAdjustmentCallbacks_;         -- not saved/restored
        // s & basicBlockCallbacks_;            -- not saved/restored
        // s & functionPrologueMatchers_;       -- not saved/restored
        // s & functionPaddingMatchers_;        -- not saved/restored
        // s & undiscoveredVertex_;             -- initialized by rebuildVertexIndices
        // s & indeterminateVertex_;            -- initialized by rebuildVertexIndices
        // s & nonexistingVertex_;              -- initialized by rebuildVertexIndices
        // s & progress_;                       -- not saved/restored
        // s & cfgProgressTotal_;               -- not saved/restored
    }

    template<class S>
    void save(S &s, const unsigned version) const {
        const_cast<Partitioner*>(this)->serializeCommon(s, version);
    }

    template<class S>
    void load(S &s, const unsigned version) {
        serializeCommon(s, version);
        rebuildVertexIndices();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Constructors
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Default constructor.
     *
     *  The default constructor does not produce a usable partitioner, but is convenient when one needs to pass a default
     *  partitioner by value or reference. */
    Partitioner();

    /** Construct a partitioner.
     *
     *  The partitioner must be provided with a disassembler, which also determines the specimen's target architecture, and a
     *  memory map that represents a (partially) loaded instance of the specimen (i.e., a process). */
    Partitioner(Disassembler *disassembler, const MemoryMap::Ptr &map);

#ifdef ROSE_PARTITIONER_MOVE
    /** Move constructor. */
    Partitioner(BOOST_RV_REF(Partitioner));

    /** Move assignment. */
    Partitioner& operator=(BOOST_RV_REF(Partitioner));
#else
    // These are unsafe
    Partitioner(const Partitioner&);
    Partitioner& operator=(const Partitioner&);
#endif

    ~Partitioner();

    /** Return true if this is a default constructed partitioner.
     *
     *  Most methods won't work when applied to a default-constructed partitioner since it has no way to obtain instructions
     *  and it has an empty memory map.
     *
     *  Thread safety: Not thread safe. */
    bool isDefaultConstructed() const { return instructionProvider_ == NULL; }

    /** Reset CFG/AUM to initial state. */
    void clear() /*final*/;

    /** Configuration information.
     *
     *  The configuration holds information about functions and basic blocks which is used to provide default values and such.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    Configuration& configuration() { return config_; }
    const Configuration& configuration() const { return config_; }
    /** @} */
        
    /** Returns the instruction provider.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    InstructionProvider& instructionProvider() /*final*/ { return *instructionProvider_; }
    const InstructionProvider& instructionProvider() const /*final*/ { return *instructionProvider_; }
    /** @} */

    /** Returns the memory map.
     *
     *  It is generally unwise to make extensive changes to a memory map after the partitioner has started using it, because
     *  that means that any instructions or data obtained earlier from the map might not be an accurate representation of
     *  memory anymore.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    MemoryMap::Ptr memoryMap() const /*final*/ { return memoryMap_; }
    /** @} */

    /** Returns true if address is executable.
     *
     *  Thread safety: Not thread safe. */
    bool addressIsExecutable(rose_addr_t va) const /*final*/ {
        return memoryMap_!=NULL && memoryMap_->at(va).require(MemoryMap::EXECUTABLE).exists();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Unparsing
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Returns an unparser.
     *
     *  This unparser is initiallly a copy of the one provided by the disassembler, but it can be reset to something else if
     *  desired. This is the unparser used by the @ref unparse method for the partitioner as a whole, functions, basic blocks,
     *  and data blocks.  Instructions use the @ref insnUnparser instead.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    Unparser::BasePtr unparser() const /*final*/;
    void unparser(const Unparser::BasePtr&) /*final*/;
    /** @} */

    /** Returns an unparser.
     *
     *  This unparser is initially a copy of the one provided by the disassembler, and adjusted to be most useful for printing
     *  single instructions. By default, it prints the instruction address, mnemonic, and operands but not raw bytes, stack
     *  deltas, comments, or anything else.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    Unparser::BasePtr insnUnparser() const /*final*/;
    void insnUnparser(const Unparser::BasePtr&) /*final*/;
    /** @} */

    /** Configure the single-instruction unparser. */
    void configureInsnUnparser(const Unparser::BasePtr&) const /*final*/;

    /** Unparse some entity.
     *
     *  Unparses an instruction, basic block, data block, function, or all functions using the unparser returned by @ref
     *  unparser (except for instructions, which use the unparser returned by @ref insnUnparser).
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    std::string unparse(SgAsmInstruction*) const;
    void unparse(std::ostream&, SgAsmInstruction*) const;
    void unparse(std::ostream&, const BasicBlock::Ptr&) const;
    void unparse(std::ostream&, const DataBlock::Ptr&) const;
    void unparse(std::ostream&, const Function::Ptr&) const;
    void unparse(std::ostream&) const;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner CFG queries
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of bytes represented by the CFG.
     *
     *  This is a constant time operation.
     *
     *  Thread safety: Not thread safe. */
    size_t nBytes() const /*final*/ { return aum_.size(); }

    /** Returns the special "undiscovered" vertex.
     *
     *  The incoming edges for this vertex originate from the basic block placeholder vertices.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    ControlFlowGraph::VertexIterator undiscoveredVertex() /*final*/ {
        return undiscoveredVertex_;
    }
    ControlFlowGraph::ConstVertexIterator undiscoveredVertex() const /*final*/ {
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
     *  Thread safety: Not thread safe.
     *
     * @{ */
    ControlFlowGraph::VertexIterator indeterminateVertex() /*final*/ {
        return indeterminateVertex_;
    }
    ControlFlowGraph::ConstVertexIterator indeterminateVertex() const /*final*/ {
        return indeterminateVertex_;
    }
    /** @} */

    /** Returns the special "non-existing" vertex.
     *
     *  The incoming edges for this vertex originate from basic blocks that have no instructions but which aren't merely
     *  placeholders.  Such basic blocks exist when an attempt is made to discover a basic block but its starting address is
     *  memory which is not mapped or memory which is mapped without execute permission.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    ControlFlowGraph::VertexIterator nonexistingVertex() /*final*/ {
        return nonexistingVertex_;
    }
    ControlFlowGraph::ConstVertexIterator nonexistingVertex() const /*final*/ {
        return nonexistingVertex_;
    }
    /** @} */

    /** Returns the control flow graph.
     *
     *  Returns the global control flow graph. The CFG should not be modified by the caller except through the partitioner's
     *  own API.
     *
     *  Thread safety: Not thread safe. */
    const ControlFlowGraph& cfg() const /*final*/ { return cfg_; }

    /** Returns the address usage map.
     *
     *  Returns the global address usage map.  The AUM should not be modified by the caller except through the paritioner's own
     *  API.
     *
     *  Thread safety: Not thread safe. */
    const AddressUsageMap& aum() const /*final*/ { return aum_; }

    /** Returns the address usage map for a single function. */
    AddressUsageMap aum(const Function::Ptr&) const /*final*/;

    /** Entities that exist at a particular address.
     *
     *  Returns a vector of @ref AddressUser objects that describe instructions, basic blocks, data blocks, and functions that
     *  exist at the specified address. */
    std::vector<AddressUser> users(rose_addr_t) const /*final*/;

    /** Determine all ghost successors in the control flow graph.
     *
     *  The return value is a list of basic block ghost successors for which no basic block or basic block placeholder exists.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa basicBlockGhostSuccessors */
    std::set<rose_addr_t> ghostSuccessors() const /*final*/;

    /** Determine if an edge is intra-procedural.
     *
     *  An intra-procedural edge is an edge whose source and target are owned by the same function and which is not part of a
     *  function call, function transfer, or function return.  This function returns true if the edge is intra-procedural and
     *  false if not.  The return value is calculated as follows:
     *
     *  @li An edge of type @c E_FUNCTION_CALL, @c E_FUNCTION_XFER, or @c E_FUNCTION_RETURN is not intra-procedural regardless
     *      of which functions own the source and target blocks.
     *
     *  @li If a function is specified and that function is listed as an owner of both the source and target blocks, then the
     *      edge is intra-procedural.
     *
     *  @li If no function is specified and neither the source nor the target block have any function owners then the edge
     *      is intra-procedural.
     *
     *  @li If no function is specified and there exists some function that is an owner of both the source and target blocks
     *      then the edge is intra-procedural.
     *
     *  @li Otherwise the edge is not intra-procedural.
     *
     *  When no function is specified it can be ambiguous as to whether a branch is intra- or inter-procedural; a branch could
     *  be both intra- and inter-procedural.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa isEdgeInterProcedural.
     *
     * @{ */
    bool isEdgeIntraProcedural(ControlFlowGraph::ConstEdgeIterator edge,
                               const Function::Ptr &function = Function::Ptr()) const /*final*/;
    bool isEdgeIntraProcedural(const ControlFlowGraph::Edge &edge,
                               const Function::Ptr &function = Function::Ptr()) const /*final*/;
    /** @} */

    /** Determine if an edge is inter-procedural.
     *
     *  An inter-procedural edge is an edge which is part of a function call, a function transfer, or a function return or an
     *  edge whose source and target blocks are owned by different functions. This function returns true if the edge is
     *  inter-procedural and false if not.  The return value is calculated as follows:
     *
     *  @li An edge of type @c E_FUNCTION_CALL, @c E_FUNCTION_XFER, or @c E_FUNCTION_RETURN is inter-procedural regardless of
     *      which functions own the source and target blocks.
     *
     *  @li If two functions are specified and the source block is owned by the first function, the target block is owned by
     *      the second function, and the functions are different then the block is inter-procedural.
     *
     *  @li If only the source function is specified and the source block is owned by the source function and the target block
     *      is not owned by the source function then the edge is inter-procedural.
     *
     *  @li If only the target function is specified and the target block is owned by the target function and the source block
     *      is not owned by the target function then the edge is inter-procedural.
     *
     *  @li If no functions are specified and neither the source nor the target block have any function owners then the edge
     *      is inter-procedural.
     *
     *  @li If no functions are specified and the list of functions owning the source block is not equal to the list of
     *      functions owning the destination block then the block is inter-procedural.
     *
     *  @li Otherwise the edge is not inter-procedural.
     *
     *  When no functions are specified it can be ambiguous as to whether a branch is intra- or inter-procedural; a branch
     *  could be both intra- and inter-procedural.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa isEdgeIntraProcedural.
     *
     * @{ */
    bool isEdgeInterProcedural(ControlFlowGraph::ConstEdgeIterator edge,
                               const Function::Ptr &sourceFunction = Function::Ptr(),
                               const Function::Ptr &targetFunction = Function::Ptr()) const /*final*/;
    bool isEdgeInterProcedural(const ControlFlowGraph::Edge &edge,
                               const Function::Ptr &sourceFunction = Function::Ptr(),
                               const Function::Ptr &targetFunction = Function::Ptr()) const /*final*/;
    /** @} */



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner instruction operations
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of instructions attached to the CFG/AUM.
     *
     *  This statistic is computed in time linearly proportional to the number of basic blocks in the control flow graph.
     *
     *  Thread safety: Not thread safe. */
    size_t nInstructions() const /*final*/;

    /** Determines whether an instruction is attached to the CFG/AUM.
     *
     *  If the CFG/AUM represents an instruction that starts at the specified address, then this method returns the
     *  instruction/block pair, otherwise it returns an empty pair. The initial instruction for a basic block does not exist if
     *  the basic block is only represented by a placeholder in the CFG; such a basic block is said to be "undiscovered".
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    AddressUser instructionExists(rose_addr_t startVa) const /*final*/ {
        return aum_.findInstruction(startVa);
    }
    AddressUser instructionExists(SgAsmInstruction *insn) const /*final*/ {
        return aum_.findInstruction(insn);
    }
    /** @} */

    /** Returns the CFG vertex containing specified instruction.
     *
     *  Returns the control flow graph vertex that contains the specified instruction. If the instruction does not exist in the
     *  CFG then the end vertex is returned.
     *
     *  Thread safety: Not thread safe. */
    ControlFlowGraph::ConstVertexIterator instructionVertex(rose_addr_t insnVa) const;

    /** Returns instructions that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct instructions that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval. An instruction overlaps the interval if any of its bytes are within the interval.
     *
     *  The returned list of instructions are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<SgAsmInstruction*> instructionsOverlapping(const AddressInterval&) const /*final*/;

    /** Returns instructions that span an entire address interval.
     *
     *  Returns a sorted list of distinct instructions that are attached to the CFG/AUM and which span the entire specified
     *  interval.  An instruction spans the interval if the set of addresses for all its bytes are a superset of the interval.
     *
     *  The returned list of instructions are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<SgAsmInstruction*> instructionsSpanning(const AddressInterval&) const /*final*/;

    /** Returns instructions that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct instructions that are attached to the CFG/AUM and which are fully contained within
     *  the specified interval.  In order to be fully contained in the interval, the set of addresses of the bytes in the
     *  instruction must be a subset of the specified interval.
     *
     *  The returned list of instructions are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<SgAsmInstruction*> instructionsContainedIn(const AddressInterval&) const /*final*/;

    /** Returns the address interval for an instruction.
     *
     *  Returns the minimal interval describing from where the instruction was disassembled.  An instruction always exists in a
     *  contiguous region of memory, therefore the return value is a single interval rather than a set of intervals. If a null
     *  pointer is specified then an empty interval is returned.
     *
     *  Thread safety: Not thread safe. */
    AddressInterval instructionExtent(SgAsmInstruction*) const /*final*/;

    /** Discover an instruction.
     *
     *  Returns (and caches) the instruction at the specified address by invoking an InstructionProvider.  Unlike @ref
     *  instructionExists, the address does not need to be known by the CFG/AUM.
     *
     *  If the @p startVa is not mapped with execute permission or is improperly aligned for the architecture then a null
     *  pointer is returned.  If an instruction cannot be disassembled at the address (e.g., bad byte code or not implemented)
     *  then a special 1-byte "unknown" instruction is returned; such instructions have indeterminate control flow successors
     *  and no semantics.  If an instruction was previously returned for this address (including the "unknown" instruction)
     *  then that same instruction will be returned this time.
     *
     *  Thread safety: Not thread safe. */
    SgAsmInstruction* discoverInstruction(rose_addr_t startVa) const /*final*/;

    /** Cross references.
     *
     *  Scans all attached instructions looking for constants mentioned in the instructions and builds a mapping from those
     *  constants back to the instructions.  Only constants present in the @p restriction set are considered. */
    CrossReferences instructionCrossReferences(const AddressIntervalSet &restriction) const /*final*/;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner basic block placeholder operations
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of basic basic block placeholders in the CFG.
     *
     *  A placeholder optionally points to a basic block, and this method returns the number of placeholders in the CFG
     *  regardless of whether they point to a discovered basic block.  Note that vertices that are mere placeholders and don't
     *  point to a discovered basic block are not represented in the AUM since a placeholder has no instructions.
     *
     *  This is a constant-time operation.
     *
     *  Thread safety: Not thread safe. */
     size_t nPlaceholders() const /*final*/;

    /** Determines whether a basic block placeholder exists in the CFG.
     *
     *  Returns true if the CFG contains a placeholder at the specified address, and false if no such placeholder exists.  The
     *  placeholder may or may not point to a discovered basic block.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa findPlaceholder */
    bool placeholderExists(rose_addr_t startVa) const /*final*/;

    /** Find the CFG vertex for a basic block placeholder.
     *  
     *  If the CFG contains a basic block placeholder at the specified address then that CFG vertex is returned, otherwise the
     *  end vertex (<code>partitioner.cfg().vertices().end()</code>) is returned.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa placeholderExists
     *
     *  @{ */
    ControlFlowGraph::VertexIterator findPlaceholder(rose_addr_t startVa) /*final*/ {
        if (Sawyer::Optional<ControlFlowGraph::VertexIterator> found = vertexIndex_.getOptional(startVa))
            return *found;
        return cfg_.vertices().end();
    }
    ControlFlowGraph::ConstVertexIterator findPlaceholder(rose_addr_t startVa) const /*final*/ {
        if (Sawyer::Optional<ControlFlowGraph::VertexIterator> found = vertexIndex_.getOptional(startVa))
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
     *  or the new placeholder.
     *
     *  Thread safety: Not thread safe. */
    ControlFlowGraph::VertexIterator insertPlaceholder(rose_addr_t startVa) /*final*/;

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
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    BasicBlock::Ptr erasePlaceholder(const ControlFlowGraph::ConstVertexIterator &placeholder) /*final*/;
    BasicBlock::Ptr erasePlaceholder(rose_addr_t startVa) /*final*/;
    /** @} */



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner basic block operations
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Automatically drop semantics for attached basic blocks.
     *
     *  Basic blocks normally cache their semantic state as they're being discovered so that the state does not need to be
     *  recomputed from the beginning of the block each time a new instruction is appended.  However, caching this information
     *  can consume a large number of symbolic expression nodes which are seldom needed once the basic block is fully
     *  discovered.  Therefore, setting this property to true will cause a basic block's semantic information to be forgotten
     *  as soon as the basic block is attached to the CFG.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa basicBlockDropSemantics
     *
     * @{ */
    bool basicBlockSemanticsAutoDrop() const /*final*/ { return settings_.basicBlockSemanticsAutoDrop; }
    void basicBlockSemanticsAutoDrop(bool b) { settings_.basicBlockSemanticsAutoDrop = b; }
    /** @} */

    /** Immediately drop semantic information for all attached basic blocks.
     *
     *  Semantic information for all attached basic blocks is immediately forgotten by calling @ref
     *  BasicBlock::dropSemantics. It can be recomputed later if necessary.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa basicBlockSemanticsAutoDrop */
    void basicBlockDropSemantics() const /*final*/;

    /** Returns the number of basic blocks attached to the CFG/AUM.
     *
     *  This method returns the number of CFG vertices that are more than mere placeholders in that they point to an actual,
     *  discovered basic block.
     *
     *  This operation is linear in the number of vertices in the CFG.  Consider using @ref nPlaceholders instead.
     *
     *  Thread safety: Not thread safe. */
    size_t nBasicBlocks() const /*final*/;

    /** Returns all basic blocks attached to the CFG.
     *
     *  The returned list contains distinct basic blocks sorted by their starting address.
     *
     *  Thread safety: Not thread safe.
     *
     * @sa basicBlocksOverlapping, @ref basicBlocksSpanning, @ref basicBlocksContainedIn */
    std::vector<BasicBlock::Ptr> basicBlocks() const /*final*/;

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
     *  Thread safety: Not thread safe.
     *
     *  @sa placeholderExists
     *
     *  @{ */
    BasicBlock::Ptr basicBlockExists(rose_addr_t startVa) const /*final*/;
    BasicBlock::Ptr basicBlockExists(const BasicBlock::Ptr&) const /*final*/;
    /** @} */

    /** Returns basic blocks that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct basic blocks that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval.  By "overlap" we mean that the basic block has at least one instruction that overlaps
     *  with the specified interval.  An instruction overlaps the interval if any of its bytes are within the interval.
     *
     *  The returned list of basic blocks are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<BasicBlock::Ptr> basicBlocksOverlapping(const AddressInterval&) const /*final*/;

    /** Returns basic blocks that span an entire address interval.
     *
     *  Returns a sorted list of distinct basic blocks that are attached to the CFG/AUM and which span the entire specified
     *  interval. In order for a basic block to span an interval its set of instructions must span the interval.  In other
     *  words, the union of the addresses of the bytes contained in all the basic block's instructions is a superset of the
     *  specified interval.
     *
     *  The returned list of basic blocks are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<BasicBlock::Ptr> basicBlocksSpanning(const AddressInterval&) const /*final*/;

    /** Returns basic blocks that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct basic blocks that are attached to the CFG/AUM and which are fully contained within
     *  the specified interval.  In order to be fully contained in the interval, the union of the addresses of the bytes in the
     *  basic block's instructions must be a subset of the specified interval.
     *
     *  The returned list of basic blocks are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<BasicBlock::Ptr> basicBlocksContainedIn(const AddressInterval&) const /*final*/;

    /** Returns the basic block that contains a specific instruction address.
     *
     *  Returns the basic block that contains an instruction that starts at the specified address, or null if no such
     *  instruction or basic block exists in the CFG/AUM.
     *
     *  Thread safety: Not thread safe. */
    BasicBlock::Ptr basicBlockContainingInstruction(rose_addr_t insnVa) const /*final*/;

    /** Returns the addresses used by basic block instructions.
     *
     *  Returns an interval set which is the union of the addresses of the bytes in the basic block's instructions.  Most
     *  basic blocks are contiguous in memory and can be represented by a single address interval, but this is not a
     *  requirement in ROSE.  ROSE only requires that the global control flow graph has edges that enter at only the initial
     *  instruction of the basic block and exit only from its final instruction.  The instructions need not be contiguous or
     *  non-overlapping.
     *
     *  Thread safety: Not thread safe. */
    AddressIntervalSet basicBlockInstructionExtent(const BasicBlock::Ptr&) const /*final*/;

    /** Returns the addresses used by basic block data.
     *
     *  Returns an interval set which is the union of the extents for each data block referenced by this basic block.
     *
     *  Thread safety: Not thread safe. */
    AddressIntervalSet basicBlockDataExtent(const BasicBlock::Ptr&) const /*final*/;

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
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    BasicBlock::Ptr detachBasicBlock(rose_addr_t startVa) /*final*/;
    BasicBlock::Ptr detachBasicBlock(const BasicBlock::Ptr &basicBlock) /*final*/;
    BasicBlock::Ptr detachBasicBlock(const ControlFlowGraph::ConstVertexIterator &placeholder) /*final*/;
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
     *  The return value is the vertex for the new placeholder.
     *
     *  Thread safety: Not thread safe. */
    ControlFlowGraph::VertexIterator truncateBasicBlock(const ControlFlowGraph::ConstVertexIterator &basicBlock,
                                                        SgAsmInstruction *insn) /*final*/;

    /** Attach a basic block to the CFG/AUM.
     *
     *  The specified basic block is inserted into the CFG/AUM.  If the CFG already has a placeholder for the block then the
     *  specified block is stored at that placeholder, otherwise a new placeholder is created first.  A basic block cannot be
     *  attached if the CFG/AUM already knows about a different basic block at the same address.  Attempting to attach a block
     *  which is already attached is allowed, and is a no-op. It is an error to specify a null pointer for the basic block.
     *
     *  If the basic block owns any data blocks, those data blocks are also attached to the partitioner.
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
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    void attachBasicBlock(const BasicBlock::Ptr&) /*final*/;
    void attachBasicBlock(const ControlFlowGraph::ConstVertexIterator &placeholder, const BasicBlock::Ptr&) /*final*/;
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
     *      indeterminate successors (see below) and no semantics.
     *
     *  @li Note: at this point the user-defined basic block successors callbacks are invoked. They can query characteristics
     *      of the basic block, adjust the basic block successor cache (see below) and other characteristics of the block, and
     *      write values into a results structure that is used by some of the subsequent block termination conditions.
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
     *  @li The user-defined basic block callbacks indicated that the block should be terminated.  If the callback set the @c
     *      terminate member of the @c results output argument to @ref BasicBlockCallback::TERMINATE_NOW or @ref
     *      BasicBlockCallback::TERMINATE_PRIOR, then the current instruction either becomes the final instruction of the basic
     *      block, or the prior instruction becomes the final instruction.
     *
     *  @li The instruction is the final instruction of the basic block according to configuration information for that block.
     *      The basic block is terminated at this instruction.
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
     *  @section successors Calculation of control flow successors
     *
     *  The basic block has a list of successor addresses and control flow edge types, but these are not added to the
     *  partitioner's CFG until the basic block is attached. A basic block's successor list can come from three places:
     *
     *  @li A block's successor list is initialized according to the final instruction. This list can be based on instruction
     *      pattern or semantic information for the entire block up to that point.
     *
     *  @li Basic block callbacks are allowed to adjust or replace the block's successor list.
     *
     *  @li If the configuration file has a successor list then that list is used.  The list only applies if the configuration
     *      for the basic block also specifies a final instruction address and that address matches the current final
     *      instruction of the basic block.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    BasicBlock::Ptr discoverBasicBlock(rose_addr_t startVa) const /*final*/;
    BasicBlock::Ptr discoverBasicBlock(const ControlFlowGraph::ConstVertexIterator &placeholder) const /*final*/;
    /** @} */

    /** Determine successors for a basic block.
     *
     *  Basic block successors are returned as a vector in no particular order.  This method returns the most basic successors;
     *  for instance, function call instructions will have an edge for the called function but no edge for the return.  The
     *  basic block holds a successor cache which is consulted/updated by this method.
     *
     *  If @p precision is @ref Precision::HIGH, then use instruction semantics if they're available. Otherwise, use a more
     *  naive method that usually works fine for most architectures with code generated by mainstream compilers.  Low precision
     *  is substantially faster than high precision.
     *
     *  The basic block need not be complete or attached to the CFG/AUM. A basic block that has no instructions has no
     *  successors.
     *
     *  Thread safety: Not thread safe. */
    BasicBlock::Successors basicBlockSuccessors(const BasicBlock::Ptr&,
                                                Precision::Level precision = Precision::HIGH) const /*final*/;

    /** Determines concrete successors for a basic block.
     *
     *  Returns a vector of distinct, concrete successor addresses.  Semantics is identical to @ref bblockSuccessors except
     *  non-concrete values are removed from the list.  The optional @p isComplete argument is set to true or false depending
     *  on whether the set of returned concrete successors represents the complete set of successors (true) or some member in
     *  the complete set is not concrete (false).
     *
     *  Thread safety: Not thread safe. */
    std::vector<rose_addr_t> basicBlockConcreteSuccessors(const BasicBlock::Ptr&, bool *isComplete=NULL) const /*final*/;

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
     *  originating from since they can originate from anywhere in the basic block.
     *
     *
     *  Thread safety: Not thread safe. */
    std::set<rose_addr_t> basicBlockGhostSuccessors(const BasicBlock::Ptr&) const /*final*/;

    /** Determine if a basic block looks like a function call.
     *
     *  If the basic block appears to be a function call by some analysis then this function returns true.  The analysis may
     *  use instruction semantics to look at the stack, it may look at the kind of instructions in the block, it may look for
     *  patterns at the callee address if known, etc. The basic block caches the result of this analysis.
     *
     *  If the analysis cannot prove that the block is a function call, then returns false.
     *
     *  Thread safety: Not thread safe. */
    bool basicBlockIsFunctionCall(const BasicBlock::Ptr&, Precision::Level precision = Precision::HIGH) const /*final*/;

    /** Determine if a basic block looks like a function return.
     *
     *  If the basic block appears by some analysis to be a return from a function call, then this function returns true.  The
     *  anlaysis may use instruction semantics to look at the stack, it may look at the kind of instructions in the lbock, it
     *  may look for patterns, etc.  The basic block caches the result of this analysis.
     *
     *  @todo Partitioner::basicBlockIsFunctionReturn does not currently detect callee-cleanup returns because the return
     *  address is not the last thing popped from the stack. FIXME[Robb P. Matzke 2014-09-15]
     *
     *  Thread safety: Not thread safe. */
    bool basicBlockIsFunctionReturn(const BasicBlock::Ptr&) const /*final*/;

    /** Determine if the basic block pops at least one byte from the stack.
     *
     * Returns true if the basic block has a net effect of popping at least one byte from the stack compared to the original
     * stack pointer. Returns false if the block does not pop or its behavior cannot be determined. */
     bool basicBlockPopsStack(const BasicBlock::Ptr&) const /*final*/;

    /** Return the stack delta expression.
     *
     *  The stack delta is the value of the stack pointer register at the entrance to the specified block minus the stack delta
     *  at the entry point of the function.  The function entry point stack delta is zero; the return address pushed onto the
     *  stack by the caller is attributed to the caller, and popping the return address during the callee's return is
     *  attributed to the callee.  Thus, most functions that use a caller-cleans-up-args ABI will have a stack delta equal to
     *  the size of the return address, and that delta will be positive for stacks that grow down, and negative for stacks that
     *  grow up.
     *
     *  The resulting stack delta can be four different kinds of values:
     *
     *  @li Never-computed is indicated by the basic block not caching any value for the stack delta.  This method will always
     *      attempt to compute a stack delta if none is cached in the basic block.
     *
     *  @li Error is indicated by a cached null expression. Errors are usually due to a reachable basic block that contains an
     *      instruction for which semantics are not known.
     *
     *  @li Constant offset, for which the @c is_number predicate applied to the return value is true.
     *
     *  @li Top, indicated by a non-null return value for which @c is_number is false.  This results when two or more paths
     *      through the control flow graph result in different constant offsets. It can also occur when the algebraic
     *      simplifications that are built into ROSE fail to simplify a constant expression.
     *
     *  Two stack deltas are computed for each basic block: the stack delta at the start of the block and the start delta at
     *  the end of the block, returned by the "in" and "out" variants of this method, respectively. Since basic blocks can be
     *  shared among multiple functions and have a different delta in each, a function context must be provided as an
     *  argument.
     *
     *  Since stack deltas use the control flow graph during the analysis, the specified basic block and function must be
     *  attached to the CFG/AUM before calling this method. Also, since predefined stack deltas are based on function names,
     *  function calls must be to basic blocks that are attached to a function. Note that currently (Dec 2014) PE thunks
     *  transfering to a non-linked dynamic function are given names by @ref ModulesPe::nameImportThunks, which runs after all
     *  basic blocks and functions have been discovered and attached to the CFG/AUM.
     *
     *  Thread safety: Not thread safe.
     *
     *  @sa functionStackDelta and @ref allFunctionStackDelta
     *
     * @{ */
    BaseSemantics::SValuePtr basicBlockStackDeltaIn(const BasicBlock::Ptr&, const Function::Ptr &function) const /*final*/;
    BaseSemantics::SValuePtr basicBlockStackDeltaOut(const BasicBlock::Ptr&, const Function::Ptr &function) const /*final*/;
    /** @} */

    /** Clears all cached stack deltas.
     *
     *  Causes all stack deltas for basic blocks and functions that are attached to the CFG/AUM to be forgotten.  This is
     *  useful if one needs to recompute deltas in light of new information.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    void forgetStackDeltas() const /*final*/;
    void forgetStackDeltas(const Function::Ptr&) const /*final*/;
    /** @} */

    /** Property: max depth for inter-procedural stack delta analysis.
     *
     *  Stack delta analysis will be interprocedural when this property has a value greater than one. Interprocedural analysis
     *  is only used when a called function's stack delta is unknown.  Large values for this property will likely be clipped by
     *  the actual dataflow implementation.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    size_t stackDeltaInterproceduralLimit() const /*final*/ { return stackDeltaInterproceduralLimit_; }
    void stackDeltaInterproceduralLimit(size_t n) /*final*/ { stackDeltaInterproceduralLimit_ = std::max(size_t(1), n); }
    /** @} */

    /** Determine if part of the CFG can pop the top stack frame.
     *
     *  This analysis enters the CFG at the specified basic block and follows certain edges looking for a basic block where
     *  @ref basicBlockIsFunctionReturn returns true.  The analysis caches results in the @ref BasicBlock::mayReturn
     *  "mayReturn" property of the reachable basic blocks.
     *
     *  Since the analysis results might change if edges are inserted or erased even on distant basic blocks, the @ref
     *  basicBlockMayReturnReset method can be used to "forget" the property for all basic blocks in the CFG/AUM.
     *
     *  A basic block's may-return property is computed as follows (the first applicable rule wins):
     *
     *  @li If the block is owned by a function and the function's name is present on a whitelist or blacklist
     *      then the block's may-return is positive if whitelisted or negative if blacklisted.
     *
     *  @li If the block is a non-existing placeholder (i.e. its address is not mapped with execute permission) then
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
     *  Thread safety: Not thread safe.
     *
     * @{ */
    Sawyer::Optional<bool> basicBlockOptionalMayReturn(const BasicBlock::Ptr&) const /*final*/;

    Sawyer::Optional<bool> basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexIterator&) const /*final*/;
    /** @} */

    /** Clear all may-return properties.
     *
     *  This function is const because it doesn't modify the CFG/AUM; it only removes the may-return property from all the
     *  CFG/AUM basic blocks.
     *
     *  Thread safety: Not thread safe. */
    void basicBlockMayReturnReset() const /*final*/;

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
    bool mayReturnIsSignificantEdge(const ControlFlowGraph::ConstEdgeIterator &edge,
                                    std::vector<MayReturnVertexInfo> &vertexInfo) const;

    // Determine (and cache in vertexInfo) whether any callees return.
    boost::logic::tribool mayReturnDoesCalleeReturn(const ControlFlowGraph::ConstVertexIterator &vertex,
                                                    std::vector<MayReturnVertexInfo> &vertexInfo) const;

    // Maximum may-return result from significant successors including phantom call-return edge.
    boost::logic::tribool mayReturnDoesSuccessorReturn(const ControlFlowGraph::ConstVertexIterator &vertex,
                                                       std::vector<MayReturnVertexInfo> &vertexInfo) const;

    // The guts of the may-return analysis
    Sawyer::Optional<bool> basicBlockOptionalMayReturn(const ControlFlowGraph::ConstVertexIterator &start,
                                                       std::vector<MayReturnVertexInfo> &vertexInfo) const;



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner data block operations
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of data blocks attached to the CFG/AUM.
     *
     *  This is a relatively expensive operation compared to querying the number of basic blocks or functions.
     *
     *  Thread safety: Not thread safe. */
    size_t nDataBlocks() const /*final*/;

    /** Determine if a data block or its equivalent is attached to the CFG/AUM.
     *
     *  If the AUM contains the specified data block or an equivalent data block, then return the non-null pointer for the data
     *  block that's already present in the AUM. Otherwise return a null pointer.
     *
     *  Thread safety: Not thread safe. */
    DataBlock::Ptr dataBlockExists(const DataBlock::Ptr&) const /*final*/;

    /** Find an existing data block.
     *
     *  Finds a data block that spans the specified address interval or which can be extended to span the address interval.
     *  The first choice is to return the smallest data block that spans the entire interval; second choice is the largest
     *  block that contains the first byte of the interval.  If there is a tie in sizes then the block with the highest
     *  starting address wins.  If no suitable data block can be found then the null pointer is returned.
     *
     *  Thread safety: Not thread safe. */
    DataBlock::Ptr findBestDataBlock(const AddressInterval&) const /*final*/;

    /** Attach a data block to the CFG/AUM.
     *
     *  Attaches the data block to the CFG/AUM if it is not already attached and there is no equivalent data block already
     *  attached. If no equivalent data block exists in the CFG/AUM then the specified block is attached and will have an
     *  ownership count of zero since none of its owners are attached (otherwise the data block or an equivalent block would
     *  also have been already attached). It is an error to supply a null pointer.
     *
     *  Returns the canonical data block, either one that already existed in the CFG/AUM or the specified data block which is
     *  now attached.
     *
     *  Thread safety: Not thread safe. */
    DataBlock::Ptr attachDataBlock(const DataBlock::Ptr&) /*final*/;

    /** Detaches a data block from the CFG/AUM.
     *
     *  The specified data block is detached from the CFG/AUM and thawed, and returned so it can be modified.  It is an error
     *  to attempt to detach a data block which is owned by attached basic blocks or attached functions.
     *
     *  Thread safety: Not thread safe. */
    void detachDataBlock(const DataBlock::Ptr&) /*final*/;

    /** Attach a data block to an attached or detached function.
     *
     *  Causes the data block to be owned by the specified function. If the function is attached to this partitioner (i.e.,
     *  appears in the control flow graph and address usage map) then the specified data block is also attached to this
     *  partitioner (if it wasn't already) and will appear in the address usage map.
     *
     *  Returns either the specified data block or an equivalent data block that's already owned by the function.
     *
     *  Thread safety: Not thread safe. */
    DataBlock::Ptr attachDataBlockToFunction(const DataBlock::Ptr&, const Function::Ptr&) /*final*/;

    /** Attach a data block to a basic block.
     *
     *  Causes the data block to be owned by the specified basic block. If the basic block is attached to this partitioner
     *  (i.e., appears in the control flow graph and address usage map) then the specified data block is also attached to this
     *  partitioner (if it wasn't already) and will appear in the address usage map.
     *
     *  If the basic block already owns a data block with the same starting address and size, then the specified data block
     *  is not attached to the basic block.
     *
     *  Returns either the specified data block or an equivalent data block that's already owned by the basic block.
     *
     *  Thread safety: Not thread safe. */
    DataBlock::Ptr attachDataBlockToBasicBlock(const DataBlock::Ptr&, const BasicBlock::Ptr&) /*final*/;

    /** Returns data blocks that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval.  All bytes represented by the data block are returned, even if they are unused or
     *  marked as padding in the data block type.
     *
     *  The returned list of data blocks are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<DataBlock::Ptr> dataBlocksOverlapping(const AddressInterval&) const /*final*/;

    /** Returns data blocks that span an entire address interval.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM and which span the entire specified
     *  interval. All bytes represented by the data block are returned, even if they are unused or marked as padding in the
     *  data block type.
     *
     *  The returned list of data blocks are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<DataBlock::Ptr> dataBlocksSpanning(const AddressInterval&) const /*final*/;

    /** Returns data blocks that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM and which are fully contained within the
     *  specified interval.  All bytes represented by the data block are returned, even if they are unused or marked as padding
     *  in the data block type.
     *
     *  The returned list of data blocks are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<DataBlock::Ptr> dataBlocksContainedIn(const AddressInterval&) const /*final*/;

    /** Returns the addresses used by a data block.
     *
     *  Returns an address interval describing all addresses of the data block, even if they are unused or marked as padding
     *  in the data block type.  Since all addresses are returned, the extent of a data block is always contiguous.
     *
     *  Thread safety: Not thread safe. */
    AddressInterval dataBlockExtent(const DataBlock::Ptr&) const /*final*/;

    /** Returns the list of all attached data blocks.
     *
     *  Returns a sorted list of distinct data blocks that are attached to the CFG/AUM.
     *
     *  Thread safety: Not thread safe. */
    std::vector<DataBlock::Ptr> dataBlocks() const /*final*/;



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner function operations
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Returns the number of functions attached to the CFG/AUM.
     *
     *  This is a constant-time operation.
     *
     *  Thread safety: Not thread safe. */
    size_t nFunctions() const /*final*/ { return functions_.size(); }

    /** Determines whether a function exists in the CFG/AUM.
     *
     *  If the CFG/AUM knows about the specified function then this method returns a pointer to that function, otherwise it
     *  returns the null pointer.
     *
     *  The argument identifies the function for which to search:
     *
     *  @li The function's entry address.
     *  @li The basic block that serves as the function's entry block.
     *  @li A function pointer.
     *
     *  If the argument is a function pointer then this method checks that the specified function exists in the CFG/AUM and
     *  returns the argument if it exists, or else null if it doesn't exist. This test uses the function pointer directly, not
     *  the entry address -- it returns non-null only if the argument is the actual function object stored in the CFG/AUM.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    Function::Ptr functionExists(rose_addr_t entryVa) const /*final*/;
    Function::Ptr functionExists(const BasicBlock::Ptr &entryBlock) const /*final*/;
    Function::Ptr functionExists(const Function::Ptr &function) const /*final*/;
    /** @} */

    /** All functions attached to the CFG/AUM.
     *
     *  Returns a vector of distinct functions sorted by their entry address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<Function::Ptr> functions() const /*final*/;

    /** Returns functions that overlap with specified address interval.
     *
     *  Returns a sorted list of distinct functions that are attached to the CFG/AUM and which overlap at least one byte in
     *  the specified address interval.  By "overlap" we mean that the function owns at least one basic block or data block
     *  that overlaps with the interval.
     *
     *  The returned list of funtions are sorted by their entry address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<Function::Ptr> functionsOverlapping(const AddressInterval&) const /*final*/;

    /** Returns functions that span an entire address interval.
     *
     *  Returns a sorted list of distinct functions that are attached to the CFG/AUM and which span the entire specified
     *  interval. In order for a function to span the interval its extent must be a superset of the interval. See @ref
     *  functionExtent.  In other words, the union of all the addresseses represented by the function's basic blocks and data
     *  blocks is a superset of the specified interval.
     *
     *  The returned list of functions are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<Function::Ptr> functionsSpanning(const AddressInterval&) const /*final*/;

    /** Returns functions that are fully contained in an address interval.
     *
     *  Returns a sorted list of distinct functions that are attached to the CFG/AUM and which are fully contained within
     *  the specified interval.  In order to be fully contained in the interval, the addresses represented by the function's
     *  basic blocks and data blocks must be a subset of the specified interval.
     *
     *  The returned list of functions are sorted by their starting address.
     *
     *  Thread safety: Not thread safe. */
    std::vector<Function::Ptr> functionsContainedIn(const AddressInterval&) const /*final*/;

    /** Returns the addresses used by a function.
     *
     *  Returns an interval set which is the union of the addresses of the function's basic blocks and/or data blocks.  Most
     *  functions are contiguous in memory and can be represented by a single address interval, but this is not a
     *  requirement in ROSE.
     *
     *  The versions of these functions that take an AddressIntervalSet argument simply insert additional address intervals
     *  into that set without clearing it first.
     *
     *  @li @ref functionExtent -- addresses of all instructions and static data
     *  @li @ref functionBasicBlockExtent -- addresses of instructions
     *  @li @ref functionDataBlockExtent -- addresses of static data
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    AddressIntervalSet functionExtent(const Function::Ptr&) const /*final*/;
    void functionExtent(const Function::Ptr &function, AddressIntervalSet &retval /*in,out*/) const /*final*/;
    AddressIntervalSet functionBasicBlockExtent(const Function::Ptr &function) const /*final*/;
    void functionBasicBlockExtent(const Function::Ptr &function, AddressIntervalSet &retval /*in,out*/) const /*final*/;
    AddressIntervalSet functionDataBlockExtent(const Function::Ptr &function) const /*final*/;
    void functionDataBlockExtent(const Function::Ptr &function, AddressIntervalSet &retval /*in,out*/) const /*final*/;
    /** @} */

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
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    size_t attachFunction(const Function::Ptr&) /*final*/;
    size_t attachFunctions(const Functions&) /*final*/;
    /** @} */

    /** Attaches or merges a function into the CFG/AUM.
     *
     *  If no other function exists at the specified function's entry address, then this method behaves identically to @ref
     *  attachFunction.  Otherwise, this method attempts to merge the specified function into an existing function.  In any
     *  case, it returns a pointer to the function in the CFG/AUM (the existing one, or the new one). If the merge is not
     *  possible, then an exception is thrown.
     *
     *  One of the things that are merged are the basic blocks.  If the function being attached is A and the partitioner
     *  already knows about B having the same entry address as A, then all basic blocks owned by A are now (also) owned by
     *  B. Some of those blocks happened to be owned by other functions also attached to the partitioner they continue to be
     *  owned also by those other functions. Data blocks are handled in a similar fashion.
     *
     *  Thread safety: Not thread safe. */
    Function::Ptr attachOrMergeFunction(const Function::Ptr&) /*final*/;

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
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    size_t attachFunctionBasicBlocks(const Functions&) /*final*/;
    size_t attachFunctionBasicBlocks(const Function::Ptr&) /*final*/;
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
     *  user directly through its API. Attempting to detach a function that is already detached has no effect.
     *
     *  Thread safety: Not thread safe. */
    void detachFunction(const Function::Ptr&) /*final*/;

    /** Finds functions that own the specified basic block.
     *
     *  Return the functions that own the specified basic block(s). The returned vector has distinct function pointers sorted
     *  by their entry addresses. Usually a basic block is owned by zero or one function. If a basic block cannot be found or
     *  if it has no owning functions then an empty vector is returned.  The returned functions are all attached to the
     *  partitioner (that's how the partitioner knows about them); detached functions are not found.
     *
     *  Basic blocks can be specified in a number of ways:
     *
     *  @li As a CFG vertex. This is the fastest method since ownership information is stored directly in the CFG vertex. This
     *  is identical to calling @ref CfgVertex::owningFunctions except the return value is a vector and the functions are
     *  sorted differently.  The only expense is sorting the return value, which is usually a single function and therefore
     *  constant time (the following bullets also assume this is constant time).
     *
     *  @li As a starting address. If a basic block with the specified starting address exists in the CFG then its function
     *  owners are retrieved from the vertex.  Runtime is O(log |V|) where |V| is the number of vertices in the CFG.
     *
     *  @li As a block pointer.  The address of the block is used to find the CFG vertex from which ownership information is
     *  obtained. Basic block ownership is stored in the CFG, therefore if the provided basic block is not attached to the
     *  partitioner, the partitioner substitutes one that is attached. Runtime is O(log |V|) where |V| is the number of
     *  vertices in the CFG.
     *
     *  @li As a vector of any of the above. The returned vector is the union of the owning functions. Run time is O(N M) where
     *  N is the length of the vector and M is the time from above.
     *
     *  If @p doSort is clear then the result vector is not sorted, although it will still consist of unique function
     *  pointers.  The return value from the variants that take more than one basic block is always sorted.
     *
     *  The returned function will be a function that is attached to the CFG/AUM; detached functions are never returned since
     *  the partitioner does not necessarily know about them.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    std::vector<Function::Ptr>
    functionsOwningBasicBlock(const ControlFlowGraph::Vertex&, bool doSort = true) const /*final*/;

    std::vector<Function::Ptr>
    functionsOwningBasicBlock(const ControlFlowGraph::ConstVertexIterator&, bool doSort = true) const /*final*/;

    std::vector<Function::Ptr>
    functionsOwningBasicBlock(rose_addr_t bblockVa, bool doSort = true) const /*final*/;

    std::vector<Function::Ptr>
    functionsOwningBasicBlock(const BasicBlock::Ptr&, bool doSort = true) const /*final*/;

    template<class Container> // container can hold any type accepted by functionsOwningBasicBlock
    std::vector<Function::Ptr>
    functionsOwningBasicBlocks(const Container &bblocks) const /*final*/ {
        std::vector<Function::Ptr> retval;
        BOOST_FOREACH (const typename Container::value_type& bblock, bblocks) {
            BOOST_FOREACH (const Function::Ptr &function, functionsOwningBasicBlock(bblock, false))
                insertUnique(retval, function, sortFunctionsByAddress);
        }
        return retval;
    }
    /** @} */

    /** Scans the CFG to find function calls.
     *
     *  Scans the CFG without modifying it and looks for edges that are marked as being function calls.  A function is created
     *  at each call if one doesn't already exist in the CFG/AUM, and the list of created functions is returned.  None of the
     *  created functions are added to the CFG/AUM.
     *
     *  See also @ref discoverFunctionEntryVertices which returns a superset of the functions returned by this method.
     *
     *  Thread safety: Not thread safe. */
    std::vector<Function::Ptr> discoverCalledFunctions() const /*final*/;

    /** Scans the CFG to find function entry basic blocks.
     *
     *  Scans the CFG without modifying it in order to find vertices (basic blocks and basic block placeholders) that are the
     *  entry points of functions.  A vertex is a function entry point if it has an incoming edge that is a function call or if
     *  it is the entry block of a function that already exists.
     *
     *  The returned function pointers are sorted by function entry address.
     *
     *  See also @ref discoverFunctionCalls which returns a subset of the functions returned by this method.
     *
     *  Thread safety: Not thread safe. */
    std::vector<Function::Ptr> discoverFunctionEntryVertices() const /*final*/;

    /** True if function is a thunk.
     *
     *  If the function is non-null and a thunk then some information about the thunk is returned, otherwise nothing is
     *  returned.  A function is a thunk if it has the @ref SgAsmFunction::FUNC_THUNK bit set in its reason mask, and it has
     *  exactly one basic block, and the basic block has exactly one successor, and the successor is concrete.
     *
     *  As a side effect, the basic block's outgoing edge type is changed to E_FUNCTION_XFER.
     *
     *  Thread safety: Not thread safe. */
    Sawyer::Optional<Thunk> functionIsThunk(const Function::Ptr&) const /*final*/;

    /** Adds basic blocks to a function.
     *
     *  Attempts to discover the basic blocks that should belong to the specified function. It does so by finding all CFG
     *  vertices that are reachable from the already-owned vertices without following edges that are marked as function calls,
     *  function transfers, or function returns and without following edges that lead to the entry point of another function.
     *
     *  The CFG is not modified by this method. The function is modified and must not exist in the CFG; the function must be in
     *  a thawed state.
     *
     *  Thread safety: Not thread safe. */
    void discoverFunctionBasicBlocks(const Function::Ptr &function) const /*final*/;

    /** Returns ghost successors for a single function.
     *
     *  Returns the set of basic block starting addresses that are naive successors for the basic blocks of a function but
     *  which are not actual control flow successors due to the presence of opaque predicates.
     *
     *  Thread safety: Not thread safe. */
    std::set<rose_addr_t> functionGhostSuccessors(const Function::Ptr&) const /*final*/;

    /** Returns a function call graph.
     *
     *  If @p allowParallelEdges is true then the returned call graph will have one edge for each function call and each edge
     *  will have a count of one.  Otherwise multiple calls between the same pair of functions are coalesced into single edges
     *  with non-unit counts in the call graph.
     *
     *  Thread safety: Not thread safe. */
    FunctionCallGraph functionCallGraph(AllowParallelEdges::Type allowParallelEdges) const /*final*/;

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
     *  non-null for each reachable block in the function.
     *
     *  If the configuration information specifies a stack delta for this function then that delta is used instead of
     *  performing any analysis.
     *
     *  Thread safety: Not thread safe. */
    BaseSemantics::SValuePtr functionStackDelta(const Function::Ptr &function) const /*final*/;

    /** Compute stack delta analysis for all functions.
     *
     *  Thread safety: Not thread safe. */
    void allFunctionStackDelta() const /*final*/;

    /** May-return analysis for one function.
     *
     *  Determines if a function can possibly return to its caller. This is a simple wrapper around @ref
     *  basicBlockOptionalMayReturn invoked on the function's entry block. See that method for details.
     *
     *  Thread safety: Not thread safe. */
    Sawyer::Optional<bool> functionOptionalMayReturn(const Function::Ptr &function) const /*final*/;

    /** Compute may-return analysis for all functions.
     *
     *  Thread safety: Not thread safe. */
    void allFunctionMayReturn() const /*final*/;

    /** Calling convention analysis for one function.
     *
     *  Analyses a function to determine characteristics of its calling convention, such as which registers are callee-saved,
     *  which registers and stack locations are input parameters, and which are output parameters.   The calling convention
     *  analysis itself does not define the entire calling convention--instead, the analysis results must be matched against a
     *  dictionary of calling convention definitions. Each function has a @ref Function::callingConventionDefinition
     *  "callingConventionDefinition" property that points to the best definition; if this method reanalyzes the calling
     *  convention then the definition is reset to the null pointer.
     *
     *  Since this analysis is based on data-flow, which is based on a control flow graph, the function must be attached to the
     *  CFG/AUM and all its basic blocks must also exist in the CFG/AUM.
     *
     *  Warning: If the specified function calls other functions for which a calling convention analysis has not been run the
     *  analysis of this function may be incorrect.  This is because the analysis of this function must know which registers
     *  are clobbered by the call in order to produce accurate results. See also, @ref allFunctionCallingConvention. If a
     *  default calling convention is supplied then it determines which registers are clobbered by a call to a function that
     *  hasn't been analyzed yet.
     *
     *  Calling convention analysis results are stored in the function object. If calling convention analysis has already been
     *  run for this function then the old results are returned.  The old results can be cleared on a per-function basis with
     *  <code>function->callingConventionAnalysis().clear()</code>.
     *
     *  See also, @ref allFunctionCallingConvention, which computes calling convention characteristics for all functions at
     *  once, and @ref functionCallingConventionDefinitions, which returns matching definitions.
     *
     *  Thread safety: Not thread safe. */
    const CallingConvention::Analysis&
    functionCallingConvention(const Function::Ptr&,
                              const CallingConvention::Definition::Ptr &dflt = CallingConvention::Definition::Ptr())
        const /*final*/;

    /** Compute calling conventions for all functions.
     *
     *  Analyzes calling conventions for all functions and caches results in the function objects. The analysis uses a depth
     *  first traversal of the call graph, invoking the analysis as the traversal unwinds. This increases the chance that the
     *  calling conventions of callees are known before their callers are analyzed. However, this analysis must break cycles in
     *  mutually recursive calls, and does so by using an optional default calling convention where the cycle is broken. This
     *  default is not inserted as a result--it only influences the data-flow portion of the analysis.
     *
     *  After this method runs, results can be queried per function with either @ref Function::callingConventionAnalysis or
     *  @ref functionCallingConvention.
     *
     *  Thread safety: Not thread safe. */
    void
    allFunctionCallingConvention(const CallingConvention::Definition::Ptr &dflt = CallingConvention::Definition::Ptr())
        const /*final*/;

    /** Return list of matching calling conventions.
     *
     *  Given a function, run a calling convention analysis (if necessary) and return the list of common architecture calling
     *  convention definitions that match the characteristics of the function.  This method differs from @ref
     *  functionCallingConvention in that the former returns an analysis object that holds the function characteristics, while
     *  this method then takes a list of common calling convention definitions (based on the architecture) and returns those
     *  definitions that are consistent with the function characteristics.
     *
     *  Since this analysis is based on data-flow, which is based on a control flow graph, the function must be attached to the
     *  CFG/AUM and all ts basic blocks must also exist in the CFG/AUM.
     *
     *  If the specified function calls other functions for which a calling convention analysis has not been run the analysis
     *  of this function may be incorrect.  This is because the analysis of this function must know which registers are
     *  clobbered by the call in order to produce accurate results. See also, @ref allFunctionCallingConvention. If a default
     *  calling convention is supplied then it determines which registers are clobbered by a call to a function that hasn't
     *  been analyzed yet.
     *
     *  If the calling convention analysis fails or no common architecture calling convention definition matches the
     *  characteristics of the function, then an empty list is returned.  This method does not access the function's calling
     *  convention property -- it recomputes the list of matching definitions from scratch.
     *
     *  Thread safety: Not thread safe.
     *
     *  See also, @ref functionCallingConvention, which returns the calling convention characteristics of a function (rather
     *  than definitions), and @ref allFunctionCallingConvention, which runs that analysis over all functions. */
    CallingConvention::Dictionary
    functionCallingConventionDefinitions(const Function::Ptr&,
                                         const CallingConvention::Definition::Ptr &dflt = CallingConvention::Definition::Ptr())
        const /*final*/;

    /** Analyzes calling conventions and saves results.
     *
     *  This method invokes @ref allFunctionCallingConvention to analyze the behavior of every function, then finds the list of
     *  matching definitions for each function. A histogram of definitions is calculated and each function is re-examined. If
     *  any function matched more than one definition, then the most frequent of those definitions is chosen as that function's
     *  "best" calling convention definition and saved in the @ref Function::callingConventionDefinition property.
     *
     *  If a default calling convention definition is provided, it gets passed to the @ref allFunctionCallingConvention
     *  analysis. The default is also assigned as the @ref Function::callingConventionDefinition property of any function for
     *  which calling convention analysis fails.
     *
     *  Thread safety: Not thread safe. */
    void
    allFunctionCallingConventionDefinition(const CallingConvention::Definition::Ptr &dflt =
                                           CallingConvention::Definition::Ptr()) const /*final*/;

    /** Adjust inter-function edge types.
     *
     *  For any CFG edge whose source and destination are two different functions but whose type is @ref E_NORMAL, replace the
     *  edge with either a @ref E_FUNCTION_CALL or @ref E_FUNCTION_XFER edge as appropriate.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    void fixInterFunctionEdges() /*final*/;
    void fixInterFunctionEdge(const ControlFlowGraph::ConstEdgeIterator&) /*final*/;
    /** @} */

    /** Function no-op analysis.
     *
     *  Analyzes a function to determine whether the function is effectively no operation. Returns true if the function can be
     *  proven to be a no-op, false if the analysis can prove the function has an effect, and also false if the analysis cannot
     *  make a determination.
     *
     *  Since this analysis is based on data-flow, which is based on a control flow graph, the function must be attached to the
     *  CFG/AUM and all its basic blocks must also exist in the CFG/AUM.
     *
     *  The no-op analysis results are stored in the function object. If no-op analysis has already been run for this function
     *  then the old results are returned. The old results can be cleared on a per-function basis with
     *  <code>function->isNoop().clear()</code>.
     *
     *  Thread safety: Not thread safe.
     *
     *  See also, @ref allFunctionIsNoop, which analyzes all functions at once and which therefore may be faster than invoking
     *  the analysis one function at a time. */
    bool functionIsNoop(const Function::Ptr&) const /*final*/;

    /** Analyze all functions for whether they are effectivly no-ops.
     *
     *  Invokes the @ref functionIsNoop analysis on each function, perhaps concurrently.
     *
     *  Thread safety: Not thread safe. */
    void allFunctionIsNoop() const /*final*/;

    /** Clears cached function no-op analysis results.
     *
     *  Clears the function no-op analysis results for the specified function or all functions.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    void forgetFunctionIsNoop() const /*final*/;
    void forgetFunctionIsNoop(const Function::Ptr&) const /*final*/;
    /** @} */

    /** Find constants in function using data-flow.
     *
     *  This function runs a simple data-flow operation on the specified function and examines all states to obtain a set
     *  of constants. */
    std::set<rose_addr_t> functionDataFlowConstants(const Function::Ptr&) const /*final*/;


    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Callbacks
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
     * Thread safety: Not thread safe.
     *
     *  @{ */
    CfgAdjustmentCallbacks& cfgAdjustmentCallbacks() /*final*/ { return cfgAdjustmentCallbacks_; }
    const CfgAdjustmentCallbacks& cfgAdjustmentCallbacks() const /*final*/ { return cfgAdjustmentCallbacks_; }
    /** @} */

    /** Callbacks for adjusting basic block during discovery.
     *
     *  Each time an instruction is appended to a basic block these callbacks are invoked to make adjustments to the block.
     *  See @ref BasicBlockCallback and @ref discoverBasicBlock for details.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    BasicBlockCallbacks& basicBlockCallbacks() /*final*/ { return basicBlockCallbacks_; }
    const BasicBlockCallbacks& basicBlockCallbacks() const /*final*/ { return basicBlockCallbacks_; }
    /** @} */

public:
    /** Ordered list of function prologue matchers.
     *
     *  @sa nextFunctionPrologue
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    FunctionPrologueMatchers& functionPrologueMatchers() /*final*/ { return functionPrologueMatchers_; }
    const FunctionPrologueMatchers& functionPrologueMatchers() const /*final*/ { return functionPrologueMatchers_; }
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
     *  If no match is found then an empty vector is returned.
     *
     *  Thread safety: Not thread safe. */
    std::vector<Function::Ptr> nextFunctionPrologue(rose_addr_t startVa) /*final*/;

public:
    /** Ordered list of function padding matchers.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    FunctionPaddingMatchers& functionPaddingMatchers() /*final*/ { return functionPaddingMatchers_; }
    const FunctionPaddingMatchers& functionPaddingMatchers() const /*final*/ { return functionPaddingMatchers_; }
    /** @} */

    /** Finds function padding.
     *
     *  Scans backward from the specified function's entry address by invoking each function padding matcher in the order
     *  returned by @ref functionPaddingMatchers until one of them finds some padding.  Once found, a data block is created and
     *  returned.  If no padding is found then the null pointer is returned.
     *
     *  Thread safety: Not thread safe. */
    DataBlock::Ptr matchFunctionPadding(const Function::Ptr&) /*final*/;



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner miscellaneous
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
     *  cached rather than only consulting the cache.
     *
     *  Thread safety: Not thread safe. */
    void dumpCfg(std::ostream&, const std::string &prefix="", bool showBlocks=true,
                 bool computeProperties=true) const /*final*/;

    /** Output CFG as GraphViz.
     *
     *  This is a wrapper around the GraphViz class. It emits a graph with function subgraphs for the vertices that are
     *  selected by the @p restrict parameter.
     *
     *  If @p showNeighbors is false then only edges whose source and target are both selected vertices are shown, otherwise
     *  edges that go from a selected vertex to an unselected vertex are also emitted, with the target vertex having
     *  abbreviated information in the GraphViz output.
     *
     *  This is only a simple wrapper around @ref GraphViz::dumpInterval. That API has many more options than are presented by
     *  this method.
     *
     *  Thread safety: Not thread safe. */
    void cfgGraphViz(std::ostream&, const AddressInterval &restrict = AddressInterval::whole(),
                     bool showNeighbors=true) const /*final*/;

    /** Name of a vertex.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    static std::string vertexName(const ControlFlowGraph::Vertex&) /*final*/;
    std::string vertexName(const ControlFlowGraph::ConstVertexIterator&) const /*final*/;
    /** @} */

    /** Name of last instruction in vertex.
     *
     *  Thread safety: Not thread safe. */
    static std::string vertexNameEnd(const ControlFlowGraph::Vertex&) /*final*/;

    /** Name of an incoming edge.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    static std::string edgeNameSrc(const ControlFlowGraph::Edge&) /*final*/;
    std::string edgeNameSrc(const ControlFlowGraph::ConstEdgeIterator&) const /*final*/;
    /** @} */

    /** Name of an outgoing edge.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    static std::string edgeNameDst(const ControlFlowGraph::Edge&) /*final*/;
    std::string edgeNameDst(const ControlFlowGraph::ConstEdgeIterator&) const /*final*/;
    /** @} */

    /** Name of an edge.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    static std::string edgeName(const ControlFlowGraph::Edge&) /*final*/;
    std::string edgeName(const ControlFlowGraph::ConstEdgeIterator&) const /*final*/;
    /** @} */

    /** Name of a basic block.
     *
     *  Thread safety: Not thread safe. */
    static std::string basicBlockName(const BasicBlock::Ptr&) /*final*/;

    /** Name of a data block.
     *
     *  Thread safety: Not thread safe. */
    static std::string dataBlockName(const DataBlock::Ptr&) /*final*/;

    /** Name of a function.
     *
     *  Thread safety: Not thread safe. */
    static std::string functionName(const Function::Ptr&) /*final*/;

    /** Expands indeterminate function calls.
     *
     *  Modifies the control flow graph so that any function call to the indeterminate vertex is replaced by function calls to
     *  every possible function. */
    void expandIndeterminateCalls();

    /** Property: How to report progress.
     *
     *  Partitioning progress is reported in two ways:
     *
     *  @li Various diagnostic facilities use the @c MARCH stream to emit a progress report to the terminal. These streams can
     *      be enabled and disabled from the command-line or with function calls using the @ref Sawyer::Message API.
     *
     *  @li The partitioner also has a @ref progress property that can be queried in thread-safe manners that allows one
     *      thread to run partitioner algorithms and other threads to query the progress.
     *
     *  If a non-null progress object is specified, then the partitioner will make progress reports to that object as well as
     *  emitting a progress bar to the Partitioner2 diagnostic stream. The progress bar can be disabled independently of
     *  reporting to a progress object, but no progress is reported if the progress object is null.
     *
     *  Thread safety: Thread safe.
     *
     *  @{ */
    Progress::Ptr progress() const /*final*/;
    void progress(const Progress::Ptr&) /*final*/;
    /** @} */

    /** Update partitioner with a new progress report.
     *
     *  This method is const because it doesn't change the partitioner, it only forwards the phase and completion to whatever
     *  @ref Progress object is associated with the partition, if any. */
    void updateProgress(const std::string &phase, double completion) const;

    /** Print some partitioner performance statistics. */
    void showStatistics() const;

    // Checks consistency of internal data structures when debugging is enable (when NDEBUG is not defined).
    void checkConsistency() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Partitioner settings.
     *
     *  These are settings that are typically controlled from the command-line.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    const BasePartitionerSettings& settings() const /*final*/ { return settings_; }
    void settings(const BasePartitionerSettings &s) /*final*/ { settings_ = s; }
    /** @} */

    /** Use or not use symbolic semantics.
     *
     *  When true, a symbolic semantics domain will be used to reason about certain code properties such as successors for a
     *  basic block.  When false, more naive but faster methods are used.
     *
     *  Thread safety: Not thread safe.
     *
     *  @{ */
    void enableSymbolicSemantics(bool b=true) /*final*/ { settings_.usingSemantics = b; }
    void disableSymbolicSemantics() /*final*/ { settings_.usingSemantics = false; }
    bool usingSymbolicSemantics() const /*final*/ { return settings_.usingSemantics; }
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
     *  Thread safety: Not thread safe.
     *
     * @{ */
    void autoAddCallReturnEdges(bool b) /*final*/ { autoAddCallReturnEdges_ = b; }
    bool autoAddCallReturnEdges() const /*final*/ { return autoAddCallReturnEdges_; }
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
     *  Thread safety: Not thread safe.
     *
     * @{ */
    void assumeFunctionsReturn(bool b) /*final*/ { assumeFunctionsReturn_ = b; }
    bool assumeFunctionsReturn() const /*final*/ { return assumeFunctionsReturn_; }
    /** @} */

    /** Property: Name for address.
     *
     *  The partitioner stores a mapping from addresses to user specified names and uses those names when no other names are
     *  specified.  For instance, if a function that has no name is attached to the CFG/AUM  and a name has been specified for
     *  its entry address, then the function is given that name.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    void addressName(rose_addr_t, const std::string&) /*final*/;
    const std::string& addressName(rose_addr_t va) const /*final*/ { return addressNames_.getOrDefault(va); }
    const AddressNameMap& addressNames() const /*final*/ { return addressNames_; }
    /** @} */

    /** Property: Source locations.
     *
     *  The partitioner stores a mapping from source locations to virtual addresses and vice versa.
     *
     * @{ */
    const SourceLocations& sourceLocations() const /*final*/ { return sourceLocations_; }
    SourceLocations& sourceLocations() /*final*/ { return sourceLocations_; }
    void sourceLocations(const SourceLocations &locs) { sourceLocations_ = locs; }
    /** @} */

    /** Property: Whether to look for function calls used as branches.
     *
     *  If this property is set, then function call instructions are not automatically assumed to be actual function calls.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    bool checkingCallBranch() const /*final*/ { return settings_.checkingCallBranch; }
    void checkingCallBranch(bool b) /*final*/ { settings_.checkingCallBranch = b; }
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Instruction semantics
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Whether to use map- or list-based memory states.
     *
     *  The partitioner can use either list-based memory states or map-based memory states. The list-based states are more
     *  precise, but the map-based states are faster.  This property determines which kind of state is created by the @ref
     *  newOperators method.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    SemanticMemoryParadigm semanticMemoryParadigm() const { return semanticMemoryParadigm_; }
    void semanticMemoryParadigm(SemanticMemoryParadigm p) { semanticMemoryParadigm_ = p; }
    /** @} */

    /** SMT solver.
     *
     *  Returns the SMT solver being used for instruction semantics. The partitioner owns the solver, so the caller should not
     *  delete it.  Some configurations will not use a solver, in which case the null pointer is returned.
     *
     *  Thread safety: Not thread safe. */
    SmtSolverPtr smtSolver() const /*final*/ { return solver_; }

    /** Obtain new RiscOperators.
     *
     *  Creates a new instruction semantics infrastructure with a fresh machine state.  The partitioner supports two kinds of
     *  memory state representations: list-based and map-based (see @ref semanticMemoryParadigm). If the memory paradigm is not
     *  specified then the partitioner's default paradigm is used. Returns a null pointer if the architecture does not support
     *  semantics.
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    BaseSemantics::RiscOperatorsPtr newOperators() const /*final*/;
    BaseSemantics::RiscOperatorsPtr newOperators(SemanticMemoryParadigm) const /*final*/;
    /** @} */

    /** Obtain a new instruction semantics dispatcher.
     *
     *  Creates and returns a new dispatcher for the instruction semantics framework.  The dispatcher will contain a copy of
     *  the RiscOperators argument initialized with a new memory/register state.  Returns a null pointer if instruction
     *  semantics are not supported for the specimen's architecture.
     *
     *  Thread safety: Not thread safe. */
    BaseSemantics::DispatcherPtr newDispatcher(const BaseSemantics::RiscOperatorsPtr&) const /*final*/;



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Python API support functions
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_ENABLE_PYTHON_API
    void pythonUnparse() const;
#endif


    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                  Partitioner internal utilities
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void init(Disassembler*, const MemoryMap::Ptr&);
    void init(const Partitioner&);
    void updateCfgProgress();

private:
    // Convert a CFG vertex iterator from one partitioner to another.  This is called during copy construction when the source
    // and destination CFGs are identical.
    ControlFlowGraph::VertexIterator convertFrom(const Partitioner &other,
                                                 ControlFlowGraph::ConstVertexIterator otherIter);
    
    // Adjusts edges for a placeholder vertex. This method erases all outgoing edges for the specified placeholder vertex and
    // then inserts a single edge from the placeholder to the special "undiscovered" vertex. */
    ControlFlowGraph::EdgeIterator adjustPlaceholderEdges(const ControlFlowGraph::VertexIterator &placeholder);

    // Adjusts edges for a non-existing basic block.  This method erases all outgoing edges for the specified vertex and
    // then inserts a single edge from the vertex to the special "non-existing" vertex. */
    ControlFlowGraph::EdgeIterator adjustNonexistingEdges(const ControlFlowGraph::VertexIterator &vertex);

    // Implementation for the discoverBasicBlock methods.  The startVa must not be the address of an existing placeholder.
    BasicBlock::Ptr discoverBasicBlockInternal(rose_addr_t startVa) const;

    // This method is called whenever a new placeholder is inserted into the CFG or a new basic block is attached to the
    // CFG/AUM. The call happens immediately after the CFG/AUM are updated.
    void bblockAttached(const ControlFlowGraph::VertexIterator &newVertex);

    // This method is called whenever a basic block is detached from the CFG/AUM or when a placeholder is erased from the CFG.
    // The call happens immediately after the CFG/AUM are updated.
    void bblockDetached(rose_addr_t startVa, const BasicBlock::Ptr &removedBlock);

    // Rebuild the vertexIndex_ and other cache-like data members from the control flow graph
    void rebuildVertexIndices();
};

} // namespace
} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Partitioner2::Partitioner, 1);

#endif
#endif
