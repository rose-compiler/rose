#ifndef EQUALITY_MAINTAINER_C
#define EQUALITY_MAINTAINER_C

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

template<typename T>
void EqualityMaintainer<T>::addEquality(T var1, T var2) {
  // a) determine whether var1 exists (found+=1)
  // b) determine whether var2 exists (found+=2)
  // case 0) none of the two vars exists: add {var1,var2}
  // case 1,2) only one of the two vars exists: add the other var to this set
  // case 3 a) both vars exist in the same set: nothing to do
  // case 3 b) the two vars exist in different sets: union those two sets

  int found=0;
  typename list<set<T> >::iterator i1=equality.end();
  typename list<set<T> >::iterator i2=equality.end();
  for(typename list<set<T> >::iterator i=equality.begin();i!=equality.end();++i) {
    typename set<T>::const_iterator j1;
    typename set<T>::const_iterator j2;
    if((j1=(*i).find(var1))!=(*i).end()) {
      found+=1;
      i1=i;
    }
    if((j2=(*i).find(var2))!=(*i).end()) {
      found+=2;
      i2=i;
    }
  }
  switch(found) {
  case 0: { set<T> s; s.insert(var1);s.insert(var2); equality.push_front(s);return;}
  case 1: { (*i1).insert(var2);return;}
  case 2: { (*i2).insert(var1);return;}
  case 3: 
    if(i1==i2) {
      // both elements are in the same set (already) => nothing to do
      return;
    } else {
      // each element in a different set => union sets
      // copy set *i2 into *i1 and remove *i2; 
      for(typename set<T>::iterator k=(*i2).begin();k!=(*i2).end();++k) {
        (*i1).insert(*k);
      }
      equality.erase(i2);
    }
    return;
  default: throw "Error: EqualityMaintainer::addEquality: programmatic error.";
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

template<typename T>
bool EqualityMaintainer<T>::removeEqualities(T var) {
  for(typename list<set<T> >::iterator i=equality.begin();i!=equality.end();++i) {
    typename set<T>::const_iterator j1;
    if((j1=(*i).find(var))!=(*i).end()) {
      (*i).erase(*j1);
      if((*i).size()<=1) {
        equality.erase(i);
      }
      return true;
    }
  }
  return false;
}

template<typename T>
T EqualityMaintainer<T>::determineDedicatedElement(const T var) const {
  for(typename list<set<T> >::const_iterator i=equality.begin();i!=equality.end();++i) {
    if((*i).find(var)!=(*i).end()) {
      // variable exists in this set. Because the set is sorted, the
      // dedicated var is the very first one in the set and we can use
      // the iterator to determine the dedicated var
      return *((*i).begin());
    }
  }
  // if no equality is found then the element itself is the dedicated element
  return var;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename T>
bool EqualityMaintainer<T>::equalityExists(const T var1, const T var2) const {
  // both vars must exist in same set
  // alternativly: both vars must have the same associated dedicated var
  return determineDedicatedElement(var1)==determineDedicatedElement(var2);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename T>
set<T> EqualityMaintainer<T>::equalElements(const T var) const {
  for(typename list<set<T> >::const_iterator i=equality.begin();i!=equality.end();++i) {
    if((*i).find(var)!=(*i).end())
      return *i;
  }
  set<T> s; // empty set
  s.insert(var);
  return s;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename T>
int EqualityMaintainer<T>::numberOfEqualElements(const T var) const {
  return equalElements(var).size();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename T>
string EqualityMaintainer<T>::toString() {
  string res;
  res+="[";
  for(typename list<set<T> >::iterator i=equality.begin();i!=equality.end();++i) {
    if(i!=equality.begin())
      res+=", ";
    res+="{";
    for(typename set<T>::iterator j=(*i).begin();j!=(*i).end();++j) {
      if(j!=(*i).begin())
      res+=", ";
      res+=(*j).variableName();
    }
    res+="}";
  }
  res+="]";
  return res;
}

#endif
