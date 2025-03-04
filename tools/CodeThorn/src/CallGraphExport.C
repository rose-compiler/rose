/*!
 * \author Peter Pirkelbauer
 * \date 2025-2025
 */

#include <CallGraphExport.h>

//~ #include <unordered_map>

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
//~ #include <boost/phoenix.hpp>

#include <nlohmann/json.hpp>

#include <ClassHierarchyAnalysis.h>
#include <ClassHierarchyAnalysis.h>
#include <CallGraphAnalysis.h>
#include <RoseCompatibility.h>
#include <sageGeneric.h>

//~ #include "Memoizer.h"

namespace json = nlohmann;
namespace ct = CodeThorn;

namespace
{
  std::intptr_t
  uniqueId(ct::FunctionKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.numericId(key);
  }

  std::intptr_t
  uniqueId(const ct::CallGraph::Vertex& vertex)
  {
    return uniqueId(vertex.value());
  }

  std::string
  nameOf(ct::FunctionKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.nameOf(key);
  }
/*
  std::string
  nameOf(const ct::CallGraph::Vertex& vertex)
  {
    return nameOf(vertex.value());
  }
*/

  struct VertexToJson
  {
    json::json
    operator()(const ct::CallGraph::Vertex& v) const
    {
      json::json                  res;
      ct::FunctionKeyType         key = v.value();

      res["id"]   = uniqueId(key);
      res["name"] = nameOf(key);

      return res;
    }
  };

  json::json
  verticesAsJsonArray(boost::iterator_range<ct::CallGraph::ConstVertexIterator> range, ct::VertexPredicate pred)
  {
    namespace adapt = boost::adaptors;

    json::json res = json::json::array();

    boost::copy( range | adapt::filtered(pred) | adapt::transformed(VertexToJson{}),
                 std::back_inserter(res)
               );

    msgWarn() << "#jsonVertices = " << res.size() << std::endl;
    return res;
  }

  struct EdgeToJson
  {
    json::json
    operator()(const ct::CallGraph::Edge& e) const
    {
      json::json res;

      res["source"] = uniqueId(*e.source());
      res["target"] = uniqueId(*e.target());

      return res;
    }
  };

  auto validEdges(ct::VertexPredicate pred)
  {
    return [vertexPred = std::move(pred)](const ct::CallGraph::Edge& e) -> bool
           {
             return vertexPred(*e.source()) && vertexPred(*e.target());
           };
  }

  json::json
  edgesAsJsonArrayWithDuplicates(boost::iterator_range<ct::CallGraph::ConstEdgeIterator> range, ct::VertexPredicate pred)
  {
    namespace adapt = boost::adaptors;

    // consider using a trivial implementation if no vertex filter is set
    json::json res = json::json::array();

    boost::copy( range | adapt::filtered(validEdges(std::move(pred))) | adapt::transformed(EdgeToJson{}),
                 std::back_inserter(res)
               );

    msgWarn() << "#jsonEdges = " << res.size() << std::endl;
    return res;
  }

  using VertexPairBase = std::tuple<const ct::CallGraph::Vertex*, const ct::CallGraph::Vertex*>;
  struct VertexPair : VertexPairBase
  {
    using base = VertexPairBase;
    using base::base;

    const ct::CallGraph::Vertex* source() const { return std::get<0>(*this); }
    const ct::CallGraph::Vertex* target() const { return std::get<1>(*this); }
  };


  struct ComputeEdgeWeights
  {
      // use unordered_map for speed up on large graphs
      //   requires std::hash<VertexPair> to be defined.
      using WeightedEdges = std::map<VertexPair, std::size_t>;

      void operator()(const ct::CallGraph::Edge& e)
      {
        const auto res = edges.insert({{&*e.source(), &*e.target()}, 0});

        ++(res.first->second); // increment the counter
      }

      operator WeightedEdges() && { return std::move(edges); }

    private:
      WeightedEdges     edges;
  };

  struct WeightedEdgeToJson
  {
    json::json
    operator()(const ComputeEdgeWeights::WeightedEdges::value_type& e) const
    {
      json::json res;

      res["source"] = uniqueId(*e.first.source());
      res["target"] = uniqueId(*e.first.target());
      res["weight"] = e.second;

      return res;
    }
  };

  json::json
  edgesAsJsonArrayWithWeights(boost::iterator_range<ct::CallGraph::ConstEdgeIterator> range, ct::VertexPredicate pred)
  {
    namespace adapt = boost::adaptors;

    using WeightedEdges = ComputeEdgeWeights::WeightedEdges;

    // consider using a trivial implementation if no vertex filter is set
    json::json    res   = json::json::array();
    WeightedEdges edges = boost::for_each( range | adapt::filtered(validEdges(std::move(pred))),
                                           ComputeEdgeWeights{}
                                         );

    std::transform( edges.begin(), edges.end(),
                    std::back_inserter(res),
                    WeightedEdgeToJson{}
                  );

    msgWarn() << "#jsonEdges = " << res.size() << std::endl;
    return res;
  }

  std::function<json::json(boost::iterator_range<ct::CallGraph::ConstEdgeIterator>, ct::VertexPredicate)>
  edgesAsJsonArray(bool useWeightForMultiEdges)
  {
    return useWeightForMultiEdges ? edgesAsJsonArrayWithWeights : edgesAsJsonArrayWithDuplicates;
  }


  json::json
  convertToJson(const ct::CallGraph& cg, bool useWeightForMultiEdges, ct::VertexPredicate pred)
  {
    json::json res;

    res["directed"]   = true;
    res["multigraph"] = false;
    res["graph"]      = json::json::object();
    res["nodes"]      = verticesAsJsonArray(cg.vertices(), pred);
    res["links"]      = edgesAsJsonArray(useWeightForMultiEdges)(cg.edges(), pred);

    return res;
  }
}


namespace CodeThorn
{
  VertexPredicate anyFunction()
  {
    return [](const CallGraph::Vertex&) -> bool { return true; };
  }

  VertexPredicate definedFunctions()
  {
    return [](const CallGraph::Vertex& v) -> bool
           {
             return CompatibilityBridge{}.hasDefinition(v.value());
           };
  }

  nlohmann::json
  toJson(const CallGraph& cg, bool useWeightForMultiEdges, VertexPredicate pred)
  {
    return convertToJson(cg, useWeightForMultiEdges, pred);
  }
} // end of namespace CodeThorn

