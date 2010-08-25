/*  -*-Mode: c++; -*-  */

#ifndef DOUBLE_MAPPED_GRID_FUNCTION_H
#define DOUBLE_MAPPED_GRID_FUNCTION_H "doubleMappedGridFunction.h"
//========================================================================================================
//    This file defines the header file for the mappedGridFunction Class
//
//  The perl script gf.p is used to convert this file into the files
//      <type>doubleMappedGridFunction.C  where <type> is one of float, double or int
//
//  Notes:
//   o a doubleMappedGridFunction is derived from an A++ array
//   o a doubleMappedGridFunction contains a pointer to a MappedGrid
//
//  Who to blame: Bill Henshaw, CIC-3, henshaw@lanl.gov
//  Date of last change:  95/04/09
//  
//========================================================================================================

#include <limits.h>            
#include "aString.H"            // Livermore aString Library
#include "doubleGenericGridFunction.h"
#include "mathutil.h"
#include "OGgetIndex.h"           // functions for Index objects
#include "BoundaryConditionParameters.h"
#include "BCTypes.h"
#include "GridFunctionParameters.h"

// *** for new A++ ***
typedef Internal_Index IndexArg;

class MappedGrid;       // forward declaration
class MappedGridData;   // forward declaration
class SparseRepForMGF;  // forward declaration

class GenericMappedGridOperators; // forward declaration, class that defines derivtaives and BC's
class GenericDataBase;
class MappedGridOperators;

// extern BoundaryConditionParameters Overture::defaultBoundaryConditionParameters();
#include "OvertureInit.h"


//----------------------------------------------------------------------------
// This reference counted data class has to be moved here because of a
// compiler bug that causes the constructor to call the A++ new operator
//----------------------------------------------------------------------------
class doubleMappedGridFunctionRCData : public ReferenceCounting 
{
  friend class doubleMappedGridFunction;
  doubleMappedGridFunctionRCData();
  ~doubleMappedGridFunctionRCData();

  int numberOfComponents;              // number of components (ie. vector, matrix, ...)
  // IntegerArray positionOfComponent;        // positions of the components
  // IntegerArray positionOfCoordinate;       // positions of the coordinate directions

  int positionOfFaceCentering;
  int faceCentering;

  int numberOfDimensions;    // equals value found in grid, this is here for convenience

  int isACoefficientMatrix;                   // true if this grid function is a coefficient array
  int stencilType;                            // type of stencil
  // IntegerArray stencilOffset;                      // (0:2,0:numberOfStencilCoefficinets-1) offsets for Indexing
  int stencilWidth;
  int updateToMatchGridOption;

  aString *name;         // pointer to an array of names for the grid function and components
  int numberOfNames;    // length of the name array
  enum
  {
    maximumNumberOfIndicies=8,    // we support up to this many Indicies for a grid function
    numberOfIndicies=4           // A++ supports this many Indicies
  };
  Range R[maximumNumberOfIndicies+1];  // here we store the Range for each indicee,
                                       // keep one extra for convenience in some computations
  Range Ra[numberOfIndicies];          // here we store the actual Ranges for the A++ array
  Range Rc[3];                         // Range object that saves special information about the
                                       // coordinate positions for grid functions that live on boundaries.

  int positionOfComponent[maximumNumberOfIndicies];  // positions of the components
  int positionOfCoordinate[maximumNumberOfIndicies]; // positions of the coordinate directions 

  doubleMappedGridFunctionRCData & operator=( const doubleMappedGridFunctionRCData & rcData );  // deep copy

private:
  
  // These are used by list's of ReferenceCounting objects
  virtual void reference( const ReferenceCounting & mgf )
  { doubleMappedGridFunctionRCData::reference( (doubleMappedGridFunctionRCData&) mgf ); }
  virtual ReferenceCounting & operator=( const ReferenceCounting & mgf )
  { return doubleMappedGridFunctionRCData::operator=( (doubleMappedGridFunctionRCData&) mgf ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ) const
  { return ::new doubleMappedGridFunctionRCData(); }  
};

//===================================================================
//  doubleMappedGridFunction
//
//  Define a grid function to be used with a mapped grid.
//  This class is derived from an A++ array so all A++ operations
//  are defined.
//
//  This is a reference counted class so that there is no need
//  to keep a pointer to a grid function. Use the reference
//  member function to make one grid function reference another.
//
//  Usage:
//     MappedGrid cg(...);  // here is a mapped grid
//     doubleMappedGridFunction u(cg),v;
//     u=5.;
//     Index I(0,10);
//     u(I,I)=3.;
//     v.reference(u);   // v is referenced to u
//     v=7.;             // changes u as well
//     v.breakReference();  // v is no longer referenced to u
//     ...
//
//==================================================================
class doubleMappedGridFunction : public doubleDistributedArray, public doubleGenericGridFunction
{
 public:
  
  enum
  {
    maximumNumberOfIndicies=doubleMappedGridFunctionRCData::maximumNumberOfIndicies,  // we support this many Indicies
    numberOfIndicies=doubleMappedGridFunctionRCData::numberOfIndicies,             // A++ supports this many Indicies
    maximumNumberOfComponents=5,
    undefinedValue=-999,
    defaultValue=-998,
    forAll=-997
  };

  enum edgeGridFunctionValues     // these enums are used to declare grid functions defined on faces or edges
  {
    startingGridIndex   =-(INT_MAX/2),              // choose a big negative number assuming that
    biggerNegativeNumber=startingGridIndex/2,       // no grid will ever have dimensions in this range
    endingGridIndex     =biggerNegativeNumber/2,
    bigNegativeNumber   =endingGridIndex/2
  };

  enum stencilTypes               // if the grid function holds a coefficient matrix
  {                               // these are the types of stencil that it may contain
    standardStencil,              // 3x3 int 2D or 3x3x3 in 3D (if 2nd order accuracy)
    starStencil,                  // 5 point star in 2D or 7pt star in 3D (if 2nd order accuracy)
    generalStencil
  };

  enum updateReturnValue  // the return value from updateToMatchGrid is a mask of the following values
  {
    updateNoChange          = 0, // no changes made
    updateReshaped          = 1, // grid function was reshaped
    updateResized           = 2, // grid function was resized
    updateComponentsChanged = 4  // component dimensions may have changed (but grid was not resized or reshaped)
  };
  

  enum UpdateToMatchGridOption
  {
    updateSize=1,
    updateCoefficientMatrix=2
  };

  // IntegerArray positionOfComponent;  // positions of the component Index's
  // IntegerArray positionOfCoordinate; // positions of the coordinate Index's

  MappedGridData *grid;
  MappedGrid *mappedGrid;
  GenericMappedGridOperators *operators;              // pointer to operators (cannot be const)
  SparseRepForMGF *sparse;                     // pointer to info on sparse representation for coefficients
  IntegerArray isCellCentered;                     // grid function may have different values from the grid

 public:
  // IntegerArray stencilOffset;                      // (0:2,0:numberOfStencilCoefficinets-1) offsets for Indexing

  //-----------------------------------------------------------------------------
  //-----------------------Constructors------------------------------------------
  //-----------------------------------------------------------------------------

  doubleMappedGridFunction();
  doubleMappedGridFunction(MappedGrid & grid);

  doubleMappedGridFunction(const doubleMappedGridFunction & cgf, const CopyType copyType = DEEP );

  doubleMappedGridFunction(MappedGridData & gridData);

  // This constructor takes ranges, the first 3 "nullRange" values are taken to be the
  // coordinate directions in the grid function.
  doubleMappedGridFunction(MappedGrid & grid, 
                     const Range & R0,
                     const Range & R1=nullRange,
                     const Range & R2=nullRange,
                     const Range & R3=nullRange,
                     const Range & R4=nullRange,
                     const Range & R5=nullRange,
                     const Range & R6=nullRange,
                     const Range & R7=nullRange );

  doubleMappedGridFunction(MappedGridData & gridData, 
                     const Range & R0, 
                     const Range & R1=nullRange,
                     const Range & R2=nullRange,
                     const Range & R3=nullRange,
                     const Range & R4=nullRange,
                     const Range & R5=nullRange,
                     const Range & R6=nullRange,
                     const Range & R7=nullRange );

  doubleMappedGridFunction(MappedGrid & grid, 
                     const int   & i0,
                     const Range & R1=nullRange,
                     const Range & R2=nullRange,
                     const Range & R3=nullRange,
                     const Range & R4=nullRange,
                     const Range & R5=nullRange,
                     const Range & R6=nullRange,
                     const Range & R7=nullRange );

  doubleMappedGridFunction(MappedGridData & gridData, 
                     const int   & i0, 
                     const Range & R1=nullRange,
                     const Range & R2=nullRange,
                     const Range & R3=nullRange,
                     const Range & R4=nullRange,
                     const Range & R5=nullRange,
                     const Range & R6=nullRange,
                     const Range & R7=nullRange );
  
  //
  // This constructor takes a GridFunctionType
  // 
  doubleMappedGridFunction(MappedGrid & mg, 
                     const GridFunctionParameters::GridFunctionType & type, 
		     const Range & component0=nullRange,       // defaults to Range(0,0)
		     const Range & component1=nullRange,
		     const Range & component2=nullRange,
		     const Range & component3=nullRange,
		     const Range & component4=nullRange );

  doubleMappedGridFunction(const GridFunctionParameters::GridFunctionType & type, 
                     const Range & component0=nullRange,       // defaults to Range(0,0)
		     const Range & component1=nullRange,
		     const Range & component2=nullRange,
		     const Range & component3=nullRange,
		     const Range & component4=nullRange );


  ~doubleMappedGridFunction ();

  doubleMappedGridFunction & operator=( const doubleMappedGridFunction & cgf );
  doubleMappedGridFunction & operator=( const double x );
  doubleMappedGridFunction & operator=( const doubleDistributedArray & X );

  // ==========Define all of the instances of the operator()=====================

  //  ------ first define all the scalar index operators-----------
  //         these return by reference so the can be used as an lvalue
  double & operator()(const int & i0 ) const
    { return doubleDistributedArray::operator()(i0); }
  double & operator()(const int & i0, const int & i1 ) const
    { return doubleDistributedArray::operator()(i0,i1); }
  double & operator()(const int   & i0, const int   & i1, const int   i2 ) const
    { return doubleDistributedArray::operator()(i0,i1,i2); }
  double & operator()(const int & i0, const int & i1, const int & i2, const int & i3 ) const
    { return doubleDistributedArray::operator()(i0,i1,i2,i3); }
  double & operator()(const int & i0, const int & i1, const int & i2, const int & i3, const int & i4) const
    { return doubleDistributedArray::operator()(i0,i1,i2,i3+rcData->R[3].length()*(i4-rcData->R[4].getBase())); }
  double & operator()(const int & i0, const int & i1, const int & i2, const int & i3, const int & i4,
                      const int & i5) const
    { return doubleDistributedArray::operator()(i0,i1,i2,i3+rcData->R[3].length()*(
                       i4-rcData->R[4].getBase()+rcData->R[4].length()*(
                       i5-rcData->R[5].getBase())));}
  double & operator()(const int & i0, const int & i1, const int & i2, const int & i3, const int & i4,
                      const int & i5, const int & i6) const
    { return doubleDistributedArray::operator()(i0,i1,i2,i3+rcData->R[3].length()*(
                       i4-rcData->R[4].getBase()+rcData->R[4].length()*(
                      +i5-rcData->R[5].getBase()+rcData->R[5].length()*(
                       i6-rcData->R[6].getBase()))));}
  double & operator()(const int & i0, const int & i1, const int & i2, const int & i3, const int & i4,
                      const int & i5, const int & i6, const int & i7) const
    { return doubleDistributedArray::operator()(i0,i1,i2,i3+rcData->R[3].length()*(
                       i4-rcData->R[4].getBase()+rcData->R[4].length()*(
                      +i5-rcData->R[5].getBase()+rcData->R[5].length()*(
                      +i6-rcData->R[6].getBase()+rcData->R[6].length()*(
                       i7-rcData->R[7].getBase())))));}

  // -------- One Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0 ) const
    { return doubleDistributedArray::operator()(I0); }

  // -------- Two Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0, const IndexArg & I1 ) const
    { return doubleDistributedArray::operator()(I0,I1); }
					       
  // -------- Three Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0, const IndexArg & I1, const IndexArg & I2 ) const
    { return doubleDistributedArray::operator()(I0,I1,I2); }

  // -------- Four Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0, 
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3 ) const
    { return doubleDistributedArray::operator()(I0,I1,I2,I3); }

  // -------- Five Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0,             // define this for efficiency
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const int & i4 ) const
    { return (*this)(I0,I1,I2,I3+rcData->R[3].length()*(i4-rcData->R[4].getBase())); }  

  doubleDistributedArray operator()(const IndexArg & I0, 
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const IndexArg & I4 ) const;

  // -------- Six Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0,                 // define this for efficiency
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const int & i4,
			 const int & i5 ) const
    { return (*this)(I0,I1,I2,I3+rcData->R[3].length()*(i4-rcData->R[4].getBase()
                                        +rcData->R[4].length()*(i5-rcData->R[5].getBase())));  }

  doubleDistributedArray operator()(const IndexArg & I0, 
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const IndexArg & I4,
			 const IndexArg & I5 ) const;

  // -------- Seven Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0,                 // define this for efficiency
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const int & i4,
			 const int & i5,
			 const int & i6 ) const
    { return (*this)(I0,I1,I2,I3+rcData->R[3].length()*(i4-rcData->R[4].getBase()
                                        +rcData->R[4].length()*(i5-rcData->R[5].getBase()
                                        +rcData->R[5].length()*(i6-rcData->R[6].getBase()))));  }

  doubleDistributedArray operator()(const IndexArg & I0, 
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const IndexArg & I4,
			 const IndexArg & I5,
			 const IndexArg & I6 ) const;

  // -------- Eight Argument functions -------------
  doubleDistributedArray operator()(const IndexArg & I0,                 // define this for efficiency
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const int & i4,
			 const int & i5,
			 const int & i6,
			 const int & i7 ) const
    { return (*this)(I0,I1,I2,I3+rcData->R[3].length()*(i4-rcData->R[4].getBase()
                                        +rcData->R[4].length()*(i5-rcData->R[5].getBase()
                                        +rcData->R[5].length()*(i6-rcData->R[6].getBase()
                                        +rcData->R[6].length()*(i7-rcData->R[7].getBase())))));  }

  doubleDistributedArray operator()(const IndexArg & I0, 
                         const IndexArg & I1, 
			 const IndexArg & I2, 
			 const IndexArg & I3,
			 const IndexArg & I4,
			 const IndexArg & I5,
			 const IndexArg & I6,
			 const IndexArg & I7 ) const;

  // Use this function when you have too many arguments to a grid function:
  //   u(i0,i1,i2,i3,i4,i5,i6,i7) -> u(i0,i1,i2,u.arg3(i3,i4,i5,i6,i7))
  int arg3(int i3, 
           int i4, 
	   int i5=defaultValue, 
	   int i6=defaultValue, 
	   int i7=defaultValue) const;


  // The sa, "standard argument" function permutes the arguments to that you can always
  // refer to a function as u(coordinate(0),coordinate(1),corrdinate(2),component(0),component(1),...)
  double & sa(const int & i0, const int & i1, const int & i2, 
              const int & c0=0, const int & c1=0, const int & c2=0, const int & c3=0, const int & c4=0) const;

  
  // positions of the component Index's
  inline int positionOfComponent(int i) const {return rcData->positionOfComponent[i];} 
  // positions of the coordinate Index's
  inline int positionOfCoordinate(int i) const {return rcData->positionOfCoordinate[i];} 


  doubleSerialArray & getSerialArray();
  const doubleSerialArray & getSerialArray() const;

  virtual aString getClassName() const;

  int getComponentBound( int component ) const;       // get the bound of the given component
  int getComponentBase( int component ) const;        // get the base of the given component
  int getComponentDimension( int component ) const;   // get the dimension of the given component

  int getCoordinateBound( int coordinate ) const;       // get the bound of the given coordinate
  int getCoordinateBase( int coordinate ) const;        // get the base of the given coordinate
  int getCoordinateDimension( int coordinate ) const;   // get the dimension of the given coordinate

  MappedGrid* getMappedGrid(const bool abortIfNull=TRUE) const;   // return a pointer to the MappedGrid
  
  GridFunctionParameters::GridFunctionType 
    getGridFunctionType(const Index & component0=nullIndex,   // return the type of the grid function
			const Index & component1=nullIndex,
			const Index & component2=nullIndex,
			const Index & component3=nullIndex,
			const Index & component4=nullIndex ) const;     

  GridFunctionParameters::GridFunctionTypeWithComponents 
    getGridFunctionTypeWithComponents(const Index & component0=nullIndex,    // return the type of the grid function
				      const Index & component1=nullIndex,
				      const Index & component2=nullIndex,
				      const Index & component3=nullIndex,
				      const Index & component4=nullIndex ) const;     

  int getNumberOfComponents() const;                  // number of components

  bool isNull();  // TRUE is this is a null grid function (no grid)
  
  void setIsACoefficientMatrix(const bool trueOrFalse=TRUE, 
                               const int stencilSize=defaultValue, 
                               const int numberOfGhostLines=1,
                               const int numberOfComponentsForCoefficients=1,
                               const int offset=0 );

  void setIsACoefficientMatrix(SparseRepForMGF *sparseRep);   

  bool getIsACoefficientMatrix() const;

  void setStencilType(const stencilTypes stencilType, const int stencilWidth=defaultValue);

  stencilTypes getStencilType() const;

  int getStencilWidth() const;

  // use for setting equation numbers for coefficient grid functions:
  virtual int setCoefficientIndex(const int  & m, 
                      const int & na, const Index & I1a, const Index & I2a, const Index & I3a,
                      const int & nb, const Index & I1b, const Index & I2b, const Index & I3b);

  int positionOfCoefficient(const int m1, const int m2, const int m3, const int component) const;  

  int dataCopy( const doubleMappedGridFunction & mgf );  // copy the array data only

  void getRanges(Range & R0,     // return the current values for the Ranges
                 Range & R1,
		 Range & R2,
		 Range & R3,
		 Range & R4,
		 Range & R5,
		 Range & R6,
		 Range & R7 );

  // link this grid function to another
  void link(const doubleMappedGridFunction & mgf, 
	    const Range & R0,                   // these 5 Ranges correspond to the 5 possible components
	    const Range & R1=nullRange,
	    const Range & R2=nullRange,
	    const Range & R3=nullRange,
	    const Range & R4=nullRange );
  
  void link(const doubleMappedGridFunction & mgf,
            const int componentToLinkTo=0,
	    const int numberOfComponents=1 );

  inline int numberOfDimensions() const { return rcData->numberOfDimensions; } 
  inline int numberOfComponents() const { return rcData->numberOfComponents; } 
  inline int positionOfFaceCentering() const { return rcData->positionOfFaceCentering; } 

  // Clean up a grid function, release the memory
  int destroy();

  // Update edges of periodic grids
  void periodicUpdate(const Range & C0=nullRange,
                      const Range & C1=nullRange,
                      const Range & C2=nullRange,
                      const Range & C3=nullRange,
                      const Range & C4=nullRange,
                      const bool & derivativePeriodic=FALSE);

  // Update arrays to match grid, keep same number of components by default
  updateReturnValue updateToMatchGrid(MappedGridData & gridData);

  updateReturnValue updateToMatchGrid(MappedGrid & grid );

  updateReturnValue updateToMatchGrid();

  updateReturnValue updateToMatchGrid(MappedGridData & gridData, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );
  
  updateReturnValue updateToMatchGrid(MappedGrid & grid, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  updateReturnValue updateToMatchGrid(const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );
  
  updateReturnValue updateToMatchGrid(MappedGrid & grid, 
			const int & i0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  updateReturnValue updateToMatchGrid(MappedGrid & grid, 
				      const GridFunctionParameters::GridFunctionType & type, 
				      const Range & component0,
				      const Range & component1=nullRange, // defaults to Range(0,0)
				      const Range & component2=nullRange,
				      const Range & component3=nullRange,
				      const Range & component4=nullRange );
  updateReturnValue updateToMatchGrid(const GridFunctionParameters::GridFunctionType & type, 
				      const Range & component0,
				      const Range & component1=nullRange,   // defaults to Range(0,0)
				      const Range & component2=nullRange,
				      const Range & component3=nullRange,
				      const Range & component4=nullRange );
  updateReturnValue updateToMatchGrid(MappedGrid & grid, 
				      const GridFunctionParameters::GridFunctionType & type);
  updateReturnValue updateToMatchGrid(const GridFunctionParameters::GridFunctionType & type);


  // update this grid function to match another grid function
  updateReturnValue updateToMatchGridFunction(const doubleMappedGridFunction & cgf);
  updateReturnValue updateToMatchGridFunction(const doubleMappedGridFunction & cgf, 
				const Range & R0, 
				const Range & R1=nullRange,
				const Range & R2=nullRange,
				const Range & R3=nullRange,
				const Range & R4=nullRange,
				const Range & R5=nullRange,
				const Range & R6=nullRange,
				const Range & R7=nullRange );

  // set the name of the grid function or a component
  void setName(const aString & name, 
               const int & component0=defaultValue,
               const int & component1=defaultValue,
               const int & component2=defaultValue,
               const int & component3=defaultValue,
               const int & component4=defaultValue );

  // get the name of the grid function or a component
  aString getName(const int & component0=defaultValue,
                 const int & component1=defaultValue,
                 const int & component2=defaultValue,
                 const int & component3=defaultValue,
                 const int & component4=defaultValue ) const;

  // reference 
  void reference( const doubleMappedGridFunction & cgf );
  // break a reference
  void breakReference();
  
  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file
  
  // inquire cell centredness
  bool getIsCellCentered(const Index & axis=nullIndex, 
                         const Index & component0=nullIndex,
                         const Index & component1=nullIndex,
                         const Index & component2=nullIndex,
                         const Index & component3=nullIndex,
                         const Index & component4=nullIndex ) const;
  // change cell centredness:
  void setIsCellCentered(const bool trueOrFalse, 
                         const Index & axis=nullIndex,
                         const Index & component0=nullIndex,
                         const Index & component1=nullIndex,
                         const Index & component2=nullIndex,
                         const Index & component3=nullIndex,
                         const Index & component4=nullIndex );

  // inquire whether the grid function is face-centred along a given axis, for a given component
  bool getIsFaceCentered(const int   & axis=forAll, 
                         const Index & component0=nullIndex,
                         const Index & component1=nullIndex,
                         const Index & component2=nullIndex,
                         const Index & component3=nullIndex,
                         const Index & component4=nullIndex ) const;
  
  // set a component to be face centred along a given axis
  void setIsFaceCentered(const int   & axis=forAll, 
                         const Index & component0=nullIndex,
                         const Index & component1=nullIndex,
                         const Index & component2=nullIndex,
                         const Index & component3=nullIndex,
                         const Index & component4=nullIndex );

  // This next function tells you whether the grid function is face centred in a standard way
  GridFunctionParameters::faceCenteringType getFaceCentering() const;

  // Set the type of face centering, the behaviour of this function depends on whether the
  //  argument "axis" has been specified ore else the current value for getFaceCentering():
  //  (1) if "axis" is given then make all components face centred in direction=axis
  //  (2) if getFaceCentering()==all : make components face centered in all directions, the
  //      grid function should have been contructed or updated using the faceRange to specify
  //      which Index is to be used for the "directions"
  void setFaceCentering( const int & axis=defaultValue );

  // ------------------Define Derivatives-----------------------------------------------------
  MappedGridOperators* getOperators() const;    // return a pointer to the operators
  void setOperators(GenericMappedGridOperators & operators );  // supply an operator to use
  

  // specify what should be updated when calls are made to updateToMatchGrid
  void setUpdateToMatchGridOption( const UpdateToMatchGridOption & updateToMatchGridOption );

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  // fixup unused points 
  virtual int fixupUnusedPoints(const RealArray & value =Overture::nullRealArray(), 
			int numberOfGhostlines=1 );
  
//   int zeroUnusedPoints(doubleMappedGridFunction & coeff, 
//                        double value = 0,
// 		       const Index & component0=nullIndex,
// 		       const Index & component1=nullIndex,
// 		       const Index & component2=nullIndex,
// 		       const Index & component3=nullIndex,
// 		       const Index & component4=nullIndex );

  // ************************************************
  // ***** DIFFERENTIATION CLASS FUNCTIONS **********
  // ************************************************


// Macro to define a typical function call
#define FUNCTION(type) \
  virtual doubleMappedGridFunction type(const Index & I1 = nullIndex,      \
				  const Index & I2 = nullIndex,      \
				  const Index & I3 = nullIndex,      \
				  const Index & I4 = nullIndex,      \
				  const Index & I5 = nullIndex,      \
				  const Index & I6 = nullIndex,      \
				  const Index & I7 = nullIndex,      \
				  const Index & I8 = nullIndex ) const; \
                                                                     \
  virtual doubleMappedGridFunction type(const GridFunctionParameters & gfType,   \
                                  const Index & I1 = nullIndex,      \
				  const Index & I2 = nullIndex,      \
				  const Index & I3 = nullIndex,      \
				  const Index & I4 = nullIndex,      \
				  const Index & I5 = nullIndex,      \
				  const Index & I6 = nullIndex,      \
				  const Index & I7 = nullIndex,      \
				  const Index & I8 = nullIndex ) const; \
   

  // parametric derivatives in the r1,r2,r3 directions
  FUNCTION(r1)
  FUNCTION(r1Coefficients)
  FUNCTION(r2)
  FUNCTION(r2Coefficients)
  FUNCTION(r3)
  FUNCTION(r3Coefficients)
  FUNCTION(r1r1)
  FUNCTION(r1r1Coefficients)
  FUNCTION(r1r2)
  FUNCTION(r1r2Coefficients)
  FUNCTION(r1r3)
  FUNCTION(r1r3Coefficients)
  FUNCTION(r2r2)
  FUNCTION(r2r2Coefficients)
  FUNCTION(r2r3)
  FUNCTION(r2r3Coefficients)
  FUNCTION(r3r3)
  FUNCTION(r3r3Coefficients)

  // FUNCTIONs in the x,y,z directions
  FUNCTION(x)
  FUNCTION(xCoefficients)
  FUNCTION(y)
  FUNCTION(yCoefficients)
  FUNCTION(z)
  FUNCTION(zCoefficients)
  FUNCTION(xx)
  FUNCTION(xxCoefficients)
  FUNCTION(xy)
  FUNCTION(xyCoefficients)
  FUNCTION(xz)
  FUNCTION(xzCoefficients)
  FUNCTION(yy)
  FUNCTION(yyCoefficients)
  FUNCTION(yz)
  FUNCTION(yzCoefficients)
  FUNCTION(zz)
  FUNCTION(zzCoefficients)

  // other forms of derivatives

 // compute face-centered variable from cell-centered variable 
  FUNCTION(cellsToFaces)

  //compute (u.grad)u (convective derivative)
  FUNCTION(convectiveDerivative)

  // compute contravariant velocity from either cell-centered or face-centered input velocity
  FUNCTION(contravariantVelocity)

  FUNCTION(div)
  FUNCTION(divCoefficients)

  //returns cell-centered divergence given normal velocities
  FUNCTION(divNormal)

  // compute faceArea-weighted normal velocity from either cell-centered or 
  // face-centered input velocity (this is just an alias for contravariantVelocity)
  FUNCTION(normalVelocity)

  FUNCTION(grad)
  FUNCTION(gradCoefficients)

  FUNCTION(identity)
  FUNCTION(identityCoefficients)

  FUNCTION(laplacian)
  FUNCTION(laplacianCoefficients)

  FUNCTION(vorticity)

#undef FUNCTION  
  // ******* derivatives in non-standard  form  ***********

  //compute (u.grad)w (convective derivative of passive variable(s))
    virtual doubleMappedGridFunction convectiveDerivative (
                                                     const doubleMappedGridFunction &w,
						     const Index & I1 = nullIndex,
						     const Index & I2 = nullIndex,
						     const Index & I3 = nullIndex
						     ) const;
  virtual doubleMappedGridFunction convectiveDerivative (const GridFunctionParameters & gfType,
                                                   const doubleMappedGridFunction &w,
						   const Index & I1 = nullIndex,
						   const Index & I2 = nullIndex,
						   const Index & I3 = nullIndex
						   ) const;

#define SCALAR_FUNCTION(type) \
  virtual doubleMappedGridFunction type(\
                                           const doubleMappedGridFunction & s,\
					   const Index & I1 = nullIndex, \
					   const Index & I2 = nullIndex, \
					   const Index & I3 = nullIndex, \
					   const Index & I4 = nullIndex,\
					   const Index & I5 = nullIndex,\
					   const Index & I6 = nullIndex,\
					   const Index & I7 = nullIndex,\
					   const Index & I8 = nullIndex\
					   ) const;  \
  virtual doubleMappedGridFunction type(const GridFunctionParameters & gfType,\
                                           const doubleMappedGridFunction & s,\
					   const Index & I1 = nullIndex, \
					   const Index & I2 = nullIndex, \
					   const Index & I3 = nullIndex, \
					   const Index & I4 = nullIndex,\
					   const Index & I5 = nullIndex,\
					   const Index & I6 = nullIndex,\
					   const Index & I7 = nullIndex,\
					   const Index & I8 = nullIndex\
					   ) const;  


  // div(s grad(u)), s=scalar field
  SCALAR_FUNCTION(divScalarGrad)
  SCALAR_FUNCTION(divScalarGradCoefficients)

  // div((1/s).grad(u))
  SCALAR_FUNCTION(divInverseScalarGrad)
  SCALAR_FUNCTION(divInverseScalarGradCoefficients)

  // div( sv u )
  SCALAR_FUNCTION(divVectorScalar)
  SCALAR_FUNCTION(divVectorScalarCoefficients)

#undef SCALAR_FUNCTION

  virtual doubleMappedGridFunction derivativeScalarDerivative(
                                           const doubleMappedGridFunction & s,
				           const int & direction1,
				           const int & direction2,
					   const Index & I1 = nullIndex, 
					   const Index & I2 = nullIndex, 
					   const Index & I3 = nullIndex, 
					   const Index & I4 = nullIndex,
					   const Index & I5 = nullIndex,
					   const Index & I6 = nullIndex,
					   const Index & I7 = nullIndex,
					   const Index & I8 = nullIndex
					   ) const;  

  virtual doubleMappedGridFunction derivativeScalarDerivative(const GridFunctionParameters & gfType,
                                           const doubleMappedGridFunction & s,
				           const int & direction1,
				           const int & direction2,
					   const Index & I1 = nullIndex, 
					   const Index & I2 = nullIndex, 
					   const Index & I3 = nullIndex, 
					   const Index & I4 = nullIndex,
					   const Index & I5 = nullIndex,
					   const Index & I6 = nullIndex,
					   const Index & I7 = nullIndex,
					   const Index & I8 = nullIndex
					   ) const;  
  virtual doubleMappedGridFunction derivativeScalarDerivativeCoefficients(				
                                                       const doubleMappedGridFunction & s,
				                       const int & direction1,
				                       const int & direction2,
						       const Index & I1 = nullIndex,
						       const Index & I2 = nullIndex,
						       const Index & I3 = nullIndex,
						       const Index & I4 = nullIndex,
						       const Index & I5 = nullIndex,
						       const Index & I6 = nullIndex,
						       const Index & I7 = nullIndex,
						       const Index & I8 = nullIndex
						       ) const;
  virtual doubleMappedGridFunction derivativeScalarDerivativeCoefficients(
                                                       const GridFunctionParameters & gfType,	
                                                       const doubleMappedGridFunction & s,
				                       const int & direction1,
				                       const int & direction2,
						       const Index & I1 = nullIndex,
						       const Index & I2 = nullIndex,
						       const Index & I3 = nullIndex,
						       const Index & I4 = nullIndex,
						       const Index & I5 = nullIndex,
						       const Index & I6 = nullIndex,
						       const Index & I7 = nullIndex,
						       const Index & I8 = nullIndex
						       ) const;


  //returns face-centered gradients
  virtual doubleMappedGridFunction FCgrad (
                                     const int c0 = 0,
				     const int c1 = 0,
				     const int c2 = 0,
				     const int c3 = 0,
				     const int c4 = 0,
				     const Index & I1 = nullIndex,				
				     const Index & I2 = nullIndex,
				     const Index & I3 = nullIndex,
				     const Index & I4 = nullIndex,
				     const Index & I5 = nullIndex,
				     const Index & I6 = nullIndex,
				     const Index & I7 = nullIndex,
				     const Index & I8 = nullIndex
				     ) const;

  virtual doubleMappedGridFunction FCgrad (const GridFunctionParameters & gfType,
                                     const int c0 = 0,
				     const int c1 = 0,
				     const int c2 = 0,
				     const int c3 = 0,
				     const int c4 = 0,
				     const Index & I1 = nullIndex,				
				     const Index & I2 = nullIndex,
				     const Index & I3 = nullIndex,
				     const Index & I4 = nullIndex,
				     const Index & I5 = nullIndex,
				     const Index & I6 = nullIndex,
				     const Index & I7 = nullIndex,
				     const Index & I8 = nullIndex
				     ) const;



  // ********************************************************************
  // ------------- Here we define the Boundary Conditions ---------------
  // ********************************************************************

  virtual void applyBoundaryConditions(const real & time = 0.);
  // fill in coefficients for the boundary conditions
  virtual void assignBoundaryConditionCoefficients(const real & time = 0.);

  // new BC interface:
  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
                              const int & boundaryCondition = BCTypes::allBoundaries,
                              const real & forcing = 0.,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
                              const int & grid=0);
  

  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const RealArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
                              const int & grid=0);
  
  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
			      const RealArray & forcing,
                              realArray *forcinga[2][3],
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
                              const int & grid=0);
  
  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const doubleMappedGridFunction & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
                              const int & grid=0);
  
#ifdef USE_PPP
  // this version takes a distributed array "forcing"
  void applyBoundaryCondition(const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const RealDistributedArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
			      = Overture::defaultBoundaryConditionParameters(),
			      const int & grid=0 );
#endif

  // fix corners and periodic update:
  void finishBoundaryConditions(const BoundaryConditionParameters & bcParameters= Overture::defaultBoundaryConditionParameters(),
     const Range & C0=nullRange);

  void applyBoundaryConditionCoefficients(const Index & Equation,
                                          const Index & Component,
					  const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
					  const int & boundaryCondition = BCTypes::allBoundaries,
					  const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
                                          const int & grid=0);
  

  // Here are functions used to evaluate a whole set of derivatives at a time (for efficiency)
  // Make a list of derivatives to be evaluated and supply arrays to save the results in

  void getDerivatives(const Index & I1 = nullIndex, 
		      const Index & I2 = nullIndex, 
                      const Index & I3 = nullIndex, 
		      const Index & I4 = nullIndex, 
		      const Index & I5 = nullIndex, 
		      const Index & I6 = nullIndex, 
		      const Index & I7 = nullIndex, 
		      const Index & I8 = nullIndex ) const;  


  // --------member functions for boundary conditions ----------------------------------

  friend class doubleGridCollectionFunction;
  

 protected:
  void setNumberOfDimensions(const int & number);
  void setNumberOfComponents(const int & number);

  int faceCentering() const { return rcData->faceCentering; } 
  int isACoefficientMatrix() const  { return rcData->isACoefficientMatrix; } 
  int stencilType() const  { return rcData->stencilType; } 
  int stencilWidth() const  { return rcData->stencilWidth; } 

  void setPositionOfFaceCentering( const int & position );

 private:

  updateReturnValue privateUpdateToMatchGrid();                 // called by other update functions

  void dimensionName();                           // make sure name is long enough
  void updateRanges(const Range & R0,             // update the R[] array
		    const Range & R1,
		    const Range & R2,
		    const Range & R3,
		    const Range & R4,
		    const Range & R5,
		    const Range & R6,
		    const Range & R7 );
  

  // These are used by list's of ReferenceCounting objects
  virtual void reference( const ReferenceCounting & mgf )
  { doubleMappedGridFunction::reference( (doubleMappedGridFunction&) mgf ); }
  virtual ReferenceCounting & operator=( const ReferenceCounting & mgf )
  { return doubleMappedGridFunction::operator=( (doubleMappedGridFunction&) mgf ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
  { return ::new doubleMappedGridFunction(*this,ct); }  

  void initialize();
  void derivativeError() const;
  void boundaryConditionError() const;
  
  aString className;
  doubleMappedGridFunctionRCData *rcData;
     

};

// Here are a non-member utility routines
doubleMappedGridFunction &
multiply( const doubleMappedGridFunction & a, const doubleMappedGridFunction & coeff );

doubleMappedGridFunction &
multiply( const doubleDistributedArray & a, const doubleMappedGridFunction & coeff );

// These next declarations are needed to be compatible with STL
intDistributedArray operator!=(const doubleMappedGridFunction& x, const doubleMappedGridFunction& y);
intDistributedArray operator> (const doubleMappedGridFunction& x, const doubleMappedGridFunction& y);
intDistributedArray operator<=(const doubleMappedGridFunction& x, const doubleMappedGridFunction& y);
intDistributedArray operator>=(const doubleMappedGridFunction& x, const doubleMappedGridFunction& y);
intDistributedArray operator!=(const doubleMappedGridFunction& x, const double& y);
intDistributedArray operator> (const doubleMappedGridFunction& x, const double& y);
intDistributedArray operator<=(const doubleMappedGridFunction& x, const double& y);
intDistributedArray operator>=(const doubleMappedGridFunction& x, const double& y);
intDistributedArray operator!=(const double& x, const doubleMappedGridFunction& y);
intDistributedArray operator> (const double& x, const doubleMappedGridFunction& y);
intDistributedArray operator<=(const double& x, const doubleMappedGridFunction& y);
intDistributedArray operator>=(const double& x, const doubleMappedGridFunction& y);

// add these to overcome STL's definition of min and max
doubleDistributedArray min(const doubleMappedGridFunction & u, const doubleMappedGridFunction & v );
doubleDistributedArray min(const doubleMappedGridFunction & u, const double& x );
doubleDistributedArray min(const double& x, const doubleMappedGridFunction & v );

doubleDistributedArray max(const doubleMappedGridFunction & u, const doubleMappedGridFunction & v );
doubleDistributedArray max(const doubleMappedGridFunction & u, const double& x );
doubleDistributedArray max(const double& x, const doubleMappedGridFunction & v );

  
#endif // doubleMappedGridFunction.h
