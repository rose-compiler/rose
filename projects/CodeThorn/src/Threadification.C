#include "sage3basic.h"
#include "Threadification.h"

using namespace std;
using namespace SPRAY;
using namespace CodeThorn;

void Threadification::transform(SgNode* root) {
  VariableIdSet varIdSet=determineVariablesOfInterest(root);
  transformVariableDeclarations(root,varIdSet);
  transformVariableAccess(root,varIdSet);
}

VariableIdSet Threadification::determineVariablesOfInterest(SgNode* root) {
  VariableIdSet varIdSet;
  cout<<"WARNING: Threadification: determination of variables not implemented yet."<<endl;
  return varIdSet;
}

void Threadification::transformVariableDeclarations(SgNode* root, SPRAY::VariableIdSet varIdSet) {
  cout<<"WARNING: Threadification: variable declaration transformation not implemented yet."<<endl;
}

void Threadification::transformVariableAccess(SgNode* root, SPRAY::VariableIdSet varIdSet) {
  cout<<"WARNING: Threadification: variable access transformation not implemented yet."<<endl;
}
