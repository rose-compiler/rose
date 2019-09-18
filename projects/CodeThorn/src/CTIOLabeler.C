#include "sage3basic.h"
#include "CTIOLabeler.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

CTIOLabeler::CTIOLabeler(SgNode* start, VariableIdMapping* variableIdMapping): CodeThorn::IOLabeler(start, variableIdMapping) {
}

bool CTIOLabeler::isStdIOLabel(Label label) {
  cerr<<"Warning: deprecated function: isStdIOLabel."<<endl;
  return CodeThorn::IOLabeler::isStdIOLabel(label);
}

bool CTIOLabeler::isStdInLabel(Label label, VariableId* id) {
  if(CodeThorn::IOLabeler::isStdInLabel(label,id)) {
    return true;
  } else if(isNonDetIntFunctionCall(label,id)) {
    return true;
  } else if(isNonDetLongFunctionCall(label,id)) {
    return true;
  }
  return false;
}

// consider to use Analyzer* instead and query this information
void CTIOLabeler::setExternalNonDetIntFunctionName(std::string name) {
  _externalNonDetIntFunctionName=name;
}

void CTIOLabeler::setExternalNonDetLongFunctionName(std::string name) {
  _externalNonDetLongFunctionName=name;
}

bool CTIOLabeler::isNonDetIntFunctionCall(Label lab,VariableId* varIdPtr){
  return isFunctionCallWithName(lab,varIdPtr,_externalNonDetIntFunctionName);
}

bool CTIOLabeler::isNonDetLongFunctionCall(Label lab,VariableId* varIdPtr){
  return isFunctionCallWithName(lab,varIdPtr,_externalNonDetLongFunctionName);
}

bool CTIOLabeler::isFunctionCallWithName(Label lab,VariableId* varIdPtr,string name){
  SgNode* node=getNode(lab);
  if(isFunctionCallLabel(lab)) {
    std::pair<SgVarRefExp*,SgFunctionCallExp*> p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
    if(p.first) {
      string funName=SgNodeHelper::getFunctionName(p.second);
      if(funName!=name) {
	return false;
      }
      if(varIdPtr) {
	*varIdPtr=_variableIdMapping->variableId(p.first);
      }
      return true;
    }
  }
  return false;
}

CTIOLabeler::~CTIOLabeler() {
}

