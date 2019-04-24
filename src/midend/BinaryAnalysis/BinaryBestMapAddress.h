#ifndef ROSE_BinaryAnalysis_BestMapAddress
#define ROSE_BinaryAnalysis_BestMapAddress

#include <Diagnostics.h>
#include <Partitioner2/Engine.h>
#include <Progress.h>
#include <RoseException.h>
#include <Sawyer/Set.h>

namespace Rose {
namespace BinaryAnalysis {

/** Finds best address for mapping code.
 *
 *  This analysis looks at function addresses and function calls to create two lists of addresses: entry addresses and target
 *  addresses, respectively. Only function calls using absolute addressing are considered. It then calculates how much the set
 *  of entry addresses should be shifted up or down in memory in order to best line up with target addresses.  The assumption
 *  is that most of the function calls found by disassembling refer to functions that were also disassembled, and that by
 *  shifting the entire executable to higher or lower addresses we can get these to match up. */
class BestMapAddress {
public:
    /** Exceptions for this analysis. */
    class Exception: public Rose::Exception {
    public:
        /** Construct an exception with a message. */
        explicit Exception(const std::string &mesg)
            : Rose::Exception(mesg) {}

        /** Destructor. */
        ~Exception() throw () {}
    };

    /** Set of addresses. */
    typedef Sawyer::Container::Set<rose_addr_t> AddressSet;

    /** Facility for emitting diagnostics. */
    static Diagnostics::Facility mlog;

private:
    typedef Sawyer::Container::Map<size_t /*nMatches*/, std::vector<rose_addr_t>/*deltas*/> MatchedDeltas;

    AddressSet entryVas_;                               // set of function entry virtual addresses
    AddressSet targetVas_;                              // set of call target (callee) addresses
    MatchedDeltas results_;                             // results of analyze() call
    bool upToDate_;                                     // are results_ up-to-date w.r.t. address sets?
    size_t maxMatches_;                                 // maximum number of matches possible for results_
    size_t nBits_;                                      // number of bits in an address
    Progress::Ptr progress_;                            // for progress reporting

public:
    /** Construct an empty analysis. */
    BestMapAddress()
        : upToDate_(true), maxMatches_(0), nBits_(0), progress_(Progress::instance()) {}

    /** Clear gathered addresses.
     *
     *  Returns the analysis to its initial state but without clearing any properties that might have been configured. To clear
     *  the analysis and reset properties, assign a default constructed analysis.
     *
     *  Note this method is named "reset" instead of "clear" because it doesn't entirely clear the object in the sense that STL
     *  "clear" methods clear their objects. */
    void reset() {
        entryVas_.clear();
        targetVas_.clear();
        results_.clear();
        maxMatches_ = 0;
        upToDate_ = true;
    }

    /** Initialize diagnostic streams.
     *
     *  This is called automatically by @ref Rose::Diagnostics::initialize. */
    static void initDiagnostics();

    /** Property: Progress reporter.
     *
     *  Optional progress reporter object. If non-null, then some of the long-running methods will update this object with
     *  periodic progress reports.
     *
     * @{ */
    Progress::Ptr progress() const { return progress_; }
    void progress(const Progress::Ptr &p) { progress_ = p; }
    /** @} */

    /** Property: Number of bits in an address.
     *
     *  This analysis represents address deltas using the same C data type as addresses, namely @ref rose_addr_t. Therefore,
     *  the analysis needs to know the total size of the address space in order to represent negative deltas. The address space
     *  size is stored as a power of two.
     *
     *  Functions that have enough information to determine the address space size (such as from a partitioner engine), set
     *  this property as part of their operation.
     *
     *  Returns zero if the address size is not yet known. An address space of size one is never valid.
     *
     * @{ */
    size_t nBits() const { return nBits_; }
    void nBits(size_t n);
    /** @} */

    /** Property: Mask to use when doing address arithmetic.
     *
     *  This is the bit mask to use for doing address arithmetic. It has the N low-order bits set, where N is the number of
     *  bits stored in the @ref nBits property. */
    rose_addr_t mask() const;

    /** Gather addresses for future analysis.
     *
     *  Given a configured partitioning engine with a memory map already defined, disassemble and partition the executable
     *  areas of the map that fall within the specified interval and create the two lists of addresses used by future
     *  analysis.  See the class description for details about these lists.
     *
     *  Although calls to this method can be expensive, the progress report associated with this analyzer (if any) is not used;
     *  rather, the progress reporting associated with the @p engine is used instead. */
    void gatherAddresses(Partitioner2::Engine &engine);

    /** Insert a function entry address.
     *
     *  Inserts a function entry address into the set of addresses that will be used during later analysis. */
    void insertEntryAddress(rose_addr_t va) {
        if (entryVas_.insert(va))
            upToDate_ = false;
    }

    /** Insert a call target address.
     *
     *  Insert the target address (i.e., the callee) of a function call into the set of addresses that will be used during
     *  later analysis. */
    void insertTargetAddress(rose_addr_t va) {
        if (targetVas_.insert(va))
            upToDate_ = false;
    }

    /** Returns set of entry addresses. */
    const AddressSet& entryAddresses() const {
        return entryVas_;
    }

    /** Returns set of target addresses. */
    const AddressSet& targetAddresses() const {
        return targetVas_;
    }

    /** Analyze the addresses that have been provided.
     *
     *  Analyses all the previously provided addresses to find the best amounts by which to shift the entry addresses so they
     *  line up with the target addresses. Only those entry addresses and target addresses that fall within the specified
     *  intervals are used.
     *
     *  This method operates in parallel according to the global setting for the number of threads. That property is normally
     *  configured with the --threads command-line switch and stored in @ref CommandlineProcessing::GenericSwitchArgs::threads.
     *
     *  If a progress reporting object has been configured for this analysis, then this method periodically updates it with
     *  progress reports.
     *
     *  Returns a reference to the analysis object so for easy chaining to a query. */
    BestMapAddress& analyze(const AddressInterval &restrictEntryAddresses = AddressInterval::whole(),
                            const AddressInterval &restrictTargetAddresses = AddressInterval::whole());

    /** Return the best shift amounts.
     *
     *  Returns a list of the best shift amounts. Usually this is a single value, but in the case when multiple shift amounts
     *  tie for best, they're all returned. */
    const std::vector<rose_addr_t>& bestDeltas() const;

    /** How well the best shift amounts performed.
     *
     *  Returns a ratio between zero and one, inclusive, that indicates how will the best deltas performed.  A return value of
     *  zero means that shifting the functions by any of the best deltas caused none of the function call targets to line up
     *  with functions, and a return value of one means all of them line up. */
    double bestDeltaRatio() const;

    /** Align executable regions of a memory map.
     *
     *  Runs the BestMapAddress repeatedly on the specified memory map in order to create a new map with the executable regions
     *  of the input map moved to better addresses. */
    static MemoryMap::Ptr align(const MemoryMap::Ptr&,
                                const Partitioner2::Engine::Settings &settings = Partitioner2::Engine::Settings(),
                                const Progress::Ptr &progress = Progress::Ptr());
};

} // namespace
} // namespace

#endif
