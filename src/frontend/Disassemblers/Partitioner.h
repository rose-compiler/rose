#ifndef ROSE_DISASSEMBLER_PARTITIONER_H
#define ROSE_DISASSEMBLER_PARTITIONER_H

class Partitioner {
protected:
    struct Function;
    
    struct BasicBlock {
        BasicBlock(): sucs_complete(false), sucs_ninsns(0), function(NULL) {}
        const Disassembler::AddressSet& successors(bool *complete=NULL); /**< Calculates known successors */
        bool is_function_call(rose_addr_t*);    /**< True if basic block appears to call a function */
        std::vector<SgAsmInstruction*> insns;   /**< Instructions composing this basic block, in address order */
        Disassembler::AddressSet sucs;          /**< Cached set of known successors */
        bool sucs_complete;                     /**< Is the set of successors known completely? */
        size_t sucs_ninsns;                     /**< Number of instructions in block when "sucs" was computed */
        Function* function;                     /**< Function to which this basic block is assigned */
    };
    typedef std::map<rose_addr_t, BasicBlock*> BasicBlocks;

    struct Function {
        Function(rose_addr_t entry_va): reason(0), pending(true), entry_va(entry_va) {}
        Function(rose_addr_t entry_va, unsigned r): reason(r), pending(true), entry_va(entry_va) {}
        Function(rose_addr_t entry_va, unsigned r, const std::string& name)
            : reason(r), name(name), pending(true), entry_va(entry_va) {}
        void clear_blocks();                    /**< Remove all blocks from this function */
        unsigned reason;                        /**< SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;                       /**< Name of function if known */
        BasicBlocks blocks;                     /**< Basic blocks belonging to this function */
        bool pending;                           /**< True if we need to (re)discover the basic blocks */
        rose_addr_t entry_va;                   /**< Entry virtual address */
    };
    typedef std::map<rose_addr_t, Function*> Functions;

    /** Data type for user-defined function detectors. */
    typedef void (*FunctionDetector)(Partitioner*, SgAsmGenericHeader*, const Disassembler::InstructionMap&, Functions&/*out*/);

public:
    /* FIXME: Backward compatibility stuff prior to 2010-01-01. These are deprecated and should eventually be removed. They
     *        are currently used by src/midend/binaryAnalyses/binary_analysis.C for some of the CFG and call graph functions. */

    /** Map of basic block starting addresses (DEPRECATED). The key is the virtual address of the first instruction in the
     *  basic block; the value is the set of all virtual addresses of instructions known to branch to this basic block (i.e.,
     *  set of all known callers). */
    typedef std::map<rose_addr_t, Disassembler::AddressSet> BasicBlockStarts;

    /** Find the beginnings of basic blocks based on instruction type and call targets (DEPRECATED). */
    BasicBlockStarts detectBasicBlocks(const Disassembler::InstructionMap&) const;

    /** Map of function starting addresses (DEPRECATED). The key is the virtual address of the first instruction in the
     *  function; the value consists of a bit flag indicating why we think this is the beginning of a function, and a name (if
     *  known) of the function. */
    struct FunctionStart {
        FunctionStart(unsigned reason, std::string name): reason(reason), name(name) {}
        unsigned reason;                        /** SgAsmFunctionDeclaration::FunctionReason bit flags */
        std::string name;                       /** Name of function if known. */
    };
    typedef std::map<rose_addr_t, FunctionStart> FunctionStarts;
    
    /** Returns a list of the currently defined functions (DEPRECATED). */
    FunctionStarts detectFunctions(SgAsmInterpretation*, const Disassembler::InstructionMap &insns,
                                   BasicBlockStarts &bb_starts/*out*/) const;


public:
    Partitioner(): func_heuristics(SgAsmFunctionDeclaration::FUNC_DEFAULT) {}
    virtual ~Partitioner() {}

    /** Sets the set of heuristics used by the partitioner.  The @p heuristics should be a bit mask containing the
     *  SgAsmFunctionDeclaration::FunctionReason bits. These same bits are assigned to the "reason" property of the resulting
     *  function nodes in the AST, depending on which heuristic detected the function. */
    void set_heuristics(unsigned heuristics) {
        func_heuristics = heuristics;
    }

    /** Returns a bit mask of SgAsmFunctionDeclaration::FunctionReason bits indicating which heuristics would be used by the
     *  partitioner.  */
    unsigned get_heuristics() const {
        return func_heuristics;
    }

    /** Top-level function to run the partitioner on some instructions and build an AST */
    SgAsmBlock* partition(SgAsmInterpretation*, const Disassembler::InstructionMap&);

    /** Reset partitioner to initial conditions */
    void clear() {
        insns.clear();
        insn2block.clear();
        functions.clear();
    }

    /** Adds additional instructions to be processed. */
    void add_instructions(const Disassembler::InstructionMap& insns) {
        this->insns.insert(insns.begin(), insns.end());
    }

    /** Adds a user-defined function detector to this partitioner. Any number of detectors can be added and they will be run
     *  in the order they were added, after the built-in methods run.  Each user-defined detector will be called first with
     *  the SgAmGenericHeader pointing to null, then once for each file header. The user-defined methods are run only if the
     *  SgAsmFunctionDeclaration::FUNC_USERDEF is set (see set_heuristics()), which is the default.   The reason for having
     *  user-defined function detectors is that the detection of functions influences the shape of the AST and so it is easier
     *  to apply those analyses here, before the AST is built, rather than in the mid-end after the AST is built. */
    void add_function_detector(FunctionDetector f) {
        user_detectors.push_back(f);
    }
    
    /** Adds a new function definition to the partitioner. */
    Function* add_function(rose_addr_t entry_va, unsigned reasons, std::string name="");

    /** Builds the AST describing all the functions */
    SgAsmBlock* build_ast();
    
protected:
    void append(BasicBlock*, SgAsmInstruction*);        /**< Add instruction to basic block */
    BasicBlock* find_bb_containing(rose_addr_t);        /**< Find basic block containing instruction address */
    BasicBlock* find_bb_containing(SgAsmInstruction* insn) {return find_bb_containing(insn->get_address());}
    void append(Function*, BasicBlock*);                /**< Append basic block to function */
    rose_addr_t address(BasicBlock*) const;             /**< Return starting address of basic block */
    BasicBlock* split(BasicBlock*, rose_addr_t);        /**< Split basic block in two at address */
    void discover_blocks(Function*, rose_addr_t);       /**< Add specified block to function recursively */
    void seed_functions(SgAsmInterpretation*);          /**< Find functions on basis other than CFG */
    void analyze_cfg();                                 /**< Detect functions by analyzing the CFG */
    SgAsmFunctionDeclaration* build_ast(Function*) const;/**< Build AST for a single function */
    SgAsmBlock* build_ast(BasicBlock*) const;           /**< Build AST for a single basic block */
    
    void mark_entry_targets(SgAsmGenericHeader*);       /**< Seeds functions for program entry points */
    void mark_eh_frames(SgAsmGenericHeader*);           /**< Seeds functions for error handling frames */
    void mark_func_symbols(SgAsmGenericHeader*);        /**< Seeds functions that correspond to function symbols */
    void mark_func_patterns(SgAsmGenericHeader*);       /**< Seeds functions according to instruction patterns */
    void name_plt_entries(SgAsmGenericHeader*);         /**< Assign names to ELF PLT functions */

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
    unsigned func_heuristics;                           /**< Bit mask of SgAsmFunctionDeclaration::FunctionReason bits. */
    std::vector<FunctionDetector> user_detectors;       /**< List of user-defined function detection methods. */
};

#endif
