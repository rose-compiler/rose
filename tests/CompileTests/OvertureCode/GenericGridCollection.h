#ifndef _GenericGridCollection
#define _GenericGridCollection

//
// Who to blame:  Geoff Chesshire
//

#include "GenericGrid.h"
#ifdef USE_STL
#include "RCVector.h"
#else
#include "ListOfGenericGrid.h"
#include "ListOfGenericGridCollection.h"
#endif // USE_STL
#include "GCMath.h"

class GenericGridCollection;

//
//  Class for reference-counted data.
//
class GenericGridCollectionData:
  public ReferenceCounting {
  public:
    enum {
      THEbaseGrid                     = 1 << 20,                 // bit 20
      THErefinementLevel              = THEbaseGrid        << 1, // bit 21
      THEcomponentGrid                = THErefinementLevel << 1, // bit 22
      THEmultigridLevel               = THEcomponentGrid   << 1, // bit 23
      ENDtheGenericGridCollectionData = THEmultigridLevel  << 1, // bit 24
      NOTHING                         = GenericGridData::NOTHING,
      THEusualSuspects                = GenericGridData::THEusualSuspects,
      THElists                        = THEbaseGrid
                                      | THErefinementLevel
                                      | THEcomponentGrid
                                      | THEmultigridLevel,
      EVERYTHING                      = GenericGridData::EVERYTHING
                                      | THEbaseGrid
                                      | THErefinementLevel
                                      | THEcomponentGrid
                                      | THEmultigridLevel,
      COMPUTEnothing                  = GenericGridData::COMPUTEnothing,
      COMPUTEtheUsual                 = GenericGridData::COMPUTEtheUsual,
      COMPUTEfailed                   = GenericGridData::COMPUTEfailed 
    };
    Integer                         computedGeometry;
#ifdef USE_STL
    Integer                         numberOfGrids;
    RCVector<GenericGrid>           grid;
    IntegerArray                    gridNumber;
    Integer                         numberOfBaseGrids;
    RCVector<GenericGridCollection> baseGrid;
    IntegerArray                    baseGridNumber;
    Integer                         numberOfRefinementLevels;
    RCVector<GenericGridCollection> refinementLevel;
    IntegerArray                    refinementLevelNumber;
    Integer                         numberOfComponentGrids;
    RCVector<GenericGridCollection> componentGrid;
    IntegerArray                    componentGridNumber;
    Integer                         numberOfMultigridLevels;
    RCVector<GenericGridCollection> multigridLevel;
    IntegerArray                    multigridLevelNumber;
#else
    Integer                         numberOfGrids;
    ListOfGenericGrid               grid;
    IntegerArray                    gridNumber;
    Integer                         numberOfBaseGrids;
    ListOfGenericGridCollection     baseGrid;
    IntegerArray                    baseGridNumber;
    Integer                         numberOfRefinementLevels;
    ListOfGenericGridCollection     refinementLevel;
    IntegerArray                    refinementLevelNumber;
    Integer                         numberOfComponentGrids;
    ListOfGenericGridCollection     componentGrid;
    IntegerArray                    componentGridNumber;
    Integer                         numberOfMultigridLevels;
    ListOfGenericGridCollection     multigridLevel;
    IntegerArray                    multigridLevelNumber;
#endif // USE_STL
    GenericGridCollectionData(const Integer numberOfGrids_ = 0);
    GenericGridCollectionData(
      const GenericGridCollectionData& x,
      const CopyType                   ct = DEEP);
    virtual ~GenericGridCollectionData();
    GenericGridCollectionData& operator=(const GenericGridCollectionData& x);
    inline GenericGrid& operator[](const Integer& i) { return grid[i]; }
    inline const GenericGrid& operator[](const Integer& i) const
      { return grid[i]; }
    void reference(const GenericGridCollectionData& x);
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
    virtual Integer update(
      GenericGridCollectionData& x,
      const Integer              what = THEusualSuspects,
      const Integer              how = COMPUTEtheUsual);
    virtual void destroy(const Integer what = NOTHING);
    virtual void geometryHasChanged(const Integer what = ~NOTHING);
    virtual Integer addRefinement(
      const Integer& level,
      const Integer  k = 0);
    virtual void deleteRefinement(const Integer& k);
    virtual void deleteRefinementLevels(const Integer level = 0);
    virtual void referenceRefinementLevels(
      GenericGridCollectionData& x,
      const Integer              level = INTEGER_MAX);
    virtual Integer addMultigridCoarsening(
      const Integer& level,
      const Integer  k = 0);
    virtual void deleteMultigridCoarsening(const Integer& k);
    virtual void deleteMultigridLevels(const Integer level = 0);
    virtual void setNumberOfGrids(const Integer& numberOfGrids_);
    void initialize(const Integer& numberOfGrids_);

  protected:
    enum GridCollectionTypes
    {
      masterGridCollection,
      refinementLevelGridCollection,
      multigridLevelGridCollection,
      baseGridGridCollection
    } gridCollectionType;

    Integer updateCollection(
      const Integer&                   what,
      Integer&                         numberOfCollections,
#ifdef USE_STL
      RCVector<GenericGridCollection>& list,
#else
      ListOfGenericGridCollection&     list,
#endif // USE_STL
      IntegerArray&                    number);
//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((GenericGridCollectionData&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((GenericGridCollectionData&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new GenericGridCollectionData(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};

class GenericGridCollection:
  public ReferenceCounting {
  public:
//  Public constants:

//  Constants to be ORed to form the first argument of update() and destroy():
    enum {
      THEbaseGrid        = GenericGridCollectionData::THEbaseGrid,
      THErefinementLevel = GenericGridCollectionData::THErefinementLevel,
      THEcomponentGrid   = GenericGridCollectionData::THEcomponentGrid,
      THEmultigridLevel  = GenericGridCollectionData::THEmultigridLevel,
      NOTHING            = GenericGridCollectionData::NOTHING,
      THEusualSuspects   = GenericGridCollectionData::THEusualSuspects,
      THElists           = GenericGridCollectionData::THElists,
      EVERYTHING         = GenericGridCollectionData::EVERYTHING
    };

//  Constants to be ORed to form the second argument of update():
    enum {
      COMPUTEnothing     = GenericGridCollectionData::COMPUTEnothing,
      COMPUTEtheUsual    = GenericGridCollectionData::COMPUTEtheUsual,
      COMPUTEfailed      = GenericGridCollectionData::COMPUTEfailed 
    };

//  Public data:
#ifdef USE_STL
//  List of grids.
    RCVector<GenericGrid>                 grid;
    const IntegerArray                    gridNumber;

//  Collections of grids having the same base grid.
    RCVector<GenericGridCollection>       baseGrid;
    const IntegerArray                    baseGridNumber;

//  Collections of grids having the same refinement level.
    RCVector<GenericGridCollection>       refinementLevel;
    const IntegerArray                    refinementLevelNumber;

//  Collections of multigrid coarsenings of the same component grid
    RCVector<GenericGridCollection>       componentGrid;
    const IntegerArray                    componentGridNumber;

//  Collections of grids having the same multigrid level.
    RCVector<GenericGridCollection>       multigridLevel;
    const IntegerArray                    multigridLevelNumber;
#else
//  List of grids.
    ListOfGenericGrid                     grid;
    const IntegerArray                    gridNumber;

//  Collections of grids having the same base grid.
    ListOfGenericGridCollection           baseGrid;
    const IntegerArray                    baseGridNumber;

//  Collections of grids having the same refinement level.
    ListOfGenericGridCollection           refinementLevel;
    const IntegerArray                    refinementLevelNumber;

//  Collections of multigrid coarsenings of the same component grid
    ListOfGenericGridCollection           componentGrid;
    const IntegerArray                    componentGridNumber;

//  Collections of grids having the same multigrid level.
    ListOfGenericGridCollection           multigridLevel;
    const IntegerArray                    multigridLevelNumber;
#endif // USE_STL
//
//  Public member functions for access to data:
//
//  This is a mask that indicates which geometrical data has been computed.
//  This must be reset to zero to invalidate the data when the geometry changes.
//
    inline const Integer& computedGeometry() const
      { return rcData->computedGeometry; }
//
//  The number of grids in the grid list.
//
    inline const Integer& numberOfGrids() const { return rcData->numberOfGrids; }
//
//  The number of GridCollections in baseGrid.
//
    inline const Integer& numberOfBaseGrids() const
      { return rcData->numberOfBaseGrids; }
//
//  The number of GridCollections in refinementLevel.
//
    inline const Integer& numberOfRefinementLevels() const
      { return rcData->numberOfRefinementLevels; }
//
//  The number of GridCollections in componentGrid.
//
    inline const Integer& numberOfComponentGrids() const
      { return rcData->numberOfComponentGrids; }
//
//  The number of GridCollections in multigridLevel.
//
    inline const Integer& numberOfMultigridLevels() const
      { return rcData->numberOfMultigridLevels; }
//
//  Public member functions:
//
//  Default constructor.
//
//  Create a GenericGridCollection with the given number of dimensions
//  and number of component grids.
//
    GenericGridCollection(const Integer numberOfGrids_ = 0);
//
//  Copy constructor.  (Does deep copy by default.)
//
    GenericGridCollection(
       const GenericGridCollection& x,
       const CopyType               ct = DEEP);
//
//  Destructor.
//
    virtual ~GenericGridCollection();
//
//  Assignment operator.  (Does a deep copy.)
//
    GenericGridCollection& operator=(const GenericGridCollection& x);
//
//  Get a reference to a component grid using C or Fortran indexing.
//
    inline GenericGrid& operator[](const Integer& i) { return grid[i]; }
    inline const GenericGrid& operator[](const Integer& i) const
      { return grid[i]; }
//
//  Make a reference.  (Does a shallow copy.)
//
    void reference(const GenericGridCollection& x);
    void reference(GenericGridCollectionData& x);
//
//  Break a reference.  (Replaces with a deep copy.)
//
    virtual void breakReference();
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
    virtual Integer update(
      GenericGridCollection& x,
      const Integer          what = THEusualSuspects,
      const Integer          how = COMPUTEtheUsual);
//
//  Destroy optional grid data.
//
    virtual void destroy(const Integer what = NOTHING);
//
//  Mark all of the geometric data out-of-date.
//
    inline void geometryHasChanged(const Integer what = ~NOTHING)
      { rcData->geometryHasChanged(what); }
//
//  Add a refinement grid to the collection.
//
    virtual Integer addRefinement(
      const Integer& level,  // The refinement level number.
      const Integer  k = 0); // The index of an ancestor of the refinement grid.
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
    virtual void referenceRefinementLevels(
      GenericGridCollection& x,
      const Integer          level = INTEGER_MAX);
//
//  Add a multigrid coarsening of grid k.
//
    virtual Integer addMultigridCoarsening(
      const Integer& level,  // The multigrid level number.
      const Integer  k = 0); // The index of the corresponding grid
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
//  Initialize the GenericGridCollection with the given number of grids.
//  These grids have their gridNumbers, baseGridNumbers and componentGridNumbers
//  set to [0, ..., numberOfGrids_-1], and their refinementLevelNumbers and
//  multigridLevelNumbers set to zero.
//
    virtual void initialize(
      const Integer& numberOfGrids_);
//
//  Comparison function.  Returns true if x is the same grid as *this.
//
    inline Logical operator==(const GenericGridCollection& x) const
      { return x.rcData == rcData; }
    inline Logical operator!=(const GenericGridCollection& x) const
      { return x.rcData != rcData; }
//
//  Return the index of x in *this, or return -1 if x is not in *this.
//
    inline Integer getIndex(const GenericGrid& x) const {
        for (Integer i=0; i<numberOfGrids(); i++) if (grid[i] == x) return i;
        return -1;
    }

    // Here is the master grid.
    GenericGridCollection & masterGridCollection();
//
//  Pointer to reference-counted data.
//
    typedef GenericGridCollectionData RCData;
    RCData* rcData; Logical isCounted;
    inline GenericGridCollectionData* operator->() { return  rcData; }
    inline const GenericGridCollectionData* operator->() const
      { return  rcData; }
    inline GenericGridCollectionData& operator*() { return *rcData; }
    inline const GenericGridCollectionData& operator*() const
      { return *rcData; }

  protected:
      GenericGridCollection *master;

//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((GenericGridCollection&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((GenericGridCollection&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new GenericGridCollection(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};
//
// Stream output operator.
//
ostream& operator<<(ostream& s, const GenericGridCollection& g);

#endif // _GenericGridCollection
