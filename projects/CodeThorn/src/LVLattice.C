
#include "sage3basic.h"

#include "LVLattice.h"

using namespace std;

CodeThorn::LVLattice::LVLattice() {
  setBot();
}

void CodeThorn::LVLattice::toStream(ostream& os, VariableIdMapping* vim) {
  if(isBot()) {
    os<<"bot";
  } else {
    os<<"{";
    for(CodeThorn::LVLattice::iterator i=begin();i!=end();++i) {
      if(i!=begin())
        os<<",";
      if(vim)
        os<<vim->uniqueVariableName(*i);
      else
        os<<(*i).toString();
    }
    os<<"}";
  }
}

CodeThorn::LVLattice::iterator CodeThorn::LVLattice::begin() const {
  return lvSet.begin();
}

CodeThorn::LVLattice::iterator CodeThorn::LVLattice::end() const {
  return lvSet.end();
}

size_t CodeThorn::LVLattice::size() const {
  return lvSet.size();
}

void CodeThorn::LVLattice::insertVariableId(VariableId var) {
  lvSet.insert(var);
  _bot=false;
}

void CodeThorn::LVLattice::removeVariableId(VariableId var) {
  lvSet.erase(var);
}

bool CodeThorn::LVLattice::isBot() const {
  return _bot;
} 

void CodeThorn::LVLattice::setBot() {
  _bot=true;
} 

#if 1
void CodeThorn::LVLattice::combine(Lattice& b) {
  LVLattice* other=dynamic_cast<LVLattice*>(&b);
  ROSE_ASSERT(other);
  if(b.isBot()) {
    return;
  }
  for(CodeThorn::LVLattice::iterator i=other->begin();i!=other->end();++i) {
    lvSet.insert(*i);
  }
  _bot=false;
}
void CodeThorn::LVLattice::combine(LVLattice& b) {
  ROSE_ASSERT(false);
}

bool CodeThorn::LVLattice::approximatedBy(Lattice& b0) const {
  LVLattice& b=dynamic_cast<LVLattice&>(b0);
  if(isBot()&&b.isBot())
    return true;
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
  for(CodeThorn::LVLattice::iterator i=begin();i!=end();++i) {
    if(!b.exists(*i))
      return false;
  }
  return true;
}
// OUTDATED
bool CodeThorn::LVLattice::approximatedBy(LVLattice& b0) {
  cerr<<"CodeThorn::LVLattice::approximatedBy(LVLattice) : outdated."<<endl;
  exit(1);
}

bool CodeThorn::LVLattice::exists(VariableId v) {
  return lvSet.find(v)!=end();
}

void CodeThorn::LVLattice::setEmptySet() {
  _bot=false;
  lvSet.clear();
}

#endif
