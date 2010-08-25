#ifndef INTERPOLANT_H
#define INTERPOLANT_H "Interpolant.h"

//============================================================================
//  Interpolant Class
//             Interpolate and Swap Periodic Edges
//             -----------------------------------
//    Supply implicit and explicit interpolation operators for
//    realCompositeGridFunctions. By default the interpolation
//    operator will swap periodic edges.
//
//  Notes:
//   o A CompositeGrid holds a pointer to an Interpolant. This pointer is set by an
//     Interpolant when the constructor Interpolant(CompositeGrid &) or the member function
//     updateToMatchGrid( CompositeGrid & ) is called. Grid functions look for
//     the pointer in the CompositeGrid when their interpolate function is called.
//   o The first time that an Interpolant is created the pointer in the CompositeGrid
//     will be set. Subsequently, when new Interpolant's are created they will
//     simply reference the existing one. Thus the CompositeGrid will point to an 
//     Interpolant (and grid functions will know how to interpolate) as long as at 
//     least one Interpolant remains in scope.
//   o The explicit interpolation is defined in Interpolant.C
//   o The implicit interpolation is peformed by Oges - the overlapping grid
//     equation solver.
//
// Usage:
//    CompositeGrid cg(...);
//    realCompositeGridFunction u(cg);   
//    ...
//    Interpolant interpolant(cg);
//    ...
//    interpolant.interpolate( u ) ;
//    ..or..  
//    u.interpolate();   // implicitly knows about the interpolant through the CompositeGrid
//
//============================================================================

#include "CompositeGrid.h"
#include "CompositeGridFunction.h"
// include "MultigridCompositeGridFunction.h"
#include "ListOfRealArray.h"
#include "ListOfDoubleArray.h"

class Oges;                     // forward declaration
class InterpolateRefinements;   // forward declaration


class Interpolant : public ReferenceCounting
{
 public:

  Interpolant();
  Interpolant(CompositeGrid & cg );
  Interpolant(GridCollection & gc );
  Interpolant(const Interpolant & interpolant, const CopyType copyType=DEEP );
  virtual ~Interpolant();
  Interpolant & operator= ( const Interpolant & interpolant );
  void reference( const Interpolant & interpolant );
  virtual void breakReference();
  
  void updateToMatchGrid(CompositeGrid & cg, int refinementLevel=0 );  

  int interpolate( realCompositeGridFunction & u, 
                   const Range & C0 = nullRange,      // optionally specify components to interpolate
		   const Range & C1 = nullRange,  
		   const Range & C2 = nullRange );

  int interpolate( realGridCollectionFunction & u, 
                   const Range & C0 = nullRange,
		   const Range & C1 = nullRange,
		   const Range & C2 = nullRange );

  int interpolate( int gridToInterpolate,             // only interpolate this grid.
                   realCompositeGridFunction & u, 
		   const Range & C0 = nullRange,      // optionally specify components to interpolate
		   const Range & C1 = nullRange,  
		   const Range & C2 = nullRange );

  int interpolate( realCompositeGridFunction & u, 
                   IntegerArray & gridsToInterpolate,  // specify which grids to interpolate
		   const Range & C0 = nullRange,      // optionally specify components to interpolate
		   const Range & C1 = nullRange,  
		   const Range & C2 = nullRange );

  int interpolate( realCompositeGridFunction & u,
                   IntegerArray & gridsToInterpolate,      // specify which grids to interpolate
                   IntegerArray & gridsToInterpolateFrom,  // specify which grids to interpolate from
		   const Range & C0 = nullRange,      // optionally specify components to interpolate
		   const Range & C1 = nullRange,  
		   const Range & C2 = nullRange );

  int interpolate( realArray & ui,                    // save results here
                   int gridToInterpolate,             // only interpolate values on this grid that
                   int interpoleeGrid,                // interpolate from this grid.
                   realCompositeGridFunction & u, 
		   const Range & C0 = nullRange,      // optionally specify components to interpolate
		   const Range & C1 = nullRange,  
		   const Range & C2 = nullRange );

  bool interpolationIsExplicit() const;
  bool interpolationIsImplicit() const;
  
  enum InterpolationMethodEnum
  {
    standard,
    optimized,
    optimizedC,  // use C style loops
    numberOfInterpolationMethods  // counts number in this list
  };
  
  int setInterpolationMethod(InterpolationMethodEnum method);

  enum ImplicitInterpolationMethodEnum
  {
    directSolve,
    sparseSolve,
    iterateToInterpolate
  };
  
  int setImplicitInterpolationMethod(ImplicitInterpolationMethodEnum method);
  ImplicitInterpolationMethodEnum getImplicitInterpolationMethod() const;
  int setImplicitInterpolationTolerance(real tol);
  
  enum InterpolationOptionEnum
  {
    interpolateOverlappingRefinementPoints,
    interpolateAllRefinementBoundaries,
    interpolateHiddenRefinementPoints
  };
    
  int setInterpolationOption(InterpolationOptionEnum option, bool trueOrFalse );
  int getInterpolationOption(InterpolationOptionEnum option);

  // supply an AMR interpolation object:
  int setInterpolateRefinements( InterpolateRefinements & interpolateRefinements );

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  
  int interpolateRefinementLevel( const int refinementLevel,
				  realCompositeGridFunction & u, 
				  const Range & C0 = nullRange,      // optionally specify components to interpolate
				  const Range & C1 = nullRange,  
				  const Range & C2 = nullRange );
  
  BoundaryConditionParameters bcParams; // used to apply BC's for AMR grids.

  int debug;
  
 protected:

  int getComponentRanges(const Range & C0, const Range & C1, const Range & C2, Range C[4],
                         realCompositeGridFunction & u );

 private:

  InterpolationMethodEnum interpolationMethod;
  ImplicitInterpolationMethodEnum implicitInterpolationMethod;
  real tolerance;  // tolerance for implicit interpolation by iteration.
  bool explicitInterpolation;
  bool interpolationIsInitialized;

  bool interpolateRefinementBoundaries;  // if true, interpolate all refinement boundaries
  bool interpolateHidden;                // if true, interpolate hidden coarse grid points from higher level refinemnts
  bool interpolateOverlappingRefinementBoundaries;
  bool interpRefinementsWasNewed;
  
  ListOfRealDistributedArray coeff;      // coeff's for explicit interpolation
  IntegerArray width;
  CompositeGrid cg;
  
  realCompositeGridFunction v;  // holds components, no need to reference count

  int updateForAdaptiveGrid;
  InterpolateRefinements *interpRefinements;
  
  void initialize();
  int initializeInterpolation();
  int initializeExplicitInterpolation();

  int internalInterpolate( realCompositeGridFunction & u, 
                           const Range C[],
			   IntegerArray & gridToInterpolate = Overture::nullIntArray(),
                           IntegerArray & gridsToInterpolateFrom = Overture::nullIntArray());

  int explicitInterpolate(realCompositeGridFunction & u, 
			  const Range C[],
                          IntegerArray & gridsToInterpolate = Overture::nullIntArray(),
			  IntegerArray & gridsToInterpolateFrom = Overture::nullIntArray() ) const;

  int implicitInterpolateByIteration(realCompositeGridFunction & u,
                                     const Range C[],
                                     IntegerArray & gridToInterpolate = Overture::nullIntArray(),
				     IntegerArray & gridsToInterpolateFrom = Overture::nullIntArray() ) const;

  virtual ReferenceCounting& operator=( const ReferenceCounting & x)
    { return operator=( *(Interpolant*) & x ); }
  virtual void reference( const ReferenceCounting & x)
    { reference( (Interpolant &) x ); }
  virtual ReferenceCounting* virtualConstructor(const CopyType ct  = DEEP) const
    { return ::new Interpolant(*this, ct); }


  // holds data to be reference counted (not A++ since they are ref. counted)
  class RCData : public ReferenceCounting
  {
   public:
    friend class GridCollectionFunction;
    RCData(); 
    ~RCData();
    RCData & operator=( const RCData & rcdata );

    Oges *implicitInterpolant;
  
   private:

    // These are used by list's of ReferenceCounting objects
    virtual void reference( const ReferenceCounting & mgf )
    { RCData::reference( (RCData&) mgf ); }
    virtual ReferenceCounting & operator=( const ReferenceCounting & mgf )
    { return RCData::operator=( (RCData&) mgf ); }
    virtual ReferenceCounting* virtualConstructor( const CopyType = DEEP ) const
    { return ::new RCData(); }  
  };
   
  RCData *rcData;
};



#endif
