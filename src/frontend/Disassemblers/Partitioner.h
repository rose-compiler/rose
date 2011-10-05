#ifndef ROSE_DISASSEMBLER_PARTITIONER_H
#define ROSE_DISASSEMBLER_PARTITIONER_H

#include "callbacks.h"
#include "Disassembler.h"

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
        BasicBlock(): reason(SgAsmBlock::BLK_NONE), function(NULL) {}

        /** Destructor. This destructor should not be called directly since there are other pointers to this block that the
         *  block does not know about. Instead, call Partitioner::discard(). */
        ~BasicBlock() {}

        /** Returns true if the block analysis cache is up to date. */
        bool valid_cache() const { return cache.age==insns.size(); }

        /** Marks the block analysis cache as being outdated. */
        void invalidate_cache() { cache.age=0; }

        /** Marks the block analysis cache as being up to date. */
        void validate_cache() { cache.age=insns.size(); }

        SgAsmInstruction* last_insn() const;    /**< Returns the last executed (exit) instruction of the block */
        rose_addr_t address() const;            /* Return the address of the basic block's first (entry) instruction. */
        unsigned reason;                        /**< Reasons this block was created; SgAsmBlock::Reason bit flags */
        std::vector<SgAsmInstruction*> insns;   /**< Non-empty set of instructions composing this basic block, in address order */
        BlockAnalysisCache cache;               /**< Cached results of local analyses */
        Function* function;                     /**< Function to which this basic block is assigned, or null */
    };
    typedef std::map<rose_addr_t, BasicBlock*> BasicBlocks;

    /** Represents a region of static data within the address space being disassembled.  Each data block will eventually become
     *  an SgAsmBlock node in the AST if it is assigned to a function.
     *
     *  The address of the first SgAsmStaticData node of a DataBlock should remain constant for the life of the DataBlock.
     *  This is because we use that address to establish a two-way link between the DataBlock and a Function object. */
    struct DataBlock {
        /** Constructor. This constructor should not be called directly since the Partitioner has other pointers that it needs
         * to establish to this block.  Instead, call one of the Partitioner's data block creation functions, such as
         * Partitioner::find_db_starting(). */
        DataBlock(): reason(SgAsmBlock::BLK_NONE), function(NULL) {}

        /** Destructor.  This destructor should not be called directly since there are other pointers in the Partitioner that
         * this block does not know about.  Instead, call Partitioner::discard(). */
        ~DataBlock() {}

        rose_addr_t address() const;            /* Return the address of the first node of a data block. */
        std::vector<SgAsmStaticData*> nodes;    /**< The static data nodes belonging to this block; not deleted. */
        unsigned reason;                        /**< Reasons this block was created; SgAsmBlock::Reason bit flags */
        Function *function;                     /**< Function to which this data block is assigned, or null */
    };
    typedef std::map<rose_addr_t, DataBlock*> DataBlocks;

    /** Represents a function within the Partitioner. Each non-empty function will become an SgAsmFunction in the AST. */
    struct Function {
        Function(rose_addr_t entry_va): reason(0), pending(true), entry_va(entry_va), returns(false) {}
        Function(rose_addr_t entry_va, unsigned r): reason(r), pending(true), entry_va(entry_va), returns(false) {}
        Function(rose_addr_t entry_va, unsigned r, const std::string& name)
            : reason(r), name(name), pending(true), entry_va(entry_va), returns(false) {}
        void clear_basic_blocks();              /**< Remove all basic blocks from this function w/out deleting the blocks. */
        void clear_data_blocks();               /**< Remove all data blocks from this function w/out deleting the blocks. */
        unsigned reason;                        /**< SgAsmFunction::FunctionReason bit flags */
        std::string name;                       /**< Name of function if known */
        BasicBlocks basic_blocks;               /**< Basic blocks belonging to this function */
        DataBlocks data_blocks;                 /**< Data blocks belonging to this function */
        bool pending;                           /**< True if we need to (re)discover the basic blocks */
        rose_addr_t entry_va;                   /**< Entry virtual address */
        Disassembler::AddressSet heads;         /**< CFG heads, excluding func entry: addresses of additional blocks */
        bool returns;                           /**< Does this function return? */
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
     *  \deprecated This function is deprecated.  Basic blocks are now represented by Partitioner::BasicBlock
     *  and the insn2block map. */
    BasicBlockStarts detectBasicBlocks(const Disassembler::InstructionMap&) const __attribute__((deprecated));

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
                                   BasicBlockStarts &bb_starts/*out*/) const __attribute__((deprecated));


    /*************************************************************************************************************************
     *                                                 Constructors, etc.
     *************************************************************************************************************************/
public:

    Partitioner()
        : disassembler(NULL), map(NULL), func_heuristics(SgAsmFunction::FUNC_DEFAULT), debug(NULL),
          allow_discont_blocks(true)
        {}
    virtual ~Partitioner() { clear(); }

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

    /** Sends diagnostics to the specified output stream. Null (the default) turns off debugging. */
    void set_debug(FILE *f) {
        debug = f;
    }

    /** Returns the file currently used for debugging; null implies no debugging. */
    FILE *get_debug() const {
        return debug;
    }

    /** Accessors for the memory map.  The partitioner needs to know the memory map that was used (or will be used, depending
     *  on the partitioning mode of operation) for disassembly.  The map should certainly include all the bytes of instructions
     *  because it may be used to construct static data blocks that are interspersed with the instructions.  It should also
     *  include all read-only data if possible because that will improve the control flow analysis for indirect branches.
     *
     *  @{ */
    void set_map(MemoryMap *mmap) {
        map = mmap;
    }
    MemoryMap *get_map() const {
        return map;
    }
    /** @} */

    /** Set progress reporting properties.  A progress report is produced not more than once every @p min_interval seconds
     * (default is 10) by sending a single line of ouput to the specified file.  Progress reporting can be disabled by supplying
     * a null pointer for the file.  Progress report properties are class variables. */
    void set_progress_reporting(FILE*, unsigned min_interval);

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
    virtual SgAsmBlock* partition(SgAsmInterpretation*, const Disassembler::InstructionMap&, MemoryMap *mmap);

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
    virtual void add_instructions(const Disassembler::InstructionMap& insns) {
        this->insns.insert(insns.begin(), insns.end());
    }

    /** Get the list of all instructions.  This includes instructions that were added with add_instructions(), instructions
     *  added by a passive partition() call, and instructions added by an active partitioner. */
    const Disassembler::InstructionMap& get_instructions() const {
        return insns;
    }

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
    virtual SgAsmInstruction* find_instruction(rose_addr_t, bool create=true);

    /** Drop an instruction from consideration.  If the instruction belongs to a basic block then the basic block is also
     *  dropped and its other instructions, if any, are returned to the (implied) list of free instructions.  The basic block
     *  must not be currently assigned to any function.  The instruction is not deleted.
     *
     *  This method always returns the null pointer. */
    virtual SgAsmInstruction* discard(SgAsmInstruction*);

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

    /** Builds the AST describing all the functions. The return value is an SgAsmBlock node that points to a list of
     *  SgAsmFunction nodes (the functions), each of which points to a list of SgAsmBlock nodes (the basic
     *  blocks). Any basic blocks that were not assigned to a function by the Partitioner will be added to a function named
     *  "***uncategorized blocks***" whose entry address will be the address of the lowest instruction, and whose reasons for
     * existence will include the SgAsmFunction::FUNC_LEFTOVERS bit.  However, if the FUNC_LEFTOVERS bit is not
     * turned on (see set_search()) then uncategorized blocks will not appear in the AST. */
    virtual SgAsmBlock* build_ast();

    /** Update SgAsmTarget block pointers.  This method traverses the specified AST and updates any SgAsmTarget nodes so their
     *  block pointers point to actual blocks.  The update only happens for SgAsmTarget objects that don't already have a node
     *  pointer. */
    virtual void update_targets(SgNode *ast);

    /**************************************************************************************************************************
     *                                  Functions for scanning through memory
     **************************************************************************************************************************/
public:

    class InsnRangeCallback {
    public:
        struct Args {
            Args(Partitioner *partitioner, SgAsmInstruction *insn_prev, SgAsmInstruction *insn_begin,
                 SgAsmInstruction *insn_end, size_t ninsns)
                : partitioner(partitioner), insn_prev(insn_prev), insn_begin(insn_begin), insn_end(insn_end),
                  ninsns(ninsns) {}
            Partitioner *partitioner;
            SgAsmInstruction *insn_prev;                /**< Previous instruction not in range, or null. */
            SgAsmInstruction *insn_begin;               /**< First instruction in range of instructions. */
            SgAsmInstruction *insn_end;                 /**< First subsequent instruction not in range, or null. */
            size_t ninsns;                              /**< Number of instructions in range. */
        };

        /** The actual callback function.  This needs to be defined in subclasses. */
        virtual bool operator()(bool enabled, const Args &args) = 0;
    };

    typedef ROSE_Callbacks::List<InsnRangeCallback> InsnRangeCallbacks;

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
    virtual void scan_contiguous_insns(Disassembler::InstructionMap insns, InsnRangeCallbacks &cblist,
                                       SgAsmInstruction *insn_prev, SgAsmInstruction *insn_end);
    void scan_contiguous_insns(const Disassembler::InstructionMap &insns, InsnRangeCallback *callback,
                               SgAsmInstruction *insn_prev, SgAsmInstruction *insn_end) {
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

    /** Callback to create inter-function instruction padding.  This callback can be passed to the scan_interfunc_insns()
     *  method's callback list.  Whenever it detects a contiguous sequence of one or more of the specified instructions (in any
     *  order) immediately after the end of a function it will either create a new SgAsmFunction::FUNC_INTERPAD function to
     *  hold the padding, or add the padding as data to the end of the preceding function.
     *
     *  Here's an example of how to use this (see the post_cfg() method for actual use):
     *  @code
     *  // Create the callback object and specify that padding
     *  // consists of any combination of x86 NOP and INT3 instructions.
     *  InterFuncInsnPadding pad1;
     *  pad1.x86_kind.insert(x86_nop);
     *  pad1.x86_kind.insert(x86_int3);
     *
     *  // Create a second callback that looks for instructions of
     *  // any architecture that consist of 5 or more zero bytes.
     *  InterFuncInsnPadding pad2;
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
     */
    struct InterFuncInsnPadding: public InsnRangeCallback {
        std::set<X86InstructionKind> x86_kinds;                         /**< Kinds of x86 instructions allowed. */
        std::vector<SgUnsignedCharList> byte_patterns;                  /**< Match instructions with specified byte patterns. */
        bool begins_contiguously;                                       /**< Must immediately follow the end of a function? */
        bool ends_contiguously;                                         /**< Must immediately precede the beginning of a func? */
        size_t minimum_size;                                            /**< Minimum size in bytes. */
        bool add_as_data;                                               /**< If true, create data otherwise create a function. */

        InterFuncInsnPadding()
            : begins_contiguously(true), ends_contiguously(true), minimum_size(0), add_as_data(true) {}
        virtual bool operator()(bool enabled, const Args &args);        /**< The actual callback function. */
    };

    /** Callback to insert unreachable intra-function blocks.  This callback can be passed to the scan_intrafunc_insns()
     *  method's callback list.  Whenever it detects a block of unassigned instructions between blocks that both belong to the
     *  same function and the function is considered contiguous via the non-strict version of the is_contiguous() method, then
     *  the block in question is added to the function.  If add_as_data is true, then the block is added as data rather that
     *  instructions. */
    struct IntraFunctionBlocks: public InsnRangeCallback {
        bool add_as_data;
        IntraFunctionBlocks(): add_as_data(true) {}
        virtual bool operator()(bool enabled, const Args &args);        /**< The actual callback function. */
    };

    /** Callback to find thunks.  Creates functions whose only instruction is a JMP to the entry point of another function.
     *  This should be called by scan_unassigned_insns() before the PostFunctionBlocks callback.
     *
     *  Note: This is highly experimental. [RPM 2011-09-22] */
    struct FindThunks: public InsnRangeCallback {
        virtual bool operator()(bool enabled, const Args &args);
    };

    /** Callback to add post-function instructions to the preceding function.  It should be called by the
     *  scan_interfunc_insns() method after inter-function padding is found.  If add_as_data is true, then the blocks are added
     *  as data blocks rather than instructions.
     *
     *  Note: This is highly experimental. [RPM 2011-09-22] */
    struct PostFunctionBlocks: public InsnRangeCallback {
        bool add_as_data;
        PostFunctionBlocks(): add_as_data(true) {}
        virtual bool operator()(bool enabled, const Args &args);
    };


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

    virtual void append(BasicBlock*, SgAsmInstruction*);        /**< Add an instruction to a basic block. */
    virtual void append(Function*, BasicBlock*, unsigned reasons, bool keep=false); /* Append a basic block to a function */
    virtual void append(Function*, DataBlock*, unsigned reasons); /* Append a data block to a function */
    virtual void remove(Function*, BasicBlock*);                /**< Remove a basic block from a function. */
    virtual void remove(Function*, DataBlock*);                 /**< Remove a data block from a function. */
    virtual BasicBlock* find_bb_containing(rose_addr_t, bool create=true); /* Find basic block containing instruction address */
    virtual BasicBlock* find_bb_starting(rose_addr_t, bool create=true);   /* Find or create block starting at specified address */
    virtual DataBlock* find_db_starting(rose_addr_t, size_t size); /* Find (or create if size>0) a data block */
    virtual Disassembler::AddressSet successors(BasicBlock*, bool *complete=NULL); /* Calculates known successors */
    virtual rose_addr_t call_target(BasicBlock*);               /* Returns address if block could be a function call */
    virtual void truncate(BasicBlock*, rose_addr_t);            /**< Remove instructions from the end of a basic block. */
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

    virtual void mark_call_insns();                             /**< Naive marking of CALL instruction targets as functions */
    virtual void mark_ipd_configuration();                      /**< Seeds partitioner with IPD configuration information */
    virtual void mark_entry_targets(SgAsmGenericHeader*);       /**< Seeds functions for program entry points */
    virtual void mark_eh_frames(SgAsmGenericHeader*);           /**< Seeds functions for error handling frames */
    virtual void mark_elf_plt_entries(SgAsmGenericHeader*);     /**< Seeds functions that are dynamically linked via .plt */
    virtual void mark_func_symbols(SgAsmGenericHeader*);        /**< Seeds functions that correspond to function symbols */
    virtual void mark_func_patterns();                          /* Seeds functions according to instruction patterns */
    virtual void name_plt_entries(SgAsmGenericHeader*);         /**< Assign names to ELF PLT functions */

    /** Returns information about the function addresses.  Every non-empty function has a minimum (inclusive) and maximum
     *  (exclusive) address which are returned by reference, but not all functions own all the bytes within that range of
     *  addresses. Therefore, the exact bytes are returned by adding them to the optional ExtentMap argument.  This function
     *  returns the number of nodes (instructions and static data items) in the function.  If the function contains no
     *  nodes then the extent map is not modified and the low and high addresses are both set to zero.
     *
     *  See also: SgAsmFunction::get_extent(), which calculates the same information but can be used only after we've constructed
     *  the AST for the function. */
    virtual size_t function_extent(Function*, ExtentMap *extents=NULL, rose_addr_t *lo_addr=NULL, rose_addr_t *hi_addr=NULL);

    /** Returns information about the datablock addresses.  Every data block has a minimum (inclusive) and maximum (exclusive)
     *  address which are returned by reference, but some of the addresses in that range might not be owned by the specified
     *  data block.  Therefore, the exact bytes are returned by adding them to the optional ExtentMap argument.  This function
     *  returns the number of nodes (static data items) in the data block.  If the data block contains no nodes then the extent
     *  map is not modified, the low and high addresses are both set to zero, and the return value is zero. */
    virtual size_t datablock_extent(DataBlock*, ExtentMap *extents=NULL, rose_addr_t *lo_addr=NULL, rose_addr_t *hi_addr=NULL);

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
     *  @endcode
     *
     * We only handle instructions that appear as the first instruction in an ELF .plt entry. */
    static rose_addr_t get_indirection_addr(SgAsmInstruction*);

    /** Returns the integer value of a value expression since there's no virtual method for doing this. (FIXME) */
    static rose_addr_t value_of(SgAsmValueExpression*);

    /** Conditionally prints a progress report. If progress reporting is enabled and the required amount of time has elapsed
     *  since the previous report, then the supplied report is emited. Also, if debugging is enabled the report is emitted to
     *  the debugging file regardless of the elapsed time. The arguments are the same as fprintf(). */
    void progress(FILE*, const char *fmt, ...) const __attribute__((format(gnu_printf, 3, 4)));

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
     *     SuccessorAddrList := AddressList | AddressList '...' | '...'
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

        class Exception {                      /**< Exception thrown when something cannot be parsed. */
        public:
            Exception(const std::string &mesg)
                : lnum(0), mesg(mesg) {}
            Exception(const std::string &mesg, const std::string &name, unsigned lnum=0)
                : name(name), lnum(lnum), mesg(mesg) {}
            std::string format() const;         /**< Format exception object into an error message; used by operator<<. */
            friend std::ostream& operator<<(std::ostream&, const Exception &e);

            std::string name;                   /**< Optional name of input */
            unsigned lnum;                      /**< Line number (1-origin); zero if unknown */
            std::string mesg;                   /**< Error message. */
        };

        void parse();                           /**< Top-level parsing function. */


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
    Disassembler::InstructionMap insns;                 /**< Instruction cache, filled in by user or populated by disassembler. */
    MemoryMap *map;                                     /**< Memory map used for disassembly if disassembler is present. */
    Disassembler::BadMap bad_insns;                     /**< Captured disassembler exceptions. */

    BasicBlocks basic_blocks;                           /**< All known basic blocks. */
    std::map<rose_addr_t, BasicBlock*> insn2block;      /**< Map from insns address to basic block. */
    Functions functions;                                /**< All known functions, pending and complete. */

    DataBlocks data_blocks;                             /**< Blocks that point to static data. */

    unsigned func_heuristics;                           /**< Bit mask of SgAsmFunction::FunctionReason bits. */
    std::vector<FunctionDetector> user_detectors;       /**< List of user-defined function detection methods. */

    FILE *debug;                                        /**< Stream where diagnistics are sent (or null). */
    bool allow_discont_blocks;                          /**< Allow basic blocks to be discontiguous in virtual memory. */
    BlockConfigMap block_config;                        /**< IPD configuration info for basic blocks. */

    static time_t progress_interval;                    /**< Minimum interval between progress reports. */
    static time_t progress_time;                        /**< Time of last report, or zero if no report has been generated. */
    static FILE *progress_file;                         /**< File to which reports are made. Null disables reporting. */

public:
    static const rose_addr_t NO_TARGET = (rose_addr_t)-1;
};

#endif
