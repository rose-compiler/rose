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
 *  heuristics, the set of which is determined by the values specified in the Partioner's set_heuristics() method. These
 *  are documented in the SgAsmFunctionDeclaration class (see the FunctionReason enumeration).  When a function is created,
 *  its reason attribute will contain a bit vector describing which heuristics detected this function.
 *
 *  Certain methods of the Disassembler class need a Partitioner. When one of those methods are called, the Disassembler
 *  object will create a default Partitioner unless one has already been explicitly created and added to the Disassembler with
 *  the Disassembler::set_partitioner() method.  ROSE has a set of built-in Disassembler objects that use default-constructed
 *  Partitioner objects, but sometimes the end user wants to influence the way functions are detected. The documentation for
 *  the Disassembler class has an example of how this can be done. */
class Partitioner {
    /*======================================================================================================================
     * Data types
     *======================================================================================================================*/
public:

    /** Map of basic block starting addresses. The key is the RVA of the first instruction in the basic block; the value is the
     *  set of all addresses of instructions known to branch to this basic block (i.e., set of all known callers). */
    typedef std::map<rose_addr_t, Disassembler::AddressSet> BasicBlockStarts;

    /** Map of function starting addresses. The key is the RVA of the first instruction in the function; the value consists of
     *  a bit flag indicating why we think this is the beginning of a function, and a name (if known) of the function. */
    struct FunctionStart {
        FunctionStart()
            : reason(SgAsmFunctionDeclaration::FUNC_NONE), part_of(0)
            {}
        FunctionStart(SgAsmFunctionDeclaration::FunctionReason reason, std::string name)
            : reason(reason), name(name), part_of(0)
            {}
        unsigned reason;                        /** SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;                       /** Name of function if known. */
        rose_addr_t part_of;                    /** If reason&FUNC_DISCONT, part_of points to main function */
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
    SgAsmBlock *partition(SgAsmInterpretation*, const Disassembler::InstructionMap &insns) const;

    /** Find the beginnings of basic blocks based on instruction type and call targets. */
    BasicBlockStarts detectBasicBlocks(const Disassembler::InstructionMap &insns) const;

    /** Find the beginnings of functions based on a variety of methods.  Since the first instruction of each function is also
     *  a basic block, the basic block starts will also be amended. */
    FunctionStarts detectFunctions(SgAsmInterpretation*, const Disassembler::InstructionMap &insns,
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
public:
    /** Data type for user-defined function detectors. */
    typedef void (*FunctionDetector)(SgAsmGenericHeader*, const Disassembler::InstructionMap&, const BasicBlockStarts&,
                                     FunctionStarts&/*out*/);

    /** Adds a user-defined function detector to this partitioner. Any number of detectors can be added and they will be run
     *  in the order they were added, after the built-in methods run.  Each user-defined detector will be called first with
     *  the SgAmGenericHeader pointing to null, then once for each file header. The user-defined methods are run only if the
     *  SgAsmFunctionDeclaration::FUNC_USERDEF is set (see set_heuristics()), which is the default.   The reason for having
     *  user-defined function detectors is that the detection of functions influences the shape of the AST and so it is easier
     *  to apply those analyses here, before the AST is built, rather than in the mid-end after the AST is built. */
    void addFunctionDetector(FunctionDetector f) {
        p_func_detectors.push_back(f);
    }
    
private:
    /** Marks program entry addresses (stored in the SgAsmGenericHeader) as functions. */
    void mark_entry_targets(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                            FunctionStarts &func_starts/*out*/) const;

    /** Marks CALL targets as functions. Not all functions are called this way, but we assume that all appearances of a CALL-like
     *  instruction with a constant target which corresponds to the known address of an instruction causes that target to become
     *  the entry point of a function.
     *  
     *  Note that any CALL whose target is the next instruction is ignored since this usually just happens for ip-relative
     *  calls that need a reloc applied, and thus have a zero offset. Such calls are common in ELF object files contained in
     *  library archives (lib*.a). This also takes care of code for loading the instruction pointer into a register:
     *
     * @code
     *        80484c3: e8 00 00 00 00 | call 80484c8
     *        80484c8: 5b             | pop ebx
     * @endcode
     */
    void mark_call_targets(const Disassembler::InstructionMap &insns, FunctionStarts &func_starts/*out*/) const;

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
    void mark_graph_edges(const Disassembler::InstructionMap &insns,
                          const BasicBlockStarts &basicBlockStarts, FunctionStarts &functionStarts/*out*/) const;

    /** Match instruction patterns to locate starts of functions. Compilers sometimes use templates to generate function
     *  prologues and epilogues. Each pattern matcher is a separate function called with the instruction map and a starting
     *  point (iterator) into that map. If the matcher detects a match at that location then it should return an iterator
     *  pointing to the function entry point, otherwise it should return the end iterator.  Returning an iterator rather than
     *  true/false allows the matcher to operate on patterns that precede a function entry point (e.g., a series of x86 NOP
     *  instructions often indicates that a function entry point follows with the next non-NOP instruction). */
    void mark_func_patterns(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                            FunctionStarts &func_starts/*out*/) const;

    /** Give names to the dynamic linking trampolines in the .plt section. This method must be called after function entry points
     *  are detected since all it does is give names to functions that already exist. */
    void name_plt_entries(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                          FunctionStarts &func_starts/*out*/) const;

    /** Splits the no-operation (usually, but not necessarily, NOP instructions) off the end of functions to create seperate
     *  functional units for them.  This is important since some kinds of analysis are sensitive to this padding. */
    void mark_func_padding(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                           FunctionStarts &func_starts/*out*/) const;

    /** Marks the beginning of a sequence of contiguous instructions as a function, taking into account that separate
     *  sequences of instructions may overlap with each other.  For instance, if the disassembler disassembled 15 multi-byte
     *  instructions beginning at address A, and 15 separate instructions beginning at address A+1 then there are two
     *  instruction sequences and each will be marked as a function.  This method should always be invoked regardless of
     *  whether the FUNC_INSNHEAD bit is set in the heuristics, because the Partitioner::buildTree() requires that every
     *  instruction belongs to a function. It does not set the FUNC_INSHEAD bit of the FunctionStart::reason member if a
     *  function start already exists at that address. */
    void mark_insn_heads(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                         FunctionStarts &func_starts/*out*/) const;

    /** A graph describing how basic blocks branch to other basic blocks.  This graph is used by mark_func_discont(). Any
     *  block which is the first block of a function is not included in the graph, nor are the edges to/from those blocks.
     *  Edges from a basic block back to the same basic block are also excluded.  The map key is the address of the node. The
     *  map's first value is a marker used during graph traversal and the second value is the set of nodes to which a node
     *  branches. */
    typedef std::map<rose_addr_t, std::pair<int, std::set<rose_addr_t> > > DiscontGraph;

    /** Looks at the branching relationships between basic blocks in an attempt to discover all the blocks of a function that
     *  has been split into discontiguous parts by the compiler. This is invoked when the
     *  SgAsmFunctionDeclaration::FUNC_DISCONT bit is set for the partitioning heuristics. */
    void mark_func_discont(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                           FunctionStarts& func_starts/*out*/) const;


    
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

    /** Update basic blocks so that every function start also starts a basic block. */
    static void update_basic_blocks(const FunctionStarts& func_starts, BasicBlockStarts& bb_starts/*out*/);

    /** Returns information about the end of a function. Given the starting address of a function, this method returns the address
     *  of the last instruction of a function and the address of the basic block to which that instruction belongs. This is not as
     *  trivial as it might seem because:
     *
     *      * Instruction sizes might be variable (e.g., x86 architectures), so we can't back up through the instructions.
     *      * Basic blocks could overlap due to variable instruction sizes. For example, two overlapping blocks could start
     *        at addresses A and A+1.  Since x86 instruction streams are "self healing" it is likely that the instructions
     *        starting at A and A+1 will converge before the end of the blocks, and the blocks will share some of their final
     *        instructions.
     *      * Functions can overlap for the same reason basic blocks overlap. Similarly, their basic blocks will most likely
     *        converge so that two overlapping functions share some of their final basic blocks.
     *
     *  Once the AST is built, finding the last instruction or its basic block is trivial.
     *
     *  The return value is a pair of addresses containing the address of the final instruction and the address of that
     *  instruction's basic block. */
    static std::pair<rose_addr_t, rose_addr_t> end_of_function(rose_addr_t addr, const Disassembler::InstructionMap&,
                                                               const BasicBlockStarts&, const FunctionStarts&);

    /** Returns the address of the last instruction of a basic block.  Basic blocks can overlap, so a naive implementation
     *  that uses std::map::upper_bound() isn't possible -- we must iterate through the instructions. */
    static rose_addr_t end_of_block(rose_addr_t addr, const Disassembler::InstructionMap&, const BasicBlockStarts&);

    /** Return the set of nodes reachable from a given node. */
    static void discont_subgraph(DiscontGraph&, DiscontGraph::iterator, Disassembler::AddressSet *result);

    /** Checks consistency of functions, basic blocks, and instructions. Every function should start with a basic block and
     *  every basic block should start with an instruction. If verbose is set then the list of function start addresses and
     *  basic block start addresses is displayed; otherwise the lists are displayed only if an inconsistency is detected. */
    static void check_consistency(const Disassembler::InstructionMap&, const BasicBlockStarts&,
                                  const FunctionStarts&, bool verbose=false);

    /** Checks final consistency of AST.  Note that this is very slow! */
    static void check_consistency(const Disassembler::InstructionMap&);

    /*======================================================================================================================
     * Data members
     *======================================================================================================================*/
private:
    unsigned p_func_heuristics;                         /**< Bit mask of SgAsmFunctionDeclaration::FunctionReason bits. */
    std::vector<FunctionDetector> p_func_detectors;     /**< List of user-defined function detection methods. */
};


#endif
