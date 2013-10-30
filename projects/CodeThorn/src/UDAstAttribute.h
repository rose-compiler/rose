#ifndef UDASTATTRIBUTE
#define UDASTATTRIBUTE

// Author: Markus Schordan, 2013.

#include "Labeler.h"
#include "VariableIdMapping.h"

#include <set>
using std::set;
#include <string>
using std::string;

#include "RDAstAttribute.h"
#include "AnalysisAbstractionLayer.h"


typedef pair<VariableId,LabelSet> VariableIdLabelSetPair;
typedef set<VariableIdLabelSetPair> UseDefInfo;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class UDAstAttribute : public AstSgNodeAttribute {
 public:
  UDAstAttribute(RDAstAttribute* rdAttr, SgNode* nodeForUseVarQuery);
  VariableIdSet useVariables(VariableIdMapping& vidm);
  LabelSet definitionsOfVariable(VariableId var);
  void toStream(ostream& os, VariableIdMapping* vim);
 private:
  RDAstAttribute* _rdAttr;
  SgNode* _node;
};

#endif
