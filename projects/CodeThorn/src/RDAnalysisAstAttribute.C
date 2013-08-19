#include "sage3basic.h"
#include "RDAnalysisAstAttribute.h"

#include <sstream>

using namespace std;

void RDAnalysisAstAttribute::toStream(ostream& os, VariableIdMapping* vim) {
  _elem->toStream(os,vim);
}

string RDAnalysisAstAttribute::getPreInfoString() { return ""; }
string RDAnalysisAstAttribute::getPostInfoString() {
  stringstream ss; 
  toStream(ss,0);
  return ss.str();
}

RDAnalysisAstAttribute::RDAnalysisAstAttribute(RDLattice* elem):_elem(elem) {
}
bool RDAnalysisAstAttribute::isBottomElement() {
  return _elem->isBot();
}
VariableIdSet RDAnalysisAstAttribute::allVariableIds() {
  VariableIdSet set;
  for(RDAnalysisAstAttribute::iterator i=begin();i!=end();++i) {
	set.insert((*i).second);
  }
  return set;
}
LabelSet RDAnalysisAstAttribute::allLabels() {
  LabelSet ls;
  for(RDAnalysisAstAttribute::iterator i=begin();i!=end();++i) {
	ls.insert((*i).first);
  }
  return ls;
}
LabelSet RDAnalysisAstAttribute::definitionsOfVariableId(VariableId varId) {
  LabelSet ls;
  for(RDAnalysisAstAttribute::iterator i=begin();i!=end();++i) {
	if(varId==(*i).second)
	  ls.insert((*i).first);
  }
  return ls;
}
VariableIdSet RDAnalysisAstAttribute::variableIdsOfDefinition(Label def) {
  VariableIdSet vset;
  for(RDAnalysisAstAttribute::iterator i=begin();i!=end();++i) {
	if(def==(*i).first)
	  vset.insert((*i).second);
  }
  return vset;
}
RDAnalysisAstAttribute::iterator
RDAnalysisAstAttribute::begin() {
  return _elem->begin();
}
RDAnalysisAstAttribute::iterator
RDAnalysisAstAttribute::end() {
  return _elem->end();
}
RDAnalysisAstAttribute::~RDAnalysisAstAttribute() {
}
