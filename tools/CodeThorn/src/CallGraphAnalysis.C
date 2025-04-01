#include "CallGraphAnalysis.h"

#include <boost/range/adaptors.hpp>

#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Rose/Diagnostics.h"
#include "sageGeneric.h"

//~ #include "FunctionIdMapping.h"
//~ #include "RoseAst.h"

#include "RoseCompatibility.h"

namespace ct = CodeThorn;

namespace
{
  /// adds call edges between vertices representing functions
  ///   also functions whose address were taken and calls that remained unresolved.
  struct InsertEdges
  {
      void insert( ct::CallGraph::ConstVertexIterator src,
                   ct::FunctionKeyType tgtkey,
                   ct::CallEdge::Property kind
                 );

      void operator()(ct::FunctionKeyType src)
      {
        ct::CompatibilityBridge            compat;
        ct::CallGraph::ConstVertexIterator srcpos = g->findVertexKey(src);
        ct::CallDataSequence               unresolvedElems;
        ct::CallDataSequence               addressTknElems;
        ct::CallDataSequence               virtualElems;

        for (ct::CallData call : compat.functionRelations(src, *isVirtualFunction))
        {
          if (call.callee())
          {
            ct::FunctionKeyType tgtkey = *call.callee();
            const bool          isCall = call.call();

            if (!isCall) // address taken
              addressTknElems.push_back(call);
            else
              insert(srcpos, tgtkey, ct::CallEdge::normalCall);

            // if the address was taken from a virtual function,
            //   we also store it in the virtual call sequence.
            if (call.virtualCall())
              virtualElems.push_back(call);
          }
          else // pointer based and other non-resolvable calls
            unresolvedElems.push_back(call);
        }

        if (unresolvedElems.size())
          unresolved.emplace_back(src, std::move(unresolvedElems));

        if (addressTknElems.size())
          addressTkn.emplace_back(src, std::move(addressTknElems));

        if (virtualElems.size())
          virtuals.emplace_back(src, std::move(virtualElems));
      }

      /// connects functions with functions whose address was taken in the
      ///   function body.
      void integrateAddressTaken();

      void expandVirtualCalls(const ct::VirtualFunctionAnalysis& vfa);

      sg::NotNull<ct::FunctionPredicate> isVirtualFunction;
      sg::NotNull<ct::CallGraph>         g;
      ct::FunctionCallDataSequence       unresolved = {};
      ct::FunctionCallDataSequence       virtuals   = {};
      ct::FunctionCallDataSequence       addressTkn = {};
  };

  void InsertEdges::insert( ct::CallGraph::ConstVertexIterator src,
                            ct::FunctionKeyType                tgtkey,
                            ct::CallEdge::Property             kind
                          )
  {
    ct::CallGraph::ConstVertexIterator tgt = g->findVertexKey(tgtkey);

    if (g->isValidVertex(tgt)) // normal taken
    {
      g->insertEdge(src, tgt, kind);
      return;
    }

    ct::CompatibilityBridge compat;

    msgError() << compat.nameOf(tgtkey) << " not found" << std::endl;
  }


  void InsertEdges::integrateAddressTaken()
  {
    for (const ct::FunctionCallData& fcd : addressTkn)
    {
      ct::FunctionKeyType                srckey = std::get<0>(fcd);
      ct::CallGraph::ConstVertexIterator srcpos = g->findVertexKey(srckey);
      ASSERT_require(g->isValidVertex(srcpos));

      for (ct::CallData cd : std::get<1>(fcd))
      {
        ASSERT_require(cd.callee());
        ct::FunctionKeyType                tgtkey = *cd.callee();

        insert(srcpos, tgtkey, ct::CallEdge::addressTaken);
      }
    }
  }

  void InsertEdges::expandVirtualCalls(const ct::VirtualFunctionAnalysis& vfa)
  {
    namespace adapt = boost::adaptors;

    auto isVirtualCall =
           [](const ct::CallData& data)->bool
           {
             return data.virtualCall();
           };

    for (const ct::FunctionCallData& fcd : virtuals)
    {
      ct::FunctionKeyType                srckey = std::get<0>(fcd);
      ct::CallGraph::ConstVertexIterator srcpos = g->findVertexKey(srckey);
      ASSERT_require(g->isValidVertex(srcpos));

      for (ct::CallData cd : std::get<1>(fcd) | adapt::filtered(isVirtualCall))
      {
        ASSERT_require(cd.callee());

        try
        {
          const ct::VirtualFunctionDesc& vfunc  = vfa.at(*cd.callee());

          for (ct::OverrideDesc overrider : vfunc.overriders())
            insert(srcpos, overrider.function(), ct::CallEdge::overrider);
        }
        catch (...)
        {
        }
      }
    }
  }
}


namespace CodeThorn
{
  std::tuple<CallGraph, FunctionCallDataSequence>
  generateCallGraphFromAST(ASTRootType n, const VirtualFunctionAnalysis* vfa, bool withAddrTaken)
  {
    CallGraph                    g;
    CompatibilityBridge          compat;
    std::vector<FunctionKeyType> allFunctions   = compat.allFunctionKeys(n);
    auto                         insertVertices = [&g](FunctionKeyType key) -> void { g.insertVertex(key); };
    ct::FunctionPredicate        alwaysFalse    = [](FunctionKeyType key) -> bool { return false; };
    ct::FunctionPredicate        isVirtualFunc  = vfa ? vfa->virtualFunctionTest() : alwaysFalse;

    std::for_each( allFunctions.begin(), allFunctions.end(),
                   insertVertices
                 );

    InsertEdges ie = std::for_each( allFunctions.begin(), allFunctions.end(),
                                    InsertEdges{&isVirtualFunc, &g}
                                  );

    if (withAddrTaken)
      ie.integrateAddressTaken();

    if (vfa != nullptr /*withOverrider*/)
      ie.expandVirtualCalls(*vfa);

    SAWYER_MESG(msgInfo())
              << "Vertices = " << g.nVertices()
              << " Edges = " << g.nEdges()
              << std::endl;

    SAWYER_MESG(ie.unresolved.empty() ? msgInfo() : msgWarn())
              << "Number of functions with unresolved calls: " << ie.unresolved.size()
              << std::endl;

    SAWYER_MESG(msgInfo())
              << "Number of functions taking addresses of functions: " <<  ie.addressTkn.size()
              << std::endl;

    return { std::move(g), std::move(ie.unresolved) };
  }
} // namespace CodeThorn
