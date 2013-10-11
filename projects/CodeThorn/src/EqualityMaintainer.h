#ifndef EQUALITY_MAINTAINER_H
#define EQUALITY_MAINTAINER_H

#include <list>
#include <set>
#include <string>

using namespace std;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename T>
class EqualityMaintainer {
 public:
  void addEquality(T var1, T var2);
  bool removeEquality(T var1, T var2);
  bool removeEqualities(T var1);
  /*! according to normalization criterion: the dedicated element is
     the element which binds all associated constraints of equal
     elements. Equal elements are those for which an equality
     constraint exists (transitive closure).
  */
  T determineDedicatedElement(const T var) const;
  bool equalityExists(const T var1, const T var2) const;
  //! return all equal elements (including the element itself)
  set<T> equalElements(const T var) const;
  int numberOfEqualElements(const T var) const;
  string toString();
 private:
  list< set<T> > equality;
};

#include "EqualityMaintainer.C"


#endif
