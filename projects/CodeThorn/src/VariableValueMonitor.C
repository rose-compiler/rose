#include "sage3basic.h"
#include "VariableValueMonitor.h"
#include "Analyzer.h"

using namespace std;
using namespace CodeThorn;

namespace CodeThorn {

VariableValueMonitor::VariableValueMonitor(){
}

void VariableValueMonitor::setVariableMode(VariableMode mode,AbstractValue variableId) {
  _variablesModeMap[variableId]=mode;
}

VariableValueMonitor::VariableMode VariableValueMonitor::getVariableMode(AbstractValue variableId) {
  return _variablesModeMap[variableId];
}

// the init function only uses the variableIds of a given estate (not its values) for initialization
void VariableValueMonitor::init(const EState* estate) {
  const PState* pstate=estate->pstate();
  init(pstate);
}

void VariableValueMonitor::init(const PState* pstate) {
  AbstractValueSet varIdSet=pstate->getVariableIds();
  for(AbstractValueSet::iterator i=varIdSet.begin(); i!=varIdSet.end(); ++i) {
    // to also allow reinit
    if(_variablesMap.find(*i)==_variablesMap.end()) {
      _variablesMap[*i]=new set<int>(); // initialize value set for each variable
      _variablesModeMap[*i]=VariableValueMonitor::VARMODE_PRECISE;
    }
  }
}

AbstractValueSet VariableValueMonitor::getVariables() {
  AbstractValueSet vset;
  for(map<AbstractValue,VariableMode>::iterator i=_variablesModeMap.begin();
      i!=_variablesModeMap.end();
      ++i) {
    vset.insert((*i).first);
  }
  return vset;
}

bool VariableValueMonitor::isHotVariable(Analyzer* analyzer, AbstractValue varId) {
  // TODO: provide set of variables to ignore
  string name=varId.toLhsString(analyzer->getVariableIdMapping());
  switch(_variablesModeMap[varId]) {
  case VariableValueMonitor::VARMODE_FORCED_TOP:
    return true;
  case VariableValueMonitor::VARMODE_ADAPTIVE_TOP: {
    if(name=="input" || name=="output") 
      return false;
    else
      return true;
  }
  case VariableValueMonitor::VARMODE_PRECISE:
    return false;
  default:
    cerr<<"Error: unknown variable monitor mode."<<endl;
    exit(1);
  }
}

string VariableValueMonitor::toString(VariableIdMapping* variableIdMapping) {
  stringstream ss;
  for(map<AbstractValue,set<int>* >::iterator i=_variablesMap.begin();
      i!=_variablesMap.end();
      ++i) {
    ss<<string("VAR:")<<((*i).first).toString(variableIdMapping)<<": "<<(*i).second->size()<<": ";
    set<int>* sp=(*i).second;
    for(set<int>::iterator i=sp->begin();i!=sp->end();++i) {
      ss<<*i<<" ";
    }
    ss<<endl;
  }
  ss<<endl;
  return ss.str();
}

} // end of namespace CodeThorn
