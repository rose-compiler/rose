#ifndef ROSE_FlowAnalysisInterface_H
#define ROSE_FlowAnalysisInterface_H

#include <RoseFirst.h>
#include <Rose/FlowGraphInterface.h>

class SgNode;
class SgFunctionDeclaration;
class SgExpression;

namespace Rose{
namespace FlowGraphInterface {

// Internally automatically select the desired analysis based on the options.
enum class AnalysisSupportOption { CodeThorn, AstSideEffect };

template <class NodeIterator, class EdgeIterator, AnalysisSupportOption Option>
class CallGraphAnalysis {
 public:
  // Call Graph Analysis Interface below.
  typedef SgFunctionDeclaration* NodeInfo;
  typedef SgExpression* EdgeInfo;
  typedef FlowGraphCreateInterface<NodeInfo,EdgeInfo, NodeIterator,EdgeIterator> GraphCreate;
  bool performAnalysis(const SgNode* ast, GraphCreate &graph);
};

}; // namespace FlowGraphInterface

}; // namespace Rose

#endif
