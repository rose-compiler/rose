#ifndef ROSE_BinaryAnalysis_Reachability_H
#define ROSE_BinaryAnalysis_Reachability_H

#include <BitFlags.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <Partitioner2/ControlFlowGraph.h>
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
        NOT_REACHABLE           = 0,                    /**< Vertex is not reachable. */
        ENTRY_POINT             = 0x00000001,           /**< Vertex is a program entry point. */
        EXPORTED_FUNCTION       = 0x00000002,           /**< Vertex is an exported function. */
        SIGNAL_HANDLER          = 0x00000004,           /**< Vertex is a signal handler. */
        ASSUMED                 = 0x00000080,           /**< Assumed reachable for cases when the analysis wasn't run. */
        PREDEFINED              = 0x000000ff,           /**< ROSE-defined reachability reasons. */
        USER_DEFINED            = 0xffffff00,           /**< User-defined reachability reasons. */
        USER_DEFINED_0          = 0x00000100,           /**< First user-defined reason bit. */
        ALL_REASONS             = 0xffffffff            /**< All reason bits. */
    };

    /** Bit flags for reachability. */
    typedef BitFlags<Reason, uint32_t> ReasonFlags;

private:
    Partitioner2::ControlFlowGraph cfg_;                // CFG upon which we're operating
    std::vector<ReasonFlags> intrinsicReachability_;    // intrinsic reachability of each vertex in the CFG
    std::vector<ReasonFlags> reachability_;             // computed reachability of each vertex in the CFG
    bool savingCfg_;                                    // whether to save the CFG when serializing.

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_NVP(savingCfg_);
        if (savingCfg_)
            s & BOOST_SERIALIZATION_NVP(cfg_);
        s & BOOST_SERIALIZATION_NVP(intrinsicReachability_);
        s & BOOST_SERIALIZATION_NVP(reachability_);
    }

    template<class S>
    void load(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(savingCfg_);
        if (savingCfg_)
            s & BOOST_SERIALIZATION_NVP(cfg_);
        s & BOOST_SERIALIZATION_NVP(intrinsicReachability_);
        s & BOOST_SERIALIZATION_NVP(reachability_);

        if (version < 1) {
            // Old versions stored an array of unsigned long
            {
                std::vector<unsigned long> tmp;
                s & boost::serialization::make_nvp("intrinsicReachability_", tmp);
                intrinsicReachability_.reserve(tmp.size());
                BOOST_FOREACH (unsigned long v, tmp)
                    intrinsicReachability_.push_back(v);
            }
            {
                std::vector<unsigned long> tmp;
                s & boost::serialization::make_nvp("reachability_", tmp);
                reachability_.reserve(tmp.size());
                BOOST_FOREACH (unsigned long v, tmp)
                    intrinsicReachability_.push_back(v);
            }
        } else {
            // Newer versions store an array of bit vectors
            s & BOOST_SERIALIZATION_NVP(intrinsicReachability_);
            s & BOOST_SERIALIZATION_NVP(reachability_);
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

public:
    /** Default constructor.
     *
     *  Constructs a new analysis object with an empty control flow graph. */
    Reachability()
        : savingCfg_(true) {}

    /** Property: Control flow graph.
     *
     *  Assigning a new control flow graph to this analysis will erase all previous information. Assigning an empty control
     *  flow graph has the same effect as calling @ref clear.
     *
     * @{ */
    const Partitioner2::ControlFlowGraph& cfg() const /*final*/ { return cfg_; }
    void cfg(const Partitioner2::ControlFlowGraph&);
    /** @} */

    /** Replace CFG without adjusting other data.
     *
     *  Unlike setting the @ref cfg property directly, this version doesn't update any other result data. It's intended to be
     *  used when restoring previous data from a Boost archive when the @ref savingCfg property was clear. The caller must
     *  ensure that the new CFG has the same size and vertex numbering as the original CFG. */
    void replaceCfg(const Partitioner2::ControlFlowGraph&);

    /** Clear previous results.
     *
     *  This is the same as setting the control flow graph to an empty graph. */
    void clear();

    /** Property: Whether to save the CFG when serializing.
     *
     * @{ */
    bool savingCfg() const { return savingCfg_; }
    void savingCfg(bool b) { savingCfg_ = b; }
    /** @} */

    /** Clear all reachability.
     *
     *  This clears all reachability (marking all vertices as not reachable) without throwing away the control flow graph. */
    void clearReachability();

    /** Query intrinsic reachability.
     *
     *  Returns the intrinsic reachability for a vertex. The vertex ID numst be valid for the graph being analyzed. The return
     *  value is a bit vector of @ref Reason bits, some of which might be user-defined. */
    ReasonFlags isIntrinsicallyReachable(size_t vertexId) const;

    /** Controls immediate propagation. */
    struct Propagate {
        enum Boolean {                                  /**< Propagation control type. */
            NO,                                         /**< Do not propagate. */
            YES                                         /**< Do propagate. */
        };
    };

    /** Change intrinsic reachability.
     *
     *  The intrinsic reachability of the specified vertex is changed to @p how, which is a bit vector of @ref Reason
     *  bits. Changing the intrinsic reachability of a vertex to @ref NOT_REACHABLE does not necessarily mark the vertex as
     *  unreachable since it might be reachable from other reachable vertices. The new reachability will be immediately
     *  propagated through the graph if @p doPropagate is set. */
    void intrinsicallyReachable(size_t vertexId, ReasonFlags how, Propagate::Boolean propagate = Propagate::YES);

    /** Query computed reachability.
     *
     *  Returns the computed reachability for a vertex.  The return value is always a superset of the vertex's intrinsic
     *  reachability and is a bit vector of @ref Reason bits. The vertex ID must be valid. */
    ReasonFlags isReachable(size_t vertexId) const;

    /** Computed reachability for all vertices.
     *
     *  The return value is a vector of @ref Reason bit flags for each vertex, indexed by vertex ID. */
    const std::vector<ReasonFlags>& reachability() const;

    /** Mark special vertices for containers.
     *
     *  Scan the list of functions in the partitioner and mark as reachable the entry points of functions whose reason for
     *  existing is because they are program entry points or exports. The specified partitioner's control flow graph must have
     *  vertices numbered consistently with the control flow graph that's been set by the @ref cfg property. */
    void markSpecialFunctions(const Partitioner2::Partitioner&);

    /** Propagate intrinsic reachability through the graph.
     *
     *  This runs a data-flow analysis to propagate the intrinsic reachability bits through the graph. The propagation happens
     *  automatically whenever @ref intrinsicallyReachable is called unless its @p doPropagate parameter is false. */
    void propagate();
};

} // namespace
} // namespace

// Class versions must be at global scope
BOOST_CLASS_VERSION(Rose::BinaryAnalysis::Reachability, 1);

#endif
