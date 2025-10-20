#ifndef CALL_GRAPH_ANALYSIS_H
#define CALL_GRAPH_ANALYSIS_H 20250214

#include <sage3basic.h>
#include <tuple>
#include <Sawyer/Graph.h>

#include "ClassHierarchyAnalysis.h"

/*!
 * \author Peter Pirkelbauer
 * \date 2022-2024
 */

namespace CodeThorn
{
  struct CallEdge
  {
    enum Property : std::int8_t
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

  inline
  CallEdge::Property
  operator|(CallEdge::Property lhs, CallEdge::Property rhs)
  {
    return static_cast<CallEdge::Property>(lhs | std::int8_t(rhs));
  }

  inline
  CallEdge::Property
  operator&(CallEdge::Property lhs, CallEdge::Property rhs)
  {
    return static_cast<CallEdge::Property>(lhs & std::int8_t(rhs));
  }


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

  /// generates a CallGraph and call information from a ROSE project
  ///   \param  ast           the ROSE project
  ///   \param  vfa           a virtual function analysis, if overriders should be added to virtual calls
  ///   \param  withAddrTaken if set, taking an address of a function generates a callgraph entry
  ///   \return a call graph and detailed function call information.
  std::tuple<CallGraph, FunctionCallDataSequence>
  generateCallGraphFromAST(ASTRootType ast, const VirtualFunctionAnalysis* vfa = nullptr, bool withAddrTaken = true);

  // CallGraph generateCallGraphFromNormalizedAST(SgProject* proj);
} // end of namespace CodeThorn

#endif /* CALL_GRAPH_ANALYSIS_H */
