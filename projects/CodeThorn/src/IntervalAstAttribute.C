#include "sage3basic.h"
#include "IntervalAstAttribute.h"

#include <sstream>

using namespace std;

void IntervalAstAttribute::toStream(ostream& os, VariableIdMapping* vim) {
  _elem->toStream(os,vim);
}

string IntervalAstAttribute::toString() {
  stringstream ss; 
  toStream(ss,0);
  return ss.str();
}

IntervalAstAttribute::IntervalAstAttribute(IntervalPropertyState* elem):_elem(elem) {
}
bool IntervalAstAttribute::isBottomElement() {
  return _elem->isBot();
}
VariableIdSet IntervalAstAttribute::allVariableIds() {
  VariableIdSet set;
  for(IntervalAstAttribute::iterator i=begin();i!=end();++i) {
    set.insert((*i).second);
  }
  return set;
}

#if 1
IntervalAstAttribute::iterator
IntervalAstAttribute::begin() {
  return _elem->begin();
}
IntervalAstAttribute::iterator
IntervalAstAttribute::end() {
  return _elem->end();
}
#endif

IntervalAstAttribute::~RDAstAttribute() {
}
