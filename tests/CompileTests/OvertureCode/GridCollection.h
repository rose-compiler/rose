#ifndef _GridCollection
#define _GridCollection

//
// Who to blame:  Geoff Chesshire
//

#include "GenericGridCollection.h"
#include "GridCollectionFunction.h"
#include "MappedGrid.h"
#include "ReparameterizationTransform.h"
#ifndef USE_STL
#include "ListOfMappedGrid.h"
#include "ListOfGridCollection.h"
#endif // USE_STL
#include "GCMath.h"

class AMR_RefinementLevelInfo;
class Interpolant;

class GridCollection;

//
//  Class for reference-counted data.
//
class GridCollectionData:
  public GenericGridCollectionData {
  public:
    enum {
      THEmask                    = MappedGridData::THEmask,
      THEinverseVertexDerivative = MappedGridData::THEinverseVertexDerivative,
      THEinverseCenterDerivative   =
        MappedGridData::THEinverseCenterDerivative,
      THEvertex                  = MappedGridData::THEvertex,
      THEcenter                  = MappedGridData::THEcenter,
      THEcorner                  = MappedGridData::THEcorner,
      THEvertexDerivative        = MappedGridData::THEvertexDerivative,
      THEcenterDerivative        = MappedGridData::THEcenterDerivative,
      THEvertexJacobian          = MappedGridData::THEvertexJacobian,
      THEcenterJacobian          = MappedGridData::THEcenterJacobian,
      THEcellVolume              = MappedGridData::THEcellVolume,
      THEcenterNormal            = MappedGridData::THEcenterNormal,
      THEcenterArea              = MappedGridData::THEcenterArea,
      THEfaceNormal              = MappedGridData::THEfaceNormal,
      THEfaceArea                = MappedGridData::THEfaceArea,
      THEvertexBoundaryNormal    = MappedGridData::THEvertexBoundaryNormal,
      THEcenterBoundaryNormal    = MappedGridData::THEcenterBoundaryNormal,
      THEcenterBoundaryTangent   = MappedGridData::THEcenterBoundaryTangent,
      THEminMaxEdgeLength        = MappedGridData::THEminMaxEdgeLength,
      THEboundingBox             = MappedGridData::THEboundingBox,
      ENDtheGridCollectionData   = ENDtheGenericGridCollectionData, // bit 24
      THEusualSuspects           = GenericGridCollectionData::THEusualSuspects
                                 | MappedGridData::THEusualSuspects,
      EVERYTHING                 = GenericGridCollectionData::EVERYTHING
                                 | MappedGridData::EVERYTHING,
      USEdifferenceApproximation = MappedGridData::USEdifferenceApproximation,
      COMPUTEgeometry            = MappedGridData::COMPUTEgeometry,
      COMPUTEgeometryAsNeeded    = MappedGridData::COMPUTEgeometryAsNeeded,
      COMPUTEtheUsual            = GenericGridCollectionData::COMPUTEtheUsual
                                 | MappedGridData::COMPUTEtheUsual,
      ISinterpolationPoint       = MappedGridData::ISinterpolationPoint,
      ISdiscretizationPoint      = MappedGridData::ISdiscretizationPoint,
      ISghostPoint               = MappedGridData::ISghostPoint,
      ISinteriorBoundaryPoint    = MappedGridData::ISinteriorBoundaryPoint,
      USESbackupRules            = MappedGridData::USESbackupRules,
      IShiddenByRefinement       = MappedGridData::IShiddenByRefinement,
      ISreservedBit2             = MappedGridData::ISreservedBit2,
      ISreservedBit1             = MappedGridData::ISreservedBit1,
      ISreservedBit0             = MappedGridData::ISreservedBit0,
      GRIDnumberBits             = MappedGridData::GRIDnumberBits,
      ISusedPoint                = MappedGridData::ISusedPoint
    };
    Integer                  numberOfDimensions;
    RealArray                boundingBox;
    IntegerArray             refinementFactor;
    IntegerArray             multigridCoarseningFactor;
#ifdef USE_STL
    RCVector<MappedGrid>     grid;            // (overloaded)
    RCVector<GridCollection> baseGrid;        // (overloaded)
    RCVector<GridCollection> refinementLevel; // (overloaded)
    RCVector<GridCollection> componentGrid;   // (overloaded)
    RCVector<GridCollection> multigridLevel;  // (overloaded)
#else
    ListOfMappedGrid         grid;            // (overloaded)
    ListOfGridCollection     baseGrid;        // (overloaded)
    ListOfGridCollection     refinementLevel; // (overloaded)
    ListOfGridCollection     componentGrid;   // (overloaded)
    ListOfGridCollection     multigridLevel;  // (overloaded)
#endif // USE_STL
    Interpolant*             interpolant;

// ********
//     // here we move the stuff that used to be in the CompositeGrid Class
//     IntegerArray *numberOfInterpolationPoints;
//     IntegerArray *interpolationWidth;    

    // arrays needed for hybrid grids:
//     IntegerArray *numberOfHybridInterfaceNodes;  // numberOfHybridNodes(grid)
//     IntegerArray *hybridInterfaceGridIndex;      // hybridInterfaceGridIndex(numberOfHybridBoundaryNodes,4)
//     IntegerArray *hybridGlobalID;                // hybridGlobalID[grid](numberOfHybridNodes(grid))  
// ********

    GridCollectionData(
      const Integer numberOfDimensions_ = 0,
      const Integer numberOfGrids_ = 0);
    GridCollectionData(
      const GridCollectionData& x,
      const CopyType            ct = DEEP);
    virtual ~GridCollectionData();
    GridCollectionData& operator=(const GridCollectionData& x);
    inline MappedGrid& operator[](const Integer& i) { return grid[i]; }
    inline const MappedGrid& operator[](const Integer& i) const
      { return grid[i]; }
    void reference(const GridCollectionData& x);
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
      GridCollectionData& x,
      const Integer       what = THEusualSuspects,
      const Integer       how = COMPUTEtheUsual)
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
      GridCollectionData& x,
      const Integer       level = INTEGER_MAX)
      { referenceRefinementLevels((GenericGridCollectionData&)x, level); }
    virtual Integer addMultigridCoarsening(
      const IntegerArray& factor,
      const Integer&      level,
      const Integer       k = 0);
    virtual void deleteMultigridCoarsening(const Integer& k);
    virtual void deleteMultigridLevels(const Integer level = 0);
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
      const Integer              how = COMPUTEtheUsual);
    virtual void referenceRefinementLevels(
      GenericGridCollectionData& x,
      const Integer              level = INTEGER_MAX);
    Integer updateCollection(
      const Integer&                   what,
      Integer&                         numberOfCollections,
#ifdef USE_STL
      RCVector<GridCollection>&        list,
      RCVector<GenericGridCollection>& genericList,
#else
      ListOfGridCollection&            list,
      ListOfGenericGridCollection&     genericList,
#endif // USE_STL
      IntegerArray&                collection);
//
//  The following functions are declared protected here in order to disallow
//  access to the corresponding functions in class GenericGridCollectionData.
//
    inline virtual Integer addRefinement(
      const Integer& level,
      const Integer  k = 0) {
        if (&level || &k); // Avoid compiler warnings.
        cerr << "virtual void GridCollectionData::addRefinement(const Integer& level, const Integer k) must not be called!"
             << "It must have been called illegally through the base class GenericGridCollectionData."
             << endl;
        abort();
        return -1;
    }
    inline virtual Integer addMultigridCoarsening(
      const Integer& level,
      const Integer  k = 0) {
        if (&level || &k); // Avoid compiler warnings.
        cerr << "virtual void GridCollectionData::addMultigridCoarsening(const Integer& level, const Integer k) must not be called!"
             << "It must have been called illegally through the base class GenericGridCollectionData."
             << endl;
        abort();
        return -1;
    }
//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((GridCollectionData&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((GridCollectionData&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new GridCollectionData(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};

class GridCollection:
  public GenericGridCollection {
  public:
//  Public constants:

//  Constants to be ORed to form the first argument of update() and destroy():
    enum {
      THEmask                    = GridCollectionData::THEmask,
      THEinverseVertexDerivative =
        GridCollectionData::THEinverseVertexDerivative,
      THEinverseCenterDerivative =
        GridCollectionData::THEinverseCenterDerivative,
      THEvertex                  = GridCollectionData::THEvertex,
      THEcenter                  = GridCollectionData::THEcenter,
      THEcorner                  = GridCollectionData::THEcorner,
      THEvertexDerivative        = GridCollectionData::THEvertexDerivative,
      THEcenterDerivative        = GridCollectionData::THEcenterDerivative,
      THEvertexJacobian          = GridCollectionData::THEvertexJacobian,
      THEcenterJacobian          = GridCollectionData::THEcenterJacobian,
      THEcellVolume              = GridCollectionData::THEcellVolume,
      THEcenterNormal            = GridCollectionData::THEcenterNormal,
      THEcenterArea              = GridCollectionData::THEcenterArea,
      THEfaceNormal              = GridCollectionData::THEfaceNormal,
      THEfaceArea                = GridCollectionData::THEfaceArea,
      THEvertexBoundaryNormal    = GridCollectionData::THEvertexBoundaryNormal,
      THEcenterBoundaryNormal    = GridCollectionData::THEcenterBoundaryNormal,
      THEcenterBoundaryTangent   = GridCollectionData::THEcenterBoundaryTangent,
      THEminMaxEdgeLength        = GridCollectionData::THEminMaxEdgeLength,
      THEboundingBox             = GridCollectionData::THEboundingBox,
      THEusualSuspects           = GridCollectionData::THEusualSuspects,
      EVERYTHING                 = GridCollectionData::EVERYTHING
    };

//  Constants to be ORed to form the second argument of update():
    enum {
      USEdifferenceApproximation =
        GridCollectionData::USEdifferenceApproximation,
      COMPUTEgeometry            = GridCollectionData::COMPUTEgeometry,
      COMPUTEgeometryAsNeeded    = GridCollectionData::COMPUTEgeometryAsNeeded,
      COMPUTEtheUsual            = GridCollectionData::COMPUTEtheUsual
    };

//  Mask bits to access data in mask.
    enum {
      ISinterpolationPoint       = GridCollectionData::ISinterpolationPoint,
      ISdiscretizationPoint      = GridCollectionData::ISdiscretizationPoint,
      ISghostPoint               = GridCollectionData::ISghostPoint,
      ISinteriorBoundaryPoint    = GridCollectionData::ISinteriorBoundaryPoint,
      USESbackupRules            = GridCollectionData::USESbackupRules,
      IShiddenByRefinement       = GridCollectionData::IShiddenByRefinement,
      ISreservedBit2             = GridCollectionData::ISreservedBit2,
      ISreservedBit1             = GridCollectionData::ISreservedBit1,
      ISreservedBit0             = GridCollectionData::ISreservedBit0,
      GRIDnumberBits             = GridCollectionData::GRIDnumberBits,
      ISusedPoint                = GridCollectionData::ISusedPoint
    };

//  Public data:
//
    const RealArray                boundingBox;
    const IntegerArray             refinementFactor;
    const IntegerArray             multigridCoarseningFactor;

    IntegerArray *nodeNumbering; // for Kyle.

#ifdef USE_STL
//  Lists of component grids.
    RCVector<MappedGrid>     grid;            // (overloaded)

//  Collections of grids having the same base grid.
    RCVector<GridCollection> baseGrid;        // (overloaded)

//  Collections of grids having the same refinement level.
    RCVector<GridCollection> refinementLevel; // (overloaded)

//  Collections of multigrid coarsenings of the same component grid
    RCVector<GridCollection> componentGrid;   // (overloaded)

//  Collections of grids having the same multigrid level.
    RCVector<GridCollection> multigridLevel;  // (overloaded)
#else
//  Lists of component grids.
    ListOfMappedGrid         grid;            // (overloaded)

//  Collections of grids having the same base grid.
    ListOfGridCollection     baseGrid;        // (overloaded)

//  Collections of grids having the same refinement level.
    ListOfGridCollection     refinementLevel; // (overloaded)

//  Collections of multigrid coarsenings of the same component grid
    ListOfGridCollection     componentGrid;   // (overloaded)

//  Collections of grids having the same multigrid level.
    ListOfGridCollection     multigridLevel;  // (overloaded)
#endif // USE_STL

    AMR_RefinementLevelInfo* refinementLevelInfo;
//
//  Public member functions for access to data:
//
//  The number of dimensions of the MappedGrids in grid.
//
    inline Integer& numberOfDimensions()
      { return rcData->numberOfDimensions; }
    inline const Integer& numberOfDimensions() const
      { return rcData->numberOfDimensions; }
//
//  Public member functions:
//
//  Default constructor.
//
//  Create a GridCollection with the given number of dimensions
//  and number of component grids.
//
    GridCollection(
       const Integer numberOfDimensions_ = 0,
       const Integer numberOfGrids_ = 0);
//
//  Copy constructor.  (Does deep copy by default.)
//
    GridCollection(
      const GridCollection& x,
      const CopyType        ct = DEEP);
//
//  Destructor.
//
    virtual ~GridCollection();
//
//  Assignment operator.  (Does a deep copy.)
//
    GridCollection& operator=(const GridCollection& x);
//
//  Get a reference to a component grid using C or Fortran indexing.
//
    inline MappedGrid& operator[](const Integer& i) { return grid[i]; }
    inline const MappedGrid& operator[](const Integer& i) const
      { return grid[i]; }
//
//  Make a reference.  (Does a shallow copy.)
//
    void reference(const GridCollection& x);
    void reference(GridCollectionData& x);
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
//
//  Check that the data structure is self-consistent.
//
    virtual void consistencyCheck() const;

    // return a pointer to the Interpolant.
    Interpolant* getInterpolant() const { return rcData->interpolant; }
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
      GridCollection& x,
      const Integer   what = THEusualSuspects,
      const Integer   how = COMPUTEtheUsual)
      { return update((GenericGridCollection&)x, what, how); }
//
//  Destroy optional grid data.
//
    virtual void destroy(const Integer what = NOTHING);

// Add a new grid. 
    virtual int add(MappedGrid & g);
// Add a new grid, built from a Mapping
    virtual int add(Mapping & map);

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
      GridCollection& x,
      const Integer   level = INTEGER_MAX)
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

    int setMaskAtRefinements();
    
    // numberOfHybridInterfaceNodes(grid)
//    inline IntegerArray& numberOfHybridInterfaceNodes(int grid){ return *rcData->numberOfHybridInterfaceNodes; }
//    inline const IntegerArray& numberOfHybridInterfaceNodes(int grid) const{ return *rcData->numberOfHybridInterfaceNodes; }
//
//    // hybridInterfaceGridIndex(numberOfHybridBoundaryNodes,4)
//    inline IntegerArray& hybridInterfaceGridIndex(){ return *rcData->hybridInterfaceGridIndex; }
//    inline const IntegerArray& hybridInterfaceGridIndex() const{ return *rcData->hybridInterfaceGridIndex; }
//
//    inline IntegerArray& hybridGlobalID(){ return *rcData->hybridGlobalID; }
//    inline const IntegerArray& hybridGlobalID() const{ return *rcData->hybridGlobalID; }


//
//  Initialize the GridCollection with the given number of dimensions and grids.
//  These grids have their gridNumbers, baseGridNumbers and componentGridNumbers
//  set to [0, ..., numberOfGrids_-1], and their refinementLevelNumbers and
//  multigridLevelNumbers set to zero.
//
    virtual void initialize(
      const Integer& numberOfDimensions_,
      const Integer& numberOfGrids_);

    // Here is the master grid.
    GridCollection & masterGridCollection();
//
//  Pointer to reference-counted data.
//
    typedef GridCollectionData RCData;
    RCData* rcData; Logical isCounted;
    inline       GridCollectionData* operator->()       { return  rcData; }
    inline const GridCollectionData* operator->() const { return  rcData; }
    inline       GridCollectionData& operator*()        { return *rcData; }
    inline const GridCollectionData& operator*()  const { return *rcData; }

  private:
    virtual Integer update(
      GenericGridCollection& x,
      const Integer          what = THEusualSuspects,
      const Integer          how = COMPUTEtheUsual);
    virtual void referenceRefinementLevels(
      GenericGridCollection& x,
      const Integer          level = INTEGER_MAX);

  protected:
//
//  The following functions are declared protected here in order to disallow
//  access to the corresponding functions in class GenericGridCollection.
//
    inline virtual Integer addRefinement(
      const Integer& level,
      const Integer  k = 0) {
        if (&level || &k); // Avoid compiler warnings.
        cerr << "virtual void GridCollection::addRefinement(const Integer& level, const Integer k) must not be called!"
             << "It must have been called illegally through the base class GenericGridCollection."
             << endl;
        abort();
        return -1;
    }
    inline virtual Integer addMultigridCoarsening(
      const Integer& level,
      const Integer  k = 0) {
        if (&level || &k); // Avoid compiler warnings.
        cerr << "virtual void GridCollection::addMultigridCoarsening(const Integer& level, const Integer k) must not be called!"
             << "It must have been called illegally through the base class GenericGridCollection."
             << endl;
        abort();
        return -1;
    }
    inline virtual void initialize(
      const Integer& numberOfGrids_) {
        if (&numberOfGrids_); // Avoid compiler warnings.
        cerr << "virtual void GridCollection::initialize(const Integer& numberOfGrids_) must not be called!"
             << "It must have been called illegally through the base class GenericGridCollection."
             << endl;
        abort();
    }

  protected:
    GridCollection *master;

//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((GridCollection&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((GridCollection&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new GridCollection(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};
//
// Stream output operator.
//
ostream& operator<<(ostream& s, const GridCollection& g);

#endif // _GridCollection
