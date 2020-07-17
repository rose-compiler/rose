#ifndef ROSE_Partitioner2_ParallelPartitioner_H
#define ROSE_Partitioner2_ParallelPartitioner_H
#include <rosePublicConfig.h>
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT) && __cplusplus >= 201103L

#include <BinaryInstructionCache.h>
#include <Progress.h>
#include <queue>
#include <Partitioner2/BasicTypes.h>
#include <Sawyer/Attribute.h>
#include <Sawyer/Graph.h>
#include <Sawyer/IntervalSetMap.h>
#include <Sawyer/Message.h>
#include <Sawyer/SharedObject.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace Experimental {
namespace ParallelPartitioner {

class Partitioner;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings and configuration, including miscellaneous basic types.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Accuracy flag. */
enum class Accuracy {
    LOW,                                        /**< Fast but less accurate, generally looking at patterns. */
    HIGH,                                       /**< Accurate but slow, generally looking at semantics. */
};

/** Reasons for being a function. */
using FunctionReasons = BitFlags<SgAsmFunction::FunctionReason>;

/** Settings. */
struct Settings {
    size_t maxAnalysisBBlockSize = 20; /** Max number of insns in basic block during various analyses. */
    Accuracy successorAccuracy = Accuracy::LOW; /**< How to determine CFG successors. */
    Accuracy functionCallDetectionAccuracy = Accuracy::LOW; /**< How to determine whether something is a function call. */
    size_t minHoleSearch = 8; /**< Do now search unused regions smaller than this many bytes. */
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diagnostics
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Diagnostic facility for things in this namespace. */
extern Sawyer::Message::Facility mlog;

// Used internally. See Rose::Diagnostics::initialize
void initDiagnostics();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cached information
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Implements a cache for a single datum. */
template<class Value, class Key>
class CachedItem {
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects all following data members
    Sawyer::Optional<Key> key_;
    Sawyer::Optional<Value> value_;

public:
    /** Set the cache to hold the specified key / value pair. */
    void set(const Key &key, const Value &value) {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        key_ = key;
        value_ = value;
    }

    /** Get the cached item if present. */
    Sawyer::Optional<Value> get(const Key &key) const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        if (key_ && *key_ == key)
            return value_;
        return {};
    }

    /** Remove the item from the cache. */
    void reset() {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        key_.reset();
        value_.reset();
    }

    /** Tests whether a value is cached. */
    bool exists(const Key &key) const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return key_ && *key_ == key;
    }
};

// Same as above, but no keys
template<class Value>
class CachedItem<Value, void> {
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;
    Sawyer::Optional<Value> value_;

public:
    void set(const Value &value) {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        value_ = value;
    }

    Sawyer::Optional<Value> get() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return value_;
    }

    void reset() {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        value_.reset();
    }

    bool exists() const {
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        return value_;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Information stored at each vertex of the CFG.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Information about an instruction. */
class InsnInfo: boost::noncopyable {
public:
    using Ptr = std::shared_ptr<InsnInfo>;      /**< Shared ownership pointer. */
    using List = std::vector<Ptr>;              /**< List of pointers to InsnInfo objects. */

    /** Cached information. */
    struct Cached: boost::noncopyable {
        CachedItem<AddressSet, uint64_t> computedConcreteSuccessors; /**< Computed CFG successor addresses. */
        CachedItem<bool, uint64_t> isFunctionCall; /**< Is this a function call. */
    };
    
private:
    InstructionPtr ast_;                        // possibly evicted AST rooted at an SgAsmInstruction node.
    const rose_addr_t va_;                      // starting address accessible even for evicted instruction
    Cached cached_;                             // cached items computed elsewhere

    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects all following data members
    size_t size_;                               // instruction size in bytes even for evicted instruction
    bool wasDecoded_;                           // has any DecodeInstruction work completed?
    FunctionReasons functionReasons_;          // is this insn the entry point of a function?

    // Cached items associated with basic blocks. The key is the hash of the basic block instruction addresses.

public:
    /** Construct info with decoded instruction.
     *
     *  The @p insn must be non-null. */
    explicit InsnInfo(const InstructionPtr &insn)
        : ast_(insn), va_(insn->get_address()), size_(insn->get_size()), wasDecoded_(false) {
        ASSERT_not_null(insn);
        ASSERT_require(size_ > 0);
    }

    /** Construct info from only an address. */
    explicit InsnInfo(rose_addr_t va)
        : va_(va), size_(0), wasDecoded_(false) {}

    /** Address of instruction.
     *
     *  The starting address of the instruction. This information is available for every object, even if we've never attempted
     *  to decode an instruction at that address and @ref wasDecoded is returning false. The address is provided by the
     *  constructor and is read only.
     *
     *  Thread safety: This function is thread safe. */
    rose_addr_t address() const {
        return va_;
    }

    /** Size of instruciton in bytes if known.
     *
     *  The size of an instruction is known only after @ref wasDecoded is returning true; before that, this function returns
     *  nothing.  After an instruction is decoded, its size is available regardless of whether the instruction AST is in memory
     *  or has been evicted.
     *
     *  Thread safety: This function is thread safe. */
    Sawyer::Optional<size_t> size() const;

    /** Location of instruction if known.
     *
     *  The addresses occupied by this instruction. This is a combination of the starting address and instruction size and
     *  therefore is available only after @ref wasDecoded is returning true. This information is available even when the
     *  instruction AST is evicted from the cache.
     *
     *  Thread safety: This function is thread safe. */
    Sawyer::Optional<AddressInterval> hull() const;

    /** Property: Function reasons.
     *
     *  This is a set of reason bits that describe why this instruction is the entry point of a function.  If the set is empty then
     *  this instruction is not a function entry point.
     *
     *  Thread safety: This function is thread safe.
     *
     *  @{ */
    FunctionReasons functionReasons() const;
    void functionReasons(FunctionReasons);
    void insertFunctionReasons(FunctionReasons);
    void eraseFunctionReasons(FunctionReasons);
    /** @} */

    /** Cached information.
     *
     *  Returns informatio that's cached at a particular CFG vertex.
     *
     *  Thread safety: This function is thread safe. */
    const Cached& cached() const { return cached_; }
    Cached& cached() { return cached_; }
    /** @} */

    /** Whether any attempt was made to decode this instruction.
     *
     *  An @ref InsnInfo object can exist without there having ever been any attempt to actually decode an instruction at the
     *  specified address. In this case, the @ref ast will be a null pointer, but that's indistinguishable from the case when a
     *  decode was attempted and failed.  Therefore, this flag will be set after the first decode is attempted.
     *
     *  Certain other properties of an instruction, such as its size, are only available after the instruction was decoded.
     *
     *  Once @c wasDecoded returns true it will always return true.
     *
     *  Thread safety: This function is thread safe.
     *
     * @{ */
    bool wasDecoded() const;
    void setDecoded();
    /** @} */

    /** Get the underlying instruction AST.
     *
     *  Returns the underlying instruction. The @ref wasDecoded flag must be set before calling this.  Returns a null pointer if
     *  the memory is invalid (not mapped or not executable), or a non-null pointer (possibly an "unknown" instruction)
     *  otherwise.
     *
     *  Thread safety: This function is thread safe. */
    InstructionPtr ast() const;

    /** For sorty by address.
     *
     *  Returns true if the address of @p a is less than the address of @p b.
     *
     *  Thread safety: This instruction is thread safe. */
    static bool addressOrder(const Ptr &a, const Ptr &b);

    /** Hash instruction list.
     *
     *  Hashes the specified list of instructions in order to get a value that can be used as a lookup key.
     *
     * Thread safety: This function is thread safe. */
    static uint64_t hash(const List&);

private:
    friend class Partitioner;

    // Set the underlying instruction.
    //
    // This sets the underlying instruction if it isn't set already, and returns the underlying instruction. If the return value
    // is the same pointer as the argument then the instruction was set, otherwise it already had some other value. Also, the
    // @ref wasDecoded flag is set.  The instruction can be a valid instruction or a null pointer.
    //
    // Thread safety: This function is thread safe.
    InstructionPtr setAstMaybe(const InstructionPtr&);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFG edge information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CfgEdge {
public:
    using EdgeTypes = BitFlags<EdgeType>;
    EdgeTypes types_;

public:
    CfgEdge()
        : types_(E_NORMAL) {}

    /*implicit*/ CfgEdge(EdgeType type)
        : types_(type) {}

    void merge(const CfgEdge &other) {
        types_.set(other.types_);
    }

    EdgeTypes types() const {
        return types_;
    }

    void types(EdgeTypes x) {
        types_.set(x);
    }

    friend std::ostream& operator<<(std::ostream&, const CfgEdge&);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Control flow graph
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lookup keys used internally for CFG vertices.
class InsnInfoKey {
    rose_addr_t va_;
public:
    /*implicit*/ InsnInfoKey(const InsnInfo::Ptr &insnInfo)
        : va_(insnInfo->address()) {}
    /*impilcit*/ InsnInfoKey(rose_addr_t &va)
        : va_(va) {}

    // FIXME: we should be using an unordered_map for the index.
    bool operator<(const InsnInfoKey &other) const {
        return va_ < other.va_;
    }
};

/** Global control flow graph.
 *
 *  Unlike most other parts of ROSE, this CFG's vertices are individual instructions rather than whole basic blocks. The
 *  partitioner's first order of business is to create a global control flow graph which it then organizes into basic blocks
 *  and instructions. */
using InsnCfg = Sawyer::Container::Graph<std::shared_ptr<InsnInfo>, CfgEdge, InsnInfoKey>;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkItem -- base class for individual parallel units of work
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Item of work to be completed.
 *
 *  This is the base class for all types of work to be completed in parallel. The user may extend this to create his own
 *  types of work.
 *
 *  This class describes one simple unit of work that must be completed as part of discovering the code for a binary
 *  specimen. The scheduler will order this work according to any dependencies known to the scheduler and dispatch the
 *  work in parallel to some set of worker threads. */
class WorkItem {
public:
    /** Coarse priority for work items. */
    enum class Priority {
        DiscoverInstruction = 0,
        NextUnusedVa = -100
    };

private:
    Partitioner &partitioner_;                  // partitioner that owns this work item
    Priority priority_;                         // primary sort key
    uint64_t sort_;                             // secondary sort key

protected:
    /** Construct new item with given priority for a specific partitioner.
     *
     *  Work items are always owned by a partitioner, which is passed as the first constructor argument.
     *
     *  The priority is used as a coarse way to sort work items based on their types. Once items are sorted coarsely,
     *  their @ref fineSort is used to sort them. Higher priority work must complete before lower priority work
     *  is started. The priorities used internally are all less than or equal to zero. */
    WorkItem(Partitioner &partitioner, Priority priority, uint64_t sort)
        : partitioner_(partitioner), priority_(priority), sort_(sort) {}

    virtual ~WorkItem() {}

    /** Partitioner set at construction time. */
    Partitioner& partitioner() const {
        return partitioner_;
    }

    /** Priority set at construction time. */
    Priority priority() const {
        return priority_;
    }

public:
    /** Sort work.
     *
     *  Sorts work items according to priority, and then according to their fine-grained sort. */
    bool operator<(const WorkItem&) const;

public:
    /** Performs the work.
     *
     *  This function runs in its own thread. */
    virtual void run() = 0;

    /** A one-line title. */
    virtual std::string title() const = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DecodeInstruction -- a unit of work
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Decode an instruction at a specific address.
 *
 *  This work item will populate an existing vertex of the CFG with an instruction, try to figure out what the instruction's
 *  CFG successors are, add CFG vertices for each successor that doesn't already exist, and create more work to populate those
 *  vertices with decoded instructions. */
class DecodeInstruction: public WorkItem {
protected:
    rose_addr_t insnVa;                     // starting address of the instruction

public:
    DecodeInstruction(Partitioner &partitioner, rose_addr_t va)
        : WorkItem(partitioner, WorkItem::Priority::DiscoverInstruction, va), insnVa(va) {}

    std::string title() const override {
        return "decode insn " + StringUtility::addrToString(insnVa);
    }

    void run() override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NextUnusedRegion -- a unit of work
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Find unused executable addresses.
 *
 *  Given a region of an address space, find all the unused executable regions of maximum size within that region, schedule
 *  an instruction search at each one, and schedule additional unused region searches to occur after instructions are found. */
class NextUnusedRegion: public WorkItem {
protected:
    AddressInterval where;                      // what part of the address space to search

public:
    NextUnusedRegion(Partitioner &partitioner, const AddressInterval &where)
        : WorkItem(partitioner, WorkItem::Priority::NextUnusedVa, where ? where.least() : uint64_t(0)), where(where) {}

    std::string title() const override {
        return "scan unused va within " + StringUtility::addrToString(where);
    }
    
    void run() override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Scheduler -- schedules units of work for parallel execution
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WorkItemSorter {
public:
    bool operator()(const std::shared_ptr<WorkItem>&, const std::shared_ptr<WorkItem>&) const;
};

/** Schedules work items for execution.
 *
 *  The scheduler contains a list of work items, sorts them according dependencies and priorities built into the
 *  scheduler, and dispatches them to the worker threads.
 *
 *  Thread safety: All methods of this class are thread safe. */
class Scheduler final {
public:
    using Item = std::shared_ptr<WorkItem>; /** Type of items stored in this scheduler. */
    using Container = std::vector<Item>;
    using Queue = std::priority_queue<Item, Container, WorkItemSorter>; /** Type of queue used by this scheduler. */

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects all following data members
    Queue queue_;

public:
    /** Insert a new work item into the work list. */
    void insert(const Item&);

    /** Test whether any work remains to be done. */
    bool isEmpty() const;

    /** Returns the next item of work to be performed */
    Item next();

    /** Output a status report. */
    void reportStatus(std::ostream&) const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Worker -- top level function for worker threads
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Handles work items when they're dispatched to a worker thread.
 *
 *  This top-level functor runs in a worker thread and is reponsible for dispatching the unit of work to the proper
 *  handler for that work item. */
class Worker {
public:
    void operator()(std::shared_ptr<WorkItem> work, Scheduler&) {
        work->run();
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Address usage map
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Address usage map.
 *
 *  Relates specimen addresses back to instructions.  Each address is associated with zero or more instruction addresses. */
using Aum = Sawyer::Container::IntervalSetMap<AddressInterval, AddressSet>;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Discovers instructions, basic blocks, and functions.
 *
 *  The partitioner is reponsible for discovering the locations of valid instructions and organizing them to form a
 *  consistent set of basic blocks that form a global control flow graph.  The basic blocks are further organized into
 *  subsets called functions. */
class Partitioner: public Sawyer::Attribute::Storage<>, public Sawyer::SharedObject, boost::noncopyable {
    Settings settings_;
    Scheduler scheduler_;
    std::shared_ptr<InstructionCache> insnCache_;
    Progress::Ptr progress_;                            // reports percent of memory disassembled
    rose_addr_t nExeVas_;                               // number of executable addresses in memory map

    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects all following data members
    bool isRunning_;                                    // true while "run" is called
    InsnCfg insnCfg_;                                   // the global control flow graph
    Aum aum_;                                           // address usage map

public:
    //--------------------------------------------------------------------------------------------------------------------
    // Constructors and initialization
    //--------------------------------------------------------------------------------------------------------------------

    /** Constructor new partitioner.
     *
     *  The memory mapping and the memory contents should not change after the partitioner is initialized, nor should the
     *  decoder or its behavior change after initialization. This is due to the requirement that any instruction AST can
     *  be deleted and when it's recreated later it must be the same as the original. */
    Partitioner(const MemoryMap::Ptr &memory, Disassembler *decoder, const Settings &settings = Settings());

    /** Behavior settings.
     *
     *  These settings control the bavior of the partitioner.
     *
     *  Thread safety: This function is not thread safe. All settings should be done prior to running the partitioner and
     *  should not change while the partitioner is running.
     *
     *  @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    /** @} */
    
    //--------------------------------------------------------------------------------------------------------------------
    // Status
    //--------------------------------------------------------------------------------------------------------------------
public:
    /** Progress reports for ratio of memory disassembled.
     *
     *  Returns the number of bytes of memory that have been disassembled as a fraction of the number of executable bytes.
     *
     *  Thread safety: This function is thread safe. */
    Progress::Ptr progress() const;

    /** Generate status reports every so often.
     *
     *  This function loops until all work is finished, printing a status report to standard error every so often.
     *  Obviously some other thread needs to be making progress or else this function will never return.
     *
     *  Thread safety: This function is thread safe. */
    void statusReports();

    //--------------------------------------------------------------------------------------------------------------------
    // Functions related to memory
    //--------------------------------------------------------------------------------------------------------------------
public:

    /** Returns the memory map used for disassembling.
     *
     *  Thread safety: This function is thread safe. */
    MemoryMap::Ptr memoryMap() const;

    /** Amount of memory with known instructions.
     *
     *  Returns the number of byte addresses of the memory where instructions have been decoded. If an instruction occupies
     *  multiple addresses then all those addresse are counted. If multiple instructions occupy a single address then the
     *  address is counted only once.
     *
     *  Thread safety: This function is thread safe. */
    size_t nDecodedAddresses() const;

    /** All unused executable addresses within region.
     *
     *  Returns the list of all unused executable addresses within the specified address range.
     *
     *  Thread safety: This function is thread safe. */
    AddressIntervalSet unusedExecutableVas(AddressInterval where) const;

    //--------------------------------------------------------------------------------------------------------------------
    // Functions related to instructions
    //--------------------------------------------------------------------------------------------------------------------
public:

    /** Decode an instruction at the specified address.
     *
     *  Returns a smart pointer to the instruction which is either already decoded or will be decoded when the pointer
     *  is dereferenced. The address need not be a vertex in the global control flow graph. If the memory at the specified
     *  address is not mapped or is not mapped with execute permission then a null pointer is returned. If the memory address
     *  is valid but the opcode at that address is meaningless or incomplete (i.e., missing or not fully executable) then
     *  an "unknown" instruction is returned.
     *
     *  Thread safety: This function is thread safe. */
    InstructionPtr decodeInstruction(rose_addr_t);

    /** Create and return information about a vertex of the CFG.
     *
     *  Returns the instruction information for the specified vertex of the control flow graph, creating it if necessary. If
     *  only an address is specified, then any created instruction info object will contain only an address and will be in a
     *  non-decoded state where its @ref InsnInfo::wasDecoded "wasDecoded" function returns false (at least until something
     *  causes it to be decoded).
     *
     *  If an instruction AST is specified (via @ref InstructionPtr argument) then all the previously described steps happen
     *  plus the specified AST is assigned to the instruction info, but only if it doesn't already have one. The instruction
     *  may be null if the memory location is invalid (not mapped or not executable). If the location is valid but the opcode
     *  is invalid then a non-null "unknown" instruction should be specified. If the instruction is non-null then it's address
     *  must equal the specified address (which is required because of the possibility of null instructions).
     *
     *  Thread safety: This function is thread safe. However, by time the caller can use the return value some other thread
     *  might have removed it from the CFG. In any case, the return value is always valid.
     *
     *  @{ */
    InsnInfo::Ptr makeInstruction(rose_addr_t);
    InsnInfo::Ptr makeInstruction(rose_addr_t, const InstructionPtr&);
    /** @} */

    /** Return information about a vertex of the global CFG.
     *
     *  If the global control flow graph contains the specified address then its information is returned as a pointer,
     *  otherwise a null pointer is returned.
     *
     *  Thread safety: This function is thread safe. */
    InsnInfo::Ptr existingInstruction(rose_addr_t);

    /** Returns the AST for an instruction.
     *
     *  This is similar to @ref existingInstruction except it obtains the AST of the instructio rather than information about
     *  the instruction. Although the instrucition information includes the AST, this function is easier to call because it
     *  does all the necessary null checks.
     *
     *  Thread safety: This function is thread safe. */
    InstructionPtr existingInstructionAst(rose_addr_t);

    /** The instruction cache.
     *
     *  The instruction cache holds the ASTs for instructions that have been decoded and used recently.
     *
     *  Thread safety: This function is thread safe. */
    InstructionCache& instructionCache() const;

    /** Object returned by @ref lockInCache. */
    struct LockInCache {
        std::vector<LockedInstruction> locks;                   /**< The locking pointers. */
        std::vector<SgAsmInstruction*> insns;                   /**< The raw pointers. */
    };

    /** Lock instructions in the cache.
     *
     *  This function should be used when you need to call an old ROSE function that uses multiple raw AST pointers. The call
     *  to that old function must ensure that none the ASTs for the specified instructions are evicted from the cache by other
     *  threads for the duration of the call.  The return value consists of two vectors named "locks", which are the locking
     *  pointers, and "insns", which are the raw pointers to the instruction ASTs.  Since the raw pointers can be obtained by
     *  invoking @c get on the @ref LockedInstruction objects, the vector of raw pointers is primarily for convenience when
     *  calling old-style functions that take vectors of instructions. This function should be used as follows:
     *
     *  @code
     *  InsnInfo::List insns = ...; // a list of instructions, such as from basicBlockEndingAt.
     *  {
     *      auto locked = partitioner.lockInCache(insns);
     *      AddressSet successors = locked.insns.front()->successors(locked.insns); // safe to call
     *  } // ASTs are no longer locked
     *  @endcode
     *
     *  Thread safety: This function is thread safe. */
    LockInCache lockInCache(const InsnInfo::List&);

    /** Information returned by @ref createLinkedCfgVertices. */
    struct CreateLinkedCfgVertices {
        bool createdSource = false;                             /**< Whether the source vertex was created. */
        bool createdTarget = false;                             /**< Whether the target vertex was created. */
        bool createdEdge = false;                               /**< Whether the edge was created. */
        InsnInfo::Ptr source;                                   /**< Source instruction. */
        InsnInfo::Ptr target;                                   /**< Target instruction. */
    };

    /** Add an edge and maybe vertices to the global CFG.
     *
     *  Ensures that the two instruction addresses exist in the global CFG and creates an edge between them if there is no such
     *  edge yet. Returns information about whether the vertices and edges where created. If an edge is created then it is set
     *  to the specified value, otherwise the specified value is merged into the existing edge.
     *
     *  Thread safety: This function is thread safe. */
    CreateLinkedCfgVertices createLinkedCfgVertices(rose_addr_t sourceVa, rose_addr_t targetVa, const CfgEdge&);

    /** Add a work item to decode an instruction.
     *
     *  When run, this work item will attempt to decode an instruction at the specified CFG node, which must exist, and
     *  then create additional CFG nodes and work items for that instruction's basic block sucessors.
     *
     *  Thread safety: This function is thread safe. */
    void scheduleDecodeInstruction(rose_addr_t insnVa);

    /** Add a work item to decode lowest unused addresses.
     *
     *  When run, this work item will find all the unused executable addresses within the specified region, group them into
     *  contiguous regions, arrange to start discovering instructions at each region, and schedule more unused address checks
     *  for later. */
    void scheduleNextUnusedRegion(const AddressInterval &where);

    //--------------------------------------------------------------------------------------------------------------------
    // Basic blocks
    //--------------------------------------------------------------------------------------------------------------------
public:
    /** Find a basic block's worth of instructions.
     *
     *  Given an instruction address, recursively follow parent pointers to construct a list of instructions that form a basic
     *  block. The returned list will end at the specified instruction even if the CFG contains successors that would also be
     *  part of the same basic block.  At most @p maxInsns instructions are returned.
     *
     *  Thread safety: This function is thread safe. */
    InsnInfo::List basicBlockEndingAt(rose_addr_t, size_t maxInsns = UNLIMITED) const;

    /** Find basic block containing specified instruction.
     *
     *  Given at least one instruction of a basic block, find all the other instructions and return them as a list.
     *
     *  Thread safety: This function is thread safe. If possible, use @ref basicBlockEnding at instead, which is slightly
     *  faster and supports limiting the block size. */
    InsnInfo::List basicBlockContaining(rose_addr_t) const;

    /** Return the computed concrete successors for an instruction.
     *
     *  If instruction semantics are available, then the successors for the instruction at the specified addresss are
     *  calculated by accumulating a basic block of instructions and then processing those instructions semantically. A vector
     *  of the concrete successor addresses is returned and cached in the CFG.
     *
     *  If the @ref Settings::successorAccuracy determines whether the calculation looks at just the specified instruction or
     *  an entire basic block.
     *
     *  Thread safety: This function is thread safe. */
    AddressSet computedConcreteSuccessors(rose_addr_t insnVa);

    /** Determine whether the instruction is a function call.
     *
     * Determines whether the specified address is a function call. The @ref Settings::functionCallDetectionAccuracy determines
     * whether this looks at just the specified instruction or an entire basic block. The result is cached in the control flow
     * graph.
     *
     * Thread safety: This function is thread safe. */
    bool isFunctionCall(rose_addr_t insnVa);

    //--------------------------------------------------------------------------------------------------------------------
    // Functions to run things.
    //--------------------------------------------------------------------------------------------------------------------
public:

    /** Run in parallel.
     *
     *  Runs all pending work in parallel and returns when that work, and all work recursively created, has completed.
     *  The work is run in at most @p maxWorkers threads at a time.
     *
     *  Thread safety: This function is not thread safe. It should be called from only one thread at a time. */
    void run(size_t maxWorkers);

    /** Whether the partitioner is running.
     *
     *  Returns true while @ref run is called.
     *
     *  Thread safety: This function is thread safe. */
    bool isRunning() const;

private:
    // Used internally to change the isRunning value. This function is thread safe.
    void isRunning(bool b);

    //--------------------------------------------------------------------------------------------------------------------
    // Post processing functions. The following functons can only be called when the partitioner is not running (i.e.,
    // only when isRunning returns false).
    //--------------------------------------------------------------------------------------------------------------------
public:

    /** Control flow graph.
     *
     *  Returns a reference to the control flow graph. This function can only be called when the partitioner is not running
     *  (i.e., while @ref isRunning returns false), and the return value is valid only as long as the partitioner remains in
     *  that state. Accessing or modifying the control flow graph while the partitioner is running has undefined behavior.
     *
     *  Thread safety: This function is not thread safe.
     *
     * @{ */
    const InsnCfg& insnCfg() const;
    InsnCfg& insnCfg();
    /** @} */

    /** Output the CFG as a DOT graph.
     *
     *  Thread safety: This function is thread safe. */
    void printInsnCfg(std::ostream&) const;

    /** Output the CFG as text for debugging.
     *
     *  The serial partitioner is used only for unparsing the instructions.
     *
     *  Thread safety: This function is thread safe. */
    void dumpInsnCfg(std::ostream&, const Rose::BinaryAnalysis::Partitioner2::Partitioner&) const;

    /** List of all basic blocks.
     *
     *  This returns a list of all the basic blocks in the global control flow graph. The list is calculated on demand and is
     *  not particularly fast (on the order of the number of instructions in teh control flow graph). The blocks are returned
     *  in no particular order.
     *
     *  Thread safety: This function is not thread safe. */
    std::vector<InsnInfo::List> allBasicBlocks() const;

    /** Create a map from instructions to basic blocks.
     *
     *  Traverses the entire CFG and creates a mapping from each instruction address to that instruction's basic block address.
     *
     *  Thread safety: This function is not thread safe. */
    std::map<rose_addr_t /*insn*/, rose_addr_t /*bb*/> calculateInsnToBbMap() const;

    /** Predicate to order blocks by starting address.
     *
     *  Compares two instruction lists and returns true if the first list starts at an earlier address than the second list. An
     *  empty list compares less than all non-empty lists. */
    static bool addressOrder(const InsnInfo::List &a, const InsnInfo::List &b);

    /** Assign instructions to functions.
     *
     *  This starts with instructions that are marked as function entry points and traverses certain edges of the control flow
     *  graph to assign reachable instructions to the same function. It's possible for an instruction to be owned by any number
     *  of functions -- including none at all -- although most instructions will normally be owned by one function.
     *
     *  Thread safety: This function is not thread safe. */
    std::map<rose_addr_t /*funcVa*/, AddressSet /*insnVas*/> assignFunctions();

    /** Build results from CFG.
     *
     *  Clears and repopulates the specified partitioner object with information from this partitioner's global control flow
     *  graph. */
    void transferResults(Rose::BinaryAnalysis::Partitioner2::Partitioner &out);
};


} // namespace
} // namespace
} // namespace
} // namespace
} // namespace
#endif
#endif
