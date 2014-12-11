
#include "sage3basic.h"

#include "LVLattice.h"
#include "SetAlgo.h"

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
SPRAY::LVLattice::LVLattice() {
  setBot();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void SPRAY::LVLattice::toStream(ostream& os, VariableIdMapping* vim) {
  if(isBot()) {
    os<<"bot";
  } else {
    os<<"{";
    for(SPRAY::LVLattice::iterator i=begin();i!=end();++i) {
      if(i!=begin())
        os<<",";
      if(vim)
        os<<vim->uniqueShortVariableName(*i);
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
SPRAY::LVLattice::iterator SPRAY::LVLattice::begin() {
  return lvSet.begin();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
SPRAY::LVLattice::iterator SPRAY::LVLattice::end() {
  return lvSet.end();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
size_t SPRAY::LVLattice::size() {
  return lvSet.size();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void SPRAY::LVLattice::insertVariableId(VariableId var) {
  lvSet.insert(var);
  _bot=false;
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void SPRAY::LVLattice::eraseVariableId(VariableId var) {
  lvSet.erase(var);
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool SPRAY::LVLattice::isBot() {
  return _bot;
} 
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void SPRAY::LVLattice::setBot() {
  _bot=true;
} 

#if 1
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void SPRAY::LVLattice::combine(Lattice& b) {
  LVLattice* other=dynamic_cast<LVLattice*>(&b);
  ROSE_ASSERT(other);
  if(b.isBot()) {
    return;
  }
  for(SPRAY::LVLattice::iterator i=other->begin();i!=other->end();++i) {
    lvSet.insert(*i);
  }
  _bot=false;
}
void SPRAY::LVLattice::combine(LVLattice& b) {
  ROSE_ASSERT(false);
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool SPRAY::LVLattice::approximatedBy(Lattice& b0) {
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
  for(SPRAY::LVLattice::iterator i=begin();i!=end();++i) {
    if(!b.exists(*i))
      return false;
  }
  return true;
}
// OUTDATED
bool SPRAY::LVLattice::approximatedBy(LVLattice& b0) {
  cerr<<"SPRAY::LVLattice::approximatedBy(LVLattice) : outdated."<<endl;
  exit(1);
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool SPRAY::LVLattice::exists(VariableId v) {
  return lvSet.find(v)!=end();
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void SPRAY::LVLattice::setEmptySet() {
  _bot=false;
  lvSet.clear();
}

#endif
