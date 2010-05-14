#ifndef ROSE_DISASSEMBLER_PARTITIONER_H
#define ROSE_DISASSEMBLER_PARTITIONER_H
#include "Disassembler.h"
/** Partitions instructions into basic blocks and functions.
 *
 *  The Partitioner classes are responsible for partitioning the set of instructions previously disassembled by a Disassembler
 *  class into AST nodes for basic blocks (SgAsmBlock) and functions (SgAsmFunctionDeclaration).  A basic block is a contiguous
 *  sequence of nonoverlapping instructions where control flow enters at only the first instruction and exits at only the last
 *  instruction. A function is a collection of basic blocks with a single entry point. In the final ROSE AST, each instruction
 *  belongs to exactly one basic block, and each basic block belongs to exactly one function.
 *
 *  The partitioner organizes instructions into basic blocks by starting at a particular instruction and then looking at its
 *  set of successor addresses. Successor addresses are edges of the eventual control-flow graph (CFG) that are calculated
 *  using instruction semantics, and are available to the end user via SgAsmBlock::get_cached_successors().  It's not always
 *  possible to statically determine the complete set of successors; in this case, get_cached_successors() will return only the
 *  statically known successors and SgAsmBlock::get_complete_successors() will return false.  Because ROSE performs semantic
 *  analysis over all instructions of the basic block (and occassionally related blocks), it can sometimes determine that
 *  instructions that are usually considered to be conditional branches are, in fact, unconditional (e.g., a PUSH followed by a
 *  RET; or a CALL to a block that discards the return address).  ROSE includes unconditional branch instructions and their
 *  targets within the same basic block (provided no other CFG edge comes into the middle of the block).
 *
 *  The partitioner organizes basic blocks into functions in three phases, all three of which can be run by a single call
 *  to the partitioner() method. The first phase considers all disassembled instructions and other available information such as
 *  symbol tables and tries to determine which addresses are entry points for functions.  For instance, if the symbol table
 *  contains function symbols, then the address stored in the symbol table is assumed to be the entry point of a function.  ROSE
 *  has a variety of these "pre-cfg" detection methods which can be enabled/disabled at runtime with the
 *  "-rose:partitioner_search" command-line switch.  ROSE also supports user-defined search methods that can be registered with
 * an existing partitioner (see add_function_detector()).
 *
 *  The second phase for assigning blocks to functions is via analysis of the control-flow graph.  In a nutshell, ROSE
 *  traverses the CFG starting with the entry address of each function, adding blocks to the function as it goes. When it
 *  detects that a block has edges coming in from two different functions, it creates a new function whose entry point is that
 *  block (see definition of "function" above; a function can only have one entry point).
 *
 *  The third and final phase, called "post-cfg", makes final adjustments, such as adding SgAsmFunctionDeclaration objects for
 *  no-op or zero padding occuring between the previously detected functions. This could also be user-extended to add blocks to
 *  functions that ROSE detected during CFG analysis (such as unreferenced basic blocks, no-ops, etc. that occur within the
 *  extent of a function.)
 *
 *  The results of block and function detection are stored in the Partitioner object itself. One usually retrieves this
 *  information via a call to the build_ast() method, which constructs a ROSE AST.  Any instructions that were not assigned to
 *  blocks of a function can be optionally discarded (see the SgAsmFunctionDeclaration::FUNC_LEFTOVERS bit of set_search(), or
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
protected:
    /*************************************************************************************************************************
     *                                        Data Structures Useful to Subclasses
     *************************************************************************************************************************/

    struct Function;

    /** Analysis that can be cached in a block. Some analyses are expensive enough that they should be cached in a block.
     *  Analyses are either locally computed (by examining only the block where they're cached) or non-locally computed (by
     *  examining other related basic blocks.  Non-local analyses are not cached locally, but they might be cached in other
     *  blocks. */
    class BlockAnalysisCache {
    public:
        BlockAnalysisCache(): age(0), sucs_complete(false), call_target(NO_TARGET) {}
        void clear() {
            age = 0;
            sucs.clear();
            sucs_complete = false;
        }

        size_t age;                             /**< Non zero implies locally computed successors are cached. The actual value
                                                 *   of this data member is the number of instructions in the basic block when
                                                 *   the successor information was computed, so if this value matches the current
                                                 *   number of instructions, the cached info is up to date. Analyses that are
                                                 *   computed over other blocks (i.e., non-local analyses) are never cached. */
        Disassembler::AddressSet sucs;          /**< Locally computed cached successors. */
        bool sucs_complete;                     /**< True if locally computed successors are fully known. */
        rose_addr_t call_target;                /**< Target of a CALL instruction if this block ends with what appears to be a
                                                 *   function call (whether, in fact, it truly is a function call is immaterial
                                                 *   since the final determination requires non-local analysis. If this block
                                                 *   does not end with a call or if the target of the call cannot be statically
                                                 *   determined, then the value is set to Partitioner::NO_TARGET. */
        bool function_return;                   /**< Does this block serve as the return of a function?  For example, does it
                                                 *   end with an x86 RET instruction that returns to the CALL fall-through
                                                 *   address? */
    };

    /** Represents a basic block within the Partitioner. Each basic block will eventually become an SgAsmBlock node in the
     *  AST. However, if the SgAsmFunctionDeclaration::FUNC_LEFTOVER bit is set in the Partitioner::set_search() method then
     *  blocks that were not assigned to any function to not result in an SgAsmBlock node. */
    struct BasicBlock {
        /** Constructor. This constructor should not be called directly since the Partitioner has other pointers that it needs
         *  to establish to this block.  Instead, call Partitioner::find_bb_containing(). */
        BasicBlock(): function(NULL) {}

        /** Destructor. This destructor should not be called directly since there are other pointers to this block that the
         *  block does not know about. Instead, call Partitioner::discard(). */
        ~BasicBlock() {}

        /** Returns true if the block analysis cache is up to date. */
        bool valid_cache() const { return cache.age==insns.size(); }

        /** Marks the block analysis cache as being outdated. */
        void invalidate_cache() { cache.age=0; }

        /** Marks the block analysis cache as being up to date. */
        void validate_cache() { cache.age=insns.size(); }

        bool is_function_call(rose_addr_t*);    /**< True if basic block appears to call a function */
        SgAsmInstruction* last_insn() const;    /**< Returns the last executed (exit) instruction of the block */
        std::vector<SgAsmInstruction*> insns;   /**< Non-empty set of instructions composing this basic block, in address order */
        BlockAnalysisCache cache;               /**< Cached results of local analyses */
        Function* function;                     /**< Function to which this basic block is assigned, or null */
    };
    typedef std::map<rose_addr_t, BasicBlock*> BasicBlocks;

    /** Represents a function within the Partitioner. Each non-empty function will become an SgAsmFunctionDeclaration in the
     *  AST. */
    struct Function {
        Function(rose_addr_t entry_va): reason(0), pending(true), entry_va(entry_va), returns(false) {}
        Function(rose_addr_t entry_va, unsigned r): reason(r), pending(true), entry_va(entry_va), returns(false) {}
        Function(rose_addr_t entry_va, unsigned r, const std::string& name)
            : reason(r), name(name), pending(true), entry_va(entry_va), returns(false) {}
        void clear_blocks();                    /**< Remove all blocks from this function w/out deleting the blocks. */
        BasicBlock* last_block() const;         /**< Return pointer to block with highest address */
        unsigned reason;                        /**< SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;                       /**< Name of function if known */
        BasicBlocks blocks;                     /**< Basic blocks belonging to this function */
        bool pending;                           /**< True if we need to (re)discover the basic blocks */
        rose_addr_t entry_va;                   /**< Entry virtual address */
        bool returns;                           /**< Does this function return? */
    };
    typedef std::map<rose_addr_t, Function*> Functions;

    /** Data type for user-defined function detectors. */
    typedef void (*FunctionDetector)(Partitioner*, SgAsmGenericHeader*, const Disassembler::InstructionMap&);

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
     *  \deprecated This function is deprecated.  Basic blocks are now represented by Partitioner::BasicBlock
     *  and the insn2block map. */
    BasicBlockStarts detectBasicBlocks(const Disassembler::InstructionMap&) const;

    /** Information about each function starting address.
     *
     *  \deprecated This data type is used only for backward compatibility by detectFunctions(). It has been replaced by
     *  Partitioner::Function, which is capable of describing noncontiguous functions. */
    struct FunctionStart {
        FunctionStart(unsigned reason, std::string name): reason(reason), name(name) {}
        unsigned reason;                        /** SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;                       /** Name of function if known. */
    };

    /** Map describing the starting address of each known function.
     *
     *  \deprecated This type has been replaced with Partitioner::Functions, which is capable of describing
     *  noncontiguous functions. */
    typedef std::map<rose_addr_t, FunctionStart> FunctionStarts;

    /** Returns a list of the currently defined functions.
     *
     *  \deprecated This function has been replaced by seed_functions() and analyze_cfg(). */
    FunctionStarts detectFunctions(SgAsmInterpretation*, const Disassembler::InstructionMap &insns,
                                   BasicBlockStarts &bb_starts/*out*/) const;


    /*************************************************************************************************************************
     *                                                 Constructors, etc.
     *************************************************************************************************************************/
public:

    Partitioner(): func_heuristics(SgAsmFunctionDeclaration::FUNC_DEFAULT), debug(NULL), allow_discont_blocks(true) {}
    virtual ~Partitioner() { clear(); }

    /*************************************************************************************************************************
     *                                              Accessors for Properties
     *************************************************************************************************************************/
public:

    /** Sets the set of heuristics used by the partitioner.  The @p heuristics should be a bit mask containing the
     *  SgAsmFunctionDeclaration::FunctionReason bits. These same bits are assigned to the "reason" property of the resulting
     *  function nodes in the AST, depending on which heuristic detected the function. */
    virtual void set_search(unsigned heuristics) {
        func_heuristics = heuristics;
    }

    /** Returns a bit mask of SgAsmFunctionDeclaration::FunctionReason bits indicating which heuristics would be used by the
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

    /** Sends diagnostics to the specified output stream. Null (the default) turns off debugging. */
    void set_debug(FILE *f) {
        debug = f;
    }

    /** Returns the file currently used for debugging; null implies no debugging. */
    FILE *get_debug() const {
        return debug;
    }

    /*************************************************************************************************************************
     *                                                High-level Functions
     *************************************************************************************************************************/
public:

    /** Adds a user-defined function detector to this partitioner. Any number of detectors can be added and they will be run
     *  by pre_cfg() in the order they were added, after the built-in methods run.  Each user-defined detector will be called
     *  first with the SgAmGenericHeader pointing to null, then once for each file header. The user-defined methods are run
     *  only if the SgAsmFunctionDeclaration::FUNC_USERDEF is set (see set_search()), which is the default.   The reason for
     *  having user-defined function detectors is that the detection of functions influences the shape of the AST and so it is
     *  easier to apply those analyses here, before the AST is built, rather than in the mid-end after the AST is built. */
    void add_function_detector(FunctionDetector f) {
        user_detectors.push_back(f);
    }
    
    /** Parses a string describing the heuristics and returns the bit vector that can be passed to set_search(). The input
     *  string should be a comma-separated list (without white space) of search specifications. Each specification should be
     *  an optional qualifier character followed by either an integer or a word. The accepted words are the lower-case
     *  versions of the constants enumerated by SgAsmFunctionDeclaration::FunctionReason, but without the leading "FUNC_".
     *  The qualifier determines whether the bits specified by the integer or word are added to the return value ("+") or
     *  removed from the return value ("-").  The "=" qualifier acts like "+" but first zeros the return value. The default
     *  qualifier is "+" except when the word is "default", in which case the specifier is "=". An optional initial bit mask
     *  can be specified (defaults to SgAsmFunctionDeclaration::FUNC_DEFAULT). */
    static unsigned parse_switches(const std::string&, unsigned initial_flags);

    /** Top-level function to run the partitioner on some instructions and build an AST */
    virtual SgAsmBlock* partition(SgAsmInterpretation*, const Disassembler::InstructionMap&);

    /** Reset partitioner to initial conditions by discarding all instructions, basic blocks, and functions. */
    virtual void clear();

    /** Adds additional instructions to be processed. New instructions are only added at addresses that don't already have an
     *  instruction. */
    virtual void add_instructions(const Disassembler::InstructionMap& insns) {
        this->insns.insert(insns.begin(), insns.end());
    }

    /** Adds a new function definition to the partitioner.  New functions can be added at any time, including during the
     *  analyze_cfg() call.  When this method is called with an entry_va for an existing function, the specified @p reasons
     *  will be merged with the existing function, and the existing function will be given the specified name if it has none. */
    virtual Function* add_function(rose_addr_t entry_va, unsigned reasons, std::string name="");

    /** Builds the AST describing all the functions. The return value is an SgAsmBlock node that points to a list of
     *  SgAsmFunctionDeclaration nodes (the functions), each of which points to a list of SgAsmBlock nodes (the basic
     *  blocks). Any basic blocks that were not assigned to a function by the Partitioner will be added to a function named
     *  "***uncategorized blocks***" whose entry address will be the address of the lowest instruction, and whose reasons for
     * existence will include the SgAsmFunctionDeclaration::FUNC_LEFTOVERS bit.  However, if the FUNC_LEFTOVERS bit is not
     * turned on (see set_search()) then uncategorized blocks will not appear in the AST. */
    virtual SgAsmBlock* build_ast();
    
    /*************************************************************************************************************************
     *                                                 Low-level Functions
     *************************************************************************************************************************/
protected:
    /* NOTE: Some of these are documented at their implementation... */
    struct AbandonFunctionDiscovery {};                         /**< Exception thrown to defer function block discovery */

    virtual void append(BasicBlock*, SgAsmInstruction*);        /**< Add instruction to basic block */
    virtual BasicBlock* find_bb_containing(rose_addr_t, bool create=true); /* Find basic block containing instruction address */
    virtual Disassembler::AddressSet successors(BasicBlock*, bool *complete=NULL); /* Calculates known successors */
    virtual rose_addr_t call_target(BasicBlock*);               /* Returns address if block could be a function call */
    virtual void append(Function*, BasicBlock*);                /**< Append basic block to function */
    virtual BasicBlock* discard(BasicBlock*);                   /**< Delete a basic block and return null */
    virtual void remove(Function*, BasicBlock*);                /**< Remove basic block from function */
    virtual rose_addr_t address(BasicBlock*) const;             /**< Return starting address of basic block */
    virtual void truncate(BasicBlock*, rose_addr_t);            /**< Remove instructions from end of basic block */
    virtual void discover_first_block(Function*);               /* Adds first basic block to empty function to start discovery. */
    virtual void discover_blocks(Function*, rose_addr_t);       /* Recursively discovers blocks of a function. */
    virtual void pre_cfg(SgAsmInterpretation*);                 /**< Detects functions before analyzing the CFG */
    virtual void analyze_cfg();                                 /**< Detect functions by analyzing the CFG */
    virtual void post_cfg(SgAsmInterpretation*);                /**< Detects functions after analyzing the CFG */
    virtual SgAsmFunctionDeclaration* build_ast(Function*);     /**< Build AST for a single function */
    virtual SgAsmBlock* build_ast(BasicBlock*);                 /**< Build AST for a single basic block */
    virtual bool pops_return_address(rose_addr_t);              /**< Determines if a block pops the stack w/o returning */
    virtual void update_analyses(BasicBlock*);                  /* Makes sure cached analysis results are current. */
    
    
    virtual void mark_entry_targets(SgAsmGenericHeader*);       /**< Seeds functions for program entry points */
    virtual void mark_eh_frames(SgAsmGenericHeader*);           /**< Seeds functions for error handling frames */
    virtual void mark_elf_plt_entries(SgAsmGenericHeader*);     /**< Seeds functions that are dynamically linked via .plt */
    virtual void mark_func_symbols(SgAsmGenericHeader*);        /**< Seeds functions that correspond to function symbols */
    virtual void mark_func_patterns(SgAsmGenericHeader*);       /**< Seeds functions according to instruction patterns */
    virtual void name_plt_entries(SgAsmGenericHeader*);         /**< Assign names to ELF PLT functions */
    virtual void create_nop_padding();                          /**< Creates functions to hold NOP padding */
    virtual void create_zero_padding();                         /**< Creates functions to hold zero padding */

    /** Return the virtual address that holds the branch target for an indirect branch. For example, when called with these
     *  instructions:
     *  
     *  @code
     *     jmp DWORD PTR ds:[0x80496b0]        -> (x86)   returns 80496b0
     *     jmp QWORD PTR ds:[rip+0x200b52]     -> (amd64) returns 200b52 + address following instruction
     *  @endcode
     *
     * We only handle instructions that appear as the first instruction in an ELF .plt entry. */
    static rose_addr_t get_indirection_addr(SgAsmInstruction*);

    /** Returns the integer value of a value expression since there's no virtual method for doing this. (FIXME) */
    static rose_addr_t value_of(SgAsmValueExpression*);

    /*************************************************************************************************************************
     *                                                     Data Members
     *************************************************************************************************************************/
protected:

    Disassembler::InstructionMap insns;                 /**< Set of all instructions to partition. */
    std::map<rose_addr_t, BasicBlock*> insn2block;      /**< Map from insns address to basic block */
    Functions functions;                                /**< All known functions, pending and complete */
    BasicBlocks blocks;                                 /**< All known basic blocks */
    unsigned func_heuristics;                           /**< Bit mask of SgAsmFunctionDeclaration::FunctionReason bits */
    std::vector<FunctionDetector> user_detectors;       /**< List of user-defined function detection methods */
    FILE *debug;                                        /**< Stream where diagnistics are sent (or null) */
    bool allow_discont_blocks;                          /**< Allow basic blocks to be discontiguous in virtual memory */

private:
    static const rose_addr_t NO_TARGET = (rose_addr_t)-1;
};

#endif
