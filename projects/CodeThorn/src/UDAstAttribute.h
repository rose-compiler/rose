#ifndef UDASTATTRIBUTE
#define UDASTATTRIBUTE

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
using std::set;
#include <string>
using std::string;

#include "RDAnalysisAstAttribute.h"
#include "AnalysisAbstractionLayer.h"


typedef pair<VariableId,LabelSet> VariableIdLabelSetPair;
typedef set<VariableIdLabelSetPair> UseDefInfo;

class UDAstAttribute : public AstSgNodeAttribute {
 public:
  UDAstAttribute(RDAnalysisAstAttribute* rdAttr, SgNode* nodeForUseVarQuery);
  VariableIdSet useVariables(VariableIdMapping& vidm);
  LabelSet definitionsOfVariable(VariableId var);
  void toStream(ostream& os, VariableIdMapping* vim);
 private:
  RDAnalysisAstAttribute* _rdAttr;
  SgNode* _node;
};

#endif
