#ifndef DATA_DEPENDENCE_VISUALIZER
#define DATA_DEPENDENCE_VISUALIZER

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
using std::set;
#include <string>
using std::string;

#include "RDAstAttribute.h"
#include "AnalysisAbstractionLayer.h"
#include "CFAnalysis.h"
#include "UDAstAttribute.h"

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, string useDefAttributeName);
  VariableIdSet useVars(SgNode* expr);
  LabelSet defLabels(SgNode* expr, VariableId useVar);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  //! requires UDAstAttribute
  void generateDefUseDotGraph(SgNode* root, string fileName);
  //! requires UDAstAttribute
  void generateUseDefDotGraph(SgNode* root, string fileName);
  void includeFlowGraphEdges(Flow* flow) { _flow=flow; }
  void generateDotFunctionClusters(SgNode* root, CFAnalysis* cfanalyzer, string fileName, bool withDataDependencies);
  string nodeSourceCode(Label lab);
  bool _showSourceCode;
  void setFunctionLabelSetSets(LabelSetSet);
 private:
  enum DDVMode { DDVMODE_USEDEF, DDVMODE_DEFUSE };
  void generateDot(SgNode* root, string fileName);
  UDAstAttribute* getUDAstAttribute(SgNode* expr,string attributeName);
  bool existsUDAstAttribute(SgNode* expr,string attributeName);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
  string  _useDefAttributeName;
  DDVMode _mode;
  Flow* _flow;
  string _dotFunctionClusters;
  LabelSetSet _functionLabelSetSets;
};

#endif
