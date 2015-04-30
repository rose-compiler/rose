#ifndef ROSE_Partitioner2_CfgPath_H
#define ROSE_Partitioner2_CfgPath_H

#include <Partitioner2/ControlFlowGraph.h>

namespace rose {
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

private:
    Sawyer::Optional<ControlFlowGraph::ConstVertexIterator> frontVertex_;
    Edges edges_;

public:
    /** Construct an empty path. */
    CfgPath() {}

    /** Construct a path having only a starting vertex. */
    explicit CfgPath(const ControlFlowGraph::ConstVertexIterator &vertex): frontVertex_(vertex) {}

    /** Construct a path given an initial edge. */
    explicit CfgPath(const ControlFlowGraph::ConstEdgeIterator &edge)
        : frontVertex_(edge->source()), edges_(1, edge) {}

    /** Makes this path empty. */
    void clear() {
        frontVertex_ = Sawyer::Nothing();
        edges_.clear();
    }

    /** Determine if a path is empty. */
    bool isEmpty() const {
        return !frontVertex_;
    }

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
     *  If the path is not empty then the source vertex for the new edge must be equal to the  @ref backVertex. */
    void pushBack(const ControlFlowGraph::ConstEdgeIterator &edge);

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

    /** Number of times vertex appears in path. */
    size_t nVisits(const ControlFlowGraph::ConstVertexIterator &vertex) const;

    /** Number of times edge appears in path. */
    size_t nVisits(const ControlFlowGraph::ConstEdgeIterator &edge) const;

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

    /** Call depth.
     *
     *  Counts the number of E_FUNCTION_CALL edges in a path.  If a non-null function is supplied then only count those edges
     *  that enter the specified function. */
    size_t callDepth(const Function::Ptr &function = Function::Ptr()) const;

    /** Print the path. */
    void print(std::ostream &out) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for operating on paths
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Finds edges that can be part of some path.
 *
 *  Returns a Boolean vector indicating whether an edge is significant.  An edge is significant if it appears on some path that
 *  originates at the @p beginVertex and reaches some vertex in @p endVertices but is not a member of @p avoidEdges and is not
 *  incident to any vertex in @p avoidVertices. */
std::vector<bool>
findPathEdges(const ControlFlowGraph &graph,
              const ControlFlowGraph::ConstVertexIterator &beginVertex, const CfgConstVertexSet &endVertices,
              const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
              const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet());

/** Find edges that are reachable.
 *
 *  Finds edges that are part of some path from the @p beginVertex to any of the @p endVertices. The paths that are
 *  considered must not traverse the @p avoidEdges or @p avoidVertices. */
CfgConstEdgeSet
findPathReachableEdges(const ControlFlowGraph &graph,
                       const ControlFlowGraph::ConstVertexIterator &beginVertex, const CfgConstVertexSet &endVertices,
                       const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
                       const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet());

/** Find edges that are unreachable.
 *
 *  Finds edges that are not part of any path from the @p beginVertex to any of the @p endVertices. The paths that are
 *  considered must not traverse the @p avoidEdges or @p avoidVertices. */
CfgConstEdgeSet
findPathUnreachableEdges(const ControlFlowGraph &graph,
                         const ControlFlowGraph::ConstVertexIterator &beginVertex, const CfgConstVertexSet &endVertices,
                         const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
                         const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet());

/** Remove edges and vertices that cannot be on the paths.
 *
 *  Removes those edges that aren't reachable in both forward and reverse directions between the specified begin and end
 *  vertices. Specified vertices must belong to the graph, although end vertices are allowed.  After edges are removed,
 *  dangling vertices are removed.  Vertices and edges are removed from the @p graph, the @p vmap, and the @p path. Removal of
 *  edges from @p path causes the path to be truncated.
 *
 *  Returns the number of edges that were removed from the @p path. */
size_t
eraseUnreachablePaths(ControlFlowGraph &graph /*in,out*/, const ControlFlowGraph::ConstVertexIterator &beginVertex,
                      const CfgConstVertexSet &endVertices, CfgVertexMap &vmap /*in,out*/, CfgPath &path /*in,out*/);

/** Compute all paths.
 *
 *  Computes all paths from @p beginVertex to any @p endVertices that does not go through any @p avoidVertices or @p
 *  avoidEdges. The paths are returned as a CFG so that cycles can be represented. A CFG can represent an exponential number of
 *  paths. The paths-CFG is formed by taking the global CFG and removing all @p avoidVertices and @p avoidEdges, any edge that
 *  cannot appear on a path from the @p beginVertex to any @p endVertices, and any vertex that has degree zero provided it is
 *  not the beginVertex.
 *
 *  Function calls are not expanded. The E_FUNCTION_CALL edges will be pruned away unless one of the @p endVertices is
 *  reachable by following the call (function return instructions don't have edges back to their call sites).  Instead,
 *  function calls are effectively skipped over by following the E_CALL_RETURN edge from the call site to the return target
 *  (usually the fall-through address of a CALL instruction).
 *
 *  If the returned graph is empty then no paths were found.  If the returned graph has a vertex but no edges then the vertex
 *  serves as both the begin and end of the path (i.e., a single path of unit length).  The @p vmap is updated to indicate the
 *  mapping from @p srcCfg vertices in the corresponding vertices in the returned graph. */
ControlFlowGraph
findPathsNoCalls(const ControlFlowGraph &srcCfg, CfgVertexMap &vmap /*out*/,
                 const ControlFlowGraph::ConstVertexIterator &beginVertex,
                 const CfgConstVertexSet &endVertices,
                 const CfgConstVertexSet &avoidVertices = CfgConstVertexSet(),
                 const CfgConstEdgeSet &avoidEdges = CfgConstEdgeSet());

/** Inline a functioon at the specified call site.
 *
 *  The @p paths graph is modified in place by inserting an inlined copy of the function(s) called from the specified @p
 *  pathsCallSite vertex.  The @p pathsCallSite only serves as the attachment point--it must have the @ref E_CALL_RETURN
 *  edge(s) but does not need any @ref E_FUNCTION_CALL edges.  The @p cfgCallSite is the vertex in the @p cfg corresponding to
 *  the @p pathsCallSite in the paths graph and provides information about which functions are called.
 *
 *  There are two versions of this function: one takes a specific function call edge and inlines only that single call. The
 *  other takes a call site vertex and inlines all functions called at that vertex.
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
 *  Returns true if some function was inserted, false if no changes were made to @p paths.
 *
 * @{ */
bool
insertCalleePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                  const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallSite,
                  const CfgConstVertexSet &cfgAvoidVertices = CfgConstVertexSet(),
                  const CfgConstEdgeSet &cfgAvoidEdges = CfgConstEdgeSet());
bool
insertCalleePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &pathsCallSite,
                  const ControlFlowGraph &cfg, const ControlFlowGraph::ConstEdgeIterator &cfgCallEdge,
                  const CfgConstVertexSet &cfgAvoidVertices = CfgConstVertexSet(),
                  const CfgConstEdgeSet &cfgAvoidEdges = CfgConstEdgeSet());
/** @} */



std::ostream& operator<<(std::ostream &out, const CfgPath &path);

} // namespace
} // namespace
} // namespace

#endif
