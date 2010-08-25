/*  -*-Mode: c++; -*-  */
#ifndef COMPOSITE_GRID_FINITE_VOLUME_OPERATORS
#define COMPOSITE_GRID_FINITE_VOLUME_OPERATORS

#include "GridCollectionFiniteVolumeOperators.h"
// *wdh*
#include "GenericCompositeGridOperators.h"

//-----------------------------------------------------------------------------------
//  
//  Define differential and Boundary operators associated with CompositeGrid's
//  CompositeGridFunctions (and CompositeGrid/CompositeGridFunctions)
//
// Notes:
//  o this class derives from GridCollectionOperators. It derives all member
//    data and member functions except for constrcutors, destructors and = operator
//-----------------------------------------------------------------------------------

//class CompositeGridFiniteVolumeOperators : public GridCollectionFiniteVolumeOperators
class CompositeGridFiniteVolumeOperators : public GenericCompositeGridOperators
{

 public:

  CompositeGridFiniteVolumeOperators();
  CompositeGridFiniteVolumeOperators( CompositeGrid & cg );                             // use this CompositeGrid
  CompositeGridFiniteVolumeOperators( MappedGridFiniteVolumeOperators & op );                       // use these operators
  CompositeGridFiniteVolumeOperators( CompositeGrid & cg, MappedGridFiniteVolumeOperators & op );   // use both
  CompositeGridFiniteVolumeOperators( const CompositeGridFiniteVolumeOperators & cgo );             // copy constructor
//  CompositeGridFiniteVolumeOperators( const GridCollectionFiniteVolumeOperators & cgo );   //v3: dont think we need this anymore
//  virtual GenericGridCollectionOperators* virtualConstructor();            // create a new object of this class
  virtual GenericCompositeGridOperators* virtualConstructor();            // create a new object of this class
  ~CompositeGridFiniteVolumeOperators();
  CompositeGridFiniteVolumeOperators & operator= ( const CompositeGridFiniteVolumeOperators & cgo );
  GenericCompositeGridOperators & operator= ( const GenericCompositeGridOperators & mgo );
// ...eliminated for v3
//  CompositeGridFiniteVolumeOperators & operator= ( const GridCollectionFiniteVolumeOperators & gco );

//  ...moved to base class in Overture.v3
//  void finishBoundaryConditions(realGridCollectionFunction &u);

  void updateToMatchGrid (CompositeGrid & gc);
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

 
  void setBoundaryConditionValue(const real & value,
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

  

// *** this is here for later compatibility with Overture.v1
/*
   void getInterpolationCoefficients (realCompositeGridFunction &u)
  {cout << "ERROR: CompositeGridFiniteVolumeOperators:getInterpolationCoefficients has been called" << endl;};
*/
  

};

#endif
