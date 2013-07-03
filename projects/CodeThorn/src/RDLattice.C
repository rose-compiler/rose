#include "RDLattice.h"
#include "SetAlgo.h"

void RDLattice::toStream(ostream& os, VariableIdMapping* vim) {
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

RDLattice::iterator RDLattice::begin() {
  return rdSet.begin();
}
RDLattice::iterator RDLattice::end() {
  return rdSet.end();
}
void RDLattice::insertPair(Label lab,VariableId var) {
  pair<Label,VariableId> p=make_pair(lab,var);
  rdSet.insert(p);
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
  return rdSet.size()==0;
} 

#if 1
void RDLattice::combine(RDLattice& b) {
  for(RDLattice::iterator i=b.begin();i!=b.end();++i) {
	rdSet.insert(*i);
  }
}
bool RDLattice::approximatedBy(RDLattice& b) {
  for(RDLattice::iterator i=begin();i!=end();++i) {
	if(!b.exists(*i))
	  return false;
  }
  return true;
}
bool RDLattice::exists(pair<Label,VariableId> p) {
  return rdSet.find(p)!=end();
}
#endif
