#ifndef __CONSTRAINTLIST_H__
#define __CONSTRAINTLIST_H__

#include "ROSETTA_macros.h"

// #include <stdio.h>
// #include <assert.h>

// #include <list.h>
// #ifndef STL_LIST_IS_BROKEN
// #include STL_LIST_HEADER_FILE
// #endif

class Constraint;

class ConstraintListType : public std::list<Constraint *>
{
  // Terminals contain a ConstraintList which is just an STL list of contraints
  // implemented through this interface class which lets us add customized functionality
  // to the list class (without modifying STL).

 public:
  ~ConstraintListType ();
  ConstraintListType ();
  ConstraintListType ( const Constraint & X );
  ConstraintListType ( const std::list<Constraint *> & X );
  // ConstraintListType ( const ConstraintListType & X ); -- JJW 10-16-2007 compiler-generated copy constructor should work
  ConstraintListType & operator= ( const ConstraintListType & X );

  Constraint & operator[] ( char* name ) const;

  bool containedInList ( char* name ) const;

  void consistencyCheck() const;
  void addConstraint( const Constraint* X );
};

#endif
