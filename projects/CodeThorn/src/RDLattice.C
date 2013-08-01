
#include "sage3basic.h"

#include "RDLattice.h"
#include "SetAlgo.h"

RDLattice::RDLattice() {
  setBot();
}
void RDLattice::toStream(ostream& os, VariableIdMapping* vim) {
  if(isBot()) {
    os<<"bot";
  } else {
    os<<"{";
    for(RDLattice::iterator i=begin();i!=end();++i) {
      if(i!=begin())
        os<<",";
      os<<"(";
      os<<(*i).first;
      os<<",";
      if(vim)
        os<<vim->variableName((*i).second)<<"_";
      os<<(*i).second.toString();
      os<<")";
    }
    os<<"}";
  }
}

RDLattice::iterator RDLattice::begin() {
  return rdSet.begin();
}
RDLattice::iterator RDLattice::end() {
  return rdSet.end();
}
size_t RDLattice::size() {
  return rdSet.size();
}
void RDLattice::insertPair(Label lab,VariableId var) {
  pair<Label,VariableId> p=make_pair(lab,var);
  rdSet.insert(p);
  _bot=false;
}
void RDLattice::erasePair(Label lab,VariableId var) {
  pair<Label,VariableId> p=make_pair(lab,var);
  rdSet.erase(p);
}
void RDLattice::eraseAllPairsWithVariableId(VariableId var) {
  for(RDLattice::iterator i=rdSet.begin();i!=rdSet.end();++i) {
    if(var==(*i).second)
      rdSet.erase(*i);
  }
}
bool RDLattice::isBot() {
  return _bot;
} 
void RDLattice::setBot() {
  _bot=true;
} 

#if 1
void RDLattice::combine(RDLattice& b) {
  if(b.isBot())
    return;
  for(RDLattice::iterator i=b.begin();i!=b.end();++i) {
    rdSet.insert(*i);
  }
  _bot=false;
}
bool RDLattice::approximatedBy(RDLattice& b) {
  if(isBot()) {
    return true;
  } else {
    if(b.isBot()) {
      return false;
    }
  }
  assert(!isBot()&&!b.isBot());
  if(size()>b.size())
     return false;
  for(RDLattice::iterator i=begin();i!=end();++i) {
    if(!b.exists(*i))
      return false;
  }
  return true;
}
bool RDLattice::exists(pair<Label,VariableId> p) {
  return rdSet.find(p)!=end();
}

void RDLattice::setEmptySet() {
  _bot=false;
  rdSet.clear();
}

#endif
