#ifndef CALL_GRAPH2_H
#define CALL_GRAPH2_H 2

#include <sage3basic.h>
#include <tuple>
#include <Sawyer/Graph.h>
#include "ClassHierarchyAnalysis.h"

/*!
 * \author Peter Pirkelbauer
 * \date 2022-2024
 */

struct SgProject;
struct SgDeclarationStatement;

namespace CodeThorn
{
  struct CallEdge
  {
    enum Property
    {
      unknown        = 0,
      normalCall     = (1 << 0),
      addressTaken   = (1 << 1),
      virtualCall    = (1 << 2),
      overrider      = (1 << 3),
      //~ resolvePointerCalls   = (1 << 3),
      defaultValue = normalCall | addressTaken | virtualCall | overrider,
    };

    Property kind;
  };

  using CGVertex  = FunctionKeyType;
  using CGEdge    = CallEdge::Property;
  using CallGraph = Sawyer::Container::Graph<CGVertex, CGEdge, CGVertex>;

  using CallDataSequence         = std::vector<CallData>;
  using FunctionCallDataBase     = std::tuple<FunctionKeyType, CallDataSequence>;

  struct FunctionCallData : FunctionCallDataBase
  {
    using base = FunctionCallDataBase;
    using base::base;

    FunctionKeyType              function() const { return std::get<0>(*this); }
    const std::vector<CallData>& calls()    const { return std::get<1>(*this); }
  };

  using FunctionCallDataSequence = std::vector<FunctionCallData>;

  std::tuple<CallGraph, FunctionCallDataSequence>
  generateCallGraphFromAST(SgProject*);

  std::tuple<CallGraph, FunctionCallDataSequence>
  generateCallGraphFromAST(SgProject*, ClassAnalysis&, VirtualFunctionAnalysis&, CallEdge::Property edgeKinds = CallEdge::defaultValue);

  // CallGraph generateCallGraphFromNormalizedAST(SgProject* proj);
} // end of namespace CodeThorn

#endif /* CALL_GRAPH2_H */
