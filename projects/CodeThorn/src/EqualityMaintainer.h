#ifndef EQUALITY_MAINTAINER_H
#define EQUALITY_MAINTAINER_H

#include <list>
#include <set>
#include <string>

/*! 
  * \author Markus Schordan
  * \date 2012.
  * requires for type T the following operators: default constructor, 
 */
template<typename T>
class EqualityMaintainer {
 public:
  // several of these functions can be improved (with additional data structures) to become O(1),
  void addEquality(T var1, T var2);
  // removes this specific equality
  bool removeEquality(T var1, T var2);
  // removes all equalities of var1
  bool removeEqualities(T var1);
  /*!
     Returns one element of the equality set in which var is an
     element of. This function is useful for selecting a dedicated element
     of the equality set of all elements of which var1 is a member of.
     NOTE: the dedicated element can change, if a new element is added
  */
  T determineDedicatedElement(const T var) const;
  //! determines whether an equality exists
  bool equalityExists(const T var1, const T var2) const;
  //! return all equal elements (including the element itself)
  std::set<T> equalElements(const T var) const;
  //! returns the number of equal elements (the size of the equality set)
  int numberOfEqualElements(const T var) const;
  //! prints all equality sets (this includes all elements, also single elements)
  std::string toString();
 private:
  std::list< std::set<T> > equality;
};

#include "EqualityMaintainer.C"


#endif
