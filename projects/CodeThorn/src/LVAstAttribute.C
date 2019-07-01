#include "sage3basic.h"
#include "LVAstAttribute.h"

#include <sstream>

using namespace std;
CodeThorn::LVAstAttribute::LVAstAttribute(CodeThorn::LVLattice* elem):_elem(elem) {
}

bool CodeThorn::LVAstAttribute::isLive(CodeThorn::VariableId variableId) {
  return _elem->exists(variableId);
}

void CodeThorn::LVAstAttribute::toStream(ostream& os, CodeThorn::VariableIdMapping* vim) {
  os<<"LV:";
  _elem->toStream(os,vim);
}

string CodeThorn::LVAstAttribute::toString() {
  stringstream ss; 
  toStream(ss,0);
  return ss.str();
}

bool CodeThorn::LVAstAttribute::isBottomElement() {
  return _elem->isBot();
}

CodeThorn::VariableIdSet CodeThorn::LVAstAttribute::allVariableIds() {
  CodeThorn::VariableIdSet set;
  for(CodeThorn::LVLattice::iterator i=begin();i!=end();++i) {
    set.insert(*i);
  }
  return set;
}
CodeThorn::LVLattice::iterator
CodeThorn::LVAstAttribute::begin() {
  return _elem->begin();
}
CodeThorn::LVLattice::iterator
CodeThorn::LVAstAttribute::end() {
  return _elem->end();
}
CodeThorn::LVAstAttribute::~LVAstAttribute() {
}
