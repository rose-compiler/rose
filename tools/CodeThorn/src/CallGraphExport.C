/*!
 * \author Peter Pirkelbauer
 * \date 2025-2025
 */

#include <CallGraphExport.h>

//~ #include <unordered_map>

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>

#include <nlohmann/json.hpp>

#include <ClassHierarchyAnalysis.h>
#include <ClassHierarchyAnalysis.h>
#include <CallGraphAnalysis.h>
#include <RoseCompatibility.h>
#include <sageGeneric.h>

//~ #include "Memoizer.h"

namespace json    = nlohmann;
namespace progrep = CodeThorn;  // progrep to make the code easily customizable
                                // for binary analysis.
namespace
{
  std::string
  uniqueId(progrep::FunctionKeyType key)
  {
    progrep::CompatibilityBridge compat;

    return compat.uniqueName(key);
  }

  std::string
  uniqueId(const progrep::CallGraph::Vertex& vertex)
  {
    return uniqueId(vertex.value());
  }

  std::string
  nameOf(progrep::FunctionKeyType key)
  {
    progrep::CompatibilityBridge compat;

    return compat.nameOf(key);
  }
/*
  std::string
  nameOf(const progrep::CallGraph::Vertex& vertex)
  {
    return nameOf(vertex.value());
  }
*/

  struct JsonCreator
  {
    virtual void field(const std::string&, json::json) = 0;

    virtual void arrayBegin(const std::string&)        = 0;
    virtual void arrayElem(json::json)                 = 0;
    virtual void arrayEnd(const std::string&)          = 0;

    template <class Range>
    void array(const std::string& nm, Range range)
    {
      arrayBegin(nm);

      for (const json::json& elem : range)
        arrayElem(elem);

      arrayEnd(nm);
    }
  };

  struct JsonObjectCreator : JsonCreator
  {
      using base = JsonCreator;

      JsonObjectCreator()
      : base(), data(1, json::json())
      {}

      ~JsonObjectCreator()                                    = default;
      JsonObjectCreator(JsonObjectCreator&& other)            = default;
      JsonObjectCreator& operator=(JsonObjectCreator&& other) = default;

      void field(const std::string& nm, json::json val) override final
      {
        data.back()[nm] = std::move(val);
      }

      void arrayBegin(const std::string& nm) override final
      {
        data.push_back(json::json::array());
      }

      void arrayElem(json::json el) override final
      {
        data.back().push_back(std::move(el));
      }

      void arrayEnd(const std::string& nm) override final
      {
        json::json tmp = std::move(data.back());

        data.pop_back();
        data.back()[nm] = std::move(tmp);
      }

      json::json result() &&
      {
        ASSERT_require(data.size() == 1);

        json::json tmp = std::move(data.back());
        return tmp;
      }

    private:
      std::vector<json::json> data;

      JsonObjectCreator(const JsonObjectCreator& other)            = delete;
      JsonObjectCreator& operator=(const JsonObjectCreator& other) = delete;
  };


  struct JsonFileCreator : JsonCreator
  {
      using base = JsonCreator;

      explicit
      JsonFileCreator(std::ostream& os)
      : base(), out(os), firstElement(1, true)
      {}

      ~JsonFileCreator()
      {
        ASSERT_require(firstElement.size() == 1);
        out << '}';
      }

      void elementSeparation()
      {
        if (!firstElement.back()) out << ','; else firstElement.back() = false;
      }

      void field(const std::string& nm, json::json val) override final
      {
        elementSeparation();
        out << "\"" << nm << "\":" << val;
      }

      void arrayBegin(const std::string& nm) override final
      {
        elementSeparation();
        out << "\"" << nm << "\":[";
        firstElement.push_back(true);
      }

      void arrayElem(json::json el) override final
      {
        elementSeparation();
        out << el;
      }

      void arrayEnd(const std::string& nm) override final
      {
        ASSERT_require(firstElement.size() > 1);
        out << ']';
        firstElement.pop_back();
      }

    private:
      std::ostream&     out;
      std::vector<bool> firstElement;

      JsonFileCreator(const JsonFileCreator& other)            = delete;
      JsonFileCreator& operator=(const JsonFileCreator& other) = delete;
      JsonFileCreator(JsonFileCreator&& other)                 = delete;
      JsonFileCreator& operator=(JsonFileCreator&& other)      = delete;
  };


  void
  verticesAsJsonArray( JsonCreator& creator,
                       boost::iterator_range<progrep::CallGraph::ConstVertexIterator> range,
                       progrep::VertexPredicate pred,
                       progrep::VertexToJsonConverter conv
                     )
  {
    namespace adapt = boost::adaptors;

    creator.array("nodes", range | adapt::filtered(pred) | adapt::transformed(conv));
  }

  struct EdgeToJson
  {
    json::json
    operator()(const progrep::CallGraph::Edge& e) const
    {
      json::json res;

      res["source"] = uniqueId(*e.source());
      res["target"] = uniqueId(*e.target());

      if ((e.value() & progrep::CallEdge::overrider) == progrep::CallEdge::overrider)
      {
        res["override"] = true;
      }

      return res;
    }
  };

  auto validEdges(progrep::VertexPredicate pred)
  {
    return [vertexPred = std::move(pred)](const progrep::CallGraph::Edge& e) -> bool
           {
             return vertexPred(*e.source()) && vertexPred(*e.target());
           };
  }

  void
  edgesAsJsonArrayWithDuplicates( JsonCreator& creator,
                                  boost::iterator_range<progrep::CallGraph::ConstEdgeIterator> range,
                                  progrep::VertexPredicate pred
                                )
  {
    namespace adapt = boost::adaptors;

    creator.array( "links",
                   range | adapt::filtered(validEdges(std::move(pred)))
                         | adapt::transformed(EdgeToJson{})
                 );
  }

  using VertexPairBase = std::tuple<const progrep::CallGraph::Vertex*, const progrep::CallGraph::Vertex*>;
  struct VertexPair : VertexPairBase
  {
    using base = VertexPairBase;
    using base::base;

    const progrep::CallGraph::Vertex* source() const { return std::get<0>(*this); }
    const progrep::CallGraph::Vertex* target() const { return std::get<1>(*this); }
  };


  struct ComputeEdgeWeights
  {
      // use unordered_map for speed up on large graphs
      //   requires std::hash<VertexPair> to be defined.
      using WeightedEdges = std::map<VertexPair, std::size_t>;

      void operator()(const progrep::CallGraph::Edge& e)
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

  void
  edgesAsJsonArrayWithWeights( JsonCreator& creator,
                               boost::iterator_range<progrep::CallGraph::ConstEdgeIterator> range,
                               progrep::VertexPredicate pred
                             )
  {
    namespace adapt = boost::adaptors;

    using WeightedEdges = ComputeEdgeWeights::WeightedEdges;

    WeightedEdges edges = boost::for_each( range | adapt::filtered(validEdges(std::move(pred))),
                                           ComputeEdgeWeights{}
                                         );

    creator.array("links", edges | adapt::transformed(WeightedEdgeToJson{}));
  }

  std::function<void(JsonCreator& creator, boost::iterator_range<progrep::CallGraph::ConstEdgeIterator>, progrep::VertexPredicate)>
  edgesAsJsonArray(bool useWeightForMultiEdges)
  {
    return useWeightForMultiEdges ? edgesAsJsonArrayWithWeights : edgesAsJsonArrayWithDuplicates;
  }

  void
  convertToJson( JsonCreator& output,
                 const progrep::CallGraph& cg,
                 bool useWeightForMultiEdges,
                 progrep::VertexPredicate pred,
                 progrep::VertexToJsonConverter conv
               )
  {
    output.field("directed", true);
    output.field("multigraph", !useWeightForMultiEdges);
    output.field("graph", json::json::object());

    verticesAsJsonArray(output, cg.vertices(), pred, std::move(conv));
    edgesAsJsonArray(useWeightForMultiEdges)(output, cg.edges(), pred);
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

  VertexToJsonConverter basicVertexConverter()
  {
    return [](const CallGraph::Vertex& v) -> json::json
           {
             json::json      res;
             FunctionKeyType key = v.value();

             res["id"]     = uniqueId(key);
             res["name"]   = nameOf(key);

             return res;
           };
  }

  VertexToJsonConverter detailedVertexConverter()
  {
    return [](const CallGraph::Vertex& v) -> json::json
           {
             json::json          res;
             FunctionKeyType     key = v.value();
             CompatibilityBridge compat;

             //~ if (nameOf(key).rfind("_M_construct < pointer >") == 0)
             //~ {
               //~ uniqueId(key);
               //~ std::cerr << ":here you go.. " << std::endl;
             //~ }

             res["id"]      = uniqueId(key);
             res["name"]    = nameOf(key);
             res["defined"] = compat.hasDefinition(key);

             if (auto clazz = compat.classType(key))
               res["class"]  = typeNameOf(*clazz);

             // res["fileid"]  = compat.location(key);

             return res;
           };
  }


  json::json
  toJson(const CallGraph& cg, bool useWeightForMultiEdges, VertexPredicate pred, VertexToJsonConverter conv)
  {
    JsonObjectCreator objCreator;

    convertToJson(objCreator, cg, useWeightForMultiEdges, std::move(pred), std::move(conv));
    return std::move(objCreator).result();
  }

  void
  writeJson(std::ostream& os, const CallGraph& cg, bool useWeightForMultiEdges, VertexPredicate pred, VertexToJsonConverter conv)
  {
    JsonFileCreator jsonFile(os);

    os << '{';
    convertToJson(jsonFile, cg, useWeightForMultiEdges, std::move(pred), std::move(conv));
    os << '}';
  }

} // end of namespace CodeThorn
