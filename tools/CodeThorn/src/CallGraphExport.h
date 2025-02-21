#ifndef CALL_GRAPH_WRITER_H
#define CALL_GRAPH_EXPORT_H 20250214

#include <nlohmann/json.h>

#include "CallGraphAnalysis.h"

/*!
 * \author Peter Pirkelbauer
 * \date 2025-2025
 */


namespace CodeThorn
{
  using VertexPredicate = std::function<bool(const CallGraph::Vertex&)>;

  /// returns a predicate that that is true for any vertex
  VertexPredicate anyFunction();

  /// returns a predicate testing if a function has an implementation.
  VertexPredicate definedFunctions();

  /// Generates a json object from the control flow graph.
  /// \param cg                     a control flow graph
  /// \param useWeightForMultiEdges generates edges that are unique in terms of (caller,callee).
  ///                               a weight is used to indicate that an caller,callee pair
  ///                               exists multiple times.
  /// \param filter                 a filter that returns true if a vertex should be included in the
  ///                               JSON object.
  nlohmann::json
  toJson(const CallGraph& cg, bool useWeightForMultiEdges = false, VertexPredicate pred = anyFunction());
} // end of namespace CodeThorn

#endif /* CALL_GRAPH_WRITER_H */
