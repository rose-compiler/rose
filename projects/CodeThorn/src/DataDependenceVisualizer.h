#ifndef DATA_DEPENDENCE_VISUALIZER
#define DATA_DEPENDENCE_VISUALIZER

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
using std::set;
#include <string>
using std::string;

#include "RDAnalysisAstAttribute.h"
#include "AnalysisAbstractionLayer.h"

using namespace CodeThorn;

#include "UDAstAttribute.h"

class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, string useDefAttributeName);
  VariableIdSet useVars(SgNode* expr);
  LabelSet defLabels(SgNode* expr, VariableId useVar);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  void generateDot(SgNode* root, string fileName);
  string nodeSourceCode(Label lab);
  bool _showSourceCode;
 private:
  UDAstAttribute* getUDAstAttribute(SgNode* expr,string attributeName);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
  string  _useDefAttributeName;
};

#endif
