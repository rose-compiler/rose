#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <sageContainer.h>

#ifdef ROSE_IMPL
#include <SgAsmInstruction.h>
#endif

/** Represents a synthesized function.
 *
 *  A function is a collection of blocks holding instructions (basic blocks) or static data.  Instructions might have
 *  references to addresses or data which are described by symbols (not to be confused with the binary's symbol table) in
 *  the function's symbol table (@ref symbolTable property).  Functions do not explicitly exist in a binary, but are
 *  synthesized by ROSE as part of the disassembly and partitioning steps. The individual instructions and/or individual
 *  static data areas need not cover a contiguous region of the address space.  Some synthesized functions will likely not
 *  be a "function" or "produceure" in the strict sense of those words due to such factors as compiler optimizations,
 *  hand-coded routines, exception handling, non-local branching, shortcomings of ROSE's partitioning solvers, etc. In any
 *  case, each function will have one primary entry address.  Although the AST requires that every function have its own
 *  basic block children, which have their own instructions (definition of "tree" data structures), logically two functions
 *  might share basic blocks, although this is uncommon.
 *
 *  Warning: Although currently basic blocks are direct children of function nodes in the AST, this may change in a future
 *  version of ROSE as new node types are introduced to describe scopes and control structures. */
class SgAsmFunction: public SgAsmSynthesizedDeclaration {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Whether a function returns. */
    enum MayReturn {
            RET_UNKNOWN,                    /**< It is unknown whether this function ever returns or not. */
            RET_NEVER,                      /**< This function is known to never return. */
            RET_SOMETIMES,                  /**< This function may return or not, depending on how it is called. */
            RET_ALWAYS                      /**< This function returns each time it is called. */
    };

    /** Reasons why an instruction might be considered the beginning of a function. These bit flags are also used by
     *  the instruction partitioner (Partitioner class) to determine what heuristics are used when partitioning
     *  instructions into functions. */
    enum FunctionReason {
            // NOTE: If you add more here, then fix Partitioner::parse_switches()
            //       Also fix SgAsmFunction::reason_key()
            FUNC_NONE        = 0x00000000,  /**< Used for initialization; not a bit flag. */
            FUNC_THUNK_TARGET= 0x00004000,  /**< Function is the target of a thunk. */
            FUNC_EXCEPTION_HANDLER
                             = 0x00008000,  /**< Function for handling an exception. */
            FUNC_ENTRY_POINT = 0x00010000,  /**< An entry point specified in the file header. */
            FUNC_CALL_TARGET = 0x00020000,  /**< Target of a function call instruction sequence in the CFG. When used as
                                             *   a partitioning heuristic, the partitioner will create new functions when
                                             *   it discovers a call-like sequence while traversing the CFG. */
            FUNC_CALL_INSN   = 0x00040000,  /**< Target of call, possibly not in the CFG (see Partitioner::mark_call_insns).
                                             *   When used as a partitioning heuristic, the partitioner will search all
                                             *   available instructions for instances of call-like instructions and define
                                             *   a function for each target. The function entry points added in this way
                                             *   become initial nodes of the CFG which is used by some of
                                             *   the other function discovery methods, including FUNC_CALL_TARGET. */
            FUNC_EH_FRAME    = 0x00080000,  /**< Address mentioned in the ELF .eh_frame section. */
            FUNC_SYMBOL      = 0x00100000,  /**< Address of a function symbol in a symbol table. */
            FUNC_PATTERN     = 0x00200000,  /**< Appears to be a function based on pattern of instructions. When used as
                                             *   a partitioning heuristic, the partitioner will search through all available
                                             *   instructions and create function entry points. The function entry points are
                                             *   added to the CFG which is used by some of the other function discovery
                                             *   methods. */
            FUNC_GRAPH       = 0x00400000,  /**< Implied by inter-basicblock branching. When used as a partitioning
                                             *   heuristic, the partitioner creates a new function when it discovers, by
                                             *   CFG traversal, that two different functions branch to a common basic block.
                                             *   The block in common becomes a new function under this rule. */
            FUNC_USERDEF     = 0x00800000,  /**< User-defined algorithm. See Partitioner::addFunctionDetector(). */
            FUNC_PADDING     = 0x01000000,  /**< Created to represent NOP padding between other functions. When used as a
                                             *   partitioning heuristic, the partitioner searches for padding after all
                                             *   CFG-based analysis has completed.  Padding can consist of either NOP
                                             *   instructions or zero bytes. The former requires that instructions at the
                                             *   interfunction addresses have been disassembled. */
            FUNC_DISCONT     = 0x02000000,  /**< Blocks of function are not contiguous in memory. This is not a partitioner
                                             *   heuristic, but rather only an indication of whether the function's basic
                                             *   blocks occupy contiguous memory locations. */
            FUNC_INSNHEAD    = 0x04000000,  /**< Initial instructions not in any other function. (UNUSED?) */
            FUNC_IMPORT      = 0x08000000,  /**< Functions dynamically linked. For ELF containers, these are the entries
                                             *   in the procedure lookup table (PLT). When used as a partitioning heuristic,
                                             *   the partitioner will scan the PLT and define a function for each entry. */
            FUNC_LEFTOVERS   = 0x10000000,  /**< Generated function to hold blocks that are otherwise not part of
                                             *   any function.  If this bit is turned off then the instruction
                                             *   Partitioner will delete instructions that it couldn't assign to
                                             *   a function. */
            FUNC_INTRABLOCK  = 0x20000000,  /**< Function contains basic blocks that were inserted by searching the
                                             *   address space between the blocks discovered by analyzing the control flow.
                                             *   Blocks added by this algorithm do not take control flow into account, and
                                             *   therefore, the global control flow graph (CFG) may have edges into the
                                             *   middle of such blocks, or such blocks may have edges into the middle of
                                             *   other blocks, including inter-function edges.  Also, blocks added by this
                                             *   method might not represent true instructions, but rather data that was
                                             *   disassembled as instructions. */
            FUNC_THUNK       = 0x40000000,  /**< Function is a thunk.  Thunks are small pieces of code whose only purpose
                                             *   is to branch to another function.  Depending on how the disassembler and
                                             *   partitioner are run, a thunk can exist either as its own function or the
                                             *   thunk's instructions will be subsumed by the called function.  The
                                             *   partitioner only sets this flag for functions that are created due to
                                             *   the thunk recognition pass; other functions that don't have this bit set
                                             *   might also be thunks, and functions that have this bit set might not
                                             *   be a thunk.  The Partitioner::is_thunk() method will return true for
                                             *   functions whose content looks like a thunk. */
            FUNC_EXPORT      = 0x80000000,  /**< Exported function. These are the functions that appear in a PE export
                                             *   table. */
            FUNC_DEFAULT     = 0xefff80ff,  /**< Default value for Partitioner class. */

            /*========= Miscellaneous Reasons ===========================================================================
             * The first half of the range (1-127, inclusive) is used for algorithms defined by ROSE.  The second half is
             * availalble for users to use as they see fit. */
            FUNC_MISCMASK    = 0x000000ff,  /**< Miscellaneous.  These are all the other special purpose function detectors
                                             *   that are implemented in the Partitioner. Each one is identified by an
                                             *   eight-bit integer stored in the low order bits.  Only one such reason can
                                             *   be stored at a time.  These are not used to control which partitioning
                                             *   heuristics to use, but rather to indicate which one (of possibly many)
                                             *   that detected the function. */
            FUNC_INTERPADFUNC = 1,          /**< Detected by Partitioner::FindInterPadFunctions, which looks for unassigned
                                             *   space between two inter-function padding blocks and makes the first such
                                             *   address the beginning of one of these functions. */
            FUNC_PESCRAMBLER_DISPATCH = 2, /**<  Dispatcher function for code generated by pescrambler. */
            FUNC_CONFIGURED = 3,           /**<  Function is defined in a configuration file. */
            FUNC_CMDLINE = 4,              /**<  Function mentioned on the command-line. */
            FUNC_SCAN_RO_DATA = 5,         /**<  Address was found in read-only data area by scanning the data. */
            FUNC_INSN_RO_DATA = 6,         /**<  Address was found in read-only data referenced by an existing
                                            *    instruction. */
    };

    /** Constants for the "function_kind" property. */
    enum function_kind_enum {
        e_unknown  = 0,
        e_standard = 1,
        e_library  = 2,
        e_imported = 3,
        e_thunk     = 4,
        e_last
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Name.
     *
     *  The optional string name of a function. */
    [[using Rosebud: rosetta, ctor_arg]]
    std::string name;

    /** Property: Reason that function exists.
     *
     *  This is a bit vector of enum constants that describe why this function was created. See @ref
     *  SgAsmFunction::FunctionReason for details. */
    [[using Rosebud: rosetta, ctor_arg]]
    unsigned reason = SgAsmFunction::FUNC_NONE;

    /** Property: Additional function existance reason comment.
     *
     *  This reason comment gets filled in automatically by certain function analyses. It's a free-form string that
     *  contains additional information about why this function exists and is used in conjunction with the @ref reason
     *  property. */
    [[using Rosebud: rosetta]]
    std::string reasonComment;

    /** Property: Kind of function.
     *
     *  This enum constant describes the kind of function. See @ref SgAsmFunction::function_kind_enum for details. */
    [[using Rosebud: rosetta, ctor_arg]]
    SgAsmFunction::function_kind_enum functionKind = SgAsmFunction::e_unknown;

    /** Property: Whether a function could return to its caller. */
    [[using Rosebud: rosetta]]
    SgAsmFunction::MayReturn mayReturn = SgAsmFunction::RET_UNKNOWN;

    /** Property: Hash of the function.
     *
     *  This is unused by ROSE, but can be set by users to identify a function by hash string. */
    [[using Rosebud: rosetta]]
    std::string nameMd5;

    /** Property: Statements that make up a function.
     *
     *  The "statements" of a function are things like instructions, static data blocks, etc. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmStatementPtrList statementList;

    // FIXME[Robb P Matzke 2017-02-13]: unused?
    [[using Rosebud: rosetta]]
    SgAsmStatementPtrList dest;

    /** Property: Primary entry address.
     *
     *  Every function has one primary entry address that uniquely identifies the function in the AST. This is the starting
     *  address of the function's entry instruction.  The abbreviation "va" means "virtual address". */
    [[using Rosebud: rosetta]]
    rose_addr_t entryVa = 0;

    /** Property: Symbol table.
     *
     *  A ROSE symbol table associated with this function. This is not the same as the symbol table that appears in the
     *  binary specimen, such as ELF or PE symbol tables. */
    [[using Rosebud: rosetta, serialize()]]
    SgSymbolTable* symbolTable = nullptr;

    // FIXME[Robb P Matzke 2017-02-13]: what is this?
    [[using Rosebud: rosetta]]
    size_t cachedVertex = (size_t)(-1);

    /** Property: Net effect of function on the stack pointer.
     *
     *  Net effect that this function has on the machine's stack pointer register.  For most x86 Linux ELF specimens the
     *  net effect is to pop the return address from the stack, thus +4 for 32-bit specimens and +8 for 64-bit specimens.
     *
     *  If the stack delta analysis has not run or could not determine a constant stack delta, then the special value @c
     *  SgAsmInstruction::INVALID_STACK_DELTA is used. */
    [[using Rosebud: rosetta]]
    int64_t stackDelta = SgAsmInstruction::INVALID_STACK_DELTA;

    /** Property: Primary calling convention.
     *
     *  This is the name of the primary calling convention for this function. When the semantics of the function match
     *  multiple calling convention definitions, this property holds the "best" one for some definition of "best". It is
     *  also possible for the semantics to not match any calling convention definition in which case this property is
     *  empty. It is also empty if the calling convention analysis was not performed. */
    [[using Rosebud: rosetta]]
    std::string callingConvention;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Adds statement to end of statement list. */
    void appendStatement(SgAsmStatement*);

    /** Erases statement from statement list.
     *
     *  If the specified statement is found in the list of statements then it is erased without being deleted. */
    void removeStatement(SgAsmStatement* statement);

    /** Function entry basic block.
     *
     *  Returns the basic block that represents the function primary entry point. Returns null for a function
     *  that contains no instructions. */
    SgAsmBlock* get_entryBlock() const;

    /** Multi-line description of function reason keys from unparser.
     *
     *  Returns a string that describes what the one-letter function reasons mean in the unparser output. */
    static std::string reasonKey(const std::string &prefix = "");

    /** Returns a very short string describing the reason mask. */
    std::string reasonString(bool pad) const;

    /** Class method that converts a reason bit vector to a human-friendly string.
     *
     *  The second argument is the bit vector of SgAsmFunction::FunctionReason bits. */
    static std::string reasonString(bool pad, unsigned reason);

    /** Selection functor for SgAsmFunction::get_extent(). */
    class NodeSelector {
    public:
            virtual ~NodeSelector() {}
            virtual bool operator()(SgNode*) = 0;
    };

    /** Returns information about the function addresses.
     *
     *  Every non-empty function has a minimum (inclusive) and maximum (exclusive) address which are returned by reference,
     *  but not all functions own all the bytes within that range of addresses. Therefore, the exact bytes are returned by
     *  adding them to the optional ExtentMap argument.  This function returns the number of nodes (instructions and static
     *  data items) in the function.  If the function contains no nodes then @p extents is not modified and the low and
     *  high addresses are both set to zero.
     *
     *  If an @p selector functor is provided, then only nodes for which it returns true are considered part of the
     *  function.  This can be used for such things as filtering out data blocks that are marked as padding.  For example:
     *
     *  @code
     *  class NotPadding: public SgAsmFunction::NodeSelector {
     *  public:
     *      virtual bool operator()(SgNode *node) {
     *          SgAsmStaticData *data = isSgAsmStaticData(node);
     *          SgAsmBlock *block = SageInterface::getEnclosingNode<SgAsmBlock>(data);
     *          return !data || !block || block->get_reason()!=SgAsmBlock::BLK_PADDING;
     *      }
     *  } notPadding;
     *
     *  Rose::BinaryAnalysis::AddressIntervalSet extents;
     *  function->get_extent(&extents, NULL, NULL, &notPadding);
     *  @endcode
     *
     *  Here's another example that calculates the extent of only the padding data, based on the negation of the filter in
     *  the previous example:
     *
     *  @code
     *  class OnlyPadding: public NotPadding {
     *  public:
     *      virtual bool operator()(SgNode *node) {
     *          return !NotPadding::operator()(node);
     *      }
     *  } onlyPadding;
     *
     *  Rose::BinaryAnalysis::AddressIntervalSet extents;
     *  function->get_extent(&extents, NULL, NULL, &onlyPadding);
     *  @endcode */
    size_t get_extent(Rose::BinaryAnalysis::AddressIntervalSet *emap=NULL, rose_addr_t *lo_addr=NULL, rose_addr_t *hi_addr=NULL,
                      NodeSelector *selector=NULL);

    /** Computes the SHA1 message digest for the bytes of a function.
     *
     *  Returns true if the SHA1 is available, false if the message digest cannot be computed because the prerequisite
     *  gcrypt functions are not available. The optional @p selector argument can be used to limit the digest to only
     *  certain nodes of the function; by default, all instructions and static data are accumulated. */
    bool get_sha1(uint8_t digest[20]/*out*/, NodeSelector *selector=NULL);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    function_kind_enum get_function_kind() const ROSE_DEPRECATED("use get_functionKind");
    void set_function_kind(function_kind_enum) ROSE_DEPRECATED("use set_functionKind");
    MayReturn get_may_return() const ROSE_DEPRECATED("use get_mayReturn");
    void set_may_return(MayReturn) ROSE_DEPRECATED("use set_mayReturn");
    const std::string& get_name_md5() const ROSE_DEPRECATED("use get_nameMd5");
    void set_name_md5(const std::string&) ROSE_DEPRECATED("use set_nameMd5");
    rose_addr_t get_entry_va() const ROSE_DEPRECATED("use get_entryVa");
    void set_entry_va(rose_addr_t) ROSE_DEPRECATED("use set_entryVa");
    SgSymbolTable* get_symbol_table() const ROSE_DEPRECATED("use get_symbolTable");
    void set_symbol_table(SgSymbolTable*) ROSE_DEPRECATED("use set_symbolTable");
    size_t get_cached_vertex() const ROSE_DEPRECATED("use get_cachedVertex");
    void set_cached_vertex(size_t) ROSE_DEPRECATED("use set_cachedVertex");
    void append_statement(SgAsmStatement*) ROSE_DEPRECATED("use appendStatement");
    void remove_statement(SgAsmStatement* statement) ROSE_DEPRECATED("use removeStatement");
    SgAsmBlock* get_entry_block() const ROSE_DEPRECATED("use get_entryBlock");
    static std::string reason_key(const std::string &prefix="") ROSE_DEPRECATED("use reasonKey");
    std::string reason_str(bool) const ROSE_DEPRECATED("use reasonString");
    static std::string reason_str(bool, unsigned) ROSE_DEPRECATED("use reasonString");
};
