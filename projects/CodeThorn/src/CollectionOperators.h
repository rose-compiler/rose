#ifndef COLLECTIONOPERATORS_H
#define COLLECTIONOPERATORS_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

// set intersection
template<typename T>
std::set<T> operator*(std::set<T>& s1, std::set<T>& s2) {
  std::set<T> result;
  for(typename std::set<T>::iterator i1=s1.begin();i1!=s1.end();++i1) {
    for(typename std::set<T>::iterator i2=s2.begin();i2!=s2.end();++i2) {
      if(s2.find(*i1)!=s2.end()) {
		result.insert(*i1);
      }
    }
  }
  return result;
}

// set difference
template<typename T>
std::set<T> operator-(std::set<T>& s1, std::set<T>& s2) {
  std::set<T> result=s1;
  for(typename std::set<T>::iterator i=s2.begin();i!=s2.end();++i) {
    result.erase(*i);
  }
  return result;
}

// set union
template<typename T>
std::set<T> operator+(std::set<T>& s1, std::set<T>& s2) {
  std::set<T> result=s1;
  for(typename std::set<T>::iterator i=s2.begin();i!=s2.end();++i) {
    result.insert(*i);
  }
  return result;
}

#endif
