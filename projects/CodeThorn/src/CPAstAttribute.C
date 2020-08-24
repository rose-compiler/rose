
#include "sage3basic.h"

#include "CPAstAttribute.h"
#include "VariableIdMapping.h"
#include "AstUtility.h"
#include "FIConstAnalysis.h"

using namespace AstUtility;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */

bool CPAstAttribute::isConstantInteger(VariableId varId) {
  ROSE_ASSERT(_elem);
  return _elem->isUniqueConst(varId);
}

CPAstAttributeInterface::ConstantInteger CPAstAttribute::getConstantInteger(VariableId varId) {
  ROSE_ASSERT(_elem);
  return _elem->uniqueConst(varId);
}

void CPAstAttribute::toStream(ostream& os, VariableIdMapping* vim) {
}

string CPAstAttribute::toString(){
  stringstream ss;
  VariableIdSet vset=_variableIdMapping->variableIdsOfAstSubTree(_node);
  for(VariableIdSet::iterator i=vset.begin();i!=vset.end();++i) {
    ss<<"("
      <<_variableIdMapping->uniqueVariableName(*i)
      <<",";
    if(isConstantInteger(*i)) {
      ss<<getConstantInteger(*i);
    } else {
      ss<<"any";
    }
    ss<<")";
  }
  ss<<endl;
  return ss.str();
}

CPAstAttribute::CPAstAttribute(VariableConstInfo* elem, SgNode* node, VariableIdMapping* vid):_elem(elem),_node(node),_variableIdMapping(vid) {
}

//CPAstAttribute::~CPAstAttribute() {
//}

