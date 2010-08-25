/*  -*-Mode: c++; -*-  */

#ifndef COMPOSITE_GRID_OPERATORS_H
#define COMPOSITE_GRID_OPERATORS_H

#include "GenericCompositeGridOperators.h"
#include "MappedGridOperators.h"
// include "ListOfMappedGridOperators.h"

//-----------------------------------------------------------------------------------
//  
//  Define differential and Boundary operators associated with GridCollections,
//  GridCollectionFunctions (and CompositeGrid/CompositeGridFunctions)
//
//-----------------------------------------------------------------------------------

class CompositeGridOperators : public GenericCompositeGridOperators
{

 public:

  CompositeGridOperators();
  CompositeGridOperators( CompositeGrid & gc );                            // use this CompositeGrid
  CompositeGridOperators( MappedGridOperators & op );                       // use these operators
  CompositeGridOperators( CompositeGrid & gc, MappedGridOperators & op );  // use both

  CompositeGridOperators( const CompositeGridOperators & mgfd );  // copy constructor
  virtual GenericCompositeGridOperators* virtualConstructor();        // create a new object of this class

  ~CompositeGridOperators();

  CompositeGridOperators & operator= ( const CompositeGridOperators & dmgf );
  GenericCompositeGridOperators & operator= ( const GenericCompositeGridOperators & mgo );

  MappedGridOperators & operator[]( const int grid ) const;   // here is the operators for a MappedGrid

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  void updateToMatchGrid( CompositeGrid & gc );              // here is a (new) CompositeGrid to use
  // void updateToMatchOperators(MappedGridOperators & op);      // Use these operators

  // get from a database file
  virtual int get( const GenericDataBase & dir, const aString & name);  
  // put to a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;  
  
  // ------------- Here we define the Boundary Condition Routines ---------------

 public:
    
  void setNumberOfBoundaryConditions(const int & number, 
                                     const int & side=forAll, 
                                     const int & axis=forAll, 
				     const int & grid=forAll);
  
  void setBoundaryCondition(const MappedGridOperators::boundaryConditionTypes & boundaryConditionType );

  void setBoundaryConditionValue(const real & value,  
                                 const int & component=forAll, 
				 const int & index=forAll, 
                                 const int & side=forAll, 
                                 const int & axis=forAll, 
                                 const int & grid=forAll);

 private:

  bool mappedGridOperatorWasNewed;       // TRUE if we have newed a MappedGridOperators



};






#endif
