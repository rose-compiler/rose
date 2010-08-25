/*  -*-Mode: c++; -*-  */
#ifndef DOUBLECOMPOSITEGRIDFUNCTION
#define DOUBLECOMPOSITEGRIDFUNCTION 

#include "doubleGridCollectionFunction.h" 
#include "ListOfDoubleCompositeGridFunction.h"

class CompositeGrid;            // forward declaration
class CompositeGridData;        // forward declaration
class Interpolant;              // forward declaration
class GridCollectionOperators;  // forward declaration
class CompositeGridOperators;  
class intCompositeGridFunction;


//===================================================================
//  doubleCompositeGridFunction
//
//  Define a grid function to be used with a composite grid.
//  This class contains a list of doubleMappedGridFunction's
//  and a CompositeGrid
//
//  This is a reference counted class so that there is no need
//  to keep a pointer to a grid function. Use the reference
//  member function to make one grid function reference another.
//
//  Usage:
//     CompositeGrid cg(...);  // here is a composite grid
//     doubleCompositeGridFunction u(cg),v;
//     u[0]=5.;                // mapped grid function for grid 0
//     Index I(0,10);
//     u[1](I,I)=3.;           // mapped grid function for grid 1
//     v.reference(u);         // v is referenced to u
//     v[1]=7.;                // changes u as well
//     v.breakReference();     // v is no longer referenced to u
//     ...
//
//==================================================================

class doubleCompositeGridFunction : public doubleGridCollectionFunction
{
 public:

  doubleCompositeGridFunction( );
  // This constructor takes ranges, the first 3 "nullRange" values are taken to be the
  // coordinate directions in the grid function.
  doubleCompositeGridFunction(CompositeGrid & grid, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  doubleCompositeGridFunction(CompositeGridData & gcData, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  doubleCompositeGridFunction(CompositeGrid & grid, 
			const int   & i0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  doubleCompositeGridFunction(CompositeGridData & gcData, 
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
  doubleCompositeGridFunction(CompositeGrid & grid, 
			const GridFunctionParameters::GridFunctionType & type, 
			const Range & Component0=nullRange,       // defaults to Range(0,0)
			const Range & Component1=nullRange,
			const Range & Component2=nullRange,
			const Range & Component3=nullRange,
			const Range & Component4=nullRange );

  doubleCompositeGridFunction(CompositeGrid & CompositeGrid);
  doubleCompositeGridFunction(CompositeGridData & CompositeGrid);

  doubleCompositeGridFunction( const doubleCompositeGridFunction & cgf, const CopyType copyType=DEEP );
  doubleCompositeGridFunction( const doubleGridCollectionFunction & cgf, const CopyType copyType=DEEP );

  virtual ~doubleCompositeGridFunction();

  // A derived class must define the = operators
  doubleCompositeGridFunction & operator= ( const doubleCompositeGridFunction & cgf );
  doubleGridCollectionFunction & operator= ( const doubleGridCollectionFunction & gcf ); 

  // Return a link to some specfied components 
  doubleCompositeGridFunction operator()(const Range & component0,
                                   const Range & component1=nullRange,
				   const Range & component2=nullRange,
				   const Range & component3=nullRange,
				   const Range & component4=nullRange );


  CompositeGridOperators* getOperators() const;    // return a pointer to the operators
  void setOperators(GenericCollectionOperators & operators );

  Interpolant* getInterpolant(const bool abortIfNull =TRUE ) const;     // return a pointer to the Interpolant

  virtual aString getClassName() const;

  CompositeGrid* getCompositeGrid(const bool abortIfNull=TRUE) const;   // return a pointer to the CompositeGrid

  void link(const doubleCompositeGridFunction & gcf,
	    const Range & R0,
	    const Range & R1=nullRange,
	    const Range & R2=nullRange,
	    const Range & R3=nullRange,
	    const Range & R4=nullRange );

  void link(const doubleGridCollectionFunction & gcf, // *New*
	    const Range & R0,
	    const Range & R1=nullRange,
	    const Range & R2=nullRange,
	    const Range & R3=nullRange,
	    const Range & R4=nullRange );

  virtual void link(const doubleCompositeGridFunction & gcf, 
            const int componentToLinkTo=0,
            const int numberOfComponents=1 );

  virtual void link(const doubleGridCollectionFunction & gcf, 
            const int componentToLinkTo=0,
            const int numberOfComponents=1 );

  void reference( const doubleCompositeGridFunction & cgf );
  void reference( const doubleGridCollectionFunction & cgf ); // *new*

  virtual void setIsACoefficientMatrix(const bool trueOrFalse=TRUE, 
                                       const int stencilSize=defaultValue, 
				       const int numberOfGhostLines=1,
				       const int numberOfComponentsForCoefficients=1,
				       const int offset=0 );

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  // define all the update functions
  updateReturnValue updateToMatchGrid();
  updateReturnValue updateToMatchGrid(CompositeGridData & gridData, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );
  
  updateReturnValue updateToMatchGrid(CompositeGrid & grid, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  // define this version to avoid overloading ambiguities
  updateReturnValue updateToMatchGrid(CompositeGrid & grid, 
			const int  & i0, 
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
  
  updateReturnValue updateToMatchGrid(CompositeGrid & grid, 
				      const GridFunctionParameters::GridFunctionType & type, 
				      const Range & component0,
				      const Range & component1=nullRange,
				      const Range & component2=nullRange,
				      const Range & component3=nullRange,
				      const Range & component4=nullRange );

  updateReturnValue updateToMatchGrid(const GridFunctionParameters::GridFunctionType & type, 
				      const Range & component0,
				      const Range & component1=nullRange,
				      const Range & component2=nullRange,
				      const Range & component3=nullRange,
				      const Range & component4=nullRange );
  updateReturnValue updateToMatchGrid(CompositeGrid & grid, 
				      const GridFunctionParameters::GridFunctionType & type);
  updateReturnValue updateToMatchGrid(const GridFunctionParameters::GridFunctionType & type);


  updateReturnValue updateToMatchGrid(CompositeGrid & grid);
  updateReturnValue updateToMatchGrid(CompositeGridData & grid);


  // update this grid function to match another grid function
  updateReturnValue updateToMatchGridFunction(const doubleCompositeGridFunction & cgf, 
				const Range & R0, 
				const Range & R1=nullRange,
				const Range & R2=nullRange,
				const Range & R3=nullRange,
				const Range & R4=nullRange,
				const Range & R5=nullRange,
				const Range & R6=nullRange,
				const Range & R7=nullRange );

  updateReturnValue updateToMatchGridFunction(const doubleCompositeGridFunction & gcf);

  // make sure the number of mappedGridFunction's is correct
  updateReturnValue updateToMatchNumberOfGrids(CompositeGrid& gc);


  //  Collections of grid functions having the same base grid.
  ListOfDoubleCompositeGridFunction baseGrid;       // overloads one base class

  //  Collections of grid functions having the same multigrid level.
  ListOfDoubleCompositeGridFunction multigridLevel; // overloads one base class

  //  Collections of grid functions having the same refinement level.
  ListOfDoubleCompositeGridFunction refinementLevel; // overloads one base class


  int interpolate(Interpolant & interpolant, 
		  const Range & C0 = nullRange,
		  const Range & C1 = nullRange,
		  const Range & C2 = nullRange );
  int interpolate(const Range & C0 = nullRange,
		  const Range & C1 = nullRange,
		  const Range & C2 = nullRange );

  friend doubleCompositeGridFunction & fabs( const doubleCompositeGridFunction & cgf );

  // use this function to  "evaulate and expression" when calling a function with an expression
  // involving grid collection functions.
  friend doubleCompositeGridFunction evaluate( doubleCompositeGridFunction & u );

#define COLLECTION_FUNCTION doubleCompositeGridFunction
#define INT_COLLECTION_FUNCTION intCompositeGridFunction
#define COLLECTION CompositeGrid
#define INTEGRAL_TYPE double
#include "derivativeDefinitions.h"
#undef COLLECTION_FUNCTION
#undef INT_COLLECTION_FUNCTION
#undef COLLECTION 
#undef INTEGRAL_TYPE

  // These are needed to avoid compiler warnings
  updateReturnValue updateToMatchGrid(GridCollectionData & gridData, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );
  
  updateReturnValue updateToMatchGrid(GridCollection & grid, 
			const Range & R0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  updateReturnValue updateToMatchGrid(GridCollection & grid, 
			const int  & i0, 
			const Range & R1=nullRange,
			const Range & R2=nullRange,
			const Range & R3=nullRange,
			const Range & R4=nullRange,
			const Range & R5=nullRange,
			const Range & R6=nullRange,
			const Range & R7=nullRange );

  
  updateReturnValue updateToMatchGrid(GridCollection & grid, 
				      const GridFunctionParameters::GridFunctionType & type, 
				      const Range & component0,
				      const Range & component1=nullRange,
				      const Range & component2=nullRange,
				      const Range & component3=nullRange,
				      const Range & component4=nullRange );

  updateReturnValue updateToMatchGrid(GridCollection & grid, 
				      const GridFunctionParameters::GridFunctionType & type);

  updateReturnValue updateToMatchGrid(GridCollection & grid);
  updateReturnValue updateToMatchGrid(GridCollectionData & grid);


  // update this grid function to match another grid function
  updateReturnValue updateToMatchGridFunction(const doubleGridCollectionFunction & cgf, 
				const Range & R0, 
				const Range & R1=nullRange,
				const Range & R2=nullRange,
				const Range & R3=nullRange,
				const Range & R4=nullRange,
				const Range & R5=nullRange,
				const Range & R6=nullRange,
				const Range & R7=nullRange );


  updateReturnValue updateToMatchGridFunction(const doubleGridCollectionFunction & gcf);

  // make sure the number of mappedGridFunction's is correct
  updateReturnValue updateToMatchNumberOfGrids(GridCollection& gc);


 protected:
  void updateReferences();
  int updateCollections(); // update refinementLevel etc.

 private:
  
  // These are used by list's of ReferenceCounting objects
  virtual void reference( const ReferenceCounting & mgf )
  { doubleCompositeGridFunction::reference( (doubleCompositeGridFunction&) mgf ); }
  virtual ReferenceCounting & operator=( const ReferenceCounting & mgf )
  { return doubleCompositeGridFunction::operator=( (doubleCompositeGridFunction&) mgf ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
  { return ::new doubleCompositeGridFunction(*this,ct); }  

  aString className;

};  



#endif 
