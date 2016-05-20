#ifndef ROSE_DISASSEMBLER_PARTITIONER_H
#define ROSE_DISASSEMBLER_PARTITIONER_H

#include "callbacks.h"
#include "Disassembler.h"
#include <Sawyer/Optional.h>

namespace rose {
namespace BinaryAnalysis {


#ifndef NAN
#define INFINITY (DBL_MAX+DBL_MAX)
#define NAN (INFINITY-INFINITY)
#endif

/** Partitions instructions into basic blocks and functions.
 *
 *  The Partitioner classes are responsible for assigning instructions to basic blocks, and basic blocks to functions.  A
 *  "basic block" is a sequence of instructions where control flow enters at only the first instruction and exits at only the
 *  last instruction. The definition can be further restricted to include only those instructions that are contiguous and
 *  non-overlapping in virtual memory by setting the set_allow_discontiguous_blocks() property to false.  Every instruction
 *  belongs to exactly one basic block.  A "function" is a collection of basic blocks having a single entry point. Every basic
 *  block belongs to exactly one function. If ROSE cannot determine what function a block belongs to, it will be placed in a
 *  special function created for the purpose of holding such blocks.
 *
 *  A partitioner can operate in one of two modes:  it can use a list of instructions that has been previously disassembled
 *  (a.k.a. "passive mode"), or it can drive a Disassembler to obtain instructions as necessary (a.k.a. "active mode").  Each
 *  mode has its  benefits:
 *
 *  <ul>
 *    <li>Active mode disassembles only what is actually necessary. Although the disassembler is recursive and can follow
 *        the flow of control, its analyses are not as thorough or robust as the partitioner. Therfore, in order to be sure
 *        that the partitioner has all the instructions it needs in this mode, the disassembler is usually run in a very
 *        aggressive mode. In the end, much of what was disassembled is then thrown away.</li>
 *    <li>Passive mode allows the partitioner to search for instruction sequences in parts of the specimen address space that
 *        the partitioner otherwise would not have disassembled in active mode.  Thus, passive mode can detect function entry
 *        addresses by searching for function prologues such as the common x86 pair "push ebp; mov ebp, esp".</li>
 *    <li>Passive mode delegates all disassembling to some other software layer. This gives the user full control over the
 *        disassembly process before partitioning even starts.</li>
 *    <li>Passive mode can be used to force the partitioner to omit certain address ranges or instruction sequences from
 *        consideration.  For example, one could disassemble certain parts of program while skipping over instructions that
 *        were inserted maliciously in order to thwart disassembly.  On the other hand a similar effect can be had by
 *        manipulating the MemoryMap used by an active partitioner.  The case where this doesn't work is when two instruction
 *        streams overlap and we want to exclude one of them from consideration.</li>
 *  </ul>
 *
 *  The partitioner organizes instructions into basic blocks by starting at a particular instruction and then looking at its
 *  set of successor addresses. Successor addresses are edges of the eventual control-flow graph (CFG) that are calculated
 *  using instruction semantics, and are available to the end user via SgAsmBlock::get_cached_successors().  It's not always
 *  possible to statically determine the complete set of successors; in this case, get_cached_successors() will return at least
 *  one successor whose isAmbiguous() property is true. Because ROSE performs semantic analysis over all instructions of the
 *  basic block (and occassionally related blocks), it can sometimes determine that instructions that are usually considered to
 *  be conditional branches are, in fact, unconditional (e.g., a PUSH followed by a RET; or a CALL to a block that discards the
 *  return address).  ROSE includes unconditional branch instructions and their targets within the same basic block (provided
 *  no other CFG edge comes into the middle of the block).
 *
 *  The partitioner organizes basic blocks into functions in three phases, all three of which can be run by a single call
 *  to the partitioner() method. The first phase considers all disassembled instructions and other available information such as
 *  symbol tables and tries to determine which addresses are entry points for functions.  For instance, if the symbol table
 *  contains function symbols, then the address stored in the symbol table is assumed to be the entry point of a function.  ROSE
 *  has a variety of these "pre-cfg" detection methods which can be enabled/disabled at runtime with the set_search() method.
 *  ROSE also supports user-defined search methods that can be registered with add_function_detector().  The three phases are
 *  initialized and influenced by the contents of an optional configuration file specified with the load_config() method.
 *
 *  The second phase for assigning blocks to functions is via analysis of the control-flow graph.  In a nutshell, ROSE
 *  traverses the CFG starting with the entry address of each function, adding blocks to the function as it goes. When it
 *  detects that a block has edges coming in from two different functions, it creates a new function whose entry point is that
 *  block (see definition of "function" above; a function can only have one entry point).
 *
 *  The third and final phase, called "post-cfg", makes final adjustments, such as adding SgAsmFunction objects for
 *  no-op or zero padding occuring between the previously detected functions. This could also be user-extended to add blocks to
 *  functions that ROSE detected during CFG analysis (such as unreferenced basic blocks, no-ops, etc. that occur within the
 *  extent of a function.)
 *
 *  By default, ROSE constructs a Partitioner to use for partitioning instructions of binary files during frontend() parsing
 *  (this happens in Disassembler::disassembleInterpretation()).  This Partitioner's settings are controlled by two
 *  command-line switches whose documentation can be seen by running any ROSE program with the --help switch.  These
 *  command-line switches operate by setting property values in the SgFile node and then transferring them to the Partitioner
 *  when the Partitioner is constructed.
 *  <ul>
 *    <li>-rose:partitioner_search initializes the detection methods by calling set_search(), and parse_switches()</li>
 *    <li>-rose:partitioner_config specifies an IPD file by calling load_config().</li>
 *  </ul>
 *
 *  The results of block and function detection are stored in the Partitioner object itself. One usually retrieves this
 *  information via a call to the build_ast() method, which constructs a ROSE AST.  Any instructions that were not assigned to
 *  blocks of a function can be optionally discarded (see the SgAsmFunction::FUNC_LEFTOVERS bit of set_search(), or
 *  the "leftovers" parameter of the "-rose:partitioner_search" command-line switch).
 *
 *  The Partitioner class can easily be subclassed by the user. Some of the Disassembler methods automatically call
 *  Partitioner::partition(), using either a temporarily instantiated Partitioner, or a partitioner provided by the user.
 *
 *  NOTE: Some of the methods used by the partitioner are more complex than one might originally imagine.  The CFG analysis,
 *  for instance, must contend with the fact that the graph nodes (basic blocks) are changing as new edges are discovered
 *  (e.g., splitting a large block when we discover an edge coming into the middle). Changes to the nodes result in
 *  changes to the edges (e.g., a PUSH/RET pair is an unconditional branch to a known target, but if the block were to be
 *  divided then the RET becomes a branch to an unknown address (i.e., the edge disappears).
 *
 *  Another complexity is that the CFG analysis must avoid circular logic. Consider the following instructions:
 *  \code
 *     1: PUSH 2
 *     2: NOP
 *     3: RET
 *  \endcode
 *
 *  When all three instructions are in the same basic block, as they are initially, the RET is an unconditional branch to the
 *  NOP. This splits the block and the RET no longer has known successors (according to block semantic analysis).  So the edge
 *  from the RET to the NOP disappears and the three instructions would coalesce back into a single basic block.  In this
 *  situation, ROSE keeps these three instructions as two basic blocks with no CFG edges to the second block.
 *
 *  A third complexity is that the Partitioner cannot rely on the usual ROSE AST traversal mechanisms because it must perform
 *  its work before the AST is created.  However, the Partitioner benefits from this situation by being able to use data
 *  structures and methods that are optimized for performance.
 *
 *  A final complexity, is that the Disassembler and Partitioner classes are both designed to be useful in a general way, and
 *  independent of each other.  These two classes can be called even when the user doesn't have an AST.  For instance, the
 *  tests/roseTests/binaryTests/disassembleBuffer.C is an example of how the Disassembler and Partitioner classes can be used
 *  to disassemble and partition a buffer of instructions obtained outside of ROSE's binary file parsing mechanisms.
 */
class Partitioner {
    /*************************************************************************************************************************
     *                                        Public Exceptions
     *************************************************************************************************************************/
public:
    struct Exception {
        std::string mesg;
        Exception(const std::string &mesg): mesg(mesg) {}
        void print(std::ostream &o) const { o <<mesg; }
        friend std::ostream& operator<<(std::ostream &o, const Exception &e);
    };

    /*************************************************************************************************************************
     *                                        Data Structures Useful to Subclasses
     *************************************************************************************************************************/
protected:

    struct Function;
    struct DataBlock;
    struct BasicBlock;

    /** Holds an instruction along with some other information about the instruction.  This is mostly an optimization.
     *  Previous versions of the partitioner kept the additional information in a separate std::map keyed by instruction
     *  address and this proved to be a major expense, partitularly within the find_bb_containing() hot path. */
    class Instruction {
    public:
        Instruction(SgAsmInstruction *node): node(node), bblock(NULL) { assert(node!=NULL); }
        SgAsmInstruction *node;                 /**< The underlying instruction node for an AST. */
        BasicBlock *bblock;                     /**< Block to which this instruction belongs, if any. */

        /* These methods are forwarded to the underlying instruction node for convenience. */
        Disassembler::AddressSet getSuccessors(bool *complete) const {
            return node->getSuccessors(complete);
        }
        rose_addr_t get_address() const { return node->get_address(); }
        size_t get_size() const { return node->get_size(); }
        bool terminates_basic_block() const { return node->terminatesBasicBlock(); }
        SgUnsignedCharList get_raw_bytes() const { return node->get_raw_bytes(); } // FIXME: should return const ref?
    };

    typedef std::map<rose_addr_t, Instruction*> InstructionMap;
    typedef std::vector<Instruction*> InstructionVector;

    /** Augments dynamic casts defined from ROSETTA.  A Partitioner::Instruction used to be just a SgAsmInstruction before we
     *  needed to combine it with some additional info for the partitioner.  Therefore, there's quite a bit of code (within the
     *  partitioner) that treats them as AST nodes.  Rather than replace every occurrance of isSgAsmInstruction(N) with
     *  something like (N?isSgAsmInstruction(N->node):NULL), we add additional versions of the necessary global functions, but
     *  define them only within the partitioner.
     *
     *  @{ */
    static SgAsmInstruction *isSgAsmInstruction(const Instruction *);
    static SgAsmInstruction *isSgAsmInstruction(SgNode*);
    static SgAsmX86Instruction *isSgAsmX86Instruction(const Instruction*);
    static SgAsmX86Instruction *isSgAsmX86Instruction(SgNode*);
    static SgAsmM68kInstruction *isSgAsmM68kInstruction(const Instruction*);
    static SgAsmM68kInstruction *isSgAsmM68kInstruction(SgNode*);
    /** @} */

    /** Analysis that can be cached in a block. Some analyses are expensive enough that they should be cached in a block.
     *  Analyses are either locally computed (by examining only the block where they're cached) or non-locally computed (by
     *  examining other related basic blocks.  Non-local analyses are not cached locally, but they might be cached in other
     *  blocks. */
    class BlockAnalysisCache {
    public:
        BlockAnalysisCache(): age(0), sucs_complete(false), is_function_call(false), call_target(NO_TARGET),
                              function_return(false), alias_for(0) {}
        void clear() {
            age = 0;
            sucs.clear();
            sucs_complete = false;
            is_function_call = false;
            call_target = NO_TARGET;
            function_return = false;
            alias_for = 0;
        }

        size_t age;                             /**< Non zero implies locally computed successors are cached. The actual value
                                                 *   of this data member is the number of instructions in the basic block when
                                                 *   the successor information was computed, so if this value matches the current
                                                 *   number of instructions, the cached info is up to date. Analyses that are
                                                 *   computed over other blocks (i.e., non-local analyses) are never cached. */
        Disassembler::AddressSet sucs;          /**< Locally computed cached successors. */
        bool sucs_complete;                     /**< True if locally computed successors are fully known. */
        bool is_function_call;                  /**< True if this block ends with a CALL-like instruction. */
        rose_addr_t call_target;                /**< Target of a CALL instruction if this block ends with what appears to be a
                                                 *   function call (whether, in fact, it truly is a function call is immaterial
                                                 *   since the final determination requires non-local analysis. If this block
                                                 *   does not end with a call or if the target of the call cannot be statically
                                                 *   determined, then the value is set to Partitioner::NO_TARGET. */
        bool function_return;                   /**< Does this block serve as the return of a function?  For example, does it
                                                 *   end with an x86 RET instruction that returns to the CALL fall-through
                                                 *   address? */
        rose_addr_t alias_for;                  /**< If non-zero, then this block is an alias for the specified block.  CFG
                                                 *   edges that pointed to this block should instead point to the specified
                                                 *   block. */
    };

    /** Represents a basic block within the Partitioner. Each basic block will eventually become an SgAsmBlock node in the
     *  AST. However, if the SgAsmFunction::FUNC_LEFTOVER bit is set in the Partitioner::set_search() method then
     *  blocks that were not assigned to any function to not result in an SgAsmBlock node.
     *
     *  The first instruction of a basic block should never change.  In particular, the address of the first instruction should
     *  not change because this address is used as a key to link a BasicBlock object to a Function object. */
    struct BasicBlock {
        /** Constructor. This constructor should not be called directly since the Partitioner has other pointers that it needs
         *  to establish to this block.  Instead, call Partitioner::find_bb_containing(). */
        BasicBlock(): reason(SgAsmBlock::BLK_NONE), function(NULL), code_likelihood(1.0) {}

        /** Destructor. This destructor should not be called directly since there are other pointers to this block that the
         *  block does not know about. Instead, call Partitioner::discard(). */
        ~BasicBlock() {}

        /** Returns true if the block analysis cache is up to date. */
        bool valid_cache() const { return cache.age==insns.size(); }

        /** Marks the block analysis cache as being outdated. */
        void invalidate_cache() { cache.age=0; }

        /** Marks the block analysis cache as being up to date. */
        void validate_cache() { cache.age=insns.size(); }

        /** Remove all data blocks from this basic block.  The data blocks continue to exist, they're just no longer associated
         *  with this basic block. */
        void clear_data_blocks();

        Instruction* last_insn() const;         /**< Returns the last executed (exit) instruction of the block */
        rose_addr_t address() const;            /* Return the address of the basic block's first (entry) instruction. */
        unsigned reason;                        /**< Reasons this block was created; SgAsmBlock::Reason bit flags */
        std::vector<Instruction*> insns;        /**< Non-empty set of instructions composing this basic block, in address order */
        std::set<DataBlock*> data_blocks;       /**< Data blocks owned by this basic block. E.g., this block's jump table. */
        BlockAnalysisCache cache;               /**< Cached results of local analyses */
        Function* function;                     /**< Function to which this basic block is assigned, or null */
        double code_likelihood;                 /**< Likelihood (0..1) that this is code. One unless detected statistically. */
    };
    typedef std::map<rose_addr_t, BasicBlock*> BasicBlocks;

    /** Represents a region of static data within the address space being disassembled.  Each data block will eventually become
     *  an SgAsmBlock node in the AST if it is assigned to a function.
     *
     *  A DataBlock can be associated with a function, a basic block, both, or neither.  When associated with both, the
     *  function takes precedence (if the function association is broken, the basic block association remains).  When
     *  associated with a basic block (and not a function), the data block will appear to move from function to function as its
     *  basic block moves.  When associated with neither, the basic block may ultimately be added to a "leftovers" function or
     *  discarded.  See the append() method for associating a data block with a function or basic block, and the remove()
     *  method for breaking the association.
     *
     *  The address of the first SgAsmStaticData node of a DataBlock should remain constant for the life of the DataBlock.
     *  This is because we use that address to establish a two-way link between the DataBlock and a Function object. */
    struct DataBlock {
        /** Constructor. This constructor should not be called directly since the Partitioner has other pointers that it needs
         * to establish to this block.  Instead, call one of the Partitioner's data block creation functions, such as
         * Partitioner::find_db_starting(). */
        DataBlock(): reason(SgAsmBlock::BLK_NONE), function(NULL), basic_block(NULL) {}

        /** Destructor.  This destructor should not be called directly since there are other pointers in the Partitioner that
         * this block does not know about.  Instead, call Partitioner::discard(). */
        ~DataBlock() {}

        rose_addr_t address() const;            /* Return the address of the first node of a data block. */
        std::vector<SgAsmStaticData*> nodes;    /**< The static data nodes belonging to this block; not deleted. */
        unsigned reason;                        /**< Reasons this block was created; SgAsmBlock::Reason bit flags */
        Function *function;                     /**< Function to which this data block is explicitly assigned, or null */
        BasicBlock *basic_block;                /**< Basic block to which this data block is bound, or null. */
    };
    typedef std::map<rose_addr_t, DataBlock*> DataBlocks;

    /** Represents a function within the Partitioner. Each non-empty function will become an SgAsmFunction in the AST. */
    class Function {
    public:
        Function(rose_addr_t entry_va)
            : reason(0), pending(true), entry_va(entry_va),
              may_return_cur(SgAsmFunction::RET_UNKNOWN), may_return_old(SgAsmFunction::RET_UNKNOWN) {}
        Function(rose_addr_t entry_va, unsigned r)
            : reason(r), pending(true), entry_va(entry_va),
              may_return_cur(SgAsmFunction::RET_UNKNOWN), may_return_old(SgAsmFunction::RET_UNKNOWN) {}
        Function(rose_addr_t entry_va, unsigned r, const std::string& name)
            : reason(r), name(name), pending(true), entry_va(entry_va),
              may_return_cur(SgAsmFunction::RET_UNKNOWN), may_return_old(SgAsmFunction::RET_UNKNOWN) {}

        /** Remove all basic blocks from this function w/out deleting the blocks. */
        void clear_basic_blocks();

        /** Remove all data blocks from this function w/out deleting the blocks. */
        void clear_data_blocks();

        /** Move all basic blocks from the other function to this one. */
        void move_basic_blocks_from(Function *other);

        /** Move all data blocks from the other function to this one. */
        void move_data_blocks_from(Function *other);

        /** Accessor for the may-return property.  The may-return property indicates whether this function returns to its
         *  caller.  This is a two-part property storing both the current value and the previous value; this enables us to
         *  detect transitions after the fact, when it is more efficient to process them than when they actually occur.
         *  Setting the current value does not update the old value; the old value is updated only by the commit_may_return()
         *  method.
         * @{ */
        SgAsmFunction::MayReturn get_may_return() const { return may_return_cur; }
        void set_may_return(SgAsmFunction::MayReturn may_return) { may_return_cur = may_return; }
        bool changed_may_return() const { return may_return_cur != may_return_old; }
        void commit_may_return() { may_return_old = may_return_cur; }
        /** @} */

        /** Can this function return?  Returns true if it is known that this function can return to its caller. */
        bool possible_may_return() const {
            return SgAsmFunction::RET_SOMETIMES==get_may_return() || SgAsmFunction::RET_ALWAYS==get_may_return();
        }

        /** Increase knowledge about the returnability of this function.  A current value of RET_UNKNOWN is always changed to
         * the @p new_value.  A current value of RET_SOMETIMES can be changed to RET_ALWAYS or RET_NEVER.  A current value of
         * RET_ALWAYS or RET_NEVER is not modified (not even to change RET_ALWAYS to RET_NEVER or vice versa). */
        void promote_may_return(SgAsmFunction::MayReturn new_value);

        /** Initialize properties from another function.  This causes the properties of the @p other function to be copied into
         *  this function without changing this function's list of blocks or entry address.  The @p pending status of this
         *  function may be set if set in @p other, but it will never be cleared.  Returns @p this. */
        Function *init_properties(const Function &other);

        /** Emit function property values. This is mostly for debugging. */
        void show_properties(std::ostream&) const;

        /** Return the pointer to the basic block at the entry address. Returns null if there is no basic block assigned
         *  to this function at that address. */
        BasicBlock *entry_basic_block() const;

    public:
        /* If you add more data members, also update detach_thunk() and/or init_properties() */
        unsigned reason;                        /**< SgAsmFunction::FunctionReason bit flags */
        std::string name;                       /**< Name of function if known */
        BasicBlocks basic_blocks;               /**< Basic blocks belonging to this function */
        DataBlocks data_blocks;                 /**< Data blocks belonging to this function */
        bool pending;                           /**< True if we need to (re)discover the basic blocks */
        rose_addr_t entry_va;                   /**< Entry virtual address */
        Disassembler::AddressSet heads;         /**< CFG heads, excluding func entry: addresses of additional blocks */

    private:
        /* If you add more data members, also update detach_thunk() and/or init_properties() */
        SgAsmFunction::MayReturn may_return_cur; /**< Is it possible for this function to return? Current value of property. */
        SgAsmFunction::MayReturn may_return_old; /**< Is it possible for this function to return? Previous value of property. */
    };
    typedef std::map<rose_addr_t, Function*> Functions;

    /** Data type for user-defined function detectors. */
    typedef void (*FunctionDetector)(Partitioner*, SgAsmGenericHeader*);

    /** Basic block configuration information. This is information which is set by loading an IPD configuration file. */
    struct BlockConfig {
        BlockConfig(): ninsns(0), alias_for(0), sucs_specified(false), sucs_complete(false) {}
        size_t ninsns;                          /**< Number of instructions expected in the basic block. */
        rose_addr_t alias_for;                  /**< If non-zero then this block is an alias for another block. */
        bool sucs_specified;                    /**< True if IPD file specifies successors for this block. */
        Disassembler::AddressSet sucs;          /**< Address to which this block might branch or fall through. */
        bool sucs_complete;                     /**< True if successors are fully known. */
        SgUnsignedCharList sucs_program;        /**< i386 code to simulate to find successors. */
    };
    typedef std::map<rose_addr_t, BlockConfig*> BlockConfigMap;

    /*************************************************************************************************************************
     *                                                     Deprecated
     *************************************************************************************************************************/
public:

    /* FIXME: Backward compatibility stuff prior to 2010-01-01. These are deprecated and should eventually be removed. They
     *        are currently used by src/midend/binaryAnalyses/binary_analysis.C for some of the CFG and call graph functions. */

    /** Map of basic block starting addresses. The key is the virtual address of the first instruction in the
     *  basic block; the value is the set of all virtual addresses of instructions known to branch to this basic block (i.e.,
     *  set of all known callers).
     *
     *  \deprecated This data type is used only for backward compatibility by detectBasicBlocks() and detectFunctions(). It
     *  has been replaced by Partitioner::BasicBlocks. */
    typedef std::map<rose_addr_t, Disassembler::AddressSet> BasicBlockStarts;

    /** Find the beginnings of basic blocks based on instruction type and call targets.
     *
     *  \deprecated This function is deprecated.  Basic blocks are now represented by Partitioner::BasicBlock. */
  BasicBlockStarts detectBasicBlocks(const Disassembler::InstructionMap&) const ROSE_DEPRECATED("");

    /** Information about each function starting address.
     *
     *  \deprecated This data type is used only for backward compatibility by detectFunctions(). It has been replaced by
     *  Partitioner::Function, which is capable of describing noncontiguous functions. */
    struct FunctionStart {
        FunctionStart(unsigned reason, std::string name): reason(reason), name(name) {}
        unsigned reason;                        /** SgAsmFunction::FunctionReason bit flags */
        std::string name;                       /** Name of function if known. */
    };

    /** Map describing the starting address of each known function.
     *
     *  \deprecated This type has been replaced with Partitioner::Functions, which is capable of describing
     *  noncontiguous functions. */
    typedef std::map<rose_addr_t, FunctionStart> FunctionStarts;

    /** Returns a list of the currently defined functions.
     *
     *  \deprecated This function has been replaced by pre_cfg(), analyze_cfg(), and post_cfg() */
    FunctionStarts detectFunctions(SgAsmInterpretation*, const Disassembler::InstructionMap &insns,
                                   BasicBlockStarts &bb_starts/*out*/) const ROSE_DEPRECATED("replaced by pre_cfg");


    /*************************************************************************************************************************
     *                                                 Constructors, etc.
     *************************************************************************************************************************/
public:

    Partitioner()
        : aggregate_mean(NULL), aggregate_variance(NULL), code_criteria(NULL), disassembler(NULL), map(NULL),
          func_heuristics(SgAsmFunction::FUNC_DEFAULT), allow_discont_blocks(true)
        {}
    virtual ~Partitioner() { clear(); }
    static void initDiagnostics();

    /*************************************************************************************************************************
     *                                              Accessors for Properties
     *************************************************************************************************************************/
public:

    /** Sets the set of heuristics used by the partitioner.  The @p heuristics should be a bit mask containing the
     *  SgAsmFunction::FunctionReason bits. These same bits are assigned to the "reason" property of the resulting
     *  function nodes in the AST, depending on which heuristic detected the function. */
    virtual void set_search(unsigned heuristics) {
        func_heuristics = heuristics;
    }

    /** Returns a bit mask of SgAsmFunction::FunctionReason bits indicating which heuristics would be used by the
     *  partitioner.  */
    virtual unsigned get_search() const {
        return func_heuristics;
    }

    /** Turns on/off the allowing of discontiguous basic blocks.  When set, a basic block may contain instructions that are
     *  discontiguous in memory. Such blocks are created when find_bb_containing() encounters an unconditional jump whose only
     *  successor is known and the successor would not be part of any other block.
     *
     *  Here's an example of a discontiguous basic block.
     *
     *  \code
     *    0x00473bf0: 83 c0 18          |...   |   add    eax, 0x18
     *    0x00473bf3: 68 e0 84 44 00    |h..D. |   push   0x004484e0
     *    0x00473bf8: e9 db 72 fc ff    |..r.. |   jmp    0x0043aed8
     *    0x0043aed8: c3                |.     |   ret
     *    0x004484e0: 89 45 f0          |.E.   |   mov    DWORD PTR ss:[ebp + 0xf0(-0x10)], eax
     *    0x004484e3: 8b 45 f0          |.E.   |   mov    eax, DWORD PTR ss:[ebp + 0xf0(-0x10)]
     *    0x004484e6: 8b 40 60          |.@`   |   mov    eax, DWORD PTR ds:[eax + 0x60]
     *    0x004484e9: 03 45 fc          |.E.   |   add    eax, DWORD PTR ss:[ebp + 0xfc(-0x04)]
     *    0x004484ec: 89 45 ec          |.E.   |   mov    DWORD PTR ss:[ebp + 0xec(-0x14)], eax
     *    0x004484ef: e9 0c 83 02 00    |..... |   jmp    0x00470800
     *    0x00470800: 8b 45 ec          |.E.   |   mov    eax, DWORD PTR ss:[ebp + 0xec(-0x14)]
     *    0x00470803: 8b 40 18          |.@.   |   mov    eax, DWORD PTR ds:[eax + 0x18]
     *    0x00470806: 48                |H     |   dec    eax
     *    0x00470807: 85 c0             |..    |   test   eax, eax
     *    0x00470809: 0f 8c 4f 3d 00 00 |..O=..|   jl     0x0047455e
     *                (successors: 0x0047080f 0x0047455e)
     *  \endcode
     *
     *  When this property is disabled, the above single basic block would have been four blocks, ending at the JMP at 0x473bf8,
     *  the RET at 0x43aed8, the JMP at 0x4484ef, and the JL at 0x470809.
     *
     *  The default is that blocks are allowed to be discontiguous. */
    void set_allow_discontiguous_blocks(bool b) {
        allow_discont_blocks = b;
    }

    /** Returns an indication of whether discontiguous blocks are allowed. See set_allow_discontiguous_blocks() for details. */
    bool get_allow_discontiguous_blocks() const {
        return allow_discont_blocks;
    }

    /** Accessors for the memory maps.
     *
     *  The first argument is usually the complete memory map.  It should define all memory that holds instructions, either
     *  instructions that have already been disassembled and provided to the Partitioner, or instructions that might be
     *  disassembled in the course of partitioning.  Depending on disassembler flags, the disassembler will probably only look
     *  at portions of the map that are marked executable.
     *
     *  The second (optional) map is used to initialize memory in the virtual machine semantics layer and should contain all
     *  read-only memory addresses for the specimen.  This map normally also includes the parts of the first argument that hold
     *  instructions.  Things such as dynamic library addresses (i.e., import sections) can also be supplied if they are
     *  initialized and not expected to change during the life of the specimen. If a null pointer is specified (the default)
     *  then this map is created from all read-only segments of the first argument.
     *
     *  The first map will be stored by the partitioner as a pointer; the other supplied maps are copied.
     *
     *  @{ */
    void set_map(MemoryMap *mmap, MemoryMap *ro_mmap=NULL);
    MemoryMap *get_map() const {
        return map;
    }
    /** @} */

    /** Set progress reporting properties.  A progress report is produced not more than once every @p min_interval seconds
     * (default is 10).  Progress reporting can be disabled by supplying a negative value. Progress report properties are class
     * variables. */
    void set_progress_reporting(double min_interval);

    void update_progress(SgAsmBlock::Reason reason, size_t pass) const;
    void update_progress() const;

    /*************************************************************************************************************************
     *                                                High-level Functions
     *************************************************************************************************************************/
public:

    /** Adds a user-defined function detector to this partitioner. Any number of detectors can be added and they will be run
     *  by pre_cfg() in the order they were added, after the built-in methods run.  Each user-defined detector will be called
     *  first with the SgAmGenericHeader pointing to null, then once for each file header. The user-defined methods are run
     *  only if the SgAsmFunction::FUNC_USERDEF is set (see set_search()), which is the default.   The reason for
     *  having user-defined function detectors is that the detection of functions influences the shape of the AST and so it is
     *  easier to apply those analyses here, before the AST is built, rather than in the mid-end after the AST is built. */
    void add_function_detector(FunctionDetector f) {
        user_detectors.push_back(f);
    }

    /** Parses a string describing the heuristics and returns the bit vector that can be passed to set_search(). The input
     *  string should be a comma-separated list (without white space) of search specifications. Each specification should be
     *  an optional qualifier character followed by either an integer or a word. The accepted words are the lower-case
     *  versions of the constants enumerated by SgAsmFunction::FunctionReason, but without the leading "FUNC_".
     *  The qualifier determines whether the bits specified by the integer or word are added to the return value ("+") or
     *  removed from the return value ("-").  The "=" qualifier acts like "+" but first zeros the return value. The default
     *  qualifier is "+" except when the word is "default", in which case the specifier is "=". An optional initial bit mask
     *  can be specified (defaults to SgAsmFunction::FUNC_DEFAULT). */
    static unsigned parse_switches(const std::string&, unsigned initial_flags);

    /** Top-level function to run the partitioner on some instructions and build an AST. The SgAsmInterpretation is optional.
     *  If it is null then those function seeding operations that depend on having file headers are not run.  The memory map
     *  argument is optional only if a memory map has already been attached to this partitioner object with the set_map()
     *  method. */
    virtual SgAsmBlock* partition(SgAsmInterpretation*, const Disassembler::InstructionMap&, MemoryMap *mmap=NULL);

    /** Top-level function to run the partitioner, calling the specified disassembler as necessary to generate instructions. */
    virtual SgAsmBlock* partition(SgAsmInterpretation*, Disassembler*, MemoryMap*);

    /** Reset partitioner to initial conditions by discarding all instructions, basic blocks, functions, and configuration
     *  file settings and definitions. */
    virtual void clear();

    /** Loads the specified configuration file. This should be called before any of the partitioning functions (such as
     *  partition()).  If an error occurs then Partitioner::IPDParser::Exception error is thrown. */
    virtual void load_config(const std::string &filename);

    /** Adds additional instructions to be processed. New instructions are only added at addresses that don't already have an
     *  instruction. */
    virtual void add_instructions(const Disassembler::InstructionMap& insns);

    /** Get the list of all instructions.  This includes instructions that were added with add_instructions(), instructions
     *  added by a passive partition() call, and instructions added by an active partitioner. */
    Disassembler::InstructionMap get_instructions() const;

    /** Get the list of disassembler errors. Only active partitioners accumulate this information since only active
     *  partitioners call the disassembler to obtain instructions. */
    const Disassembler::BadMap& get_disassembler_errors() const {
        return bad_insns;
    }

    /** Clears errors from the disassembler.  This might be useful in order to cause the partitioner to call the disassembler
     *  again for certain addresses. Normally, if the partitioner fails to obtain an instruction at a particular address it
     *  remembers the failure and does not try again.  The bad map is also cleared by the Partitioner::clear() method, which
     *  clears various other things in addition. */
    void clear_disassembler_errors() {
        bad_insns.clear();
    }

    /** Finds an instruction at the specified address.  If the partitioner is operating in active mode and @p create is true,
     *  then the disassembler will be invoked if necessary to obtain the instruction.  This function returns the null pointer
     *  if no instruction is available.  If the disassembler was called and threw an exception, then we catch the exception
     *  and add it to the bad instruction list. */
    virtual Instruction* find_instruction(rose_addr_t, bool create=true);

    /** Drop an instruction from consideration.  If the instruction is the beginning of a basic block then drop the entire
     *  basic block, returning its subsequent instructions back to the (implied) list of free instructions.  If the instruction
     *  is in the middle of a basic block, then either drop the entire basic block, or truncate it at the specified
     *  instruction depending on whether discard_entire_block is true or false.
     *
     *  This method always returns the null pointer. */
    virtual Instruction* discard(Instruction*, bool discard_entire_block=false);

    /** Drop a basic block from the partitioner.  The specified basic block, which must not belong to any function, is removed
     *  from the Partitioner, deleted, and its instructions all returned to the (implied) list of free instructions. This
     *  function always returns the null pointer. */
    virtual BasicBlock *discard(BasicBlock*);

    /** Adds a new function definition to the partitioner.  New functions can be added at any time, including during the
     *  analyze_cfg() call.  When this method is called with an entry_va for an existing function, the specified @p reasons
     *  will be merged with the existing function, and the existing function will be given the specified name if it has none. */
    virtual Function* add_function(rose_addr_t entry_va, unsigned reasons, std::string name="");

    /** Looks up a function by address.  Returns the function pointer if found, the null pointer if not found. */
    virtual Function* find_function(rose_addr_t entry_va);

    /** Determines if address is used in a function.  An address is used if there is an instruction that is part of a basic
     *  block, which is part of a function. */
    virtual Function* find_function_containing_code(rose_addr_t va);

    /** Determines if address is part of a function's data.  An address is part of a function's data if the address is part of
     *  SgAsmStaticData node that belongs to a partitioner data block, that belongs to a function.  The pointer to the function
     *  is returned for true, null for false. */
    virtual Function* find_function_containing_data(rose_addr_t va);

    /** Determines if address is part of a function.  Returns true if the address is the beginning of an instruction that
     *  belongs to a function, or if it is any address inside data that belongs to a function. */
    virtual Function* find_function_containing(rose_addr_t va);

    /** Builds the AST describing all the functions.
     *
     *  The return value is an SgAsmBlock node that points to a list of SgAsmFunction nodes (the functions), each of which
     *  points to a list of SgAsmBlock nodes (the basic blocks). Any basic blocks that were not assigned to a function by the
     *  Partitioner will be added to a function named "***uncategorized blocks***" whose entry address will be the address of
     *  the lowest instruction, and whose reasons for existence will include the SgAsmFunction::FUNC_LEFTOVERS bit.  However,
     *  if the FUNC_LEFTOVERS bit is not turned on (see set_search()) then uncategorized blocks will not appear in the AST.
     *
     *  If an interpretation is supplied, then it will be used to obtain information about where various file sections are
     *  mapped into memory.  This mapping is used to fix-up various kinds of pointers in the instructions to make them relative
     *  to a file section.  For instance, a pointer into the ".bss" section will be made relative to the beginning of that
     *  section. */
    virtual SgAsmBlock* build_ast(SgAsmInterpretation *interp=NULL);

    /** Update control flow graph edge nodes.  This method traverses the specified AST and updates any edge nodes so their
     *  block pointers point to actual blocks rather than just containing virtual addresses.  The update only happens for
     *  edges that don't already have a node pointer. */
    virtual void fixup_cfg_edges(SgNode *ast);

    /** Updates pointers inside instructions.  This method traverses each instruction in the specified AST and looks for
     *  integer value expressions that that have no base node (i.e., those that have only an absolute value).  For each such
     *  value it finds, it tries to determine if that value points to code or data.  Code pointers are made relative to the
     *  instruction or function (for function calls) to which they point; data pointers are made relative to the data to which
     *  they point.
     *
     *  The specified interpretation is only used to obtain a list of all mapped sections.  The sections are used to determine
     *  whether a value is a data pointer even if it doesn't point to any specific data that was discovered during
     *  disassembly.
     *
     *  This method is called by build_ast(), but can also be called explicitly. Only pointers that are not already relative to
     *  some object are affected. */
    virtual void fixup_pointers(SgNode *ast, SgAsmInterpretation *interp=NULL);

    /** Called by frontend() to disassemble an entire interpretation. Disassembles the specified interpretation using
     *  configuration settings from the ROSE command-line and then links the resulting SgAsmBlock into the AST.  The
     *  disassembly is partitioner-driven.
     *
     *  See also: Disassembler::disassembleInterpratation(), which does Disassembler-driven disassembly. */
    static void disassembleInterpretation(SgAsmInterpretation*);

    /**************************************************************************************************************************
     *                                  Range maps relating address ranges to objects
     **************************************************************************************************************************/
public:
    /** Value type for FunctionRangeMap.  See base class for documentation. */
    class FunctionRangeMapValue: public RangeMapValue<Extent, Function*> {
    public:
        FunctionRangeMapValue():            RangeMapValue<Extent, Function*>(NULL) {}
        FunctionRangeMapValue(Function *f): RangeMapValue<Extent, Function*>(f)    {} // implicit

        FunctionRangeMapValue split(const Extent &my_range, const Extent::Value &new_end) {
            assert(my_range.contains(Extent(new_end)));
            return *this;
        }

        void print(std::ostream &o) const {
            if (NULL==value) {
                o <<"(null)";
            } else {
                o <<"F" <<StringUtility::addrToString(value->entry_va);
            }
        }
    };

    /** Range map associating addresses with functions. */
    typedef RangeMap<Extent, FunctionRangeMapValue> FunctionRangeMap;

    /** Value type for DataRangeMap.  See base class for documentation. */
    class DataRangeMapValue: public RangeMapValue<Extent, DataBlock*> {
    public:
        DataRangeMapValue():             RangeMapValue<Extent, DataBlock*>(NULL) {}
        DataRangeMapValue(DataBlock *d): RangeMapValue<Extent, DataBlock*>(d)    {} // implicit

        DataRangeMapValue split(const Extent &my_range, const Extent::Value &new_end) {
            assert(my_range.contains(Extent(new_end)));
            return *this;
        }

        void print(std::ostream &o) const {
            if (NULL==value) {
                o <<"(null)";
            } else {
                o <<"D" <<StringUtility::addrToString(value->address());
            }
        }
    };

    /** Range map associating addresses with functions. */
    typedef RangeMap<Extent, DataRangeMapValue> DataRangeMap;

    /**************************************************************************************************************************
     *                                  Methods for characterizing whether something is code
     **************************************************************************************************************************/
public:

    /** Statistics computed over a region of an address space.  Most of the members are floating point because they are also
     *  used to compute averages. For instance, aggregate_statistics() can compute the average number of instructions per
     *  function.
     *
     *  This is a virtual class so that users can easily augment it with additional analyses.  The Partitioner never
     *  instantiates RegionStats objects directly, but rather always by calling Partitioner::new_region_stats().  The
     *  statistics are computed by methods like Partitioner::region_statistics() and Partitioner::aggregate_statistics(), which
     *  the user can also agument or replace.
     *
     *  @code
     *  // TODO: add an example showing how to specialize this class.
     *  @endcode
     *
     *  See also, Partitioner::CodeCriteria. */
    class RegionStats {
    private:
        struct DictionaryEntry {
            DictionaryEntry(): weight(0.0) {}
            DictionaryEntry(const std::string &name, const std::string &desc, double weight)
                : name(name), desc(desc), weight(weight) {}
            std::string name;
            std::string desc;
            double weight;                                      /**< Default weight for CodeCriteria. */
        };

        struct AnalysisResult {
            AnalysisResult(): sum(0), nsamples(0) {}
            AnalysisResult(double d): sum(d), nsamples(1) {} // implicit
            double sum;
            size_t nsamples;
        };

        static std::vector<DictionaryEntry> dictionary;
        std::vector<AnalysisResult> results;

    public:
        /** IDs for predefined analyses.  Derived classes should start their numbering at the value returned by get_nanalyses()
         *  after allowing this base class to initialize itself. */
        enum AnalysisEnum {
            RA_NBYTES=0, RA_NINSNS, RA_NCOVERAGE, RA_RCOVERAGE, RA_NSTARTS, RA_NFAILS, RA_RFAILS, RA_NOVERLAPS, RA_ROVERLAPS,
            RA_NINCOMPLETE, RA_RINCOMPLETE, RA_NBRANCHES, RA_RBRANCHES, RA_NCALLS, RA_RCALLS, RA_NNONCALLS, RA_RNONCALLS,
            RA_NINTERNAL, RA_RINTERNAL, RA_NICFGEDGES, RA_RICFGEDGES, RA_NCOMPS, RA_RCOMPS, RA_NIUNIQUE, RA_RIUNIQUE,
            RA_NREGREFS, RA_RREGREFS, RA_REGSZ, RA_REGVAR, RA_NPRIV, RA_RPRIV, RA_NFLOAT, RA_RFLOAT
        };

        RegionStats() { init_class(); }
        virtual ~RegionStats() {}
        virtual RegionStats* create() const;                    /**< Return a new, allocated copy of this object. */

        /** Add a new analysis to this RegionStats container. */
        static size_t define_analysis(const std::string &name, const std::string &desc, double weight, size_t id=(size_t)(-1));
        static size_t find_analysis(const std::string &name);   /**< Return the ID for an analysis based on name. */
        static size_t get_nanalyses();                          /**< Number of anlyses defined by this class and super classes. */
        static const std::string& get_name(size_t id);          /**< Returns name of analysis. */
        static const std::string& get_desc(size_t id);          /**< Returns one-line description of analysis. */
        static double get_weight(size_t id);                    /**< Returns the default weight in the analysis definition. */

        virtual void add_sample(size_t id, double val, size_t nsamples=1);  /**< Add another sample point to this container. */
        virtual size_t get_nsamples(size_t id) const;           /**< Returns the number of samples accumulated for an analysis. */
        virtual double get_sum(size_t id) const;                /**< Returns the sum stored for the analysis. */
        virtual double get_value(size_t id) const;              /**< Returns the value (sum/nsamples) of an analysis. */
        virtual void compute_ratios();                          /**< Called to compute ratios, etc. from other stats. */
        virtual void set_value(size_t id, double val);          /**< Set value to indicated single sample unless value is NaN. */

        double divnan(size_t num_id, size_t den_id) const;      /**< Safely computes the ratio of two values. */
        void add_samples(const RegionStats*);                   /**< Add samples from other statistics object to this one. */
        void square_diff(const RegionStats*);                   /**< Compute square of differences. */

        virtual void print(std::ostream&) const;
        friend std::ostream& operator<<(std::ostream&, const RegionStats&);
    protected:
        static void init_class();

    };

    /** Criteria to decide whether a region of memory contains code.
     *
     *  Ultimately, one often needs to answer the question of whether an arbitrary region of memory contains code or data.  A
     *  CodeCriteria object can be used to help answer that question.  Such an object contains criteria for multiple analyses.
     *  The criteria can be initialized by hand, or by running the analyses over parts of the program that we already know to
     *  be code (see Partitioner::aggregate_statistics()). In the latter case, the criteria are automatically fine tuned based
     *  on characteristics of the specimen executable itself.
     *
     *  Each criterion is assumed to have a Gaussian distribution (this class can be specialized if something else is needed)
     *  and therefore stores a mean and variance.  Each criterion also stores a weight relative to the other criteria.
     *
     *  To determine the probability that a sample contains code, the analyses, \f$A_i\f$, are run over the sample to produce a
     *  set of analysis results \f$R_i\f$.  Each analysis result is compared against the corresponding probability
     *  density function \f$f_i(x)\f$ to obtain the likelihood (in the range zero to one) that the sample is code.  The
     *  probability density function is characterized by the criterion mean, \f$\mu_i\f$, and variance \f$\sigma_i^2\f$. The
     *  Guassian probability distribution function is:
     *
     *  \f[ f_i(x) = \frac{1}{\sqrt{2 \pi \sigma^2}} e^{-\frac{(x - \mu)^2}{2\sigma^2}} \f]
     *
     *
     *  The likelihood, \f$C_i(R_i)\f$ that \f$R_i\f$ is representative of valid code is computed as the area under the
     *  probability density curve further from the mean value than \f$R_i\f$.  In other words:
     *
     *  \f[ C_i(x) = 2 \int_{-\inf}^{\mu_i-|\mu_i-x|} f_i(x) dx = 1 - {\rm erf}(-\frac{|x-\mu_i|}{\sqrt{2\sigma_i^2}}) \f]
     *
     *  A criterion that has an undefined \f$R_i\f$ value does not contribute to the final vote. Similarly, criteria that have
     *  zero variance contribute a vote of zero or one:
     *
     *  \f[
     *      C_i(x) = \left\{
     *          \begin{array}{cl}
     *              1 & \quad \mbox{if } x = \mu_i \\
     *              0 & \quad \mbox{if } x \ne \mu_i
     *          \end{array}
     *      \right.
     *   \f]
     *
     *  The individual probabilities from each analysis are weighted relative to one another to obtain a final probability,
     *  which is then compared against a threshold.  If the probability is equal to or greater than the threshold, then the
     *  sample is considered to be code.
     *
     *  The Partitioner never instantiates a CodeCriteria object directly, but rather always uses the new_code_criteria()
     *  virtual method.  This allows the user to easily augment this class to do something more interesting.
     *
     *  Here's an example of using this class to determine if some uncategorized region of memory contains code.  First we
     *  compute aggregate statistics across all the known functions.  Then we use the mean and variance in those statistics to
     *  create a code criteria specification.  Then we run the same analyses over the uncategorized region of memory and ask
     *  whether the results satisfy the criteria.  This example is essentially the implementation of Partitioner::is_code().
     *
     *  @code
     *  partitioner->aggregate_statistics(); // compute stats if not already cached
     *  Partitioner::RegionStats *mean = partitioner->get_aggregate_mean();
     *  Partitioner::RegionStats *variance = partitioner->get_aggregate_variance();
     *  Partitioner::CodeCriteria *cc = partitioner->new_code_criteria(mean, variance);
     *
     *  ExtentMap uncategorized_region = ....;
     *  Partitioner::RegionStats *stats = region_statistics(uncategorized_region);
     *  if (cc->satisfied_by(stats))
     *      std::cout <<"this looks like code" <<std::endl;
     *  delete stats;
     *  delete cc;
     *  @endcode
     */
    class CodeCriteria {
    private:
        struct DictionaryEntry {
            DictionaryEntry() {}
            DictionaryEntry(const std::string &name, const std::string &desc): name(name), desc(desc) {}
            std::string name;
            std::string desc;
        };

        struct Criterion {
            Criterion(): mean(0.0), variance(0.0), weight(0.0) {}
            double mean;
            double variance;
            double weight;
        };

        static std::vector<DictionaryEntry> dictionary;
        std::vector<Criterion> criteria;
        double threshold;

    public:
        CodeCriteria(): threshold(0.5) { init_class(); }
        CodeCriteria(const RegionStats *mean, const RegionStats *variance, double threshold) {
            init_class();
            init(mean, variance, threshold);
        }
        virtual ~CodeCriteria() {}
        virtual CodeCriteria* create() const;

        static size_t define_criterion(const std::string &name, const std::string &desc, size_t id=(size_t)(-1));
        static size_t find_criterion(const std::string &name);
        static size_t get_ncriteria();
        static const std::string& get_name(size_t id);
        static const std::string& get_desc(size_t id);

        virtual double get_mean(size_t id) const;
        virtual void set_mean(size_t id, double mean);
        virtual double get_variance(size_t id) const;
        virtual void set_variance(size_t id, double variance);
        virtual double get_weight(size_t id) const;
        virtual void set_weight(size_t id, double weight);
        void set_value(size_t id, double mean, double variance, double weight) {
            set_mean(id, mean);
            set_variance(id, variance);
            set_weight(id, weight);
        }

        double get_threshold() const { return threshold; }
        void set_threshold(double th) { threshold=th; }
        virtual double get_vote(const RegionStats*, std::vector<double> *votes=NULL) const;
        virtual bool satisfied_by(const RegionStats*, double *raw_vote_ptr=NULL, std::ostream *debug=NULL) const;

        virtual void print(std::ostream&, const RegionStats *stats=NULL, const std::vector<double> *votes=NULL,
                           const double *total_vote=NULL) const;
        friend std::ostream& operator<<(std::ostream&, const CodeCriteria&);

    protected:
        static void init_class();
        virtual void init(const RegionStats *mean, const RegionStats *variance, double threshold);
        
    };

    /** Create a new region statistics object.  We do it this way because the statistics class is closely tied to the
     *  partitioner class, but users might want to augment the statistics.  The RegionStats is a virtual class as is this
     *  creator. */
    virtual RegionStats *new_region_stats() {
        return new RegionStats;
    }

    /** Create a new criteria object.  This allows a user to derive a new class from CodeCriteria and have that class be used
     *  by the partitioner.
     * @{ */
    virtual CodeCriteria *new_code_criteria() {
        return new CodeCriteria;
    }
    virtual CodeCriteria *new_code_criteria(const RegionStats *mean, const RegionStats *variance, double threshold) {
        return new CodeCriteria(mean, variance, threshold);
    }
    /** @} */

    /** Computes various statistics over part of an address space.  If no region is supplied then the statistics are calculated
     *  over the part of the Partitioner memory map that contains execute permission.  The statistics are returned by argument
     *  so that subclasses have an easy way to augment them.
     *@{ */
    virtual RegionStats *region_statistics(const ExtentMap&);
    virtual RegionStats *region_statistics(Function*);
    virtual RegionStats *region_statistics();
    /** @} */

    /** Computes aggregate statistics over all known functions.  This method computes region statistics for each individual
     *  function (except padding and leftovers) and obtains an average and, optionally, the variance.  The average and variance
     *  are cached in the partitioner and can be retrieved by get_aggregate_mean() and get_aggregate_variance().  This method
     *  also returns the mean regardless of whether its cached. The values are not recomputed if they are already cached; the
     *  cache can be cleared with clear_aggregate_cache(). */
    virtual RegionStats *aggregate_statistics(bool do_variance=true);

    /** Accessors for cached aggregate statistics.  If the partitioner has aggregated statistics over known functions, then
     *  that information is available by this method: get_aggregate_mean() returns the average values over all functions, and
     *  get_aggregate_variance() returns the variance.  The partitioner normally calculates this information immediately after
     *  performing the first CFG analysis, after most instructions are added to most functions, but before data blocks are
     *  added.  A null pointer is returned if the information is not available.  The user is allowed to modify the values, but
     *  should not free the objects.  New values can be computed by clearing the cache (clear_aggregate_statistics()) and then
     *  calling a function that computes them again, such as aggregate_statistics() or is_code().
     * @{ */
    virtual RegionStats *get_aggregate_mean() const { return aggregate_mean; }
    virtual RegionStats *get_aggregate_variance() const { return aggregate_variance; }
    /** @} */

    /** Causes the partitioner to forget statistics.  The statistics aggregated over known functions are discarded, and
     *  subsequent calls to get_aggregate_mean() and get_aggregate_variance() will return null pointers until the data is
     *  recalculated (if ever). */
    virtual void clear_aggregate_statistics() {
        delete aggregate_mean;       aggregate_mean = NULL;
        delete aggregate_variance;   aggregate_variance = NULL;
    }

    /** Counts the number of distinct kinds of instructions.  The counting is based on the instructions' get_kind() method.
     *  @{ */
    virtual size_t count_kinds(const InstructionMap&);
    virtual size_t count_kinds() { return count_privileged(insns); }
    /** @} */

    /** Counts the number of privileged instructions.  Such instructions are generally don't appear in normal code.
     *  @{ */
    virtual size_t count_privileged(const InstructionMap&);
    virtual size_t count_privileged() { return count_privileged(insns); }
    virtual double ratio_privileged() { return insns.empty() ? NAN : (double)count_privileged(insns) / insns.size(); }
    /** @} */

    /** Counts the number of floating point instructions.
     *  @{ */
    virtual size_t count_floating_point(const InstructionMap&);
    virtual size_t count_floating_point() { return count_floating_point(insns); }
    virtual double ratio_floating_point() { return insns.empty() ? NAN : (double)count_floating_point(insns) / insns.size(); }
    /** @} */

    /** Counts the number of register references.  Returns the total number of register reference expressions, but the real
     *  value of this method is that it also computes the an average register reference size and variance.  Register sizes are
     *  represented as a power of two in an attempt to weight common register sizes equally.  In other words, a 16 bit program
     *  with a couple of 8 bit values should have a variance that's close to a similar sized 32-bit program with a couple of
     *  16-bit values.
     * @{ */
    virtual size_t count_registers(const InstructionMap&, double *mean=NULL, double *variance=NULL);
    virtual size_t count_registers(double *mean=NULL, double *variance=NULL) { return count_registers(insns, mean, variance); }
    virtual double ratio_registers(double *mean=NULL, double *variance=NULL) {
        return insns.empty() ? NAN : (double)count_registers(mean, variance) / insns.size();
    }
    /** @} */

    /** Returns the variance of instruction bit widths.  The variance is computed over the instruction size, the address size,
     *  and the operand size.  The sizes 16-, 32-, and 64-bit are mapped to the integers 0, 1, and 2 respectively and the mean
     *  is computed.  The variance is the sum of squares of the difference between each data point and the mean.  Returns NAN
     *  if the instruction map is empty.  Most valid code has a variance of less than 0.05.
     *  @{ */
    virtual double count_size_variance(const InstructionMap &insns);
    virtual double count_size_variance() { return count_size_variance(insns); }
    /** @} */

    /** Determines if a region contains code.  The determination is made by computing aggregate statistics over each of the
     *  functions that are already known, then building a CodeCriteria object.  The same analysis is run over the region in
     *  question and the compared with the CodeCriteria object.  The criteria is then discarded.
     *
     *  If the partitioner's get_aggregate_mean() and get_aggregate_variance() return non-null values, then those statistics
     *  are used in favor of computing new ones.  If new statistics are computed, they will be cached for those methods to
     *  return later.
     *
     *  If a raw_vote_ptr is supplied, then upon return it will hold a value between zero and one, inclusive, which is the
     *  weighted average of the votes from the individual analyses.  The raw vote is the value compared against the code
     *  criteria threshold to obtain a Boolean result. */
    virtual bool is_code(const ExtentMap &region, double *raw_vote_ptr=NULL, std::ostream *debug=NULL);

    /** Accessors for code criteria.  A CodeCriteria object can be associated with the Partitioner, in which case the
     *  partitioner does not compute statistics over the known functions, but rather uses the code criteria directly.
     *  The caller is reponsible for allocating and freeing the criteria.  If no criteria is supplied, then one is created as
     *  necessary by calling new_code_criteria() and passing it the average and variance computed over all the functions
     *  (excluding leftovers and padding) or use the values cached in the partitioner.
     * @{ */
    virtual CodeCriteria *get_code_criteria() const { return code_criteria; }
    virtual void set_code_criteria(CodeCriteria *cc) { code_criteria = cc; }
    /** @} */

protected:
    RegionStats *aggregate_mean;                /**< Aggregate statistics returned by get_region_stats_mean(). */
    RegionStats *aggregate_variance;            /**< Aggregate statistics returned by get_region_stats_variance(). */
    CodeCriteria *code_criteria;                /**< Criteria used to determine if a region contains code or data. */

    /**************************************************************************************************************************
     *                                  Functions for scanning through memory
     **************************************************************************************************************************/
public:

    /** Base class for instruction scanning callbacks. */
    class InsnRangeCallback {
    public:
        /** Arguments for the callback. */
        struct Args {
            Args(Partitioner *partitioner, Instruction *insn_prev, Instruction *insn_begin,
                 Instruction *insn_end, size_t ninsns)
                : partitioner(partitioner), insn_prev(insn_prev), insn_begin(insn_begin), insn_end(insn_end),
                  ninsns(ninsns) {}
            Partitioner *partitioner;
            Instruction *insn_prev;                     /**< Previous instruction not in range, or null. */
            Instruction *insn_begin;                    /**< First instruction in range of instructions. */
            Instruction *insn_end;                      /**< First subsequent instruction not in range, or null. */
            size_t ninsns;                              /**< Number of instructions in range. */
        };

        virtual ~InsnRangeCallback() {}

        /** The actual callback function.  This needs to be defined in subclasses. */
        virtual bool operator()(bool enabled, const Args &args) = 0;
    };
    typedef ROSE_Callbacks::List<InsnRangeCallback> InsnRangeCallbacks;

    /** Base class for byte scanning callbacks. */
    class ByteRangeCallback {
    public:
        /** Arguments for the callback. */
        struct Args {
            Args(Partitioner *partitioner, MemoryMap *restrict_map, const FunctionRangeMap &ranges, const Extent &range)
                : partitioner(partitioner), restrict_map(restrict_map), ranges(ranges), range(range) {}
            Partitioner *partitioner;
            MemoryMap *restrict_map;                    /**< Optional memory map supplied to scan_*_bytes() method. */
            const FunctionRangeMap &ranges;             /**< The range map over which we are iterating. */
            Extent range;                               /**< Range of address space being processed by the callback. */
        };

        virtual ~ByteRangeCallback() {}

        /** The actual callback function.  This needs to be defined in subclasses. */
        virtual bool operator()(bool enabled, const Args &args) = 0;
    };
    typedef ROSE_Callbacks::List<ByteRangeCallback> ByteRangeCallbacks;

    /** Scans contiguous sequences of instructions.  The specified callbacks are invoked for each contiguous sequence of
     *  instructions in the specified instruction map.  At each iteration of the loop, we choose the instruction with the
     *  lowest address and the subsequent instructions that are contiguous in memory, build up the callback argument list,
     *  invoke the callbacks on the list, and remove those instructions from consideration by subsequent iterations of the
     *  loop.
     *
     *  The callback arguments are built from the supplied values of @p insn_prev and @p insn_end.  The @p insn_begin member is
     *  the instruction with the lowest address in this iteration and @p ninsns is the number of contiguous instructions.
     *
     *  @{ */
    virtual void scan_contiguous_insns(InstructionMap insns, InsnRangeCallbacks &cblist,
                                       Instruction *insn_prev, Instruction *insn_end);
    void scan_contiguous_insns(const InstructionMap &insns, InsnRangeCallback *callback,
                               Instruction *insn_prev, Instruction *insn_end) {
        InsnRangeCallbacks cblist(callback);
        scan_contiguous_insns(insns, cblist, insn_prev, insn_end);
    }
    /** @} */

    /** Scans ranges of unassigned instructions.  Scans through the list of existing instructions that are not assigned to any
     *  function and invokes all of the specified callbacks on each range of such instructions.  The ranges of unassigned
     *  instructions are not necessarily contiguous or non-overlapping but are bounded by the @p insn_begin (inclusive) and @p
     *  insn_end (exclusive, or null) callback arguments. The callbacks are invoked via the scan_contiguous_insns() method with
     *  a different @p insn_begin for each call.
     *
     *  Callbacks are allowed to disassemble additional instructions and/or assign/break associations between instructions and
     *  functions.  Only the instructions that are already disassembled at the beginning of this call are considered by the
     *  iterators, but the instruction/function associations may change during the iteration.
     *
     *  All callbacks should honor their "enabled" argument and do nothing if it is clear.  This feature is used by some of the
     *  other instruction scanning methods to filter out certain ranges of instructions.  For instance, the
     *  scan_intrafunc_insns() will set "enabled" to true only for ranges of unassigned instructions whose closest surrounding
     *  assigned instructions both belong to the same function.
     *
     *  @{ */
    virtual void scan_unassigned_insns(InsnRangeCallbacks &callbacks);
    void scan_unassigned_insns(InsnRangeCallback *callback) {
        InsnRangeCallbacks cblist(callback);
        scan_unassigned_insns(cblist);
    }
    /** @} */

    /** Scans the unassigned instructions within a function.  The specified callbacks are invoked for each range of unassigned
     *  instructions whose closest surrounding assigned instructions both belong to the same function.  This can be used, for
     *  example, to discover instructions that should probably be considered part of the same function as the surrounding
     *  instructions.
     *
     *  This method operates by making a temporary copy of @p callbacks, prepending a filtering callback, and then invoking
     *  scan_unassigned_insns().  Therefore, the callbacks supplied by the user should all honor their "enabled" argument.
     *
     *  @{ */
    virtual void scan_intrafunc_insns(InsnRangeCallbacks &callbacks);
    void scan_intrafunc_insns(InsnRangeCallback *callback) {
        InsnRangeCallbacks cblist(callback);
        scan_intrafunc_insns(cblist);
    }
    /** @} */

    /** Scans the instructions between functions.  The specified callbacks are invoked for each set of instructions (not
     *  necessarily contiguous in memory) that fall "between" two functions.  Instruction I(x) at address x is between two
     *  functions, Fa and Fb, if there exists a lower address a<x such that I(a) belongs to Fa and there exists a higher
     *  address b>x such that I(b) belongs to Fb; and for all instructions I(y) for a<y<b, I(y) does not belong to any
     *  function.
     *
     *  Additionally, if no I(a) exists that belongs to a function, and/or no I(b) exists that belongs to a function, then I(x)
     *  is also considered part of an inter-function region and the lower and/or upper functions are undefined. In other words,
     *  instructions appearing before all functions or after all functions are also considered to be between functions, and all
     *  instructions are considered to be between functions if there are no functions.
     *
     *  Only instructions that have already been disassembled are considered.
     *
     *  @{ */
    virtual void scan_interfunc_insns(InsnRangeCallbacks &callbacks);
    void scan_interfunc_insns(InsnRangeCallback *callback) {
        InsnRangeCallbacks cblist(callback);
        scan_interfunc_insns(cblist);
    }
    /** @} */

    /** Scans ranges of the address space that have not been assigned to any function.  For each contiguous range of address
     *  space that is not associated with any function, each of the specified callbacks is invoked in turn until one of them
     *  returns false.  The determination of what parts of the address space belong to functions is made before any of the
     *  callbacks are invoked and not updated for the duration of this function.  The determination is made by calling
     *  Partitioner::function_extent() across all known functions, and then passing that mapping to each of the callbacks.
     *
     *  If a @p restrict_map MemoryMap is specified then only addresses that are also defined in the map are considered.
     *
     *  @{ */
    virtual void scan_unassigned_bytes(ByteRangeCallbacks &callbacks, MemoryMap *restrict_map=NULL);
    void scan_unassigned_bytes(ByteRangeCallback *callback, MemoryMap *restrict_map=NULL) {
        ByteRangeCallbacks cblist(callback);
        scan_unassigned_bytes(cblist, restrict_map);
    }
    /** @} */

    /** Scans unassigned ranges of the address space within a function.  The specified callbacks are invoked for each range of
     *  the address space whose closest surrounding assigned addresses both belong to the same function.  This can be used,
     *  for example, to discover static data or unreachable instructions (by static analysis) that should probably belong to
     *  the surrounding function.
     *
     *  If a @p restrict_map MemoryMap is specified then only addresses that are also defined in the map are considered.
     *
     *  @{ */
    virtual void scan_intrafunc_bytes(ByteRangeCallbacks &callbacks, MemoryMap *restrict_map=NULL);
    void scan_intrafunc_bytes(ByteRangeCallback *callback, MemoryMap *restrict_map=NULL) {
        ByteRangeCallbacks cblist(callback);
        scan_intrafunc_bytes(cblist, restrict_map);
    }
    /** @} */

    /** Scans unassigned ranges of the address space between functions.  The specified callbacks are invoked for each range of
     *  addresses that fall "between" two functions.  An address is between two functions if the next lower assigned address
     *  belongs to one function and the next higher assigned address belongs to some other function, or if there is no assigned
     *  lower address and/or no assigned higher address.
     *
     *  If a @p restrict_map MemoryMap is specified then only addresses that are also defined in the map are considered.
     *
     *  @{ */
    virtual void scan_interfunc_bytes(ByteRangeCallbacks &callbacks, MemoryMap *restrict_map=NULL);
    void scan_interfunc_bytes(ByteRangeCallback *callback, MemoryMap *restrict_map=NULL) {
        ByteRangeCallbacks cblist(callback);
        scan_interfunc_bytes(cblist, restrict_map);
    }
    /** @}*/

    /** Callback to detect padding.  This callback looks for repeated patterns of bytes that are used for padding and adds them
     *  as a static data block to the preceding function.  Multiple patterns can be specified per callback object for
     *  efficiency, and the first pattern that matches will be used.  Each pattern is matched as many times as possible (up to
     *  a user-specified maximum).  Once a repeated matching of the pattern is found, it is considered padding only if it
     *  matches at least some minimum (user-specified) number of times and is anchored to (contiguous with) a preceding and/or
     *  following function according to the @p begins_contiguously and @p ends_contiguously properties.
     *
     *  This callback can be invoked by scan_unassigned_bytes(), scan_intrafunc_bytes(), or scan_interfunc_bytes() depending on
     *  the kind of padding for which it is searching. */
    struct FindDataPadding: public ByteRangeCallback {
        std::vector<SgUnsignedCharList> patterns;       /**< Pattern of padding, repeated at least minimum_size times. */
        size_t minimum_nrep;                            /**< Minimum number of matched patterns to be considered padding. */
        size_t maximum_nrep;                            /**< Maximum number of mathced patterns to be considered padding. */
        bool begins_contiguously;                       /**< If true, pattern must start immediately after a function. */
        bool ends_contiguously;                         /**< If true, pattern must end immediately before a function. */
        rose_addr_t maximum_range_size;                 /**< Skip this callback if the range is larger than this. */
        size_t nfound;                                  /**< Total number of blocks found by this callback. */

        FindDataPadding()
            : minimum_nrep(2), maximum_nrep(1024*1024), begins_contiguously(false), ends_contiguously(true),
              maximum_range_size(100*1024*1024),  nfound(0) {}
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to add unassigned addresses to a function.  Any unassigned addresses are added as data blocks to the preceding
     *  normal function.  Normal functions are anything but FUNC_INTERPAD or FUNC_THUNK.  This callback is generally used as a
     *  final pass over the over the address space to vacuum up anything that couldn't be assigned in previous passes.  It
     *  should be invoked after padding is detected, or else the padding will end up as part of the same data block. */
    struct FindData: public ByteRangeCallback {
        unsigned excluded_reasons;                      /**< Bit mask of function reasons to be avoided. */
        DataRangeMap *padding_ranges;                   /**< Padding ranges created on demand and cached. */
        size_t nfound;                                  /**< Number of data blocks added by this callback. */

        FindData(): excluded_reasons(SgAsmFunction::FUNC_PADDING|SgAsmFunction::FUNC_THUNK), padding_ranges(NULL), nfound(0) {}
        ~FindData() { delete padding_ranges; }
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to create inter-function instruction padding.  This callback can be passed to the scan_interfunc_insns()
     *  method's callback list.  Whenever it detects a contiguous sequence of one or more of the specified instructions (in any
     *  order) immediately after the end of a function it will either create a new SgAsmFunction::FUNC_INTERPAD function to
     *  hold the padding, or add the padding as data to the end of the preceding function.
     *
     *  Here's an example of how to use this (see the post_cfg() method for actual use):
     *  @code
     *  // Create the callback object and specify that padding
     *  // consists of any combination of x86 NOP and INT3 instructions.
     *  FindInsnPadding pad1;
     *  pad1.x86_kind.insert(x86_nop);
     *  pad1.x86_kind.insert(x86_int3);
     *
     *  // Create a second callback that looks for instructions of
     *  // any architecture that consist of 5 or more zero bytes.
     *  FindInsnPadding pad2;
     *  SgUnsignedCharList zero;
     *  zero.push_back(0x00);
     *  zero.minimum_size = 5;
     *  pad2.byte_patterns.push_back(zero);
     *
     *  // Build the callback list
     *  InsnRangeCallbacks cblist;
     *  cblist.append(&pad1);
     *  cblist.append(&pad2);
     *
     *  // Run the callback(s) on the list, invoking them for
     *  // contiguous sequences of instructions that are not yet
     *  // assigned to functions and which appear between two
     *  // functions.
     *  scan_interfunc_insns(cblist);
     *  @endcode
     *
     *  If we want padding to be a sequence of either NOP instructions or INT3 instructions but not a mixture of both, then
     *  we would create two callback objects, one for NOP and one for INT3.  They can both be added to the callback list for a
     *  single invocation of scan_interfunc_insns(), or we can make two separate calls to scan_interfunc_insns(). Likewise,
     *  if we had added the zero byte pattern to the first callback instead of creating a second callback, the padding could
     *  consist of any combination of NOP, INT3, or zero bytes.
     *
     *  See also FindDataPadding, which doesn't need pre-existing instructions.
     */
    struct FindInsnPadding: public InsnRangeCallback {
        std::set<X86InstructionKind> x86_kinds;                 /**< Kinds of x86 instructions allowed. */
        std::vector<SgUnsignedCharList> byte_patterns;          /**< Match instructions with specified byte patterns. */
        bool begins_contiguously;                               /**< Must immediately follow the end of a function? */
        bool ends_contiguously;                                 /**< Must immediately precede the beginning of a func? */
        size_t minimum_size;                                    /**< Minimum size in bytes. */
        bool add_as_data;                                       /**< If true, create data otherwise create a function. */
        size_t nfound;                                          /**< Number of padding areas found by this callback. */

        FindInsnPadding()
            : begins_contiguously(true), ends_contiguously(true), minimum_size(0), add_as_data(true), nfound(0) {}
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to insert unreachable code fragments.  This callback can be passed to the scan_unassigned_bytes() method's
     *  callback list.  Whenever it detects a region of unassigned bytes that looks like it might be code, it generates basic
     *  blocks and attaches them to the immediately preceding function.
     *
     *  If the @p require_noninterleaved property is set (the default) then the callback is triggered only if the preceding
     *  function's extent is not interleaved with other functions.  Normally, if two or more functions are interleaved then we
     *  cannot assume that the range of instructions being analyzed by this callback belongs to the surrounding function or
     *  some other (possibly interleaved) function.
     *
     *  If the @p require_intrafunction property is set (default is clear) then the region being analyzed must be immediately
     *  followed by something that belongs to the preceding function.
     *
     *  A fragment is added to the preceding function only if the fragment looks statistically like code.  A CodeCriteria
     *  object is created on the first call if necessary and is initialized based on the statistics computed across all known
     *  functions (as returned by Partitioner::aggregate_statistics()) and the @p threshold data member.  The caller can supply
     *  its own CodeCriteria if desired, in which case FindFunctionFragments::threshold is unused.  In either case, the
     *  CodeCriteria object is deleted when the FindFunctionFragments object is deleted.
     *
     *  This callback is skipped if the preceding function contains any of the SgAsmFunction::FunctionReason bits that are set
     *  in the @p excluded_reasons data member.  The default is to exclude functions created for padding or thunks.
     *
     *  This callback might create new basic blocks as a side effect even if those blocks are not added to any function. */
    struct FindFunctionFragments: public ByteRangeCallback {
        bool require_noninterleaved;                            /**< If set, then preceding function cannot be interleaved. */
        bool require_intrafunction;                             /**< If set, range must be inside the preceding function. */
        double threshold;                                       /**< Threshold for determining whether range is code. */
        unsigned excluded_reasons;                              /**< Functions for which callback should be skipped. */
        size_t nfound;                                          /**< Number of basic blocks added as code fragments. */

        FunctionRangeMap *function_extents;                     /**< Cached function extents computed on first call. */
        CodeCriteria *code_criteria;                            /**< Cached code criteria computed on first call. */

        FindFunctionFragments()
            : require_noninterleaved(true), require_intrafunction(false), threshold(0.7),
              excluded_reasons(SgAsmFunction::FUNC_PADDING|SgAsmFunction::FUNC_THUNK),
              nfound(0), function_extents(NULL), code_criteria(NULL)
            {}
        virtual ~FindFunctionFragments() {
            delete function_extents;
            delete code_criteria;
        }
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to find thunks.  Creates functions whose only instruction is a JMP to the entry point of another function.
     *  This should be called by scan_unassigned_insns() before the PostFunctionBlocks callback. Since this is an instruction
     *  callback, it only scans existing instructions.
     *
     *  A thunk by this definition is a JMP instruction that is not already in the middle of a basic block and which has a
     *  single successor that's the entry point of an existing function.
     *
     *  See also, FindThunkTables class. */
    struct FindThunks: public InsnRangeCallback {
        size_t validate_targets;        /**< If true, then the successor must point to the entry point of an existing function. */
        size_t nfound;                  /**< Incremented for each thunk found and added. */

        FindThunks(): validate_targets(true), nfound(0) {}
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to find thunk tables.  Creates functions whose only instruction is a JMP.  The detection is only triggered
     *  when a user-specified consecutive number of JMP instructions are encountered.  The definition of a thunk in this case
     *  is a JMP to an address that has an instruction, provided the JMP does not already appear in a basic block containing
     *  more than just the JMP instruction.  If the validate_targets property is false, then the callback does not verify that
     *  the JMP successors are addresses where an instruction can be disassembled.
     *
     *  See also, FindThunks class. */
    struct FindThunkTables: public ByteRangeCallback {
        bool begins_contiguously;       /**< Match table only at the beginning of the address range. */
        bool ends_contiguously;         /**< Match table only at the end of the address range. */
        size_t minimum_nthunks;         /**< Mininum number of JMPs necessary to be considered a thunk table. */
        bool validate_targets;          /**< If true, then successors must point to instructions. */
        size_t nfound;                  /**< Number of thunks (not tables) found by this callback. */

        FindThunkTables()
            : begins_contiguously(false), ends_contiguously(false), minimum_nthunks(3), validate_targets(true), nfound(0) {}
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to find functions that are between padding.  This callback looks for BLK_PADDING data blocks that are separated
     *  from one another by a region of unassigned bytes, and causes the first of those bytes to be a function entry address.
     *  The padding that follows is moved into the new function. */
    struct FindInterPadFunctions: public ByteRangeCallback {
        DataRangeMap *padding_ranges;   /**< Information about all the padding data blocks that belong to functions. */
        size_t nfound;                  /**< Number of functions found and added by this callback. */

        FindInterPadFunctions(): padding_ranges(NULL), nfound(0) {}
        ~FindInterPadFunctions() { delete padding_ranges; }
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to add post-function instructions to the preceding function.  Any instructions that immediately follow a
     *  normal function (anything but FUNC_INTERPAD or FUNC_THUNK) but are not assigned to any function are added to that
     *  function as instructions.  This should be called after inter-function padding has been discovered, or else the padding
     *  will end up as part of the same data block.
     *
     *  See also FindData.  It probably doesn't make sense to use both.
     *
     *  Note: This is highly experimental. [RPM 2011-09-22] */
    struct FindPostFunctionInsns: public InsnRangeCallback {
        size_t nfound;                  /**< Number of basic blocks added to functions by this callback. */

        FindPostFunctionInsns(): nfound(0) {}
        virtual bool operator()(bool enabled, const Args &args);
    };

    /**************************************************************************************************************************
     *                                  Methods for finding functions by patterns
     **************************************************************************************************************************/
protected:
    /** Looks for stack frame setup. Tries to match "(mov rdi,rdi)?; push rbp; mov rbp,rsp" (or the 32-bit equivalent). The
     *  first MOV instruction is a two-byte no-op used for hot patching of executables (single instruction rather than two NOP
     *  instructions so that no thread is executing at the second byte when the MOV is replaced by a JMP).  The PUSH and second
     *  MOV are the standard way to set up the stack frame. */
    static InstructionMap::const_iterator pattern1(const InstructionMap& insns, InstructionMap::const_iterator first,
                                                   Disassembler::AddressSet &exclude);

#if 0 /* Definitions are also commented out */
    /** Matches after NOP padding. Tries to match "nop;nop;nop" followed by something that's not a nop and returns the
     *  something that's not a nop if successful. */
    static InstructionMap::const_iterator pattern2(const InstructionMap& insns, InstructionMap::const_iterator first,
                                                   Disassembler::AddressSet &exclude);

    /** Matches after stack frame destruction. Matches x86 "leave;ret" followed by one or more "nop" followed by a non-nop
     *  instruction and if matching, returns the iterator for the non-nop instruction. */
    static InstructionMap::const_iterator pattern3(const InstructionMap& insns, InstructionMap::const_iterator first,
                                                   Disassembler::AddressSet &exclude);
#endif

    /** Matches an x86 "enter xxxx, 0" instruction and creates a function at that address. */
    static InstructionMap::const_iterator pattern4(const InstructionMap &insns, InstructionMap::const_iterator first,
                                                   Disassembler::AddressSet &exclude);

    /** Matches an M68k "link a6, xxxx" instruction where xxxx is zero or negative and creates a function at that address. */
    static InstructionMap::const_iterator pattern5(const InstructionMap &insns, InstructionMap::const_iterator first,
                                                   Disassembler::AddressSet &exclude);

    /** Matches M68k "rts; (trapf)?; lea.l [a7+X], a7". X is an arbitrarily small-magnitude, negative number. The function
     *  begins at the LEA instruction. */
    static InstructionMap::const_iterator pattern6(const InstructionMap &insns, InstructionMap::const_iterator first,
                                                   Disassembler::AddressSet &exclude);



    /*************************************************************************************************************************
     *                                                 Low-level Functions
     *
     * These are public because they might need to be called by the partitioner's instruction or address traversal callbacks,
     * and its often convenient to declare those functors outside any Partitioner subclass.
     *************************************************************************************************************************/
public:
    /* NOTE: Some of these are documented at their implementation because the documentation is more than what conveniently
     *       fits here. */
    struct AbandonFunctionDiscovery {};                         /**< Exception thrown to defer function block discovery. */

    virtual void append(BasicBlock*, Instruction*);             /**< Add an instruction to a basic block. */
    virtual void append(BasicBlock*, DataBlock*, unsigned reasons); /* Add a data block to a basic block. */
    virtual void append(Function*, BasicBlock*, unsigned reasons, bool keep=false); /* Append a basic block to a function */
    virtual void append(Function*, DataBlock*, unsigned reasons, bool force=false); /* Append a data block to a function */
    virtual void remove(Function*, BasicBlock*);                /* Remove a basic block from a function. */
    virtual void remove(Function*, DataBlock*);                 /* Remove a data block from a function. */
    virtual void remove(BasicBlock*, DataBlock*);               /* Remove association between basic block and data block. */
    virtual BasicBlock* find_bb_containing(rose_addr_t, bool create=true); /* Find basic block containing instruction address */
    virtual BasicBlock* find_bb_starting(rose_addr_t, bool create=true);   /* Find or create block starting at specified address */
    virtual DataBlock* find_db_starting(rose_addr_t, size_t size); /* Find (or create if size>0) a data block */
    virtual Disassembler::AddressSet successors(BasicBlock*, bool *complete=NULL); /* Calculates known successors */
    virtual rose_addr_t call_target(BasicBlock*);               /* Returns address if block could be a function call */
    virtual void truncate(BasicBlock*, rose_addr_t);            /* Remove instructions from the end of a basic block. */
    virtual void discover_first_block(Function*);               /* Adds first basic block to empty function to start discovery. */
    virtual void discover_blocks(Function*, unsigned reason);   /* Start to recursively discover blocks of a function. */
    virtual void discover_blocks(Function*, rose_addr_t, unsigned reason); /* Recursively discovers blocks of a function. */
    virtual void pre_cfg(SgAsmInterpretation *interp=NULL);     /**< Detects functions before analyzing the CFG. */
    virtual void analyze_cfg(SgAsmBlock::Reason);               /**< Detect functions by analyzing the CFG. */
    virtual void post_cfg(SgAsmInterpretation *interp=NULL);    /**< Detects functions after analyzing the CFG. */
    virtual SgAsmFunction* build_ast(Function*);                /**< Build an AST for a single function. */
    virtual SgAsmBlock* build_ast(BasicBlock*);                 /**< Build an AST for a single basic block. */
    virtual SgAsmBlock* build_ast(DataBlock*);                  /**< Build an AST for a single data block. */
    virtual bool pops_return_address(rose_addr_t);              /**< Determines if a block pops the stack w/o returning */
    virtual void update_analyses(BasicBlock*);                  /* Makes sure cached analysis results are current. */
    virtual rose_addr_t canonic_block(rose_addr_t);             /**< Follow alias links in basic blocks. */
    virtual bool is_function_call(BasicBlock*, rose_addr_t*);   /* True if basic block appears to call a function. */
    virtual bool is_thunk(Function*);                           /* True if function is a thunk. */
    virtual Function *effective_function(DataBlock*);           /* Function to which a data block is currently bound. */

    virtual void mark_call_insns();                             /**< Naive marking of CALL instruction targets as functions */
    virtual void mark_ipd_configuration();                      /**< Seeds partitioner with IPD configuration information */
    virtual void mark_entry_targets(SgAsmGenericHeader*);       /**< Seeds functions for program entry points */
    virtual void mark_export_entries(SgAsmGenericHeader*);      /**< Seeds functions for PE exports */
    virtual void mark_eh_frames(SgAsmGenericHeader*);           /**< Seeds functions for error handling frames */
    virtual void mark_elf_plt_entries(SgAsmGenericHeader*);     /**< Seeds functions that are dynamically linked via .plt */
    virtual void mark_func_symbols(SgAsmGenericHeader*);        /**< Seeds functions that correspond to function symbols */
    virtual void mark_func_patterns();                          /* Seeds functions according to instruction patterns */
    virtual void name_plt_entries(SgAsmGenericHeader*);         /* Assign names to ELF PLT functions */
    virtual void name_import_entries(SgAsmGenericHeader*);      /* Assign names to PE import functions */
    virtual void find_pe_iat_extents(SgAsmGenericHeader*);      /* Find addresses for all PE Import Address Tables */

    /** Adds extents for all defined functions.  Scans across all known functions and adds their extents to the specified
     *  RangeMap argument. Returns the sum of the return values from the single-function function_extent() method. */
    virtual size_t function_extent(FunctionRangeMap *extents);

    /** Returns addresses that are not part of any function.  This is the complement of the set returned by @ref
     *  function_extent. */
    virtual ExtentMap unused_addresses();

    /** Determines whether an address is part of any function.  Returns true if the address is part of an instruction that
     *  belongs to a function, or part of a static data block that belongs to a function.  This isn't a very efficient function
     *  the way it's currently implemented. */
    bool is_used_address(rose_addr_t);

    /** Returns information about the function addresses.  Every non-empty function has a minimum (inclusive) and maximum
     *  (exclusive) address which are returned by reference, but not all functions own all the bytes within that range of
     *  addresses. Therefore, the exact bytes are returned by adding them to the optional ExtentMap argument.  This function
     *  returns the number of nodes (instructions and static data items) in the function.  If the function contains no
     *  nodes then the extent map is not modified and the low and high addresses are both set to zero.
     *
     *  See also: SgAsmFunction::get_extent(), which calculates the same information but can be used only after we've constructed
     *  the AST for the function. */
    virtual size_t function_extent(Function*,
                                   FunctionRangeMap *extents=NULL/*in,out*/,
                                   rose_addr_t *lo_addr=NULL/*out*/, rose_addr_t *hi_addr=NULL/*out*/);

    /** Returns information about the datablock addresses.  Every data block has a minimum (inclusive) and maximum (exclusive)
     *  address which are returned by reference, but some of the addresses in that range might not be owned by the specified
     *  data block.  Therefore, the exact bytes are returned by adding them to the optional ExtentMap argument.  This function
     *  returns the number of nodes (static data items) in the data block.  If the data block contains no nodes then the extent
     *  map is not modified, the low and high addresses are both set to zero, and the return value is zero. */
    virtual size_t datablock_extent(DataBlock*,
                                    DataRangeMap *extents=NULL/*in,out*/,
                                    rose_addr_t *lo_addr=NULL/*out*/, rose_addr_t *hi_addr=NULL/*out*/);

    /** Adds assigned datablocks to extent.  Scans across all known data blocks and for any block that's assigned to a
     *  function, adds that block's extents to the supplied RangeMap.  Return value is the number of data blocks added. */
    virtual size_t datablock_extent(DataRangeMap *extent/*in,out*/);

    /** Adds padding datablocks to extent.  Scans across all known data blocks, and for any padding block that's assigned to a
     * function, adds that block's extents to the supplied RangeMap.  Return value is the number of padding blocks added. */
    virtual size_t padding_extent(DataRangeMap *extent/*in,out*/);

    /** Returns an indication of whether a function is contiguous.  All empty functions are contiguous. If @p strict is true,
     *  then a function is contiguous if it owns all bytes in a contiguous range of the address space.  If @p strict is false
     *  then the definition is relaxed so that the instructions need not be contiguous in memory as long as no other function
     *  owns any of the bytes between this function's low and high address range. */
    virtual bool is_contiguous(Function*, bool strict=false);

    /** Return the virtual address that holds the branch target for an indirect branch. For example, when called with these
     *  instructions:
     *
     *  @code
     *     jmp DWORD PTR ds:[0x80496b0]        -> (x86)   returns 80496b0
     *     jmp QWORD PTR ds:[rip+0x200b52]     -> (amd64) returns 200b52 + address following instruction
     *     jmp DWORD PTR ds:[ANY_GPR+0x18]     -> (x86)   returns offset+0x18
     *     // anything else return zero
     *  @endcode
     *
     * We only handle instructions that appear as the first instruction in an ELF .plt entry. */
    static rose_addr_t get_indirection_addr(SgAsmInstruction*, rose_addr_t offset);

    /** Returns the integer value of a value expression since there's no virtual method for doing this. (FIXME) */
    static rose_addr_t value_of(SgAsmValueExpression*);

    /** Splits thunks off of the start of functions.  Splits as many thunks as possible from the front of all known functions.
     *  Returns the number of thunks split off from functions.  It's not important that this be done, but doing so results in
     *  functions that more closely match what some other disassemblers do when provided with debug info. */
    virtual size_t detach_thunks();

    /** Splits one thunk off the start of a function if possible.  Since the partitioner constructs functions according to the
     *  control flow graph, thunks (JMP to start of function) often become part of the function to which they jump.  This can
     *  happen if the real function has no direct callers and was not detected as a function entry point due to any pattern or
     *  symbol.  The detach_thunks() function traverses all defined functions and looks for cases where the thunk is attached
     *  to the jumped-to function, and splits them into two functions. */
    virtual bool detach_thunk(Function*);

    /** Returns true if the basic block is a PE dynamic linking thunk. If the argument is a basic block, then the only
     *  requirement is that the basic block contains a single instruction, which in the case of x86, is an indirect JMP through
     *  an Import Address Table. If the argument is a function, then the function must contain a single basic block which is a
     *  dynamic linking thunk. The addresses of the IATs must have been previously initialized by pre_cfg() or other.
     * @{ */
    bool is_pe_dynlink_thunk(Instruction*);
    bool is_pe_dynlink_thunk(BasicBlock*);
    bool is_pe_dynlink_thunk(Function*);
    /** @} */

    /** Gives names to PE dynamic linking thunks if possible. The names come from the PE Import Table if an interpretation
     *  is supplied as an argument. This also marks such functions as being thunks. */
    void name_pe_dynlink_thunks(SgAsmInterpretation *interp/*=NULL*/);
    
    /** Adjusts ownership of padding data blocks.  Each padding data block should be owned by the prior function in the address
     *  space.  This is normally the case, but when functions are moved around, split, etc., the padding data blocks can get
     *  mixed up.  This method puts them all back where they belong. */
    virtual void adjust_padding();

    /** Merge function fragments.  The partitioner sometimes goes crazy breaking functions into smaller and smaller parts.
     *  This method attempts to merge all those parts after the partitioner's function detection has completed.  A function
     *  fragment is any function whose only reason code is one of the GRAPH codes (function detected by graph analysis and the
     *  rule that every function has only one entry point). */
    virtual void merge_function_fragments();

    /** Merge two functions.  The @p other function is merged into @p parent and then @p other is deleted. */
    virtual void merge_functions(Function *parent, Function *other);

    /** Looks for a jump table.  This method looks at the specified basic block and tries to discover if the last instruction
     *  is an indirect jump through memory.  If it is, then the entries of the jump table are returned by value (i.e., the
     *  control flow successors of the given basic block), and the addresses of the table are added to the optional extent
     *  map.  It is possible for the jump table to be discontiguous, but this is not usually the case.  If @p do_create is true
     *  then data blocks are created for the jump table and added to the basic block. */
    Disassembler::AddressSet discover_jump_table(BasicBlock *bb, bool do_create=true, ExtentMap *table_addresses=NULL);

    /** Looks for functions that follow padding.  This method repeatedly looks for padding bytes and then tries to discover a
     *  function immediately after those bytes by recursively disassembling and following the control flow graph.  Searching for
     *  the padding and function discovery are interleaved: look for padding, discover function, repeat.  The supplied map is
     *  used for searching, but the usual map is used for discovery. */
    virtual void discover_post_padding_functions(const MemoryMap &map);

    /** Return the next unused address.  Scans through the memory map starting at the specified address and returns the first
     *  address found to be mapped but not belonging to any basic block or data block. Returns none on failure. */
    virtual Sawyer::Optional<rose_addr_t> next_unused_address(const MemoryMap &map, rose_addr_t start_va);

    /*************************************************************************************************************************
     *                                   IPD Parser for initializing the Partitioner
     *************************************************************************************************************************/
public:

    /** This is the parser for the instruction partitioning data (IPD) files.  These files are text-based descriptions of the
     *  functions and basic blocks used by the partitioner and allow the user to seed the partitioner with additional information
     *  that is not otherwise available to the partitioner.
     *
     *  For instance, the analyst may know that a function begins at a certain virtual address but for some reason the partitioner
     *  does not discover this address in its normal mode of operation.  The analyst can create an IPD file that describes the
     *  function so that the Partitioning process finds the function.
     *
     *  An IPD file is able to:
     *  <ul>
     *    <li>specify an entry address of a function that is otherwise not detected.</li>
     *    <li>give a name to a function that doesn't have one.</li>
     *    <li>specify whether the function ever returns to the caller.</li>
     *    <li>list additional basic blocks that appear in the function.</li>
     *    <li>specify the address of a basic block that is otherwise not detected.</li>
     *    <li>indicate that a basic block is semantically equivalent to another basic block.</li>
     *    <li>override the control-flow successors for a basic block.</li>
     *  </ul>
     *
     *  The language non-terminals are:
     *  \code
     *     File := Declaration+
     *     Declaration := FuncDecl | BlockDecl
     *
     *     FuncDecl := 'function' Address [Name] [FuncBody]
     *     FuncBody := '{' FuncStmtList '}'
     *     FuncStmtList := FuncStmt [';' FuncStmtList]
     *     FuncStmt := ( Empty | BlockDecl | ReturnSpec )
     *     ReturnSpec := 'return' | 'returns' | 'noreturn'
     *
     *     BlockDecl := 'block' Address Integer [BlockBody]
     *     BlockBody := '{' BlockStmtList '}'
     *     BlockStmtList := BlockStmt [';' BlockStmtList]
     *     BlockStmt := ( Empty | Alias | Successors ) ';'
     *     Alias := 'alias' Address
     *     Successors := ('successor' | 'successors') [SuccessorAddrList|AssemblyCode]
     *     SuccessorAddrList := '{' (AddressList | AddressList '...' | '...') '}'
     *
     *     AddressList := Address ( ',' AddressList )*
     *     Address: Integer
     *     Integer: DECIMAL_INTEGER | OCTAL_INTEGER | HEXADECIMAL_INTEGER
     *     Name: STRING
     *     AssemblyCode: asm '{' ASSEMBLY '}'
     *  \endcode
     *
     *  Language terminals:
     *  \code
     *     HEXADECIMAL_INTEGER: as in C, for example: 0x08045fe2
     *     OCTAL_INTEGER: as in C, for example, 0775
     *     DECIMAL_INTEGER: as in C, for example, 1234
     *     STRING: double quoted. Use backslash to escape embedded double quotes
     *     ASSEMBLY: x86 assembly instructions (must contain balanced curly braces, if any)
     *  \endcode
     *
     *  Comments begin with a hash ('#') and continue to the end of the line.  The hash character is not treated specially inside
     *  quoted strings.  Comments within an ASSEMBLY terminal must conform to the syntax accepted by the Netwide Assembler (nasm),
     *  namely semicolon in place of a hash.
     *
     *  <h2>Semantics</h2>
     *
     *  A block declaration specifies the virtual memory address of the block's first instruction. The integer after the
     *  address specifies the number of instructions in the block.  If the specified length is less than the number of
     *  instructions that ROSE would otherwise place in the block at that address, then ROSE will create a block of exactly
     *  the specified size. Likewise, if the specified address is midway into a block that ROSE would otherwise create, ROSE
     *  will create a block at the specified address anyway, causing the previous instructions to be in a separate
     *  block (or blocks).  If the specified block size is larger than what ROSE would otherwise place in the block, the block
     *  will be created with fewer instructions but the BlockBody will be ignored.
     *
     *  A function declaration specifies the virtual memory address of the entry point of a function. The body may specify
     *  whether the function returns. As of this writing [2010-05-13] a function declared as non-returning will be marked as
     *  returning if ROSE discovers that a basic block of the function returns.
     *
     *  If a block declaration appears inside a function declaration, then ROSE will assign the block to the function.
     *
     *  The block 'alias' attribute is used to indicate that two basic blocks perform the exact same operation.  The specified
     *  address is the address of the basic block to use instead of this basic block.  All control-flow edges pointing to this
     *  block will be rewritten to point to the specified address instead.
     *
     *  Example file:
     *  \code
     *     function 0x805116 "func11" {             # declare a new function named "func11"
     *         returns;                             # this function returns to callers
     *         block 0x805116 {                     # block at 0x805116 is part of func11
     *             alias 0x8052116, 0x8052126       # use block 0x805116 in place of 0x8052116 and 0x8052126
     *         }
     *     }
     *  \endcode
     *
     *  <h2>Basic Block Successors</h2>
     *
     *  A block declaration can specify control-flow successors in two ways: as a list of addresses, or as an x86 assembly
     *  language program that's interpretted by ROSE.  The benefits of using a program to determine the successors is that the
     *  program can directly extract information, such as jump tables, from the specimen executable.
     *
     *  The assembly source code is fed to the Netwide Assembler, nasm (http://www.nasm.us/), which assembles it into i386
     *  machine code. When ROSE needs to figure out the successors for a basic block it will interpret the basic block, then
     *  load the successor program and interpret it, then extract the successor list from the program's return value. ROSE
     *  interprets the program rather than running it directly so that the program can operate on unknown, symbolic data
     *  values rather than actual 32-bit numbers.
     *
     *  The successor program is interpretted in a context that makes it appear to have been called (via CALL instruction)
     *  from the end of the basic block being analyzed.  These arguments are passed to the program:
     *
     *  <ul>
     *    <li>The address of an "svec" object to be filled in by the program. The first four-byte word at this address
     *        is the number of successor addresses that immediately follow and must be a known value upon return of the
     *        program.  The following values are the successors--either known values or unknown values.</li>
     *    <li>The size of the "svec" object in bytes. The object is allocated by ROSE and is a fixed size (8192 bytes at
     *        the time of this writing--able to hold 2047 successors).</li>
     *    <li>The starting virtual address of the first instruction of the basic block.</li>
     *    <li>The address immediately after the last instruction of the basic block. Depending on the Partitioner settings,
     *        basic block may or may not be contiguous in memory.</li>
     *    <li>The value of the stack pointer at the end of the basic block. ROSE creates a new stack before starting the
     *        successor program because the basic block's stack might not be at a known memory address.</li>
     *  </ul>
     *
     *  The successor program may either fall off the end or execute a RET statement.
     *
     *  For instance, if the 5-instruction block at virtual address 0x00c01115 ends with an indirect jump through a
     *  256-element jump table beginning at 0x00c037fa, then a program to compute the successors might look like this:
     *
     *  \code
     *    block 0x00c01115 5 {
     *      successors asm {
     *          push ebp
     *          mov ebp, esp
     *          ; ecx is the base address of the successors return vector,
     *          ; the first element of which is the vector size.
     *          mov ecx, [ebp+8]
     *          add ecx, 4
     *          ; loop over the entries in the jump table, copying each
     *          ; address from the jump table to the svec return value
     *          xor eax, eax
     *        loop:
     *          cmp eax, 256
     *          je done
     *          mov ebx, [0x00c037fa+eax*4]
     *          mov [ecx+eax*4], ebx
     *          inc eax
     *          jmp loop
     *        done:
     *          ; set the number of entries in the svec
     *          mov ecx, [ebp+8]
     *          mov DWORD [ecx], 256
     *          mov esp, ebp
     *          pop ebp
     *          ret
     *  \endcode
     *
     *  <h2>Example Programmatic Usage</h2>
     *
     *  The easiest way to parse an IPD file is to read it into memory and then call the parse() method.  The
     *  following code demonstrates the use of mmap to read the file into memory, parse it, and release it from memory.  For
     *  simplicity, we do not check for errors in this example.
     *  \code
     *    Partitioner p;
     *    int fd = open("test.ipd", O_RDONLY);
     *    struct stat sb;
     *    fstat(fd, &sb);
     *    const char *content = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
     *    Partitioner::IPDParser(p, content, sb.st_size).parse();
     *    munmap(content, sb.st_size);
     *  \endcode
     */
    class IPDParser {
    private:
        Partitioner *partitioner;               /**< Partitioner to be initialized. */
        const char *input;                      /**< Input to be parsed. */
        size_t len;                             /**< Length of input, not counting NUL termination (if any). */
        std::string input_name;                 /**< Optional name of input (usually a file name). */
        size_t at;                              /**< Current parse position w.r.t. "input". */
        Function *cur_func;                     /**< Non-null when inside a FuncBody nonterminal. */
        BlockConfig *cur_block;                 /**< Non-null when inside a BlockBody nonterminal. */

    public:
        IPDParser(Partitioner *p, const char *input, size_t len, const std::string &input_name="")
            : partitioner(p), input(input), len(len), input_name(input_name), at(0), cur_func(NULL), cur_block(NULL) {}

        /** Exception thrown when something cannot be parsed. */
        class Exception: public std::runtime_error {
        public:
            Exception(const std::string &mesg)
                : std::runtime_error(mesg), lnum(0) {}
            Exception(const std::string &mesg, const std::string &name, unsigned lnum=0)
                : std::runtime_error(mesg), name(name), lnum(lnum) {}
            ~Exception() throw() {}
            std::string format() const;         /**< Format exception object into an error message; used by operator<<. */
            friend std::ostream& operator<<(std::ostream&, const Exception &e);

            std::string name;                   /**< Optional name of input */
            unsigned lnum;                      /**< Line number (1-origin); zero if unknown */
        };

        void parse();                           /**< Top-level parsing function. */
        static void unparse(std::ostream&, SgNode *ast); /**< Unparse an AST into an IPD file. */


        /*************************************************************************************************************************
         * Lexical analysis functions.
         *************************************************************************************************************************/
    private:
        void skip_space();

        /* The is_* functions return true if the next token after white space and comments is of the specified type. */
        bool is_terminal(const char *to_match);
        bool is_symbol(const char *to_match);
        bool is_string();
        bool is_number();

        /* The match_* functions skip over white space and comments and attempt to match (and consume) the next token. If the next
         * token is not as expected then an exception is thrown. */
        void match_terminal(const char *to_match);
        void match_symbol(const char *to_match);
        std::string match_symbol();
        std::string match_string();
        rose_addr_t match_number();
        std::string match_asm();        /* assembly code inside nested curly braces */


        /*************************************************************************************************************************
         * Parsing functions (see rules above). Each returns true if the construct is present and was parsed, false if the
         * construct was not present. They throw an exception if the construct was partially present but an error occurred during
         * parsing.
         *************************************************************************************************************************/
    private:
        bool parse_File();
        bool parse_Declaration();
        bool parse_FuncDecl();
        bool parse_FuncBody();
        bool parse_FuncStmtList();
        bool parse_FuncStmt();
        bool parse_ReturnSpec();
        bool parse_BlockDecl();
        bool parse_BlockBody();
        bool parse_BlockStmtList();
        bool parse_BlockStmt();
        bool parse_Alias();
        bool parse_Successors();
    };

    /*************************************************************************************************************************
     *                                                     Data Members
     *
     * These are public so they can be accessed by user-defined traversal callbacks that might be declared outside any
     * Partitioner subclass.
     *************************************************************************************************************************/
public:
    Disassembler *disassembler;                         /**< Optional disassembler to call when an instruction is needed. */
    InstructionMap insns;                               /**< Instruction cache, filled in by user or populated by disassembler. */
    MemoryMap *map;                                     /**< Memory map used for disassembly if disassembler is present. */
    MemoryMap ro_map;                                   /**< The read-only parts of 'map', used for insn semantics mem reads. */
    ExtentMap pe_iat_extents;                           /**< Virtual addresses for all PE Import Address Tables. */
    Disassembler::BadMap bad_insns;                     /**< Captured disassembler exceptions. */

    BasicBlocks basic_blocks;                           /**< All known basic blocks. */
    Functions functions;                                /**< All known functions, pending and complete. */

    DataBlocks data_blocks;                             /**< Blocks that point to static data. */

    unsigned func_heuristics;                           /**< Bit mask of SgAsmFunction::FunctionReason bits. */
    std::vector<FunctionDetector> user_detectors;       /**< List of user-defined function detection methods. */

    static Sawyer::Message::Facility mlog;              /**< Logging facility for partitioners. */
    bool allow_discont_blocks;                          /**< Allow basic blocks to be discontiguous in virtual memory. */
    BlockConfigMap block_config;                        /**< IPD configuration info for basic blocks. */

    static double progress_interval;                    /**< Minimum interval between progress reports in seconds. */
    static double progress_time;                        /**< Time of last report, or zero if no report has been generated. */

public:
    static const rose_addr_t NO_TARGET = (rose_addr_t)-1;
};

} // namespace
} // namespace


#endif
