
#include "sage3basic.h"

#include "LVLattice.h"
#include "SetAlgo.h"

using namespace std;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
CodeThorn::LVLattice::LVLattice() {
  setBot();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
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

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
CodeThorn::LVLattice::iterator CodeThorn::LVLattice::begin() const {
  return lvSet.begin();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
CodeThorn::LVLattice::iterator CodeThorn::LVLattice::end() const {
  return lvSet.end();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
size_t CodeThorn::LVLattice::size() const {
  return lvSet.size();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void CodeThorn::LVLattice::insertVariableId(VariableId var) {
  lvSet.insert(var);
  _bot=false;
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void CodeThorn::LVLattice::removeVariableId(VariableId var) {
  lvSet.erase(var);
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool CodeThorn::LVLattice::isBot() const {
  return _bot;
} 
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void CodeThorn::LVLattice::setBot() {
  _bot=true;
} 

#if 1
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
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
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
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
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool CodeThorn::LVLattice::exists(VariableId v) {
  return lvSet.find(v)!=end();
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void CodeThorn::LVLattice::setEmptySet() {
  _bot=false;
  lvSet.clear();
}

#endif
