#include "sage3basic.h"
#include "VariableValueMonitor.h"
#include "Analyzer.h"

using namespace std;
using namespace SPRAY;

namespace CodeThorn {

bool VariableValueMonitor::isActive() {
  return _threshold!=-1;
}

VariableValueMonitor::VariableValueMonitor():_threshold(-1){
}

// in combination with adaptive-top mode
void VariableValueMonitor::setThreshold(size_t threshold) {
  _threshold=threshold;
}

void VariableValueMonitor::setVariableMode(VariableMode mode,VariableId variableId) {
  _variablesModeMap[variableId]=mode;
}

VariableValueMonitor::VariableMode VariableValueMonitor::getVariableMode(VariableId variableId) {
  return _variablesModeMap[variableId];
}

// the init function only uses the variableIds of a given estate (not its values) for initialization
void VariableValueMonitor::init(const EState* estate) {
  const PState* pstate=estate->pstate();
  init(pstate);
}

void VariableValueMonitor::init(const PState* pstate) {
  VariableIdSet varIdSet=pstate->getVariableIds();
  for(VariableIdSet::iterator i=varIdSet.begin(); i!=varIdSet.end(); ++i) {
    // to also allow reinit
    if(_variablesMap.find(*i)==_variablesMap.end()) {
      _variablesMap[*i]=new set<int>(); // initialize value set for each variable
      _variablesModeMap[*i]=VariableValueMonitor::VARMODE_PRECISE;
    }
  }
}

VariableIdSet VariableValueMonitor::getHotVariables(Analyzer* analyzer, const PState* pstate) {
  if(pstate->size()!=_variablesMap.size()) {
    // found a new variable during analysis (e.g. local variable)
    init(pstate);
  }
  VariableIdSet hotVariables;
  VariableIdSet varIdSet=pstate->getVariableIds();
  for(VariableIdSet::iterator i=varIdSet.begin(); i!=varIdSet.end(); ++i) {
    if(isHotVariable(analyzer,*i)) {
      hotVariables.insert(*i);
    }
  }
  return hotVariables;
}

VariableIdSet VariableValueMonitor::getHotVariables(Analyzer* analyzer, const EState* estate) {
  const PState* pstate=estate->pstate();
  return getHotVariables(analyzer,pstate);
}

void VariableValueMonitor::update(Analyzer* analyzer,EState* estate) {
  VariableIdSet hotVariables=getHotVariables(analyzer,estate);
  const PState* pstate=estate->pstate();
  if(pstate->size()!=_variablesMap.size()) {
    //cerr<<"WARNING: variable map size mismatch (probably local var)"<<endl;
    //cerr<<"... reinitializing."<<endl;
    init(estate);
  }
      
  VariableIdSet varIdSet=pstate->getVariableIds();
  for(VariableIdSet::iterator i=varIdSet.begin(); i!=varIdSet.end(); ++i) {
    VariableId varId=*i;
    bool isHotVariable=hotVariables.find(varId)!=hotVariables.end();
    if(!isHotVariable) {
      if(pstate->varIsConst(varId)) {
        AValue abstractVal=pstate->varValue(varId);
        ROSE_ASSERT(abstractVal.isConstInt());
        int intVal=abstractVal.getIntValue();
        _variablesMap[varId]->insert(intVal);
      }
    }
  }
}

VariableIdSet VariableValueMonitor::getVariables() {
  VariableIdSet vset;
  for(map<VariableId,VariableMode>::iterator i=_variablesModeMap.begin();
      i!=_variablesModeMap.end();
      ++i) {
    vset.insert((*i).first);
  }
  return vset;
}

bool VariableValueMonitor::isHotVariable(Analyzer* analyzer, VariableId varId) {
  // TODO: provide set of variables to ignore
  string name=SgNodeHelper::symbolToString(analyzer->getVariableIdMapping()->getSymbol(varId));
  switch(_variablesModeMap[varId]) {
  case VariableValueMonitor::VARMODE_FORCED_TOP:
    return true;
  case VariableValueMonitor::VARMODE_ADAPTIVE_TOP: {
    if(name=="input" || name=="output") 
      return false;
    else
      return _threshold!=-1 && ((long int)_variablesMap[varId]->size())>=_threshold;
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
  for(map<VariableId,set<int>* >::iterator i=_variablesMap.begin();
      i!=_variablesMap.end();
      ++i) {
    ss<<string("VAR:")<<variableIdMapping->uniqueShortVariableName((*i).first)<<": "<<(*i).second->size()<<": ";
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
