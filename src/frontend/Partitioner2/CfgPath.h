#ifndef ROSE_Partitioner2_CfgPath_H
#define ROSE_Partitioner2_CfgPath_H

#include <Partitioner2/ControlFlowGraph.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** A path through a control flow graph.
 *
 *  A CFG path consists of a starting CFG vertex plus zero or more CFG edges. The first edge is an outgoing edge of the
 *  starting vertex and subsequent edges must be connected through inter-edge vertices.  An empty path is a path with no edges
 *  and no starting vertex.  A path acts like a stack in that edges can be pushed and popped from the end of the path. */
class CfgPath {
public:
    /** Stack of inter-connected edges. */
    typedef std::vector<ControlFlowGraph::ConstEdgeIterator> Edges;

    /** Stack of vertices. */
    typedef std::vector<ControlFlowGraph::ConstVertexIterator> Vertices;

    /** Stores user-defined attributes. */
    typedef Sawyer::Attribute::Storage<Sawyer::SingleThreadedTag> Attributes;

private:
    Sawyer::Optional<ControlFlowGraph::ConstVertexIterator> frontVertex_;
    Edges edges_;

    // Optional edge ordering information. This vector parallels, "edges_", although it can be smaller if default ordering is
    // used. Assuming the vectors are the same size, then edgeOrder_[i] contains the back-tracking information for
    // edges_[i]. If edges_[i] is popped during backtracking and edgeOrder_[i] exists and is not empty, then edges_[i] will be
    // replaced by edgeOrders_[i].back() and that edge is popped from edgeOrders_[i].
    std::vector<Edges> edgeOrders_;

    // Attributes stored at each vertex and edge
    std::vector<Attributes> vertexAttributes_;
    std::vector<Attributes> edgeAttributes_;

public:
    /** Construct an empty path. */
    CfgPath() {}

    /** Construct a path having only a starting vertex. */
    explicit CfgPath(const ControlFlowGraph::ConstVertexIterator &vertex)
        : frontVertex_(vertex), vertexAttributes_(1, Attributes()) {}

    /** Construct a path given an initial edge. */
    explicit CfgPath(const ControlFlowGraph::ConstEdgeIterator &edge)
        : frontVertex_(edge->source()), edges_(1, edge), vertexAttributes_(2, Attributes()), edgeAttributes_(1, Attributes()) {}

    /** Makes this path empty. */
    void clear() {
        frontVertex_ = Sawyer::Nothing();
        edges_.clear();
        edgeOrders_.clear();
        vertexAttributes_.clear();
        edgeAttributes_.clear();
    }

    /** Determine if a path is empty. */
    bool isEmpty() const {
        return !frontVertex_;
    }

    /** Verify that path edges are connected.
     *
     *  Checks whether adjacent edges in the path go through a common vertex. Returns true if they do, false otherwise. Returns
     *  true for a path with no edges. */
    bool isConnected() const;

    /** Number of edges in a path.
     *
     *  A path with zero edges is not necessarily empty; it may have an initial vertex. */
    size_t nEdges() const {
        return edges_.size();
    }

    /** Number of vertices in a path.
     *
     *  The number of vertices in a non-empty path is one more than the number of edges. An empty path has zero vertices. */
    size_t nVertices() const {
        return isEmpty() ? 0 : (1+nEdges());
    }

    /** Returns the vertex where the path starts.
     *
     *  The path must not be empty. */
    ControlFlowGraph::ConstVertexIterator frontVertex() const {
        ASSERT_forbid(isEmpty());
        return *frontVertex_;
    }

    /** Returns the vertex where the path ends.
     *
     *  The path must not be empty. */
    ControlFlowGraph::ConstVertexIterator backVertex() const {
        ASSERT_forbid(isEmpty());
        return edges_.empty() ? *frontVertex_ : edges_.back()->target();
    }

    /** Returns all the edges in a path.
     *
     *  A path with no edges is not necessarly an empty path; it may have an initial vertex. */
    const Edges& edges() const {
        return edges_;
    }

    /** Return all the vertices in a path.
     *
     *  The list of vertices is not stored explicitly by this path object and must be recomputed for each call. Vertices are
     *  not necessarily unique within a path since they can be reached sometimes by multiple edges. */
    Vertices vertices() const;

    /** Append a new edge to the end of the path.
     *
     *  If the path is not empty then the source vertex for the new edge must be equal to the @ref backVertex. The specified
     *  edge is pushed onto the path and the path is configured to visit the sibling edges during the @ref backtrack operation
     *  in order by incrementing the given iterator. */
    void pushBack(ControlFlowGraph::ConstEdgeIterator edge);

    /** Append a new edge to the end of the path.
     *
     *  The argument is a list of edges all originating from the same vertex. If the path is non-empty, then the originating
     *  vertex must be equal to the @ref backVertex.  The argument specifies the order in which the @ref backtrack operation
     *  will visit the edges, and only the first edge of this list is actually appended to the path. */
    void pushBack(const std::vector<ControlFlowGraph::ConstEdgeIterator> &edges);

    /** Insert a new edge to the front of the path.
     *
     *  If the path is not empty, then the target vertex for the new edge must be equal to the @ref frontVertex. The specified
     *  edge is inserted at the front of the path and the path is configured to visit the sibling edges during the @ref
     *  backtrack operation in order by incrementing the given iterator.
     *
     *  Pushing edges onto the front of a path is not efficient; it requires moving all previous edges, taking time linearly
     *  proportional to the length of the path. */
    void pushFront(ControlFlowGraph::ConstEdgeIterator edge);

    /** Insert a new edge to the front of the path.
     *
     *  The argument is a list of edges all pointing to the same vertex. If the path is non-empty, then the pointed to vertex
     *  must be equal to @ref frontVertex.  The argument specifies the order in which the @ref backtrack operation will visit
     *  the edges, and only the first edge of this list is actually inserted at the front of the path. */
    void pushFront(const std::vector<ControlFlowGraph::ConstEdgeIterator> &edges);

    /** Erase the final edge from a path.
     *
     *  Erasing the only remaining edge will leave the path in a state where it has only a starting vertex and no
     *  edges. Calling this method on such a path will remove the starting vertex. This method should not be called if the path
     *  is empty (has no edges and no starting vertex). */
    void popBack();

    /** Backtrack to next path.
     *
     *  Pops edges from the path until a vertex is reached where some other (later) edge can be followed, then push that edge
     *  onto the path.  If no subsequent path through the CFG is available, then modify this path to be empty. This happens
     *  when this path's edges are all final outgoing edges for each vertex in the path.
     *
     *  Returns the edges that were removed in the order that they were removed. I.e., the first edge popped from the end of
     *  the path is at the front of the returned vector. */
    std::vector<ControlFlowGraph::ConstEdgeIterator>  backtrack();

    /** User-defined attributes for the nth vertex.
     *
     *  Each vertex in the path has a corresponding attribute storage system. The attribute storage lifetime is the same as
     *  that of the vertex to which it corresponds; when the path through the vertex index changes, the storage is reset. Note
     *  that there is always one more vertex than edge (except when the path is completely empty). Edge number @em i
     *  has two endpoints that are vertices @em i and @em i+1.
     *
     * @{ */
    Attributes& vertexAttributes(size_t);
    const Attributes& vertexAttributes(size_t) const;
    /** @} */

    /** User-defined attributes for the nth edge.
     *
     *  Each edge in the path has a corresponding attribute storage system. The attribute storage lifetime is the same as
     *  that of the edge to which it corresponds; when the path through the edge index changes, the storage is reset. Note
     *  that there is always one more vertex than edge (except when the path is completely empty). Edge number @em i
     *  has two endpoints that are vertices @em i and @em i+1.
     *
     * @{ */
    Attributes& edgeAttributes(size_t);
    const Attributes& edgeAttributes(size_t) const;
    /** @} */

    /** Number of times vertex appears in path. */
    size_t nVisits(const ControlFlowGraph::ConstVertexIterator &vertex) const;

    /** Number of times edge appears in path. */
    size_t nVisits(const ControlFlowGraph::ConstEdgeIterator &edge) const;

    /** Number of function calls.
     *
     *  Counts the number of E_FUNCTION_CALL edges in a path.  If a non-null function is supplied then only count those edges
     *  that enter the specified function. */
    size_t nCalls(const Function::Ptr &function = Function::Ptr()) const;

    /** Number of function returns.
     *
     *  Counts the number of E_FUNCTION_RETURN edges in a path. If a non-null function is supplied then only count those edges
     *  that return from the specified function. */
    size_t nReturns(const Function::Ptr &function = Function::Ptr()) const;

    /** Call depth.
     *
     *  Returns the function call depth at the end of the path.  The call depth is incremented for each E_FUNCTION_CALL edge
     *  and decremented for each E_FUNCTION_RETURN edge, and the value at the end of the path is returned. If a non-null
     *  function is specified, then count only calls to that function and returns from that function. The return value may be
     *  negative if more return edges than call edges are encountered. */
    ssize_t callDepth(const Function::Ptr &function = Function::Ptr()) const;

    /** Maximum call depth.
     *
     *  Returns the maximum function call depth in the path.  The call depth is incremented for each E_FUNCTION_CALL edge and
     *  decremented for each E_FUNCTION_RETURN edge, and its maximum value is returned. If a non-null function is specified,
     *  then count only calls to that function and returns from that function. */
    size_t maxCallDepth(const Function::Ptr &function = Function::Ptr()) const;

    /** Truncate the path.
     *
     *  Erase edges from the end of this path until this path contains none of the specified edges.
     *
     *  Returns the edges that were removed in the order that they were removed. I.e., the first edge popped from the end of
     *  the path is at the front of the returned vector.
     *
     * @{ */
    std::vector<ControlFlowGraph::ConstEdgeIterator> truncate(const ControlFlowGraph::ConstEdgeIterator&);
    std::vector<ControlFlowGraph::ConstEdgeIterator> truncate(const CfgConstEdgeSet&);
    /** @} */

    /** Print the path. */
    void print(std::ostream &out) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for operating on paths
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Finds edges that can be part of some path.
 *
 *  Returns a Boolean vector indicating whether an edge is significant.  An edge is significant if it appears on some path that
 *  originates from some vertex in @p beginVertices and reaches some vertex in @p endVertices but is not a member of @p
 *  avoidEdges and is not incident to any vertex in @p avoidVertices. An edge is not significant if it is a function call or
 *  function return and @ref avoidCallsAndReturns is true. */
std::vector<bool>
findPathEdges(const ControlFlowGraph &graph,
              const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
              const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
              const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet(), bool avoidCallsAndReturns = false);

/** Find edges that are reachable.
 *
 *  Finds edges that are part of some path from any of the @p beginVertices to any of the @p endVertices. The paths that are
 *  considered must not traverse the @p avoidEdges or @p avoidVertices. */
CfgConstEdgeSet
findPathReachableEdges(const ControlFlowGraph &graph,
                       const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
                       const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet(),
                       bool avoidCallsAndReturns = false);

/** Find edges that are unreachable.
 *
 *  Finds edges that are not part of any path from any of the @p beginVertices to any of the @p endVertices. The paths that are
 *  considered must not traverse the @p avoidEdges or @p avoidVertices. */
CfgConstEdgeSet
findPathUnreachableEdges(const ControlFlowGraph &graph,
                         const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                         const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
                         const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet(),
                         bool avoidCallsAndReturns = false);

/** Remove edges and vertices that cannot be on the paths.
 *
 *  Removes those edges that aren't reachable in both forward and reverse directions between the specified begin and end
 *  vertices. Specified vertices must belong to the graph.  After edges are removed, dangling vertices are removed.  Vertices
 *  and edges are removed from all arguments. Removal of edges from @p path causes the path to be truncated.
 *
 *  Returns the number of edges that were removed from the @p path. */
size_t
eraseUnreachablePaths(ControlFlowGraph &graph /*in,out*/, CfgConstVertexSet &beginVertices /*in,out*/,
                      CfgConstVertexSet &endVertices /*in,out*/, CfgVertexMap &vmap /*in,out*/, CfgPath &path /*in,out*/);

/** Compute all paths.
 *
 *  Computes all paths from any @p beginVertices to any @p endVertices that does not go through any @p avoidVertices or @p
 *  avoidEdges. The paths are returned as a paths graph (CFG) so that cycles can be represented. A paths graph can represent an
 *  exponential number of paths. The paths graph is formed by taking the global CFG and removing all @p avoidVertices and @p
 *  avoidEdges, any edge that cannot appear on a path from the @p beginVertex to any @p endVertices, and any vertex that has
 *  degree zero provided it is not the beginVertex.
 *
 *  If @p avoidCallsAndReturns is true then E_FUNCTION_CALL and E_FUNCTION_RETURN edges are not followed.  Note that the normal
 *  partitioner CFG will have E_CALL_RETURN edges that essentially short circuit a call to a function that might return, and
 *  that E_FUNCTION_RETURN edges normally point to the indeterminate vertex rather than concrete return targets.
 *
 *  If the returned graph, @p paths, is empty then no paths were found.  If the returned graph has a vertex but no edges then
 *  the vertex serves as both the begin and end of the path (i.e., a single path of unit length).  The @p vmap is updated to
 *  indicate the mapping from @p srcCfg vertices in the corresponding vertices in the returned graph. */
void
findPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
          const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
          const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
          const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet(),
          bool avoidCallsAndReturns = false);

/** Compute all paths within one function.
 *
 *  This is a convenience method for @ref findPaths in a mode that avoids function call and return edges. */
void
findFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                  const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                  const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
                  const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet());

/** Compute all paths across function calls and returns.
 *
 *  This is a convenience method for @ref findPaths in a mode that follows function call and return edges. Note that in the
 *  normal partitioner CFG function return edges point to the indeterminate vertex rather than back to the place the function
 *  was called.  In order to get call-sensitive paths you'll have to do something else. */
void
findInterFunctionPaths(const ControlFlowGraph &srcCfg, ControlFlowGraph &paths /*out*/, CfgVertexMap &vmap /*out*/,
                       const CfgConstVertexSet &beginVertices, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
                       const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet());

/** Inline a function at the specified call site.
 *
 *  The @p paths graph is modified in place by inserting an inlined copy of the function(s) called from the specified @p
 *  pathsCallSite vertex.  The @p pathsCallSite only serves as the attachment point--it must have the @ref E_CALL_RETURN
 *  edge(s) but does not need any @ref E_FUNCTION_CALL edges.  The @p cfgCallSite is the vertex in the @p cfg corresponding to
 *  the @p pathsCallSite in the paths graph and provides information about which functions are called.
 *
 *  There are two similar functions: one inlines all the functions called from a particular call site in the CFG, the other
 *  inlines one specific function specified by its entry vertex.  In the latter case, the CFG doesn't actually need to have an
 *  edge to the called function.
 *
 *  Usually, @p cfgCallSite has one outgoing @ref E_FUNCTION_CALL edge and @p pathsCallSite (and @p cfgCallSite) has one
 *  outgoing @ref E_CALL_RETURN edge. If the @p pathsCallSite has no @ref E_CALL_RETURN edge, or the called function has no
 *  return sites, this operation is a no-op.  A call site may call multiple functions, in which case each is inserted, even if
 *  some @ref E_FUNCTION_CALL edges point to the same function.  A called function may return to multiple addresses, such as
 *  longjmp, in which case multiple @ref E_CALL_RETURN edges may be present--all return sites are linked to all return targets
 *  by this operation.
 *
 *  The vertices and edges in the inlined version that correspond to the @p cfgAvoidEVertices and @p cfgAvoidEdges are not
 *  copied into the @ref paths graph. If this results in the called function having no paths that can return, then that
 *  function is not inserted into @p paths.
 *
 *  The @ref E_CALL_RETURN edges in @p paths are not erased by this operation, but are usually subsequently erased by the
 *  user since they are redundant after this insertion--they represent a short-circuit over the called function(s).
 *
 *  Returns true if some function was inserted, false if no changes were made to @p paths.  If @p newVertices is non-null then
 *  all newly inserted vertices are also pushed onto the end of the vector.
 *
 * @{ */
bool
inlineMultipleCallees(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                      const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallSite,
                      const CfgConstVertexSet &cfgAvoidVertices = CfgConstVertexSet(),
                      const CfgConstEdgeSet &cfgAvoidEdges = CfgConstEdgeSet(),
                      std::vector<ControlFlowGraph::ConstVertexIterator> *newEdges = NULL);
bool
inlineOneCallee(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallTarget,
                const CfgConstVertexSet &cfgAvoidVertices, const CfgConstEdgeSet &cfgAvoidEdges,
                std::vector<ControlFlowGraph::ConstVertexIterator> *newVertices = NULL);
/** @} */

/** Binary inliner.
 *
 *  This binary inliner builds a new CFG (called here the paths graph) from a master CFG. The paths graph initially contains
 *  some vertices that correspond to a function call site in the master CFG, and this inliner will copy part of the master CFG
 *  corresponding to the called function into the paths graph.
 *
 *  When a function is copied from the master CFG to the paths graph, only those edges in the called function that participate
 *  in some path from the entry vertex to a returning vertex are copied.  Usually a call site will call a single function, but
 *  an indirect call (e.g., x86 <code>call eax</code>) could have calls to multiple functions, and/or the special indeterminate
 *  vertex (e.g., depending on whether anything is known about @c eax in <code>call eax</code>).  Once all the functions called
 *  from a call site are inlined, the @ref E_CALL_RETURN edge is removed from that vertex.
 *
 *  The inliner has an optional list of blacklisted vertices and edges. These vertices and edges cannot appear in any path.
 *
 *  A user-defined predicate determines whether a function call should be inlined or skipped. */
class Inliner {
public:

    /** What action to take for inlining. */
    enum HowInline {
        INLINE_NONE,                                    /**< Do not inline anything for this call. */
        INLINE_NORMAL,                                  /**< Normal inlining for this call. */
        INLINE_USER,                                    /**< Add a @ref V_USER_DEFINED vertex for this call. */
    };

    /** Predicate to determine whether inlining should be performed.
     *
     *  This is the base class for a predicate that determines whether a function should be inlined. */
    class ShouldInline: public Sawyer::SharedObject {
        size_t maxCallDepth_;                           // max depth for inlining
    protected:
        ShouldInline(): maxCallDepth_(100) {}
    public:
        virtual ~ShouldInline() {}

        /**  Shared ownership pointer to a predicate object. See @ref heap_object_shared_ownership. */
        typedef Sawyer::SharedPointer<ShouldInline> Ptr;

        /** Factory class method. */
        static Ptr instance() { return Ptr(new ShouldInline); }

        /** Property: maximum call depth.
         *
         * @{ */
        size_t maxCallDepth() const { return maxCallDepth_; }
        void maxCallDepth(size_t n) { maxCallDepth_ = n; }
        /** @} */

        /** Whether to inline a function. */
        virtual HowInline operator()(const Partitioner&, const ControlFlowGraph::ConstEdgeIterator cfgCallEdge,
                                     const ControlFlowGraph &paths, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                                     size_t callDepth);
    };

private:
    struct CallSite {
        ControlFlowGraph::ConstVertexIterator pathsVertex;
        size_t callDepth;
        CallSite(const ControlFlowGraph::ConstVertexIterator &pathsVertex, size_t callDepth)
            : pathsVertex(pathsVertex), callDepth(callDepth) {}
    };

    ControlFlowGraph paths_;                            // the resulting paths graph
    CfgVertexMap vmap_;                                 // relates CFG vertices to paths graph vertices
    CfgConstVertexSet pathsBeginVertices_;              // vertices where traversal starts
    CfgConstVertexSet pathsEndVertices_;                // vertices where traversal ends
    std::list<CallSite> workList_;                      // call sites to be processed
    ShouldInline::Ptr shouldInline_;                    // user predicate for whether to inline a call

public:
    /** Default constructor.
     *
     *  This inliner is configured to inline functions to a certain maximum depth. */
    Inliner()
        : shouldInline_(ShouldInline::instance()) {}

    /** Property: inline predicate.
     *
     *  User predicate that controls whether a particular function should be inlined.  If null, then all function calls are
     *  inlined.
     *
     * @{ */
    ShouldInline::Ptr shouldInline() const { return shouldInline_; }
    void shouldInline(const ShouldInline::Ptr &p) { shouldInline_ = p; }
    /** @} */

    /** Construct a CFG with inlined functions.
     *
     *  Computes paths in the partitioner's global CFG from @p cfgBeginVertices to @p cfgEndVertices that do not pass through
     *  @p cfgAvoidVertices or @p cfgAvoidEdges. Any function calls along these paths are then inlined, but only those paths
     *  through the function that don't pass through the @p cfgAvoidVertices or @p cfgAvoidEdges.  If a call satisfies the @ref
     *  shouldSummarizeCall predicate or if the call depth becomes too deep then instead of inlining, the a special summary
     *  vertex is inserted.
     *
     *  Returns the resulting control flow graph, a.k.a., the paths graph. */
    void inlinePaths(const Partitioner &partitioner, const CfgConstVertexSet &cfgBeginVertices,
                     const CfgConstVertexSet &cfgEndVertices, const CfgConstVertexSet &cfgAvoidVertices,
                     const CfgConstEdgeSet &cfgAvoidEdges);

    /** Resulting paths graph.
     *
     *  Returns the paths graph that resulted from inlining. */
    const ControlFlowGraph& paths() const { return paths_; }

    /** Paths begin vertices.
     *
     *  This is the set of vertices corresponding to the @c cfgBeginVertices of the @ref inlinePaths function.  Only those
     *  CFG vertices that are part of a path are in this set. */
    const CfgConstVertexSet& pathsBeginVertices() const { return pathsBeginVertices_; }

    /** Paths end vertices.
     *
     *  This is the set of vertices corresponding to the @c cfgEndVertices of the @ref inlinePaths function. Only those CFG
     *  vertices that are part of a path are in this set. */
    const CfgConstVertexSet& pathsEndVertices() const { return pathsEndVertices_; }

private:
    void reset(const Partitioner &partitioner, const CfgConstVertexSet &cfgBeginVertices,
               const CfgConstVertexSet &cfgEndVertices, const CfgConstVertexSet &cfgAvoidVertices,
               const CfgConstEdgeSet &cfgAvoidEdges);

    // Returns true if pathVertex is a function call. Does so by consulting the corresponding vertex in the partitioner's
    // global CFG.
    static bool isFunctionCall(const Partitioner&, const ControlFlowGraph::ConstVertexIterator &pathVertex);

    // Convert a path vertex to the corresponding vertex in the partitioner's global CFG.
    static ControlFlowGraph::ConstVertexIterator pathToCfg(const Partitioner &partitioner,
                                                           const ControlFlowGraph::ConstVertexIterator &pathVertex);

    // Convert global CFG vertices to paths graph vertices. */
    static CfgConstVertexSet cfgToPaths(const CfgConstVertexSet &vertices, const CfgVertexMap &vmap);
};

std::ostream& operator<<(std::ostream &out, const CfgPath &path);

} // namespace
} // namespace
} // namespace

#endif
