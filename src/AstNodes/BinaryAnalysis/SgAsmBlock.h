#include <Rose/BinaryAnalysis/Address.h>

#include <sageContainer.h>

#ifdef ROSE_IMPL
#include <SgAsmInstruction.h>                           // for SgAsmInstruction::INVALID_STACK_DELTA
#endif

/** Instruction basic block.
 *
 *  A SgAsmBlock usually represents a sequence of instructions. It's also used for grouping other things such as
 *  functions. A SgAsmBlock represents a basic block if and only if it has at least one descendant of type @ref
 *  SgAsmInstruction and it has no descendants of type SgAsmBlock.
 *
 *  In the absence of interrupt handling, the instructions of a basic block are executed entirely.  In the absense of
 *  multi-threading, no other instructions intervene.
 *
 *  The instructions of a basic block need not be contiguous in virtual memory. They also do not need to be at increasing
 *  virtual addresses.
 *
 *  If the basic block has a parent pointer, then the closest @ref SgAsmFunction ancestor is the one to which this basic
 *  block belongs. In the @ref Rose::BinaryAnalysis::Partitioner2::Partitioner "Partitioner" API, a basic block can be
 *  owned by more than one function.
 *
 *  An AST may have multiple SgAsmBlock objects that represent the same basic block. This happens when a single block
 *  belongs to more than one function.
 *
 *  See also, @ref Rose::BinaryAnalysis::Partitioner2::BasicBlock, which represents a basic block outside the AST. */
class SgAsmBlock: public SgAsmStatement {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Reasons why a basic block might have been assigned to a function. */
    enum Reason {
        // Please update SgAsmBlock::reasonString() if you change this enum!
        BLK_NONE        = 0x00000000,           /**< No particular reason.  Mostly just for initialization. */
        BLK_ENTRY_POINT = 0x00010000,           /**< Block is an entry point for the function. */
        BLK_PADDING     = 0x00020000,           /**< Block is used for padding. */
        BLK_FRAGMENT    = 0x00080000,           /**< This block created because it seems to belong to the function although
                                                 *   CFG traversal did not find it. */
        BLK_CFGHEAD     = 0x00100000,           /**< Block serves as an explicit starting point for CFG analysis. */
        BLK_USERDEF     = 0x00200000,           /**< User says block belongs to the function. */
        BLK_LEFTOVERS   = 0x00400000,           /**< Block is being assigned to a FUNC_LEFTOVERS function because it could
                                                 *   not be assigned to any other function. */
        BLK_JUMPTABLE   = 0x00800000,           /**< Table of code addresses used by indirect branches. */
        BLK_GRAPH1      = 0x01000000,           /**< Block was added by the main CFG analysis. */
        BLK_GRAPH2      = 0x02000000,           /**< Block was added by a second pass of CFG analysis. */
        BLK_GRAPH3      = 0x04000000,           /**< Block was added by a third pass of CFG analysis. */

        BLK_DEFAULT     = BLK_NONE,             //NO_STRINGIFY

        // ========= Miscellaneous Reasons ===========================================================================
        // The first half of the range (1-127, inclusive) is used for algorithms defined by ROSE.  The second half is
        // availalble for users to use as they see fit.
        BLK_MISCMASK    = 0x000000ff,           /**< Miscellaneous reasons go here. We can store only one such reason at
                                                 *   a time. */
        BLK_FINDDATA    = 0x00000001,           /**< Added by Partitioner::FindData, which attaches unassigned parts of the
                                                 *   disassembly address space to the preceding function. */
        BLK_POSTFUNC    = 0x00000002            /**< Added by Partitioner::FindPostFunctionInsns, which adds unassigned
                                                 *   instructions to the immediately preceding function. */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    // FIXME[Robb Matzke 2023-03-18]: is the lack of serialization a bug?
    /** Property: Identification.
     *
     *  Block unique identification number. Not used by ROSE. */
    [[using Rosebud: rosetta, serialize()]]
    Rose::BinaryAnalysis::Address id = 0;

    /** Property: Reasons this block was created.
     *
     *  This property holds a bit mask of @ref SgAsmBlock::Reason bits that indicate why this block was created. */
    [[using Rosebud: rosetta]]
    unsigned reason = SgAsmBlock::BLK_NONE;

    // statementList and successors should have been pointers to nodes that contain the list rather than being the lists
    // themselves because ROSETTA doesn't allow traversals on multiple list data members--we can traverse either one list or
    // the other, but not both.  It's too late to change how this part of the AST is structured because so much user code
    // already depends on it, therefore we can only traverse statementList and not successors. [Robb Matzke 2016-02-25]
    /** Property: Statements of which this block is composed.
     *
     *  This is the list of "statements" that belong to this block. Statements are usually instructions, but historical
     *  code may have used basic blocks with other children. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmStatementPtrList statementList;

    // statementList and successors should have been pointers to nodes that contain the list rather than being the lists
    // themselves because ROSETTA doesn't allow traversals on multiple list data members--we can traverse either one list or
    // the other, but not both.  It's too late to change how this part of the AST is structured because so much user code
    // already depends on it, therefore we can only traverse statementList and not successors. [Robb Matzke 2016-02-25]
    /** Property: Control flow successors.
     *
     *  This property holds the list of addresses which are control flow successors of this block.  The @ref
     *  Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow graph
     *  that can reference indeterminate addresses and store data in the edges, and which is copiable. */
    [[using Rosebud: rosetta, large]]
    SgAsmIntegerValuePtrList successors;

    /** Property: Whether the successors list is complete.
     *
     *  This property is true if the "successors" property holds an incomplete list of successors. Since this
     *  representation of a control flow graph is unable to represent edges that point to indeterminate addresses (e.g.,
     *  computed branches), the "successors_complete" property can be used.
     *
     *  The @ref Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
     *  graph that can reference indeterminate addresses and store data in the edges, and which is copiable. */
    [[using Rosebud: rosetta]]
    bool successorsComplete = false;

    /** Property: Holds the immediate dominator block in the control flow graph.
     *
     *  The immediate dominator is the closest block to this one (by following reverse control flow edges) through which
     *  all control paths pass in order to get from the function entry block to this block.
     *
     *  The @ref Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
     *  graph that can reference indeterminate addresses and store data in the edges, and which is copiable. */
    [[using Rosebud: rosetta]]
    SgAsmBlock* immediateDominator = nullptr;

    /** Property: Cached vertex for control flow graphs.
     *
     *  This property is used by the virtual control flow graph mechanism.
     *
     *  The @ref Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
     *  graph that can reference indeterminate addresses and store data in the edges, and which is copiable. */
    [[using Rosebud: rosetta]]
    size_t cachedVertex = (size_t)(-1); // see BinaryAnalysis::ControlFlow

    /** Property: Likelihood that this block represents real instructions.
     *
     *  This property holds the results of an analysis that determines how likely it is that the memory from which this
     *  basic block was disassembled represents actual instructions that would be executed when the specimen runs. */
    [[using Rosebud: rosetta]]
    double codeLikelihood = 0.0;

    /** Property: Stack pointer at block exit w.r.t. stack pointer at function entry.
     *
     *  This is the difference between the stack pointer after the last instruction of this block executes and the stack
     *  pointer when this block's function was entered.  It stores the result of a stack delta analysis. If stack delta
     *  analysis hasn't run, or was unable to determine a constant delta, then the special value @ref
     *  SgAsmInstruction::INVALID_STACK_DELTA is stored. */
    [[using Rosebud: rosetta]]
    int64_t stackDeltaOut = SgAsmInstruction::INVALID_STACK_DELTA;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Add the specified statement to the end of the statement list.
     *
     *  This is is usually used to add the next instruction to the end of a basic block. */
    void appendStatement(SgAsmStatement*);

    /** Erase the specified statement.
     *
     *  If the specified statement exists in the "statementList" property then it is erased but not deleted. */
    void removeStatement(SgAsmStatement*);

    // FIXME[Robb P Matzke 2017-02-13]: wrong name -- erases only statements, not all children
    /** Removes all statements from the block.
     *
     *  This makes the block empty, and not having a unique starting virtual address. It does not erase all children, just
     *  the statement children.  None of the statements that are erased are deleted. */
    void removeChildren();

    /** Fall-through virtual address.
     *
     *  A block's fall-through address is the virtual address that follows the last byte of the block's last instruction.
     *  The block must have instructions (e.g., it cannot be a strict data block). */
    Rose::BinaryAnalysis::Address get_fallthroughVa();

    /** Returns the function that owns this block.
     *
     *  This is just a convenience wrapper around @ref SageInterface::getEnclosingNode. */
    SgAsmFunction *get_enclosingFunction() const;

    /** Determins if a block contains instructions.
     *
     *  Returns true if the block has instructions, false otherwise. We look only at the immediate descendants of this
     *  block.  See also, @ref SageInterface::querySubTree in order to get the list of all instructions or to consider all
     *  descendants. */
    bool hasInstructions() const;

    /** Determine if a block contains instructions.
     *
     *  Returns true if the block has instructions, false otherwise. We look only at the immediate descendants of this
     *  block.  See also, @ref SageInterface::querySubTree in order to get the list of all instructions or to consider all
     *  descendants. */
    bool isBasicBlock() const;

    /** Returns true if basic block appears to be a function call.
     *
     *  If the target address is known and is a single value then it is stored in the @p target_va argument, otherwise we
     *  store the maximum 64-bit address.  If the return address for the function call is known then it is stored in the @p
     *  return_va argument, otherwise @p return_va will contain the maximum 64-bit address. The return address is usually
     *  the fall-through address of the basic block.
     *
     * Note: Use this function in preference to SgAsmInstruction::isFunctionCallSlow() because the latter is intended to be
     * used by the Partitioner before an AST is created and might not be as accurate. */
    bool isFunctionCall(Rose::BinaryAnalysis::Address &target_va/*out*/, Rose::BinaryAnalysis::Address &return_va/*out*/);

    /** Multi-line string describing the letters used for basic block reasons.
     *
     *  The letters are returned by the padding version of @ref reasonString and appear in unparser output. */
    static std::string reasonKey(const std::string &prefix = "");

    /** Returns reason string for this block.
     *
     *  The reason string is a very short string describing the reason that the block was created. */
    std::string reasonString(bool pad) const;

    /** Converts a reason bit vector to a human-friendly string.
     *
     *  The second argument is the bit vector of @ref SgAsmBlock::Reason bits.  Some of the positions in the padded return
     *  value are used for more than one bit.  For instance, the first character can be "L" for leftovers, "N" for padding,
     *  "E" for entry point, or "-" for none of the above. */
    static std::string reasonString(bool pad, unsigned reason);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // [Robb Matzke 2023-11-06]: deprecated 2023-11
    bool get_successors_complete() const ROSE_DEPRECATED("use get_successorsComplete");
    void set_successors_complete(bool) ROSE_DEPRECATED("use get_successorsComplete");
    SgAsmBlock* get_immediate_dominator() const ROSE_DEPRECATED("use get_immediateDominator");
    void set_immediate_dominator(SgAsmBlock*) ROSE_DEPRECATED("use set_immediateDominator");
    size_t get_cached_vertex() const ROSE_DEPRECATED("use get_cachedVertex");
    void set_cached_vertex(size_t) ROSE_DEPRECATED("use set_cachedVertex");
    double get_code_likelihood() const ROSE_DEPRECATED("use get_codeLikelihood");
    void set_code_likelihood(double) ROSE_DEPRECATED("use set_codeLikelihood");
    void append_statement(SgAsmStatement*) ROSE_DEPRECATED("use appendStatement");
    void remove_statement(SgAsmStatement*) ROSE_DEPRECATED("use removeStatement");
    void remove_children() ROSE_DEPRECATED("use removeChildren");
    Rose::BinaryAnalysis::Address get_fallthrough_va() ROSE_DEPRECATED("use getFallthroughVa");
    SgAsmFunction* get_enclosing_function() const ROSE_DEPRECATED("use get_enclosingFunction");
    bool has_instructions() const ROSE_DEPRECATED("use hasInstructions");
    bool is_basic_block() const ROSE_DEPRECATED("use isBasicBlock");
    bool is_function_call(Rose::BinaryAnalysis::Address&, Rose::BinaryAnalysis::Address&) ROSE_DEPRECATED("use isFunctionCall");
    static std::string reason_key(const std::string& = "") ROSE_DEPRECATED("use reasonKey");
    std::string reason_str(bool) const ROSE_DEPRECATED("use reasonString");
    static std::string reason_str(bool, unsigned) ROSE_DEPRECATED("use reasonString");
};
