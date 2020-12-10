#ifndef ROSE_BinaryAnalysis_Reachability_H
#define ROSE_BinaryAnalysis_Reachability_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BitFlags.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <Partitioner2/ControlFlowGraph.h>
#include <Sawyer/Map.h>
#include <Sawyer/Tracker.h>
#include <set>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

/** Analysis that computes reachability of CFG vertices.
 *
 *  Certain CFG vertices are marked as intrinsically reachable, such as program entry points, exported functions, signal
 *  handlers, etc., and then reachability is propagated through the graph. */
class Reachability {
public:
    /** Predefined bit flags for why something is reachable. */
    enum Reason {
        // Predefined reasons
        NOT_REACHABLE           = 0,                    /**< Vertex is not reachable. */
        PROGRAM_ENTRY_POINT     = 0x00000001,           /**< Vertex is a program entry point. */
        EXPORTED_FUNCTION       = 0x00000002,           /**< Vertex is an exported function. */
        SIGNAL_HANDLER          = 0x00000004,           /**< Vertex is a signal handler. */
        ASSUMED                 = 0x00000080,           /**< Assumed reachable for cases when the analysis wasn't run. */
        EXPLICIT_MEM_CONSTANT   = 0x00000100,           /**< Address appears in memory. */
        EXPLICIT_INSN_CONSTANT  = 0x00000200,           /**< Address appears as a constant in some reachable instruction. */
        IMPLICIT_FUNC_CONSTANT  = 0x00000400,           /**< Address appears during data-flow in some reachable function. */

        // User-defined reasons
        USER_DEFINED_0          = 0x00010000,           /**< User-defined reason. */
        USER_DEFINED_1          = 0x00020000,           /**< User-defined reason. */
        USER_DEFINED_2          = 0x00040000,           /**< User-defined reason. */
        USER_DEFINED_3          = 0x00080000,           /**< User-defined reason. */
        USER_DEFINED_4          = 0x00100000,           /**< User-defined reason. */
        USER_DEFINED_5          = 0x00200000,           /**< User-defined reason. */
        USER_DEFINED_6          = 0x00400000,           /**< User-defined reason. */
        USER_DEFINED_7          = 0x00800000,           /**< User-defined reason. */
        USER_DEFINED_8          = 0x01000000,           /**< User-defined reason. */
        USER_DEFINED_9          = 0x02000000,           /**< User-defined reason. */
        USER_DEFINED_10         = 0x04000000,           /**< User-defined reason. */
        USER_DEFINED_11         = 0x08000000,           /**< User-defined reason. */
        USER_DEFINED_12         = 0x10000000,           /**< User-defined reason. */
        USER_DEFINED_13         = 0x20000000,           /**< User-defined reason. */
        USER_DEFINED_14         = 0x40000000,           /**< User-defined reason. */
        USER_DEFINED_15         = 0x80000000            /**< User-defined reason. */
    };

    /** Bit flags for reachability. */
    typedef BitFlags<Reason, uint32_t> ReasonFlags;

    /** Settings controlling the analysis.
     *
     *  The members of type @ref ReasonFlags enable a particular feature as well as specify which @ref Reason values to
     *  use for that feature. If the member is an empty set of reasons, then the feature is disabed. */
    struct Settings {
        ReasonFlags markingEntryFunctions;               /**< If not empty, run @ref markEntryFunctions at startup. */
        ReasonFlags markingExportFunctions;              /**< If not empty, run @ref markExportFunctions at startup. */
        ReasonFlags markingExplicitMemoryReferents;      /**< If not empty, run @ref markExplicitMemoryReferents at startup. */
        ReasonFlags markingExplicitInstructionReferents; /**< If not empty, run @ref markExplicitInstructionReferents during iteration. */
        ReasonFlags markingImplicitFunctionReferents;    /**< If not empty, run @ref markImplicitFunctionReferents during iteration. */

        rose_addr_t addressAlignment;                    /**< Alignment when reading constants from virtual memory. */
        size_t addressNBytes;                            /**< Size of addresses when reading constants from virtual memory. */
        ByteOrder::Endianness byteOrder;                 /**< Byte order to use when reading constants from virtual memory. */

        bool precomputeImplicitFunctionReferents;        /**< Implicit function referents are precomputed in parallel. */
        Sawyer::Optional<size_t> nThreads;               /**< Parallelism; 0 means system; unset means use global value. */
        

        Settings()
            : markingEntryFunctions(PROGRAM_ENTRY_POINT),
              markingExportFunctions(EXPORTED_FUNCTION),
              markingExplicitMemoryReferents(EXPLICIT_MEM_CONSTANT),
              markingExplicitInstructionReferents(EXPLICIT_INSN_CONSTANT),
              markingImplicitFunctionReferents(NOT_REACHABLE), // disabled by default because it's slow
              addressAlignment(0), addressNBytes(0), byteOrder(ByteOrder::ORDER_UNSPECIFIED),
              precomputeImplicitFunctionReferents(true)
            {}
    };

    /* Mapping from functions to sets of CFG vertex IDs. */
    typedef Sawyer::Container::Map<Partitioner2::Function::Ptr, std::set<size_t/*vertexId*/> > FunctionToVertexMap;

public:
    /** Facility for emitting diagnostics. */
    static Diagnostics::Facility mlog;

private:
    Settings settings_;                                   // settings that affect behavior
    std::vector<ReasonFlags> intrinsicReachability_;      // intrinsic reachability of each vertex in the CFG
    std::vector<ReasonFlags> reachability_;               // computed reachability of each vertex in the CFG
    FunctionToVertexMap dfReferents_;                     // results from findImplicitFunctionReferents
    Sawyer::Container::Tracker<size_t> scannedVertexIds_; // vertex IDs that have been used for marking intrinsic reachability

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        // Version zero files are no longer supported. I don't think there are two many of these around anyway and supporting
        // backward compatibility for version 0 is not trivial.
        if (version < 1) {
            mlog[Sawyer::Message::FATAL] <<"cannot de-serialized version " <<version <<" object\n";
            ASSERT_not_reachable("aborting due to unsupported old serialization file format");
        }
        s & BOOST_SERIALIZATION_NVP(intrinsicReachability_);
        s & BOOST_SERIALIZATION_NVP(reachability_);
        s & BOOST_SERIALIZATION_NVP(intrinsicReachability_);
        s & BOOST_SERIALIZATION_NVP(reachability_);
        // s & BOOST_SERIALIZATION_NVP(settings_); -- not serialized
        // s & BOOST_SERIALIZATION_NVP(dfReferents_); -- not serialized
        // s & BOOST_SERIALIZATION_NVP(scannedVertexIds_); -- not serialized
    }
#endif

public:
    /** Default constructor.
     *
     *  Constructs a new analysis object with an empty control flow graph. */
    Reachability() {}

    /** Initialize diagnostic streams.
     *
     *  This is called automatically by @ref Rose::Diagnostics::initialize. */
    static void initDiagnostics();

    /** Property: Settings that influence the analysis.
     *
     * @{ */
    const Settings& settings() const { return settings_; }
    Settings& settings() { return settings_; }
    void settings(const Settings &s) { settings_ = s; }
    /** @} */

    /** Describes how to parse switches related to reachability.
     *
     *  The returned switch group is intended to be inserted into a command-line parser. The input values of the @p settings
     *  are documented as the defaults, and after applying the parse resuts will contain the arguments of the corresponding
     *  switches. */
    static Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings &settings /*in,out*/);

    /** Convert a reachability reason enum constant to a parsable reason name.
     *
     *  This is used for building command-line parsers. The returned string is the same as what would be recognized by the
     *  command-line parser for the specified enum constant. */
    static std::string reasonArgument(Reason);

    /** Creates a command-line parser for reachability reason names.
     *
     *  The parser accepts a list of reason names (those strings returned by @ref reasonArgument) separated by commas. It
     *  parses each string of the list, converts it to a @ref Reason enum constant, and sets that flag in the provided @p
     *  storage variable (but not until the parsed command-line is applied). If the string is that corresponding to @ref
     *  NOT_REACHABLE, then the storage is cleared. */
    static Sawyer::CommandLine::ValueParser::Ptr reasonParser(ReasonFlags &storage);

    /** Documentation for the possible reason arguments.
     *
     *  This returns a documentation string describing all possible strings accepted by the @ref reasonParser. */
    static std::string reasonArgumentDocumentation();

    /** Insert a switch that takes a reason argument.
     *
     *  Inserts the specified switch into the switch group, and also inserts a "no-*" version of the switch that behaves as
     *  if the primary switch were given the reason string corresponding to @ref NOT_REACHABLE. The @p storage should already
     *  be initialized with a default set of flags. The @p dfltArg is the argument used for the switch if none is given on the
     *  command-line. In other words, the initial value of @p storage are the reasons that will result if the user doesn't
     *  specify the switch, and the @p dfltArg value is used if the user specifies the switch but no argument. */
    static void insertReasonSwitch(Sawyer::CommandLine::SwitchGroup&, const std::string &switchName, ReasonFlags &storage,
                                   Reason dfltArg, const std::string &doc);

    /** Generate documentation for the command-line switches.
     *
     *  The return value is a string suitable for incorporation into the documentation strings for the command-line parser.  If
     *  the @p dflt is something other than @ref NOT_REACHABLE, then it's used to indicate a default argument for the
     *  command-line switch. */
    static std::string reasonDocumentation(Reason dflt);

    /** Clear previous results.
     *
     *  This does not clear the settings, only the results. */
    void clear();

    /** Clear all reachability.
     *
     *  This clears all reachability (marking all vertices as not reachable) without throwing away any other data. */
    void clearReachability();

    /** Mark starting points as intrinsically reachable according to settings.
     *
     *  Marks the initial, intrinsically reachable starting points in the graph according the the analysis settings. These are
     *  starting points whose reachability doesn't depend directly or indirectly on any other vertices. For example, the entry
     *  points of exported functions are intrinsically reachable regardless of what other vertices are reachable because these
     *  functions can be presumably called from other specimens.
     *
     *  If the analysis settings specify that memory should be scanned to find addresses of basic blocks to mark as being
     *  intrinsically reachable, then the supplied memory map is used for this purpose. If the map is a null pointer, then the
     *  map from the partitioner is used instead.
     *
     *  Returns number of affected vertices.
     *
     *  See also, @ref markEntryFunctions, @ref markExportFunctions, @ref markMemoryConstants. */
    size_t markStartingPoints(const Partitioner2::Partitioner&, MemoryMap::Ptr map = MemoryMap::Ptr());

    /** Mark entry points as intrinsically reachable.
     *
     *  Mark all specimen entry points as intrinsically reachable. For example, ELF files will likely have their "_start"
     *  functions marked this way (although the marking is based on the ELF header, not the name of the function).
     *
     *  Returns number of affected vertices. */
    size_t markEntryFunctions(const Partitioner2::Partitioner&, ReasonFlags how = PROGRAM_ENTRY_POINT);

    /** Mark exported functions as intrinsically reachable.
     *
     *  Marks all entry vertices of exported functions as intrinsically reachable.
     *
     *  Returns number of affected vertices. */
    size_t markExportFunctions(const Partitioner2::Partitioner&, ReasonFlags how = EXPORTED_FUNCTION);

    /** Mark vertices whose addresses appear in instructions.
     *
     *  Traverses the specified basic block to find all explicit constants. For each constant which is the address
     *  of a basic block, mark that block as intrinsically reachable. */
    size_t markExplicitInstructionReferents(const Partitioner2::Partitioner&, const Partitioner2::BasicBlock::Ptr&,
                                            ReasonFlags how = EXPLICIT_INSN_CONSTANT);

    /** Mark vertices whose addresses appear in memory.
     *
     *  Scans all bytes of the supplied memory map to find integer constants of the specified size, alignment, and byte
     *  order. If the integer is the starting address of a basic block, then that CFG vertex is marked as reachable with the
     *  specified @p how flags.
     *
     *  If @p bytesPerWord, @p alignment, or @p sex are default values, then the analysis @ref settings are used, or else if
     *  those are defaults, the values are obtained from the specimen architecture definition.
     *
     *  Returns number of affected vertices. */
    size_t markExplicitMemoryReferents(const Partitioner2::Partitioner&, const MemoryMap::Ptr&, size_t bytesPerWord = 0,
                                       size_t alignment = 0, ByteOrder::Endianness sex = ByteOrder::ORDER_UNSPECIFIED,
                                       ReasonFlags how = EXPLICIT_MEM_CONSTANT);

    /** Mark vertices whose addressses appear in a function.
     *
     *  This looks for implicit values in the function by performing a symbolic data-flow analysis and then examining the
     *  state after each basic block in order to find concrete values. Any concrete value that's the start of some basic
     *  block causes that basic block to become intrinsically reachable. */
    size_t markImplicitFunctionReferents(const Partitioner2::Partitioner&, const Partitioner2::Function::Ptr&,
                                         ReasonFlags how = IMPLICIT_FUNC_CONSTANT);

    /** Mark all basic blocks with in the specified ranges.
     *
     *  Any basic block that has an instruction beginning in any of the address intervals is marked with the given reason.
     *
     *  Returns number of affected vertices. */
    size_t markSpecifiedVas(const Partitioner2::Partitioner&, const std::vector<AddressInterval>&, ReasonFlags);

    /** Change intrinsic reachability for one vertex.
     *
     *  The intrinsic reachability of the specified vertex is changed to @p how, which is a bit vector of @ref Reason
     *  bits. Changing the intrinsic reachability of a vertex to @ref NOT_REACHABLE does not necessarily mark the vertex as
     *  unreachable since it might be reachable from other reachable vertices.
     *
     *  Returns number of affected vertices (0 or 1). */
    size_t intrinsicallyReachable(size_t vertexId, ReasonFlags how);

    /** Change intrinsic reachabability for multiple vertices.
     *
     *  Returns number of affected vertices. */
    template<class ForwardIterator>
    size_t intrinsicallyReachable(ForwardIterator begin, ForwardIterator end, ReasonFlags how) {
        size_t nChanged = 0;
        while (begin != end)
            nChanged += intrinsicallyReachable(*begin++, how);
        return nChanged;
    }

    /** Predicate to match reason flags.
     *
     *  Returns true if the @p reasons matches the other arguments. Returns true if all of the following are true:
     *
     *  @li @p any is non-empty and @p reasons contains at least one of those bits, or @p any is empty and @p reasons contains
     *      any bit.
     *
     *  @li @p reasons contains all the bits that are set in @p all.
     *
     *  @li @p reasons contains none of the bits that are set in @p none. */
    static bool hasReasons(ReasonFlags reasons,
                           ReasonFlags any = ReasonFlags(), ReasonFlags all = ReasonFlags(), ReasonFlags none = ReasonFlags());
    
    /** Query whether a vertex is intrinsic reachable..
     *
     *  Returns true if the specified vertex's intrinsic reachability matches according to @ref hasReasons. */
    bool isIntrinsicallyReachable(size_t vertexId,
                                  ReasonFlags any = ReasonFlags(), ReasonFlags all = ReasonFlags(),
                                  ReasonFlags none = ReasonFlags()) const;

    /** Query whether a vertex is reachable.
     *
     *  Returns true if the specified vertex's computed reachability matches according to @ref hasReasons. */
    bool isReachable(size_t vertexId,
                     ReasonFlags any = ReasonFlags(), ReasonFlags all = ReasonFlags(),
                     ReasonFlags none = ReasonFlags()) const;

    /** Query intrinsic reachability.
     *
     *  When a vertex ID is specified, then return the reachability reasons for that one vertex, or an empty set of reasons
     *  if the ID is out of range.
     *  
     *  When no arguments are specified, return all intrinsic reachability information.
     *
     * @{ */
    ReasonFlags intrinsicReachability(size_t vertexId) const;
    const std::vector<ReasonFlags>& intrinsicReachability() const;
    /** @} */

    /** Query computed reachability.
     *
     *  When a vertex ID is specified, then return the reachability reasons for that one vertex, or an empty set of reasons
     *  if the ID is out of range.
     *  
     *  When no arguments are specified, return all computed reachability information.
     *
     * @{ */
    ReasonFlags reachability(size_t vertexId) const;
    const std::vector<ReasonFlags>& reachability() const;
    /** @} */
    
    /** Return a list of reachable vertex IDs.
     *
     *  Returns the sorted list of vertex IDs that have at least one of the @p any flags (or at least one flag if @p any is
     *  empty), all of the @p all flags, and none of the @p none flags. The return value is sorted by increasing vertex
     *  ID. */
    std::vector<size_t> reachableVertices(ReasonFlags any = ReasonFlags(), ReasonFlags all = ReasonFlags(),
                                          ReasonFlags none = ReasonFlags()) const;
    
    /** Propagate intrinsic reachability through the graph.
     *
     *  This runs a data-flow analysis to propagate the intrinsic reachability bits through the graph.
     *
     *  Returns the number of affected vertices.  If a vector of vertex ID is supplied as an argument, then any vertex whose
     *  reachability has changed will be appended to that vector.
     *
     * @{ */
    size_t propagate(const Partitioner2::Partitioner&);
    size_t propagate(const Partitioner2::Partitioner&, std::vector<size_t> &changedVertexIds /*in,out*/);
    /** @} */

    /** Iteratively propagate and mark.
     *
     *  This function calls runs a @ref propagate step and a marking step in a loop until a steady state is reached. The
     *  marking step looks for new basic blocks that can be marked as intrinsically reachable based on the newly reachable
     *  blocks from the previous propagate step. */
    void iterate(const Partitioner2::Partitioner &partitioner);

    /** Find all CFG vertices mentioned explicitly in a basic block.
     *
     * Scans the instructions of the specified basic block to look for explicit constants (i.e., "immediates") that are
     * addresses of other basic blocks, and return the CFG vertex ID numbers for those other blocks. */
    static std::set<size_t>
    findExplicitInstructionReferents(const Partitioner2::Partitioner&, const Partitioner2::BasicBlock::Ptr&);

    /** Find all CFG vertices mentioned in memory.
     *
     *  Scans the specified memory and reads values that satisfy the size, alignment, and byte order and returns the set
     *  of CFG vertices (by ID) for which starting addresses were found. If @p bytesPerWord, @p alignment, or @p sex are
     *  default values, then the analysis @ref settings are used, or else if those are defaults, the values are obtained
     *  from the specimen architecture definition. */
    std::set<size_t>
    findExplicitMemoryReferents(const Partitioner2::Partitioner&, const MemoryMap::Ptr&, size_t bytesPerWord = 0,
                                size_t alignment = 0, ByteOrder::Endianness sex = ByteOrder::ORDER_UNSPECIFIED);

    /** Find all CFG vertices referenced from a function.
     *
     *  Performs a symbolic data-flow analysis on the specified function in order to find all CFG vertices that are mentioned
     *  in the semantic state. */
    static std::set<size_t>
    findImplicitFunctionReferents(const Partitioner2::Partitioner&, const Partitioner2::Function::Ptr&);

private:
    // Implementation of the "propagate" functions
    size_t propagateImpl(const Partitioner2::Partitioner&, std::vector<size_t>*);

    // Resize vectors based on partitioner CFG size
    void resize(const Partitioner2::Partitioner&);

    // Run findImplicitFunctionReferents on all (or specified) functions in parallel and cache the results
    void cacheAllImplicitFunctionReferents(const Partitioner2::Partitioner&);
    void cacheImplicitFunctionReferents(const Partitioner2::Partitioner&, const std::set<Partitioner2::Function::Ptr>&);

    // Do the marking part of the "iterate" function.
    size_t iterationMarking(const Partitioner2::Partitioner&, const std::vector<size_t> &vertexIds);
};

} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Reachability, 1);

#endif
#endif
