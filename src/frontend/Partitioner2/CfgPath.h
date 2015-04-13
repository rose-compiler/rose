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
     *  when this path's edges are all final outgoing edges for each vertex in the path. */
    void backtrack();

    /** Number of times vertex appears in path. */
    size_t nVisits(const ControlFlowGraph::ConstVertexIterator &vertex) const;

    /** Number of times edge appears in path. */
    size_t nVisits(const ControlFlowGraph::ConstEdgeIterator &edge) const;

    /** Truncate the path.
     *
     *  Erases all edges starting at the specified edge.  The specified edge will not be in the resulting path. */
    void truncate(const ControlFlowGraph::ConstEdgeIterator &edge);

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
              ControlFlowGraph::ConstVertexIterator beginVertex, const CfgConstVertexSet &endVertices,
              const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges);

/** Remove edges and vertices that cannot be on the paths.
 *
 *  Removes those edges that aren't reachable in both forward and reverse directions between the specified begin and end
 *  vertices. Specified vertices must belong to the paths-CFG, although end vertices are allowed.  After edges are removed,
 *  dangling vertices are removed.  Vertices and edges are removed from the @p paths graph, the @p vmap, and the @p
 *  path. Removal of edges from @p path causes the path to be truncated. */
void
eraseUnreachablePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstVertexIterator &beginPathVertex,
                      const CfgConstVertexSet &endPathVertices, CfgVertexMap &vmap /*in,out*/, CfgPath &path /*in,out*/);

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
findPathsNoCalls(const ControlFlowGraph &srcCfg, const ControlFlowGraph::ConstVertexIterator &beginVertex,
                 const CfgConstVertexSet &endVertices, const CfgConstVertexSet &avoidVertices, const CfgConstEdgeSet &avoidEdges,
                 CfgVertexMap &vmap /*out*/);

/** Replace a call-return edge with a function call.
 *
 *  The @p paths graph is modified in place by insert an inlined copy of the function(s) called from the specified @p
 *  pathsCretEdge call-return edge's source vertex. The @p pathsCretEdge usually goes from a CALL instruction to the CALL's
 *  fall-through address.  Edges are inserted from the new inlined return sites to the return target. The call-return edge is
 *  not erased, but the caller often does that as the next step.
 *
 *  The @p cfgCallSite is the vertex in the @p cfg corresponding to the source vertex for @p pathsCretEdge in the paths graph
 *  and provides information about which functions are called.  The vertices and edges in the inlined version that correspond
 *  to the @p cfgAvoidEVertices and @p cfgAvoidEdges are not copied into the destination graph. */
void
insertCalleePaths(ControlFlowGraph &paths /*in,out*/, const ControlFlowGraph::ConstEdgeIterator &pathsCretEdge,
                  const ControlFlowGraph &cfg, const ControlFlowGraph::ConstVertexIterator &cfgCallSite,
                  const CfgConstVertexSet &cfgAvoidVertices, const CfgConstEdgeSet &cfgAvoidEdges);



std::ostream& operator<<(std::ostream &out, const CfgPath &path);

} // namespace
} // namespace
} // namespace

#endif
