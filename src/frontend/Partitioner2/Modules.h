#ifndef ROSE_Partitioner2_Modules_H 
#define ROSE_Partitioner2_Modules_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/Utility.h>

#include <sawyer/SharedPointer.h>

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
        const Partitioner *partitioner;                 /**< Partitioner requesting basic block successors. */
        BasicBlock::Ptr bblock;                         /**< Basic block whose successors are to be computed. */
        Results &results;                               /**< Results to control basic block discovery. */
        Args(const Partitioner *partitioner, const BasicBlock::Ptr &bblock, Results &results)
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
    virtual bool match(const Partitioner*, rose_addr_t anchor) = 0;
};


/** Base class for matching function prologues.
 *
 *  A function prologue is a pattern of bytes or instructions that typically mark the beginning of a function.  For
 *  instance, many x86-based functions start with "PUSH EBX; MOV EBX, ESP" while many M68k functions begin with a single
 *  LINK instruction affecting the A6 register.  A subclass must implement the @ref match method that does the actual
 *  pattern matching.  If the @ref match method returns true, then the partitioner will call the @ref function method to
 *  obtain a function object.
 *
 *  The matcher will be called only with anchor addresses that are mapped with execute permission and which are not a
 *  starting address of any instruction in the CFG.  The matcher should ensure similar conditions are met for any
 *  additional addresses, especially the address returned by @ref functionVa. */
class FunctionPrologueMatcher: public InstructionMatcher {
public:
    /** Shared-ownership pointer. The partitioner never explicitly frees matchers. Their pointers are copied when
     *  partitioners are copied. */
    typedef Sawyer::SharedPointer<FunctionPrologueMatcher> Ptr;

    /** Returns the function for the previous successful match.  If the previous call to @ref match returned true then this
     *  method should return a function for the matched function prologue.  Although the function returned by this method
     *  is often at the same address as the anchor for the match, it need not be.  For instance, a matcher could match
     *  against some amount of padding followed the instructions for setting up the stack frame, in which case it might
     *  choose to return a function that starts at the stack frame setup instructions and includes the padding as static
     *  data. The partitioner will never call @ref function without first having called @ref match. */
    virtual Function::Ptr function() const = 0;
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
    virtual rose_addr_t match(const Partitioner*, rose_addr_t anchor) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Generic modules
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Modules {

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

} // namespace

} // namespace
} // namespace
} // namespace

#endif
