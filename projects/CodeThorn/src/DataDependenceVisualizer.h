#ifndef DATA_DEPENDENCE_VISUALIZER
#define DATA_DEPENDENCE_VISUALIZER

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
using std::set;

using namespace CodeThorn;

class VariableIdSetAttribute : public AstSgNodeAttribute {
 public:
  VariableIdSetAttribute(set<VariableId> set):_variableIdSet(set){}
  set<VariableId> getVariableIdSet() { return _variableIdSet; }
 private:
  set<VariableId> _variableIdSet;
};

class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping);
  set<VariableId> useVars(SgNode* expr);
  set<VariableId> defVars(SgNode* expr);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  void generateDot(SgNode* root, string fileName);
 private:
  set<VariableId> getAstAttributeVariableIdSet(SgNode*, string);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#endif
