#ifndef ROSE_DISASSEMBLER_PARTITIONER_H
#define ROSE_DISASSEMBLER_PARTITIONER_H
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
 *  has a variety of these "pre-cfg" detection methods which can be enabled/disabled at runtime with the set_search() method.
 *  ROSE also supports user-defined search methods that can be registered with add_function_detector().  The three phases are
 *  initialized and influenced by the contents of an optional configuration file specified with the load_config() method.
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
     *  \deprecated This function has been replaced by pre_cfg(), analyze_cfg(), and post_cfg() */
    FunctionStarts detectFunctions(SgAsmInterpretation*, const Disassembler::InstructionMap &insns,
                                   BasicBlockStarts &bb_starts/*out*/) const;


    /*************************************************************************************************************************
     *                                                 Constructors, etc.
     *************************************************************************************************************************/
public:

    Partitioner()
        : disassembler(NULL), map(NULL), func_heuristics(SgAsmFunctionDeclaration::FUNC_DEFAULT), debug(NULL),
          allow_discont_blocks(true)
        {}
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

    /** Set the memory map that was used for disassembly. */
    void set_map(MemoryMap *mmap) {
        map = mmap;
    }
    
    /** Return the memory map that was used for disassembly. */
    MemoryMap *get_map() const {
        return map;
    }

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

    /** Top-level function to run the partitioner on some instructions and build an AST. The SgAsmInterpretation is optional.
     *  If it is null then those function seeding operations that depend on having file headers are not run. */
    virtual SgAsmBlock* partition(SgAsmInterpretation*, const Disassembler::InstructionMap&);

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

    /** Adds a new function definition to the partitioner.  New functions can be added at any time, including during the
     *  analyze_cfg() call.  When this method is called with an entry_va for an existing function, the specified @p reasons
     *  will be merged with the existing function, and the existing function will be given the specified name if it has none. */
    virtual Function* add_function(rose_addr_t entry_va, unsigned reasons, std::string name="");

    /** Looks up a function by address.  Returns the function pointer if found, the null pointer if not found. */
    virtual Function* find_function(rose_addr_t entry_va);

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
    /* NOTE: Some of these are documented at their implementation because the documentation is more than what conveniently
     *       fits here. */
    struct AbandonFunctionDiscovery {};                         /**< Exception thrown to defer function block discovery */

    virtual void append(BasicBlock*, SgAsmInstruction*);        /**< Add instruction to basic block */
    virtual BasicBlock* find_bb_containing(rose_addr_t, bool create=true); /* Find basic block containing instruction address */
    virtual BasicBlock* find_bb_starting(rose_addr_t, bool create=true);   /* Find or create block starting at specified address */
    virtual Disassembler::AddressSet successors(BasicBlock*, bool *complete=NULL); /* Calculates known successors */
    virtual rose_addr_t call_target(BasicBlock*);               /* Returns address if block could be a function call */
    virtual void append(Function*, BasicBlock*);                /**< Append basic block to function */
    virtual BasicBlock* discard(BasicBlock*);                   /**< Delete a basic block and return null */
    virtual void remove(Function*, BasicBlock*);                /**< Remove basic block from function */
    virtual rose_addr_t address(BasicBlock*) const;             /**< Return starting address of basic block */
    virtual void truncate(BasicBlock*, rose_addr_t);            /**< Remove instructions from end of basic block */
    virtual void discover_first_block(Function*);               /* Adds first basic block to empty function to start discovery. */
    virtual void discover_blocks(Function*, rose_addr_t);       /* Recursively discovers blocks of a function. */
    virtual void pre_cfg(SgAsmInterpretation *interp=NULL);     /**< Detects functions before analyzing the CFG */
    virtual void analyze_cfg();                                 /**< Detect functions by analyzing the CFG */
    virtual void post_cfg(SgAsmInterpretation *interp=NULL);    /**< Detects functions after analyzing the CFG */
    virtual SgAsmFunctionDeclaration* build_ast(Function*);     /**< Build AST for a single function */
    virtual SgAsmBlock* build_ast(BasicBlock*);                 /**< Build AST for a single basic block */
    virtual bool pops_return_address(rose_addr_t);              /**< Determines if a block pops the stack w/o returning */
    virtual void update_analyses(BasicBlock*);                  /* Makes sure cached analysis results are current. */
    virtual rose_addr_t canonic_block(rose_addr_t);             /**< Follow alias links in basic blocks. */
    virtual bool is_function_call(BasicBlock*, rose_addr_t*);   /* True if basic block appears to call a function. */

    virtual void mark_ipd_configuration();                      /**< Seeds partitioner with IPD configuration information */
    virtual void mark_entry_targets(SgAsmGenericHeader*);       /**< Seeds functions for program entry points */
    virtual void mark_eh_frames(SgAsmGenericHeader*);           /**< Seeds functions for error handling frames */
    virtual void mark_elf_plt_entries(SgAsmGenericHeader*);     /**< Seeds functions that are dynamically linked via .plt */
    virtual void mark_func_symbols(SgAsmGenericHeader*);        /**< Seeds functions that correspond to function symbols */
    virtual void mark_func_patterns();                          /* Seeds functions according to instruction patterns */
    virtual void name_plt_entries(SgAsmGenericHeader*);         /**< Assign names to ELF PLT functions */
    virtual void create_nop_padding();                          /**< Creates functions to hold NOP padding */
    virtual void create_zero_padding();                         /* Creates functions to hold zero padding */

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
    void progress(FILE*, const char *fmt, ...) const;

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
     *************************************************************************************************************************/
protected:
    Disassembler *disassembler;                         /**< Optional disassembler to call when an instruction is needed. */
    Disassembler::InstructionMap insns;                 /**< Instruction cache, filled in by user or populated by disassembler. */
    MemoryMap *map;                                     /**< Memory map used for disassembly if disassembler is present. */
    Disassembler::BadMap bad_insns;                     /**< Captured disassembler exceptions. */

    BasicBlocks blocks;                                 /**< All known basic blocks. */
    std::map<rose_addr_t, BasicBlock*> insn2block;      /**< Map from insns address to basic block. */
    Functions functions;                                /**< All known functions, pending and complete. */

    unsigned func_heuristics;                           /**< Bit mask of SgAsmFunctionDeclaration::FunctionReason bits. */
    std::vector<FunctionDetector> user_detectors;       /**< List of user-defined function detection methods. */

    FILE *debug;                                        /**< Stream where diagnistics are sent (or null). */
    bool allow_discont_blocks;                          /**< Allow basic blocks to be discontiguous in virtual memory. */
    BlockConfigMap block_config;                        /**< IPD configuration info for basic blocks. */

    static time_t progress_interval;                    /**< Minimum interval between progress reports. */
    static time_t progress_time;                        /**< Time of last report, or zero if no report has been generated. */
    static FILE *progress_file;                         /**< File to which reports are made. Null disables reporting. */

private:
    static const rose_addr_t NO_TARGET = (rose_addr_t)-1;
};

#endif
