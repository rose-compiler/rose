#include "RDLattice.h"
#include "SetAlgo.h"

void RDLattice::toStream(ostream& os) {
  os<<"{";
  for(RDLattice::iterator i=begin();i!=end();++i) {
	if(i!=begin())
	  os<<",";
	os<<"(";
	os<<(*i).first;
	os<<",";
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
bool RDLattice::isBot() {
  return rdSet.size()==0;
} 

#if 1
RDLattice& RDLattice::combine(RDLattice& b) {
  for(RDLattice::iterator i=b.begin();i!=b.end();++i) {
	rdSet.insert(*i);
  }
  return *this;
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
