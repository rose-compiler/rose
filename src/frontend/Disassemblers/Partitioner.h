#ifndef ROSE_DISASSEMBLER_PARTITIONER_H
#define ROSE_DISASSEMBLER_PARTITIONER_H
#include "Disassembler.h"
/** Partitions instructions into basic blocks and functions.
 *
 *  The Partitioner classes are responsible for partitioning the set of instructions disassembled by a Disassembler class into
 *  AST nodes for basic blocks (SgAsmBlock) and functions (SgAsmFunctionDeclaration).  A basic block is a contiguous sequence
 *  of nonoverlapping instructions where control flow enters at only the first instruction and exits at only the last
 *  instruction. A function is a collection of basic blocks with a single entry point. In the final ROSE AST, each instruction
 *  belongs to exactly one basic block, and each basic block belongs to exactly one function.
 *
 *  The partitioner organizes instructions into basic blocks by starting at a particular instruction and then looking at its
 *  set of successor addresses from SgAsmInstruction::get_successors().  If the following address is the only successor then
 *  the instruction at that address is added to the basic block and the process repeats.
 *
 *  The partitioner organizes basic blocks into functions in two phases. The first phase considers all disassembled
 *  instructions and other available information such as symbol tables. The heuristics used to find function entry points are
 *  controlled by setting or clearing various SgAsmFunctionDeclaration::FunctionReason bits with the set_search() method.
 *  Although this phase is part of the main partition() method, it can also be invoked explicitly by calling seed_functions().
 *
 *  The second phase uses a control flow graph (CFG) to create basic blocks and assign them to functions. While analyzing the
 *  CFG additional function entry points might be recognized.  Some aspects of the second phase are also controlled by
 *  set_search(), such as recognition of x86 "CALL" targets as function entry points.  Although this phase is part of the
 *  main partition() method, it can also be invoked explicitly by calling analyze_cfg().
 *
 *  Once all functions are detected and basic blocks are created and assigned to the functions, the AST can be created by
 *  calling build_ast().
 *
 *  The tests/roseTests/binaryTests/disassembleBuffer.C file is an example of how a Partitioner can be specialized to generate
 *  an AST from an Disassembler::InstructionMap that was produced from a buffer containing the raw machine instructions for a
 *  single function.
 */
class Partitioner {
protected:
    struct Function;

    /** Represents a basic block within the Partitioner. Each basic block will become an SgAsmNode in the AST. */
    struct BasicBlock {
        BasicBlock(): sucs_complete(false), sucs_ninsns(0), function(NULL) {}
        bool is_function_call(rose_addr_t*);    /**< True if basic block appears to call a function */
        SgAsmInstruction* last_insn() const;    /**< Returns the last executed (exit) instruction of the block */
        std::vector<SgAsmInstruction*> insns;   /**< Non-empty set of instructions composing this basic block, in address order */
        Disassembler::AddressSet sucs;          /**< Cached set of known successors */
        bool sucs_complete;                     /**< Is the set of successors known completely? */
        size_t sucs_ninsns;                     /**< Number of instructions in block when "sucs" was computed */
        Function* function;                     /**< Function to which this basic block is assigned, or null */
    };
    typedef std::map<rose_addr_t, BasicBlock*> BasicBlocks;

    /** Represents a function within the Partitioner. Each non-empty function will become an SgAsmFunctionDeclaration in the
     *  AST. */
    struct Function {
        Function(rose_addr_t entry_va): reason(0), pending(true), entry_va(entry_va) {}
        Function(rose_addr_t entry_va, unsigned r): reason(r), pending(true), entry_va(entry_va) {}
        Function(rose_addr_t entry_va, unsigned r, const std::string& name)
            : reason(r), name(name), pending(true), entry_va(entry_va) {}
        void clear_blocks();                    /**< Remove all blocks from this function */
        BasicBlock* last_block() const;         /**< Return pointer to block with highest address */
        unsigned reason;                        /**< SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;                       /**< Name of function if known */
        BasicBlocks blocks;                     /**< Basic blocks belonging to this function */
        bool pending;                           /**< True if we need to (re)discover the basic blocks */
        rose_addr_t entry_va;                   /**< Entry virtual address */
    };
    typedef std::map<rose_addr_t, Function*> Functions;

    /** Data type for user-defined function detectors. */
    typedef void (*FunctionDetector)(Partitioner*, SgAsmGenericHeader*, const Disassembler::InstructionMap&);

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


public:
    Partitioner(): func_heuristics(SgAsmFunctionDeclaration::FUNC_DEFAULT), debug(NULL) {}
    virtual ~Partitioner() {}

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

    /** Sends diagnostics to the specified output stream. Null (the default) turns off debugging. */
    void set_debug(FILE *f) {
        debug = f;
    }

    /** Returns the file currently used for debugging; null implies no debugging. */
    FILE *get_debug() const {
        return debug;
    }

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
     *  analyze_cfg() call. */
    virtual Function* add_function(rose_addr_t entry_va, unsigned reasons, std::string name="");

    /** Builds the AST describing all the functions. The return value is an SgAsmBlock node that points to a list of
     *  SgAsmFunctionDeclaration nodes (the functions), each of which points to a list of SgAsmBlock nodes (the basic
     *  blocks). Any basic blocks that were not assigned to a function by the Partitioner will be added to a function named
     *  "***uncategorized blocks***" whose entry address will be the address of the lowest instruction.  However, if the
     *  FUNC_LEFTOVERS bit is not turned on (see set_search()) then uncategorized blocks will not appear in the AST. */
    virtual SgAsmBlock* build_ast();
    
protected:
    virtual void append(BasicBlock*, SgAsmInstruction*);        /**< Add instruction to basic block */
    virtual BasicBlock* find_bb_containing(rose_addr_t);        /**< Find basic block containing instruction address */
    virtual BasicBlock* find_bb_containing(SgAsmInstruction* insn) {return find_bb_containing(insn->get_address());}
    virtual const Disassembler::AddressSet& successors(BasicBlock*, bool *complete=NULL); /**< Calculates known successors */
    virtual void append(Function*, BasicBlock*);                /**< Append basic block to function */
    virtual BasicBlock* discard(BasicBlock*);                   /**< Delete a basic block and return null */
    virtual void remove(Function*, BasicBlock*);                /**< Remove basic block from function */
    virtual rose_addr_t address(BasicBlock*) const;             /**< Return starting address of basic block */
    virtual void truncate(BasicBlock*, rose_addr_t);            /**< Remove instructions from end of basic block */
    virtual void discover_blocks(Function*, rose_addr_t);       /* see implementation */
    virtual void pre_cfg(SgAsmInterpretation*);                 /**< Detects functions before analyzing the CFG */
    virtual void analyze_cfg();                                 /**< Detect functions by analyzing the CFG */
    virtual void post_cfg(SgAsmInterpretation*);                /**< Detects functions after analyzing the CFG */
    virtual SgAsmFunctionDeclaration* build_ast(Function*) const;/**< Build AST for a single function */
    virtual SgAsmBlock* build_ast(BasicBlock*) const;           /**< Build AST for a single basic block */
    virtual bool pops_return_address(rose_addr_t);              /**< Determines if a block pops the stack w/o returning */
    
    
    virtual void mark_entry_targets(SgAsmGenericHeader*);       /**< Seeds functions for program entry points */
    virtual void mark_eh_frames(SgAsmGenericHeader*);           /**< Seeds functions for error handling frames */
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

protected:
    Disassembler::InstructionMap insns;                 /**< Set of all instructions to partition. */
    std::map<rose_addr_t, BasicBlock*> insn2block;      /**< Map from insns address to basic block */
    Functions functions;                                /**< All known functions, pending and complete */
    unsigned func_heuristics;                           /**< Bit mask of SgAsmFunctionDeclaration::FunctionReason bits */
    std::vector<FunctionDetector> user_detectors;       /**< List of user-defined function detection methods */
    FILE *debug;                                        /**< Stream where diagnistics are sent (or null) */

private:
    static const rose_addr_t NO_TARGET = (rose_addr_t)-1;
};

#endif
