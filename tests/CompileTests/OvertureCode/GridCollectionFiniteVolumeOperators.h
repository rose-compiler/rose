/*  -*-Mode: c++; -*-  */

#ifndef GRID_COLLECTION_FINITE_VOLUME_OPERATORS
#define GRID_COLLECTION_FINITE_VOLUME_OPERATORS

#include "GenericGridCollectionOperators.h"
#include "MappedGridFiniteVolumeOperators.h"



//-----------------------------------------------------------------------------------
//  
//  Define finite volume and Boundary operators associated with GridCollections,
//  GridCollectionFunctions (and CompositeGrid/CompositeGridFunctions)
//  960119 DLB from WDH's GridCollectionOperators.h
//
//-----------------------------------------------------------------------------------

class GridCollectionFiniteVolumeOperators : public GenericGridCollectionOperators
{

 public:

  GridCollectionFiniteVolumeOperators();
  GridCollectionFiniteVolumeOperators( GridCollection & gc );                            // use this GridCollection
  GridCollectionFiniteVolumeOperators( MappedGridFiniteVolumeOperators & op );           // use these operators
  GridCollectionFiniteVolumeOperators( GridCollection & gc, MappedGridFiniteVolumeOperators & op );  // use both

  GridCollectionFiniteVolumeOperators( const GridCollectionFiniteVolumeOperators & mgfd );  // copy constructor
  virtual GenericGridCollectionOperators* virtualConstructor();        // create a new object of this class

  ~GridCollectionFiniteVolumeOperators();

  GridCollectionFiniteVolumeOperators & operator= ( const GridCollectionFiniteVolumeOperators & dmgf );
  GenericGridCollectionOperators & operator= ( const GenericGridCollectionOperators & mgo );
  MappedGridFiniteVolumeOperators & operator[]( const int grid );   // here is the operators for a MappedGrid

  void updateToMatchGrid( GridCollection & gc );              // here is a (new) GridCollection to use
  // void updateToMatchOperators(MappedGridOperators & op);      // Use these operators


// don't need these; use base class versions
//  virtual void get( const Dir & dir, const aString & name);    // get from a database file
//  virtual void put( const Dir & dir, const aString & name);    // put to a database file

  void setIsVolumeScaled (const bool trueOrFalse);
  bool getIsVolumeScaled ();
  
  // ------------- Here we define the Boundary Condition Routines ---------------

 public:
    
  void setNumberOfBoundaryConditions(const int & number,
                                     const int & side = forAll,
				     const int & axis = forAll,
				     const int & grid0 = forAll
				     );

  void setBoundaryCondition(const MappedGridFiniteVolumeOperators::boundaryConditionTypes & boundaryConditionType );

 
  void setBoundaryConditionValue( const real & value,
                                 const int & component,
				 const int & index,
				 const int & side = forAll,
				 const int & axis = forAll,
				 const int & grid0 = forAll);

  void setBoundaryConditionRightHandSide (const realGridCollectionFunction & boundaryConditionRightHandSide);
 
/* use the generic one
  void applyBoundaryConditions(
                               realGridCollectionFunction & u,
			       const real & time = 0.,
			       const int & grid = forAll
			       );
*/

  void applyRightHandSideBoundaryConditions(
					    realCompositeGridFunction & rightHandSide,
					    const real & time = 0.
					    );
 
  void applyBoundaryConditionsToCoefficients(realGridCollectionFunction & coeff, // replaces ApplyBoundaryConditions
                                             const real & time = 0.);
  

 private:

  bool mappedGridOperatorWasNewed;       // TRUE if we have newed a MappedGridOperators
  int numberOfComponentGrids;            // local copy for convenience
  int numberOfGrids;                     // replaces numberOfComponentGrids in newer versions of Overture
  int numberOfDimensions;                // local copy for convenience
  bool isVolumeScaled;

};






#endif
