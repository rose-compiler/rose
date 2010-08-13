/*  -*-Mode: c++; -*-  */

#ifndef GRID_COLLECTION_OPERATORS_H
#define GRID_COLLECTION_OPERATORS_H

#include "GenericGridCollectionOperators.h"
#include "MappedGridOperators.h"
// include "ListOfMappedGridOperators.h"

//-----------------------------------------------------------------------------------
//  
//  Define differential and Boundary operators associated with GridCollections,
//  GridCollectionFunctions (and CompositeGrid/CompositeGridFunctions)
//
//-----------------------------------------------------------------------------------

class GridCollectionOperators : public GenericGridCollectionOperators
{

 public:

  GridCollectionOperators();
  GridCollectionOperators( GridCollection & gc );                            // use this GridCollection
  GridCollectionOperators( MappedGridOperators & op );                       // use these operators
  GridCollectionOperators( GridCollection & gc, MappedGridOperators & op );  // use both

  GridCollectionOperators( const GridCollectionOperators & mgfd );  // copy constructor
  virtual GenericGridCollectionOperators* virtualConstructor();        // create a new object of this class

  ~GridCollectionOperators();

  GridCollectionOperators & operator= ( const GridCollectionOperators & dmgf );
  GenericGridCollectionOperators & operator= ( const GenericGridCollectionOperators & mgo );

  MappedGridOperators & operator[]( const int grid ) const;   // here is the operators for a MappedGrid

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  void updateToMatchGrid( GridCollection & gc );              // here is a (new) GridCollection to use
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
