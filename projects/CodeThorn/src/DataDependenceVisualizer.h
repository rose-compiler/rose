#ifndef DATA_DEPENDENCE_VISUALIZER
#define DATA_DEPENDENCE_VISUALIZER

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
using std::set;

using namespace CodeThorn;

typedef pair<VariableId,LabelSet> VariableIdLabelSetPair;
typedef set<VariableIdLabelSetPair> UseDefInfo;

class UseDefInfoAttribute : public AstSgNodeAttribute {
 public:
  UseDefInfoAttribute(UseDefInfo set):_useDefInfo(set){}
  UseDefInfo& getUseDefInfo() { return _useDefInfo; }
  void insert(VariableIdLabelSetPair udpair) { _useDefInfo.insert(udpair); }
 private:
  UseDefInfo _useDefInfo;
};

class DataDependenceVisualizer {
 public:
  DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping);
  VariableIdSet useVars(SgNode* expr);
  VariableIdSet defVars(SgNode* expr);
  Label getLabel(SgNode* stmt);
  SgNode* getNode(Label lab);
  void generateDot(SgNode* root, string fileName);
 private:
  UseDefInfoAttribute* getUseDefInfoAttribute(SgNode*, string);
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#endif
