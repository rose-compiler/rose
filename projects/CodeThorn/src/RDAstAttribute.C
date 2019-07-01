#include "sage3basic.h"
#include "RDAstAttribute.h"

#include <sstream>

using namespace std;
using namespace CodeThorn;

void RDAstAttribute::toStream(ostream& os, VariableIdMapping* vim) {
  os<<"RD:";
  _elem->toStream(os,vim);
}

string RDAstAttribute::toString() {
  stringstream ss;
  toStream(ss,0);
  return ss.str();
}

RDAstAttribute::RDAstAttribute(RDLattice* elem):_elem(elem) {
}
bool RDAstAttribute::isBottomElement() {
  return _elem->isBot();
}
VariableIdSet RDAstAttribute::allVariableIds() {
  VariableIdSet set;
  for(RDAstAttribute::iterator i=begin();i!=end();++i) {
    set.insert((*i).second);
  }
  return set;
}
LabelSet RDAstAttribute::allLabels() {
  LabelSet ls;
  for(RDAstAttribute::iterator i=begin();i!=end();++i) {
    ls.insert((*i).first);
  }
  return ls;
}
LabelSet RDAstAttribute::definitionsOfVariableId(VariableId varId) {
  LabelSet ls;
  for(RDAstAttribute::iterator i=begin();i!=end();++i) {
    if(varId==(*i).second)
      ls.insert((*i).first);
  }
  return ls;
}
VariableIdSet RDAstAttribute::variableIdsOfDefinition(Label def) {
  VariableIdSet vset;
  for(RDAstAttribute::iterator i=begin();i!=end();++i) {
    if(def==(*i).first)
      vset.insert((*i).second);
  }
  return vset;
}
RDAstAttribute::iterator
RDAstAttribute::begin() {
  return _elem->begin();
}
RDAstAttribute::iterator
RDAstAttribute::end() {
  return _elem->end();
}
RDAstAttribute::~RDAstAttribute() {
}
