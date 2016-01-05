#ifndef ROSE_Partitioner2_Modules_H 
#define ROSE_Partitioner2_Modules_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Utility.h>

#include <Sawyer/SharedPointer.h>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Base class for adjusting basic blocks during discovery.
 *
 *  User-defined basic block callbacks are invoked as each instruction is discovered for a basic block. See @ref
 *  Partitioner::discoverBasicBlock for details.
 *
 *  One of the important uses for basic block callbacks is to adjust the control flow successors for a basic block.  As each
 *  instruction of a basic block is discovered the partitioner calculates its control flow successors to decide what to do.
 *  The successors are calculated by evaluating the basic block instructions in a symbolic domain, and if that fails, by
 *  looking at the final instruction's concrete successors.  Once these successors are obtained, the partitioner invokes user
 *  callbacks so that the user has a chance to make adjustments. By time this callback is invoked, the basic block's initial
 *  successors have been computed and cached in the basic block. That list can be obtained by invoking @ref
 *  Partitioner::basicBlockSuccessors (or similar) or by accessing the @ref BasicBlock::successors cache directly. Likewise,
 *  the successor list can be adjusted by invoking methods in the partitioner API or by modifying the cache directly.
 *
 *  Another important use for these callbacks is to tell the partitioner when a basic block is finished.  The partitioner has a
 *  fairly long list of criteria that it uses as documented in @ref Partitioner::discoverBasicBlock.  One of these criteria is
 *  to look at the <code>args.results.termination</code> enum returned by the callbacks: if it is TERMINATE_NOW or
 *  TERMINATE_PRIOR then the block is forcibly terminated regardless of what would have otherwise happened.
 *
 *  The partitioner expects callbacks to have shared ownership and references them only via Sawyer::SharedPointer.  Therefore,
 *  subclasses should implement an @c instance class method that allocates a new object and returns a shared pointer. */
class BasicBlockCallback: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<BasicBlockCallback> Ptr;

    /** Whether to terminate a basic block. */
    enum Termination {
        CONTINUE_DISCOVERY,                             /**< Do not explicitly terminate block here. */
        TERMINATE_NOW,                                  /**< Make current instruction the final instruction of the block. */
        TERMINATE_PRIOR,                                /**< Make previous instruction the final instruction of the block. */
    };

    /** Results coordinated across all callbacks. */
    struct Results {
        Termination terminate;                          /**< Whether block should be explicitly terminated. */
        Results(): terminate(CONTINUE_DISCOVERY) {}
    };

    /** Arguments passed to the callback. */
    struct Args {
        const Partitioner &partitioner;                 /**< Partitioner requesting basic block successors. */
        BasicBlock::Ptr bblock;                         /**< Basic block whose successors are to be computed. */
        Results &results;                               /**< Results to control basic block discovery. */
        Args(const Partitioner &partitioner, const BasicBlock::Ptr &bblock, Results &results)
            : partitioner(partitioner), bblock(bblock), results(results) {}
    };

    /** Callback method.
     *
     *  This is the method invoked for the callback.  The @p chain argument is the return value from the previous callback in
     *  the list (true for the first callback).  The successor callbacks use @p chain to indicate whether subsequent callbacks
     *  should do anything. */
    virtual bool operator()(bool chain, const Args&) = 0;
};

/** Base class for matching an instruction pattern.
 *
 *  Instruction matchers are generally reference from the partitioner via shared-ownership pointers.  Subclasses must
 *  implement a @ref match method that performs the actual matching. */
class InstructionMatcher: public Sawyer::SharedObject {
public:
    /** Shared-ownership pointer. The partitioner never explicitly frees matchers. Their pointers are copied when
     *  partitioners are copied. */
    typedef Sawyer::SharedPointer<InstructionMatcher> Ptr;

    /** Attempt to match an instruction pattern.
     *
     *  If the subclass implementation is able to match instructions, bytes, etc. anchored at the @p anchor address then it
     *  should return true, otherwise false.  The anchor address will always be valid for the situation (e.g., if the
     *  partitioner is trying to match something anchored at an address that is not in the CFG, then the @p anchor will be
     *  such an address; if it is trying to match something that is definitely an instruction then the address will be
     *  mapped with execute permission; etc.).  This precondition makes writing matchers that match against a single
     *  address easier to write, but matchers that match at additional locations must explicitly check those other
     *  locations with the same conditions (FIXME[Robb P. Matzke 2014-08-04]: perhaps we should pass those conditions as an
     *  argument). */
    virtual bool match(const Partitioner&, rose_addr_t anchor) = 0;
};


/** Base class for matching function prologues.
 *
 *  A function prologue is a pattern of bytes or instructions that typically mark the beginning of a function.  For
 *  instance, many x86-based functions start with "PUSH EBX; MOV EBX, ESP" while many M68k functions begin with a single
 *  LINK instruction affecting the A6 register.  A subclass must implement the @ref match method that does the actual
 *  pattern matching.  If the @ref match method returns true, then the partitioner will call the @ref functions method to
 *  obtain the new function objects.
 *
 *  The matcher will be called only with anchor addresses that are mapped with execute permission and which are not a
 *  starting address of any instruction in the CFG.  The matcher should ensure similar conditions are met for any
 *  additional addresses. */
class FunctionPrologueMatcher: public InstructionMatcher {
public:
    /** Shared-ownership pointer. The partitioner never explicitly frees matchers. Their pointers are copied when
     *  partitioners are copied. */
    typedef Sawyer::SharedPointer<FunctionPrologueMatcher> Ptr;

    /** Returns the function(s) for the previous successful match.
     *
     *  If the previous call to @ref match returned true then this method should return at least one function for the matched
     *  function prologue.  Although the function returned by this method is often at the same address as the anchor for the
     *  match, it need not be.  For instance, a matcher could match against some amount of padding followed the instructions
     *  for setting up the stack frame, in which case it might choose to return a function that starts at the stack frame setup
     *  instructions and includes the padding as static data.
     *
     *  Multiple functions can be created. For instance, if the matcher matches a thunk then two functions will likely be
     *  created: the thunk itself, and the function to which it points.
     *
     *  The partitioner will never call @ref function without first having called @ref match. */
    virtual std::vector<Function::Ptr> functions() const = 0;
};


/** Base class for matching function padding.
 *
 *  Function padding is bytes that appear immediately prior to the entry address of a function usually in order to align the
 *  function on a suitable boundary.  Some assemblers emit zero bytes, others emit no-op instructions, and still others emit
 *  combinations of no-ops and zeros.  It's conceivable that some compiler might even emit random garbage. */
class FunctionPaddingMatcher: public Sawyer::SharedObject {
public:
    /** Shared-ownership pointer.  The partitioner never explicitly frees matches. Their pointers are copied when partitioners
     *  are copied. */
    typedef Sawyer::SharedPointer<FunctionPaddingMatcher> Ptr;

    /** Attempt to match padding.
     *
     *  Attempts to match function padding that ends at the address immediately prior to @p anchor.  If a match is successful
     *  then the return value is the starting address for the padding and must be less than @p anchor. When no match is found
     *  then @p anchor is returned. The size of the matched padding is always <code>anchor-retval</code> where @c retval is
     *  the returned value. */
    virtual rose_addr_t match(const Partitioner&, rose_addr_t anchor) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Generic modules
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Modules {

/** Convert system function names to ROSE canonical form.
 *
 *  ROSE always stores library function names as "function@library" with the reasoning that the function name is typically more
 *  important than the library name and storing them this way leads to better presentation in sorted lists.  This function
 *  recognizes and converts the form "library.dll:function" and will convert "KERNEL32.dll:EncodePointer" to
 *  "EncodePointer@KERNEL32.dll"
 *
 * @sa ModulesPe::systemFunctionName and possibly other OS-specific formatters. */
std::string canonicalFunctionName(const std::string&);

/** Follow basic block ghost edges.
 *
 *  If this callback is registered as a partitioner basic block callback then the partitioner will follow ghost edges when
 *  discovering instructions for basic blocks.  A ghost edge is a control flow edge whose target address is mentioned in the
 *  machine instruction but which is never followed.  Examples are branch instructions with opaque predicates due to the
 *  compiler not optimizing away the branch.
 *
 *  An alternative to following ghost edges as basic block instructions are discovered is to look for dead code after the
 *  function is discovered.  That way the CFG does not contain edges into the dead code, which can make things like data flow
 *  analysis faster. */
class AddGhostSuccessors: public BasicBlockCallback {
public:
    static Ptr instance() { return Ptr(new AddGhostSuccessors); }
    virtual bool operator()(bool chain, const Args &args) ROSE_OVERRIDE;
};

/** Prevent discontiguous basic blocks.
 *
 *  This basic block callback causes the basic block to terminate when it encounters an unconditional branch, in effect causing
 *  all basic blocks to have instructions that are contiguous in memory and ordered by their address.  ROSE normally does not
 *  require such strict constraints: a basic block is normally one or more distinct instructions having a single entry point
 *  and a single exit point and with control flowing linearly across all instructions. */
class PreventDiscontiguousBlocks: public BasicBlockCallback {
public:
    static Ptr instance() { return Ptr(new PreventDiscontiguousBlocks); }
    virtual bool operator()(bool chain, const Args &args) ROSE_OVERRIDE;
};

/** List some instructions at a certain time.
 *
 *  See @ref docString for full documentation. */
class InstructionLister: public CfgAdjustmentCallback {
public:
    struct Settings {
        AddressInterval where;                          // which basic block(s) we should we monitor
        Trigger::Settings when;                         // once found, how often we produce a list
        AddressInterval what;                           // what instructions to list
        Settings(): what(AddressInterval::whole()) {}
    };
private:
    Settings settings_;
    Trigger trigger_;
protected:
    explicit InstructionLister(const Settings &settings): settings_(settings), trigger_(settings.when) {}
public:
    static Ptr instance(const Settings &settings) { return Ptr(new InstructionLister(settings)); }
    static Ptr instance(const std::string &config);
    static Ptr instance(const std::vector<std::string> &args);
    static Sawyer::CommandLine::SwitchGroup switches(Settings&);
    static std::string docString();
    virtual bool operator()(bool chain, const AttachedBasicBlock &args) ROSE_OVERRIDE;
    virtual bool operator()(bool chain, const DetachedBasicBlock&) ROSE_OVERRIDE { return chain; }
};

/** Produce a GraphViz file for the CFG at a certain time.
 *
 *  See @ref docString for full documentation. */
class CfgGraphVizDumper: public CfgAdjustmentCallback {
public:
    struct Settings {
        AddressInterval where;                          // what basic block(s) we should monitor (those starting within)
        Trigger::Settings when;                         // once found, which event triggers the output
        AddressInterval what;                           // which basic blocks should be in the output
        bool showNeighbors;                             // should neighbor blocks be included in the output?
        std::string fileName;                           // name of output; '%' gets expanded to a distinct identifier
        Settings(): what(AddressInterval::whole()), showNeighbors(true), fileName("cfg-%.dot") {}
    };
private:
    Settings settings_;
    Trigger trigger_;
protected:
    CfgGraphVizDumper(const Settings &settings): settings_(settings), trigger_(settings.when) {}
public:
    static Ptr instance(const Settings &settings) { return Ptr(new CfgGraphVizDumper(settings)); }
    static Ptr instance(const std::string &config);
    static Ptr instance(const std::vector<std::string> &args);
    static Sawyer::CommandLine::SwitchGroup switches(Settings&);
    static std::string docString();
    virtual bool operator()(bool chain, const AttachedBasicBlock &args) ROSE_OVERRIDE;
    virtual bool operator()(bool chain, const DetachedBasicBlock&) ROSE_OVERRIDE { return chain; }
};

/** Produce a hexdump at a certain time.
 *
 *  See @ref docString for full documentation. */
class HexDumper: public CfgAdjustmentCallback {
public:
    struct Settings {
        AddressInterval where;                          // what basic block(s) we should monitor (those starting within)
        Trigger::Settings when;                         // once found, which event triggers the output
        AddressInterval what;                           // which bytes should be in the output
        bool accentSpecialValues;                       // use "." and "##" for 0 and 0xff?
        Settings(): accentSpecialValues(true) {}
    };
private:
    Settings settings_;
    Trigger trigger_;
protected:
    HexDumper(const Settings &settings): settings_(settings), trigger_(settings.when) {}
public:
    static Ptr instance(const Settings &settings) { return Ptr(new HexDumper(settings)); }
    static Ptr instance(const std::string &config);
    static Ptr instance(const std::vector<std::string> &args);
    static Sawyer::CommandLine::SwitchGroup switches(Settings&);
    static std::string docString();
    virtual bool operator()(bool chain, const AttachedBasicBlock &args) ROSE_OVERRIDE;
    virtual bool operator()(bool chain, const DetachedBasicBlock&) ROSE_OVERRIDE { return chain; }
};

/** Convenient place to attach a debugger.
 *
 *  See @ref docString for full documentation. */
class Debugger: public CfgAdjustmentCallback {
public:
    struct Settings {
        AddressInterval where;                          // what basic block(s) should we monitor (those starting within)
        Trigger::Settings when;                         // once found, which event triggers
    };
private:
    Settings settings_;
    Trigger trigger_;
protected:
    Debugger(const Settings &settings): settings_(settings), trigger_(settings.when) {}
public:
    static Ptr instance(const Settings &settings) { return Ptr(new Debugger(settings)); }
    static Ptr instance(const std::string &config);
    static Ptr instance(const std::vector<std::string> &args);
    static Sawyer::CommandLine::SwitchGroup switches(Settings&);
    static std::string docString();
    virtual bool operator()(bool chain, const AttachedBasicBlock &args) ROSE_OVERRIDE;
    virtual bool operator()(bool chain, const DetachedBasicBlock&) ROSE_OVERRIDE { return chain; }
    void debug(rose_addr_t, const BasicBlock::Ptr&);
};

/** Remove execute permissions for zeros.
 *
 *  Scans memory to find consecutive zero bytes and removes execute permission from them.  Returns the set of addresses whose
 *  access permissions were changed.  Only occurrences of at least @p threshold consecutive zeros are changed. If @p threshold
 *  is zero then nothing happens. */
AddressIntervalSet deExecuteZeros(MemoryMap &map /*in,out*/, size_t threshold);

/** Give labels to addresses that are symbols.
 *
 *  Scans the specified binary container and labels those virtual addresses that correspond to symbols.  This function does not
 *  create any functions in the partitioner, it only gives names to certain addresses.  If the same address is labeled more
 *  than once by symbols with different names then one name is chosen arbitrarily.
 *
 * @{ */
void labelSymbolAddresses(Partitioner&, SgAsmGenericHeader*);
void labelSymbolAddresses(Partitioner&, SgAsmInterpretation*);
/** @} */

/** Give labels to string constants.
 *
 *  Finds integer constants that are the address if a C-style NUL-terminated ASCII string and adds a comment to the constant
 *  (if it had none previously) to describe the string. All instructions that are attached to the CFG/AUM are processed. The
 *  instructions are modified by attaching the comment, but the comments are not added to the partitioners address name map. */
void nameStrings(const Partitioner&);

/** Finds functions for which symbols exist.
 *
 *  Scans the specified AST to find symbols that point to functions and makes a function at each such address.  A function is
 *  made only if an instruction can be disassembled at the address. The return value is a sorted list of unique functions.
 *
 * @{ */
std::vector<Function::Ptr> findSymbolFunctions(const Partitioner&, SgAsmGenericHeader*);
std::vector<Function::Ptr> findSymbolFunctions(const Partitioner&, SgAsmInterpretation*);
size_t findSymbolFunctions(const Partitioner&, SgAsmGenericHeader*, std::vector<Function::Ptr>&);
/** @} */

/** Gives names to constants in instructions.
 *
 *  Scans the entire list of attached instructions and give each constant integer expression a name if the value of the
 *  expression happens to be an address that has a name. */
void nameConstants(const Partitioner&);

/** Find functions that are no-ops.
 *
 *  Finds functions that are no-ops and returns them in ascending order of entry addresses. */
std::vector<Function::Ptr> findNoopFunctions(const Partitioner&);

/** Give names to functions that are no-ops.
 *
 *  Scans the list of attached functions to find those whose only action is to return to the caller (via @ref
 *  findNoopFunctions) and gives names to those that don't have names.  The names are of the form "noop_ADDR() -> void" where
 *  ADDR is the hexadecimal entry address. The C++ trailing return type syntax is used so that functions can be easily sorted
 *  according to their names. */
void nameNoopFunctions(const Partitioner&);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Partitioner conversion to AST
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Build AST for basic block.
 *
 *  Builds and returns an AST for the specified basic block. The basic block must not be a null pointer, but it need not be in
 *  the CFG.  If the basic block has no instructions then it would violate ROSE's invariants, so a null pointer is returned
 *  instead; however, if @p relaxed is true then an IR node is returned anyway. */
SgAsmBlock* buildBasicBlockAst(const Partitioner&, const BasicBlock::Ptr&, const Function::Ptr&, bool relaxed=false);

/** Build AST for data block.
 *
 *  Builds and returns an AST for the specified data block.  The data block must not be a null pointer, but it need not be in
 *  the CFG.  If @p relaxed is true then IR nodes are created even if they would violate some ROSE invariant, otherwise invalid
 *  data blocks are ignored and a null pointer is returned for them. */
SgAsmBlock* buildDataBlockAst(const Partitioner&, const DataBlock::Ptr&, bool relaxed=false);

/** Build AST for function.
 *
 *  Builds and returns an AST for the specified function.  The function must not be a null pointer, but it need not be in the
 *  CFG.  The function will have children created only for its basic blocks that exist in the CFG (otherwise the partitioner
 *  doesn't know about them).  If no children were created then the returned function IR node violates ROSE's invariants, so a
 *  null pointer is returned instead; however, if @p relaxed is true then an IR node is returned anyway. */
SgAsmFunction* buildFunctionAst(const Partitioner&, const Function::Ptr&, bool relaxed=false);

/** Builds the global block AST.
 *
 *  A global block's children are all the functions contained in the AST, which in turn contain SgAsmBlock IR nodes for the
 *  basic blocks, which in turn contain instructions.  If no functions exist in the CFG then the returned node would violate
 *  ROSE's invariants, so a null pointer is returned instead; however, if @p relaxed is true then the IR node is returned
 *  anyway. */
SgAsmBlock* buildGlobalBlockAst(const Partitioner&, bool relaxed=false);

/** Builds an AST from the CFG.
 *
 *  Builds an abstract syntax tree from the control flow graph.  The returned SgAsmBlock will have child functions; each
 *  function (SgAsmFunction) will have child basic blocks; each basic block (SgAsmBlock) will have child instructions.  If @p
 *  relaxed is false then all IR nodes in the returned tree will satisfy ROSE's invariants concerning them at the expense of not
 *  including certain things in the AST; otherwise, when @p relaxed is true, the AST will be as complete as possible but may
 *  violate some invariants.
 *
 *  This function is the same as @ref buildGlobalBlockAst except it also calls various AST fixup functions. Providing an
 *  interpretation allows more fixups to occur.  Also, if @p interp is non-null then the returned global block is attached to
 *  the interpretation in the AST (any previous global block is detached but not destroyed). */
SgAsmBlock* buildAst(const Partitioner&, SgAsmInterpretation *interp=NULL, bool relaxed=false);

/** Fixes pointers in the AST.
 *
 *  Traverses the AST to find SgAsmIntegerValueExpressions and changes absolute values to relative values.  If such an
 *  expression is the starting address of a function then the expression will point to that function; else if the expression is
 *  the starting address of a basic block then the expression will point to that basic block; else if the expression is the
 *  starting address of an instruction then the expression will point to that instruction; else if the expression evaluates to
 *  an address inside a mapped section, then the expression will become relative to the start of the best section. Pointers
 *  into sections are only created if an interpretation is specified. */
void fixupAstPointers(SgNode *ast, SgAsmInterpretation *interp=NULL);

/** Fixes calling convention results.
 *
 *  This function uses the results of previous calling convention analysis to assign calling convention definitions to the
 *  functions in the specified AST. There's often more than one definition that matches the analysis results, and this fixup
 *  pass attempts to assign the best definitions. It does so by first ranking the definitions according to how often they match
 *  across all the functions known to the partitioner. Then, for each function in the specified @ref ast, the we also get a
 *  list matching definitions (if a calling convention analysis has been run on that function). We copy into the AST that
 *  function's definition which has the highest global ranking. */
void fixupAstCallingConventions(const Partitioner&, SgNode *ast);

} // namespace

} // namespace
} // namespace
} // namespace

#endif
