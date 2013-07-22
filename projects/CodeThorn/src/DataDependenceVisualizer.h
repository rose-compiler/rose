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
  VariableIdSetAttribute(VariableIdMapping::VariableIdSet set):_variableIdSet(set){}
  VariableIdMapping::VariableIdSet getVariableIdSet() { return _variableIdSet; }
 private:
  VariableIdMapping::VariableIdSet _variableIdSet;
};

class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping);
  VariableIdMapping::VariableIdSet useVars(SgNode* expr);
  VariableIdMapping::VariableIdSet defVars(SgNode* expr);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  void generateDot(SgNode* root, string fileName);
 private:
  VariableIdMapping::VariableIdSet getAstAttributeVariableIdSet(SgNode*, string);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#endif
