#ifndef CALL_GRAPH_WRITER_H
#define CALL_GRAPH_EXPORT_H 20250310

#include <nlohmann/json.h>

#include "CallGraphAnalysis.h"

/*!
 * \author Peter Pirkelbauer
 * \date 2025-2025
 */


namespace CodeThorn
{
  using VertexPredicate       = std::function<bool(const CallGraph::Vertex&)>;
  using VertexToJsonConverter = std::function<nlohmann::json(const CallGraph::Vertex&)>;
  using EdgeToJsonConverter   = std::function<nlohmann::json(const CallGraph::Edge&)>;

  /// returns a predicate that that is true for any vertex
  VertexPredicate anyFunction();

  /// returns a predicate testing if a function has an implementation.
  VertexPredicate definedFunctions();

  /// returns a converter generating node ID and name
  VertexToJsonConverter basicVertexConverter();

  /// returns a converter generating basicVertexConverter + definition status, class member
  VertexToJsonConverter detailedVertexConverter();

  /// returns a converter generating node ID and name
  EdgeToJsonConverter basicEdgeConverter();



  /// Generates a networkx compatible json object from the control flow graph.
  /// \param cg                     a control flow graph
  /// \param useWeightForMultiEdges generates edges that are unique in terms of (caller,callee).
  ///                               a weight is used to indicate that an caller,callee pair
  ///                               exists multiple times.
  /// \param pred                   a filter that returns true if a vertex should be included in the
  ///                               JSON object.
  /// \param vertexConv             a function that creates a JSON representation from a Sawyer Vertex
  /// \param edgeConv               a function that creates a JSON representation from a Sawyer Edge
  /// \result                       a JSON object representing the entire call graph.
  nlohmann::json
  toJson( const CallGraph& cg,
          bool useWeightForMultiEdges = false,
          VertexPredicate pred = anyFunction(),
          VertexToJsonConverter vertexConv = basicVertexConverter(),
          EdgeToJsonConverter edgeConv = basicEdgeConverter()
        );

  /// Writes a networkx compatible json object to the stream \p os,
  ///   without creating a complete JSON representation in memory.
  /// \param cg                     a control flow graph
  /// \param useWeightForMultiEdges generates edges that are unique in terms of (caller,callee).
  ///                               a weight is used to indicate that an caller,callee pair
  ///                               exists multiple times.
  /// \param pred                   a filter that returns true if a vertex should be included in the
  ///                               JSON object.
  /// \param vertexConv             a function that creates a JSON representation from a Sawyer Vertex
  /// \param edgeConv               a function that creates a JSON representation from a Sawyer Edge
  void
  writeJson( std::ostream& os,
             const CallGraph& cg,
             bool useWeightForMultiEdges = false,
             VertexPredicate pred = anyFunction(),
             VertexToJsonConverter vertexConv = basicVertexConverter(),
             EdgeToJsonConverter edgeConv = basicEdgeConverter()
           );
} // end of namespace CodeThorn

#endif /* CALL_GRAPH_WRITER_H */
