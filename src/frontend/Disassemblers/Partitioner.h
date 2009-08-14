#ifndef ROSE_DISASSEMBLER_PARTITIONER_H
#define ROSE_DISASSEMBLER_PARTITIONER_H

/** Partitions instructions into basic blocks, functions, etc.
 *
 *  When grouping instructions into basic blocks, the partitioner looks at the instruction type and known successor addresses.
 *  A known successor address is a virtual address where the processor will disassemble and execute the next instruction.
 *  Unconditional branch instructions typically have a single known successor (the branch target); conditional branches
 *  usually have two successors (the following, or fall-through, address and the branch target); data processing and testing
 *  instructions have one successor (the following address); and interrupt-causing instructions have no known successors. A
 *  branch to a calculated (non-immediate) address does not qualify as a known successor. The SgAsmInstruction's
 *  terminatesBasicBlock virtual method is used to make this determination.
 *
 *  Once instructions are assigned to basic blocks, the partitioner assigns the basic blocks to functions using a variety of
 *  heuristics, the set of which is determined by the values specified in the Partioner's {\tt set\_heuristics} method. These
 *  are documented in the SgAsmFunctionDeclaration class (see the FunctionReason enumeration).  When a function is created,
 *  its {\tt reason} attribute will contain a bit vector describing which heuristics detected this function. */
class Partitioner {
    /*======================================================================================================================
     * Data types
     *======================================================================================================================*/
public:

    /* Map of basic block starting addresses. The key is the RVA of the first instruction in the basic block; the value is the
     * set of all addresses of instructions known to branch to this basic block (i.e., set of all known callers). */
    typedef std::map<rose_addr_t, std::set<rose_addr_t> > BasicBlockStarts;

    /* Map of function starting addresses. The key is the RVA of the first instruction in the function; the value consists of
     * a bit flag indicating why we think this is the beginning of a function, and a name (if known) of the function. */
    struct FunctionStart {
        FunctionStart()
            : reason(SgAsmFunctionDeclaration::FUNC_NONE)
            {}
        FunctionStart(SgAsmFunctionDeclaration::FunctionReason reason, std::string name)
            : reason(reason), name(name)
            {}
        unsigned reason;                        /* SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;
    };
    typedef std::map<rose_addr_t, FunctionStart> FunctionStarts;


    
    /*======================================================================================================================
     * Constructors
     *======================================================================================================================*/
public:
    Partitioner()
        : p_func_heuristics(SgAsmFunctionDeclaration::FUNC_DEFAULT)
        {}

    Partitioner(SgAsmFunctionDeclaration::FunctionReason func_heuristics)
        : p_func_heuristics(func_heuristics)
        {}



    /*======================================================================================================================
     * High-level functions.
     *======================================================================================================================*/
public:
    /** Convenience function that detects all the basic block and function starts and then calls buildTree(). */
    SgAsmBlock *partition(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns) const;

    /** Find the beginnings of basic blocks based on instruction type and call targets. */
    BasicBlockStarts detectBasicBlocks(const Disassembler::InstructionMap &insns) const;

    /** Find the beginnings of functions based on a variety of methods.  Since the first instruction of each function is also
     *  a basic block, the basic block starts will also be amended. */
    FunctionStarts detectFunctions(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                                   BasicBlockStarts &bb_starts/*out*/) const;

    /** Organize instructions into a single SgAsmBlock containing a list of functions (SgAsmFunctionDeclaration), which in turn
     *  each contain a list of basic blocks (SgAsmBlock), which each contain instructions (SgAsmInstruction).  The top-level
     *  block is returned. */
    SgAsmBlock *buildTree(const Disassembler::InstructionMap &insns, const BasicBlockStarts&, const FunctionStarts&) const;

    /** Sets the set of heuristics used by the partitioner.  The @p heuristics should be a bit mask containing the
     *  SgAsmFunctionDeclaration::FunctionReason bits. These same bits are assigned to the "reason" property of the resulting
     *  function nodes in the AST, depending on which heuristic detected the function. */
    void set_heuristics(unsigned heuristics) {
        p_func_heuristics = heuristics;
    }

    /** Returns a bit mask of SgAsmFunctionDeclaration::FunctionReason bits indicating which heuristics would be used by the
     *  partitioner.  */
    unsigned get_heuristics() const {
        return p_func_heuristics;
    }


    
    /*======================================================================================================================
     * Methods for finding the beginnings of functions.
     *======================================================================================================================*/
private:
    /** Marks program entry addresses (stored in the SgAsmGenericHeader) as functions. */
    void mark_entry_targets(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                            FunctionStarts &func_starts/*out*/) const;

    /** Marks CALL targets as functions. Not all functions are called this way, but we assume that all appearances of a CALL-like
     *  instruction with a constant target which corresponds to the known address of an instruction causes that target to become
     *  the entry point of a function.
     *  
     *  Note: any CALL whose target is the next instruction is ignored since this usually just happens for ip-relative calls that
     *        need a reloc applied, and thus have a zero offset. Such calls are common in ELF object files contained in library
     *        archives (lib*.a). */
    void mark_call_targets(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                           FunctionStarts &func_starts/*out*/) const;

    /** Use the Frame Descriptor Entry Records of the ELF .eh_frame section to mark functions. */
    void mark_eh_frames(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                        FunctionStarts &func_starts/*out*/) const;

    /** Use symbol tables to determine function entry points. */
    void mark_func_symbols(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                           FunctionStarts &func_starts/*out*/) const;

    /** Use the inter-basicblock branch information to determine the length of each function, creating new functions as
     *  necessary when a suspected function is shortened.  We make the following assumptions:
     *     1. Function entry points that are already determined will remain
     *     2. The first (lowest address) basic block serves as the single entry point for a function.
     *     3. The body of the function is contiguous basic blocks.
     *     4. Each basic block that follows a function is the entry point of a subsequent function. */
    void mark_graph_edges(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                          const BasicBlockStarts &basicBlockStarts, FunctionStarts &functionStarts/*out*/) const;

    /** Match instruction patterns to locate starts of functions. Compilers sometimes use templates to generate function
     *  prologues and epilogues. */
    void mark_func_patterns(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                            FunctionStarts &func_starts/*out*/) const;

    /** Give names to the dynamic linking trampolines in the .plt section. This method must be called after function entry points
     *  are detected since all it does is give names to functions that already exist. */
    void name_plt_entries(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                          FunctionStarts &func_starts/*out*/) const;



    
    /*======================================================================================================================
     * Utilities
     *======================================================================================================================*/
private:
    /** Returns the integer value of a value expression since there's no virtual method for doing this. (FIXME) */
    static rose_addr_t value_of(SgAsmValueExpression*);

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

    /** Creates a map of basic blocks and fills them with the appropriate instructions. The instructions are reparented to be
     *  children of the returned basic blocks.  This function returns only basic blocks that have at least one instruction. */
    std::map<rose_addr_t, SgAsmBlock*> buildBasicBlocks(const Disassembler::InstructionMap &insns,
                                                        const BasicBlockStarts &bb_starts) const;


    
    /*======================================================================================================================
     * Data members
     *======================================================================================================================*/
private:
    unsigned p_func_heuristics;         /**< Bit mask of SgAsmFunctionDeclaration::FunctionReason bits. */
};


#endif
