
#include "sage3basic.h"

#include "RDLattice.h"
#include "SetAlgo.h"

using namespace std;
using namespace SPRAY;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
RDLattice::RDLattice() {
  setBot();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
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
        os<<vim->uniqueShortVariableName((*i).second);
      else
        os<<(*i).second.toString();
      os<<")";
    }
    os<<"}";
  }
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
RDLattice::iterator RDLattice::begin() {
  return rdSet.begin();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
RDLattice::iterator RDLattice::end() {
  return rdSet.end();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
size_t RDLattice::size() {
  return rdSet.size();
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDLattice::insertPair(Label lab,VariableId var) {
  pair<Label,VariableId> p=make_pair(lab,var);
  rdSet.insert(p);
  _bot=false;
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDLattice::erasePair(Label lab,VariableId var) {
  pair<Label,VariableId> p=make_pair(lab,var);
  rdSet.erase(p);
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDLattice::removeAllPairsWithVariableId(VariableId var) {
  RDLattice::iterator i=rdSet.begin();
  while(i!=rdSet.end()) {
    if(var==(*i).second) {
       rdSet.erase(i++);
    }
     else
       ++i;
  }
}
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool RDLattice::isBot() {
  return _bot;
} 
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDLattice::setBot() {
  _bot=true;
} 

#if 1
/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDLattice::combine(Lattice& b) {
  RDLattice* other=dynamic_cast<RDLattice*>(&b);
  ROSE_ASSERT(other);
  if(b.isBot()) {
    return;
  }
  for(RDLattice::iterator i=other->begin();i!=other->end();++i) {
    rdSet.insert(*i);
  }
  _bot=false;
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool RDLattice::approximatedBy(Lattice& b0) {
  RDLattice& b=dynamic_cast<RDLattice&>(b0);
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
  for(RDLattice::iterator i=begin();i!=end();++i) {
    if(!b.exists(*i))
      return false;
  }
  return true;
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
bool RDLattice::exists(pair<Label,VariableId> p) {
  return rdSet.find(p)!=end();
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
void RDLattice::setEmptySet() {
  _bot=false;
  rdSet.clear();
}

#endif
