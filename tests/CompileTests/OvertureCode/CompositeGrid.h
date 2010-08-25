#ifndef _CompositeGrid
#define _CompositeGrid

//
// Who to blame:  Geoff Chesshire
//

#include "GridCollection.h"
#include "CompositeGridFunction.h"
#ifdef USE_STL
#include "RVector.h"
#else
#include "ListOfGenericGridCollection.h"
#include "ListOfGridCollection.h"
#include "ListOfCompositeGrid.h"
#include "ListOfIntegerArray.h"
#include "ListOfRealArray.h"
#endif // USE_STL
#include "TrivialArray.h"
#include "BoundaryAdjustment.h"
#include "CompositeGridHybridConnectivity.h"

class CompositeGrid;

// #ifndef ALLOW_NESTED_CLASSES
// class CompositeGridData_BoundaryAdjustment {
//   public:
//     Integer                computedGeometry;
//     RealMappedGridFunction boundaryAdjustment;
//     RealMappedGridFunction acrossGrid;
//     RealMappedGridFunction oppositeBoundary;
//     inline CompositeGridData_BoundaryAdjustment():
//       computedGeometry(GridCollection::NOTHING) { };
//     inline CompositeGridData_BoundaryAdjustment(
//       const CompositeGridData_BoundaryAdjustment& x):
//       computedGeometry  (x.computedGeometry),
//       boundaryAdjustment(x.boundaryAdjustment),
//       acrossGrid        (x.acrossGrid),
//       oppositeBoundary  (x.oppositeBoundary) { }
//     inline CompositeGridData_BoundaryAdjustment& operator=(
//       const CompositeGridData_BoundaryAdjustment& x) {
//         computedGeometry   = x.computedGeometry;
//         boundaryAdjustment = x.boundaryAdjustment;
//         acrossGrid         = x.acrossGrid;
//         oppositeBoundary   = x.oppositeBoundary;
//         return *this;
//     }
//     inline ~CompositeGridData_BoundaryAdjustment() { }
// };
// #endif // ALLOW_NESTED_CLASSES

//
//  Class for reference-counted data.
//
class CompositeGridData:
  public GridCollectionData {
  public:
// #ifdef ALLOW_NESTED_CLASSES
//     class BoundaryAdjustment {
//       public:
//         Integer                computedGeometry;
//         RealMappedGridFunction boundaryAdjustment;
//         RealMappedGridFunction acrossGrid;
//         RealMappedGridFunction oppositeBoundary;
//         inline BoundaryAdjustment():
//           computedGeometry(NOTHING) { };
//         inline BoundaryAdjustment(const BoundaryAdjustment& x):
//           computedGeometry  (x.computedGeometry),
//           boundaryAdjustment(x.boundaryAdjustment),
//           acrossGrid        (x.acrossGrid),
//           oppositeBoundary  (x.oppositeBoundary) { }
//         inline BoundaryAdjustment& operator=(const BoundaryAdjustment& x) {
//             computedGeometry   = x.computedGeometry;
//             boundaryAdjustment = x.boundaryAdjustment;
//             acrossGrid         = x.acrossGrid;
//             oppositeBoundary   = x.oppositeBoundary;
//             return *this;
//         }
//         inline ~BoundaryAdjustment() { }
//     };
// #else
//     typedef CompositeGridData_BoundaryAdjustment       BoundaryAdjustment;
// #endif // ALLOW_NESTED_CLASSES

    typedef TrivialArray     BoundaryAdjustmentArray;
    typedef TrivialArray BoundaryAdjustmentArray2;
    enum {
      THEinterpolationCoordinates = ENDtheGridCollectionData,         // bit 24
      THEinterpoleeGrid           = THEinterpolationCoordinates << 1, // bit 25
      THEinterpoleeLocation       = THEinterpoleeGrid           << 1, // bit 26
      THEinterpolationPoint       = THEinterpoleeLocation       << 1, // bit 27
      THEinterpolationCondition   = THEinterpolationPoint       << 1, // bit 28
      THEinverseMap               = THEinterpolationCondition   << 1, // bit 29
      ENDtheCompositeGridData     = THEinverseMap               << 1, // bit 30
      THEusualSuspects            = GridCollectionData::THEusualSuspects
                                  | THEinterpolationCoordinates
                                  | THEinterpoleeGrid
                                  | THEinterpoleeLocation
                                  | THEinterpolationPoint,
      EVERYTHING                  = GridCollectionData::EVERYTHING
                                  | THEinterpolationCoordinates
                                  | THEinterpoleeGrid
                                  | THEinterpoleeLocation
                                  | THEinterpolationPoint
                                  | THEinterpolationCondition
                                  | THEinverseMap,
      COMPUTEtheUsual             = GridCollectionData::COMPUTEtheUsual,
      ISgivenByInterpoleePoint    = INT_MIN                // (sign bit) bit 31
    };
    Integer                      numberOfCompleteMultigridLevels;
    Real                         epsilon;

    IntegerArray                 numberOfInterpolationPoints;          // total number of interp. pts
    IntegerArray                 numberOfImplicitInterpolationPoints;  // for grids with mixed implicit/explicit
    IntegerArray                 interpolationStartEndIndex;           // for interp. pts ordered by interpolee grid

    // IntegerArray                 numberOfInterpoleePoints;         // **** what is this?

    Logical                      interpolationIsAllExplicit;
    Logical                      interpolationIsAllImplicit;
    LogicalArray                 interpolationIsImplicit;

    // LogicalArray                 backupInterpolationIsImplicit;     // *** get rid of these backup arrays ***
    // IntegerArray                 backupInterpolationWidth;
    // RealArray                    backupInterpolationOverlap;
    // RealArray                    backupInterpolationConditionLimit;
    // LogicalArray                 mayBackupInterpolate;
    // IntegerArray                 interpoleeGridRange;              // ****wdh* what is this

    IntegerArray                 interpolationWidth; // *wdh* move to GridCollection
    RealArray                    interpolationOverlap;
    RealArray                    maximumHoleCuttingDistance;  // maximumHoleCuttingDistance(side,axis,grid)
//    RealArray                    interpolationConditionLimit;
    IntegerArray                 interpolationPreference;
    LogicalArray                 mayInterpolate;
    LogicalArray                 mayCutHoles;
    LogicalArray                 sharedSidesMayCutHoles;
    IntegerArray                 multigridCoarseningRatio;
    IntegerArray                 multigridProlongationWidth;
    IntegerArray                 multigridRestrictionWidth;
#ifdef USE_STL
    RVector<RealDistributedArray>       interpolationCoordinates;
    RVector<IntDistributedArray>        interpoleeGrid;
    RVector<IntDistributedArray>        variableInterpolationWidth;
//    RVector<IntegerArray>        interpoleePoint;                  // ****wdh* what is this -- remove this I think
    RVector<IntDistributedrArray>        interpoleeLocation;
    RVector<IntDistributedArray>        interpolationPoint;
//    RVector<RealArray>           interpolationCondition;          // ****wdh* is this needed? -- remove this I think
    RCVector<CompositeGrid>      multigridLevel; // (overloaded)
#else
    ListOfRealDistributedArray   interpolationCoordinates;
    ListOfIntDistributedArray           interpoleeGrid;
    ListOfIntDistributedArray           variableInterpolationWidth;
//    ListOfIntegerArray           interpoleePoint;
    ListOfIntDistributedArray           interpoleeLocation;
    ListOfIntDistributedArray           interpolationPoint;
//    ListOfRealArray              interpolationCondition;
    ListOfCompositeGrid          multigridLevel; // (overloaded)
#endif // USE_STL
//    RealCompositeGridFunction    inverseCondition;
    RealCompositeGridFunction    inverseCoordinates;
    IntegerCompositeGridFunction inverseGrid;
    BoundaryAdjustmentArray2     boundaryAdjustment;
//    IntegerArray                 sidesShare;         // keeps track of shared sides.

    //
    // hybrid structured - unstructured connectivity data (should be private ??)
    //
    CompositeGridHybridConnectivity hybridConnectivity;

    CompositeGridData(
      const Integer numberOfDimensions_ = 0,
      const Integer numberOfComponentGrids_ = 0);
    CompositeGridData(
      const CompositeGridData& x,
      const CopyType           ct = DEEP);
    virtual ~CompositeGridData();
    CompositeGridData& operator=(const CompositeGridData& x);
    void reference(const CompositeGridData& x);
    virtual void breakReference();
    virtual void consistencyCheck() const;
    virtual Integer get(
      const GenericDataBase& db,
      const aString&         name);
    virtual Integer put(
      GenericDataBase& db,
      const aString&   name) const;
    inline Integer update(
      const Integer what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual)
      { return update(*this, what, how); }
    inline Integer update(
      CompositeGridData& x,
      const Integer      what = THEusualSuspects,
      const Integer      how = COMPUTEtheUsual)
      { return update((GenericGridCollectionData&)x, what, how); }
    virtual void destroy(const Integer what = NOTHING);
    virtual Integer addRefinement(
      const IntegerArray& range,
      const IntegerArray& factor,
      const Integer&      level,
      const Integer       k = 0);
    virtual void deleteRefinement(const Integer& k);
    virtual void deleteRefinementLevels(const Integer level = 0);
    inline void referenceRefinementLevels(
      CompositeGridData& x,
      const Integer      level = INTEGER_MAX)
      { referenceRefinementLevels((GenericGridCollectionData&)x, level); }
    virtual Integer addMultigridCoarsening(
      const IntegerArray& factor,
      const Integer&      level,
      const Integer       k = 0);
    virtual void makeCompleteMultigridLevels();
    virtual void deleteMultigridCoarsening(const Integer& k);
    virtual void deleteMultigridLevels(const Integer level = 0);
    void getInterpolationStencil(
      const Integer&      k1,
      const Integer&      k2,
      const RealArray&    r,
      const IntegerArray& interpolationStencil,
      const intArray& useBackupRules);
    void getInterpolationStencil(
      const MappedGrid&   g,
      const Integer&      k10,
      const Integer&      k20,
      const RealArray&    r,
      const IntegerArray& interpolationStencil,
      const intArray& useBackupRules);

    Logical canInterpolate(
      const Integer&      k1,
      const Integer&      k2,
      const realArray&    r,
      const intArray& ok,
      const intArray& useBackupRules,
      const Logical       checkForOneSided = LogicalFalse);
//     Logical canInterpolate(
//       const MappedGrid&   g,
//       CompositeMask&      g_mask,
//       const Integer&      k10,
//       const Integer&      k20,
//       const RealArray&    r,
//       const LogicalArray& ok,
//       const LogicalArray& useBackupRules,
//       const Logical       checkForOneSided);
    void isInteriorBoundaryPoint(
      const Integer&      k1,
      const Integer&      k2,
      const IntegerArray& i1,
      const RealArray&    r2,
      const LogicalArray& ok);
//     void adjustBoundary(
//       const Integer&      k1,
//       const Integer&      k2,
//       const IntegerArray& i1,
//       const RealArray&    x);
    virtual void setNumberOfGrids(const Integer& numberOfGrids_);
    virtual void setNumberOfDimensions(const Integer& numberOfDimensions_);
    virtual void setNumberOfDimensionsAndGrids(
      const Integer& numberOfDimensions_,
      const Integer& numberOfGrids_);
    void initialize(
      const Integer& numberOfDimensions_,
      const Integer& numberOfGrids_);

  protected:
    virtual Integer update(
      GenericGridCollectionData& x,
      const Integer              what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual);
    virtual void referenceRefinementLevels(
      GenericGridCollectionData& x,
      const Integer              level = INTEGER_MAX);
    Integer updateCollection(
      const Integer&                   what,
      Integer&                         numberOfCollections,
#ifdef USE_STL
      RCVector<CompositeGrid>&         list,
      RCVector<GridCollection>&        gridCollectionList,
      RCVector<GenericGridCollection>& genericGridCollectionList,
#else
      ListOfCompositeGrid&             list,
      ListOfGridCollection&            gridCollectionList,
      ListOfGenericGridCollection&     genericGridCollectionList,
#endif // USE_STL
      IntegerArray&                    number);
//
//  The following functions are declared protected here in order to disallow
//  access to the corresponding functions in class GenericGridCollectionData.
//
    inline virtual Integer addRefinement(
      const Integer& level,
      const Integer  k = 0)
      { return GridCollectionData::addRefinement(level, k); }
    inline virtual Integer addMultigridCoarsening(
      const Integer& level,
      const Integer  k = 0)
      { return GridCollectionData::addMultigridCoarsening(level, k); }
  private:
    Integer computeGeometry(
      const Integer& what,
      const Integer& how);
//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((CompositeGridData&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((CompositeGridData&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new CompositeGridData(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};

class CompositeGrid:
  public GridCollection {
  public:
//  Public constants:

//  Constants to be ORed to form the first argument of update() and destroy():
    enum {
      THEinterpolationCoordinates =
        CompositeGridData::THEinterpolationCoordinates,
      THEinterpoleeGrid          = CompositeGridData::THEinterpoleeGrid,
      THEinterpoleeLocation      = CompositeGridData::THEinterpoleeLocation,
      THEinterpolationPoint      = CompositeGridData::THEinterpolationPoint,
      THEinterpolationCondition  = CompositeGridData::THEinterpolationCondition,
      THEinverseMap              = CompositeGridData::THEinverseMap,
      THEusualSuspects           = CompositeGridData::THEusualSuspects,
      EVERYTHING                 = CompositeGridData::EVERYTHING,
      ISgivenByInterpoleePoint   = CompositeGridData::ISgivenByInterpoleePoint
    };

//  Constants to be ORed to form the second argument of update():
    enum {
      COMPUTEtheUsual            = CompositeGridData::COMPUTEtheUsual
    };

//  Public data:
//
//  The number of interpolation points on each component grid.
//
    IntegerArray                 numberOfInterpolationPoints;
    IntegerArray                 numberOfImplicitInterpolationPoints;  // for grids with mixed implicit/explicit
    IntegerArray                 interpolationStartEndIndex;           // for interp. pts ordered by interpolee grid
//
//  The number of interpolee points listed separately on each component grid.
//
//    IntegerArray                 numberOfInterpoleePoints;
//
//  The type of interpolation (toGrid, fromGrid).
//
    LogicalArray                 interpolationIsImplicit;
//    LogicalArray                 backupInterpolationIsImplicit;
//
//  The width of the interpolation stencil (direction, toGrid, fromGrid).
//
    IntegerArray                 interpolationWidth;  // *wdh* moved to GridCollection
//    IntegerArray                 backupInterpolationWidth;
//
//  The minimum overlap for interpolation (direction, toGrid, fromGrid).
//
    RealArray                    interpolationOverlap;

     // maximumHoleCuttingDistance(side,axis,grid) : cut hole points at most this far away from the face.
    RealArray                    maximumHoleCuttingDistance; 

//    RealArray                    backupInterpolationOverlap;
//
//  The maximum condition number allowed for interpolation.
//
//    RealArray                    interpolationConditionLimit;
//    RealArray                    backupInterpolationConditionLimit;
//
//  The list of preferences for interpolation of each grid.
//
    IntegerArray                 interpolationPreference;
//
//  Indicates which grids may interpolate from which.
//
    LogicalArray                 mayInterpolate;
//    LogicalArray                 mayBackupInterpolate;
//
//  Indicates which grids may cut holes in other grids.
//
    LogicalArray                 mayCutHoles;
//
//  By default shared sides do not cut holes
//
    LogicalArray                 sharedSidesMayCutHoles;
//
//  Multigrid Coarsening ratio.
//
    IntegerArray                 multigridCoarseningRatio;
//
//  Multigrid Prolongation stencil width.
//
    IntegerArray                 multigridProlongationWidth;
//
//  Multigrid Restriction stencil width.
//
    IntegerArray                 multigridRestrictionWidth;
//
//  Range of points interpolated from each interpolee grid.
//
//    IntegerArray                 interpoleeGridRange;
#ifdef USE_STL
//
//  Coordinates of interpolation point.
//
    RVector<RealDistributedArray>           interpolationCoordinates;
//
//  Index of interpolee grid.
//
    RVector<IntDistributedArray>        interpoleeGrid;

//  width of interpolation (if not constant)    
    RVector<IntDistributedArray>        variableInterpolationWidth;
//
//  Index of the grid number of each separately-listed interpolee point.
//
//    RVector<IntegerArray>        interpoleePoint;
//
//  Location of interpolation stencil.
//
    RVector<IntDistributedArray>        interpoleeLocation;
//
//  Indices of interpolation point.
//
    RVector<IntDistributedArray>        interpolationPoint;
//
//  Interpolation condition number.
//
//    RVector<RealArray>           interpolationCondition;
//
//  Collections of grids having the same multigrid level.
//
    RCVector<CompositeGrid>      multigridLevel; // (overloaded)
#else
//
//  Coordinates of interpolation point.
//
    ListOfRealDistributedArray              interpolationCoordinates;
//
//  Index of interpolee grid.
//
    ListOfIntDistributedArray           interpoleeGrid;
//  width of interpolation (if not constant)    
    ListOfIntDistributedArray           variableInterpolationWidth;
//
//  Index of the grid number of each separately-listed interpolee point.
//
//    ListOfIntegerArray           interpoleePoint;
//
//  Location of interpolation stencil.
//
    ListOfIntDistributedArray           interpoleeLocation;
//
//  Indices of interpolation point.
//
    ListOfIntDistributedArray           interpolationPoint;
//
//  Interpolation condition number.
//
//    ListOfRealArray              interpolationCondition;
//
//  Collections of grids having the same multigrid level.
//
    ListOfCompositeGrid          multigridLevel; // (overloaded)
#endif // USE_STL

//  Data used by class Cgsh for optimization of overlap computation:
//    RealCompositeGridFunction    inverseCondition;   // Inverse quality.
    RealCompositeGridFunction    inverseCoordinates; // Inversion coordinates.
    IntegerCompositeGridFunction inverseGrid;        // Inverse component grid.
//
//  Public member functions for access to data:
//
//  The number of component grids.
//
    inline Integer& numberOfCompleteMultigridLevels()
      { return rcData->numberOfCompleteMultigridLevels; }
    inline const Integer& numberOfCompleteMultigridLevels() const
      { return rcData->numberOfCompleteMultigridLevels; }
//
//  The tolerance for accuracy of interpolation coordinates.
//
    inline       Real& epsilon()       { return rcData->epsilon; }
    inline const Real& epsilon() const { return rcData->epsilon; }
//
//  The type of interpolation between all pairs of grids.
//
    inline Logical& interpolationIsAllExplicit()
      { return rcData->interpolationIsAllExplicit; }
    inline const Logical& interpolationIsAllExplicit() const
      { return rcData->interpolationIsAllExplicit; }
    inline Logical& interpolationIsAllImplicit()
      { return rcData->interpolationIsAllImplicit; }
    inline const Logical& interpolationIsAllImplicit() const
      { return rcData->interpolationIsAllImplicit; }
//
//  Public member functions:
//
//  Default constructor.
//
//  If numberOfDimensions_==0 (e.g., by default) then create a null
//  CompositeGrid.  Otherwise create a CompositeGrid
//  with the given number of dimensions and number of component grids.
//
    CompositeGrid(
      const Integer numberOfDimensions_ = 0,
      const Integer numberOfComponentGrids_ = 0);
//
//  Copy constructor.  (Does deep copy by default.)
//
    CompositeGrid(
      const CompositeGrid& x,
      const CopyType       ct = DEEP);
//
//  Destructor.
//
    virtual ~CompositeGrid();
//
//  Assignment operator.  (Does a deep copy.)
//
    CompositeGrid& operator=(const CompositeGrid& x);
//
//  Make a reference.  (Does a shallow copy.)
//
    void reference(const CompositeGrid& x);
    void reference(CompositeGridData& x);
//
//  Break a reference.  (Replaces with a deep copy.)
//
    virtual void breakReference();
//
//  Change the grid to be all vertex-centered.
//
    virtual void changeToAllVertexCentered();
//
//  Change the grid to be all cell-centered.
//
    virtual void changeToAllCellCentered();

    // reduce interpolation width of an already computed overlapping grid.
    int changeInterpolationWidth( int width );
//
//  Check that the data structure is self-consistent.
//
    virtual void consistencyCheck() const;
//
//  "Get" and "put" database operations.
//
    virtual Integer get(
      const GenericDataBase& db,
      const aString&         name);
    virtual Integer put(
      GenericDataBase& db,
      const aString&   name) const;
//
//  Set references to reference-counted data.
//
    void updateReferences(const Integer what = EVERYTHING);
//
//  Update the grid.
//
    inline Integer update(
      const Integer what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual)
      { return update(*this, what, how); }
//
//  Update the grid, sharing the data of another grid.
//
    inline Integer update(
      CompositeGrid& x,
      const Integer  what = THEusualSuspects,
      const Integer  how = COMPUTEtheUsual)
      { return update((GenericGridCollection&)x, what, how); }
//
//  Destroy optional grid data.
//
    virtual void destroy(const Integer what = NOTHING);

// Add a new grid, built from a Mapping
    virtual int add(Mapping & map);
    
// Add a new grid
    virtual int add(MappedGrid & g);
    
    // delete a grid:
    virtual int deleteGrid(Integer k); 

    // delete a list of grids:
    virtual int deleteGrid(const IntegerArray & gridsToDelete );

    // delete a list of grids (use this when you also know the list of grids to save).
    virtual int deleteGrid(const IntegerArray & gridsToDelete, const IntegerArray & gridsToSave );

//
//  Add a refinement grid to the collection.
//
    virtual Integer addRefinement(
      const IntegerArray& range,  // The indexRange of the refinement grid.
      const IntegerArray& factor, // The refinement factor w.r.t. level-1.
      const Integer&      level,  // The refinement level number.
      const Integer       k = 0); // The index of an ancestor of the refinement.
    inline Integer addRefinement(
      const IntegerArray& range,
      const Integer&      factor,
      const Integer&      level,
      const Integer       k = 0) {
        IntegerArray factors(3); factors = factor;
        return addRefinement(range, factors, level, k);
    }
//
//  Delete all multigrid levels of refinement grid k.
//
    virtual void deleteRefinement(const Integer& k);
//
//  Delete all grids with refinement level greater than the given level.
//
    virtual void deleteRefinementLevels(const Integer level = 0);
//
//  Reference x[i] for refinementLevelNumber(i) <= level.
//  Delete all other grids.
//
    inline void referenceRefinementLevels(
      CompositeGrid& x,
      const Integer  level = INTEGER_MAX)
      { referenceRefinementLevels((GenericGridCollection&)x, level); }
//
//  Add a multigrid coarsening of grid k.
//
    virtual Integer addMultigridCoarsening(
      const IntegerArray& factor, // The coarsening factor w.r.t level-1
      const Integer&      level,  // The multigrid level number.
      const Integer       k = 0); // The index of the corresponding grid
                                  // at any finer multigrid level.
//
//  Add multigrid coarsenings of grids in order to complete the multigrid levels.
//
    virtual void makeCompleteMultigridLevels();
//
//  Delete grid k, a multigrid coarsening, and all of its multigrid coarsenings.
//
    virtual void deleteMultigridCoarsening(const Integer& k);
//
//  Delete all of the grids with multigrid level greater than the given level.
//
    virtual void deleteMultigridLevels(const Integer level = 0);
//
//  Set the number of grids.
//
    virtual void setNumberOfGrids(const Integer& numberOfGrids_);
//
//  Set the number of dimensions.
//
    virtual void setNumberOfDimensions(const Integer& numberOfDimensions_);
//
//  Set the number of dimensions and grids.
//
    virtual void setNumberOfDimensionsAndGrids(
      const Integer& numberOfDimensions_,
      const Integer& numberOfGrids_);

    // return size of this object  
    virtual real sizeOf(FILE *file = NULL ) const;

//
//  Initialize the CompositeGrid with the given number of dimensions and grids.
//  These grids have their gridNumbers, baseGridNumbers and componentGridNumbers
//  set to [0, ..., numberOfGrids_-1], and their refinementLevelNumbers and
//  multigridLevelNumbers set to zero.
//
    virtual void initialize(
      const Integer& numberOfDimensions_,
      const Integer& numberOfGrids_);
//
//  Return the bounds on the cube of points used for interpolation.
//
//    Input:
//      k1              Index of the grid containing the interpolation points.
//      k2              Index of the grid containing the interpolee points.
//      r(0:2,p1:p2)    Interpolation coordinates of the interpolation points.
//                      The dimensions of this array determine p1 and p2.
//      useBackupRules(p1:p2)
//                      Must be LogicalFalse unless backup interpolation
//                      rules should be used to determine the stencil.
//    Output:
//      interpolationStencil(0:1,0:2,p1:p2)
//                      lower and upper index bounds of the stencils.
//
    inline void getInterpolationStencil(
      const Integer&      k1,
      const Integer&      k2,
      const RealArray&    r,
      const IntegerArray& interpolationStencil,
      const intArray& useBackupRules) {
        rcData->getInterpolationStencil(k1, k2, r, interpolationStencil,
          useBackupRules);
    }
    inline void getInterpolationStencil(
      const MappedGrid&   g, // The unrefined grid corresponding to grid[k20].
      const Integer&      k1,
      const Integer&      k2,
      const RealArray&    r,
      const IntegerArray& interpolationStencil,
      const intArray& useBackupRules) {
        rcData->getInterpolationStencil(g, k1, k2, r, interpolationStencil,
          useBackupRules);
    }
//
//  Determine whether points on grid k1 at r in the coordinates of grids k2
//  can be interpolated from grids k2.
//
    inline Logical canInterpolate(
      const Integer&      k1,
      const Integer&      k2,
      const realArray&    r,              // (0:numberOfDimensions-1,p1:p2)
      const intArray& ok,             // (p1:p2)
      const intArray& useBackupRules, // (p1:p2)
      const Logical       checkForOneSided = LogicalFalse) {
        return rcData->canInterpolate(k1, k2, r, ok,
          useBackupRules, checkForOneSided);
    }
//     inline Logical canInterpolate(
//       const MappedGrid&   g, // The unrefined grid corresponding to grid[k20].
//       CompositeMask&      g_mask, // Masks on k2 (possibly) and its siblings.
//       const Integer&      k1,
//       const Integer&      k2,
//       const RealArray&    r,              // (0:numberOfDimensions-1,p1:p2)
//       const LogicalArray& ok,             // (p1:p2)
//       const LogicalArray& useBackupRules, // (p1:p2)
//       const Logical       checkForOneSided = LogicalFalse) {
//         return rcData->canInterpolate(g, g_mask, k1, k2, r, ok,
//           useBackupRules, checkForOneSided);
//     }
//
//  Check if these boundary discretization points of this grid
//  lie at least epsilon inside the parameter space of grid g2.
//
//     void isInteriorBoundaryPoint(
//       const Integer&      k1,
//       const Integer&      k2,
//       const IntegerArray& i1,
//       const RealArray&    r2,
//       const LogicalArray& ok)
//       { rcData->isInteriorBoundaryPoint(k1, k2, i1, r2, ok); }
//
//  Adjust the position of interpolation points to take
//  into account mismatch between shared boundaries.
//
//     inline void adjustBoundary(
//       const Integer&      k1,
//       const Integer&      k2,
//       const IntegerArray& i1,
//       const RealArray&    x)
//       { rcData->adjustBoundary(k1, k2, i1, x); }

    // Here is the master grid.
    CompositeGrid & masterGridCollection();
//
//  Pointer to reference-counted data.
//
    typedef CompositeGridData RCData;
    RCData* rcData; Logical isCounted;
    inline       CompositeGridData* operator->()       { return  rcData; }
    inline const CompositeGridData* operator->() const { return  rcData; }
    inline       CompositeGridData& operator*()        { return *rcData; }
    inline const CompositeGridData& operator*()  const { return *rcData; }

    //
    // set the hybridConnectivity for this CompositeGrid
    //
    void setHybridConnectivity(const int grid,
                               intArray * gridIndex2UVertex_,
			       intArray & uVertex2GridIndex_,
			       intArray * gridVertex2UVertex_, // could be build from gridIndex2UVertex ?
			       intArray & boundaryFaceMapping_);

    //
    // get the hybrid connectivity for this composite grid
    //
    const CompositeGridHybridConnectivity & getHybridConnectivity() const;

  private:
    virtual Integer update(
      GenericGridCollection& x,
      const Integer          what = THEusualSuspects,
      const Integer          how = COMPUTEtheUsual);
    virtual void referenceRefinementLevels(
      GenericGridCollection& x,
      const Integer          level = INTEGER_MAX);
  protected:

    CompositeGrid *master;   // overloaded

//
//  The following functions are declared protected here in order to disallow
//  access to the corresponding functions in class GenericGridCollectionData.
//
    inline virtual Integer addRefinement(
      const Integer& level,
      const Integer  k = 0)
      { return GridCollection::addRefinement(level, k); }
    inline virtual Integer addMultigridCoarsening(
      const Integer& level,
      const Integer  k = 0)
      { return GridCollection::addMultigridCoarsening(level, k); }
    inline virtual void initialize(
      const Integer& numberOfGrids_)
      { GridCollection::initialize(numberOfGrids_); }
//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((CompositeGrid&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((CompositeGrid&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new CompositeGrid(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};
//
// Stream output operator.
//
ostream& operator<<(ostream& s, const CompositeGrid& g);

#endif // _CompositeGrid
