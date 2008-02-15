#ifndef _GenericGrid
#define _GenericGrid

//
// Who to blame:  Geoff Chesshire
//

#include <limits.h>
#include "ReferenceCounting.h"
#include "GenericGridFunction.h"
#include "GenericDataBase.h"

//
//  Class for reference-counted data.
//
class GenericGridData:
  public ReferenceCounting {
  public:
    enum {
      ENDtheGenericGridData = 1, // bit 0
      NOTHING               = 0,
      THEusualSuspects      = INT_MIN, // (sign bit)
      EVERYTHING            = NOTHING,
      COMPUTEnothing        = 0,
      COMPUTEtheUsual       = COMPUTEnothing,
      COMPUTEfailed         = INT_MIN  // (sign bit)
    };
    Integer computedGeometry;
    GenericGridData();
    GenericGridData(
      const GenericGridData& x,
      const CopyType         ct = DEEP);
    virtual ~GenericGridData();
    GenericGridData& operator=(const GenericGridData& x);
    void reference(const GenericGridData& x);
    virtual void breakReference();
    virtual void consistencyCheck() const;

    virtual Integer get(const GenericDataBase& db,
			const aString&         name,
			bool getMapping=true );   // for AMR grids we may not get the mapping.

    virtual Integer put(GenericDataBase& db,
			const aString&   name,
			bool putMapping = true ) const;  // for AMR grids we may not save the mapping.

    inline Integer update(
      const Integer what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual)
      { return update(*this, what, how); }
    virtual Integer update(
      GenericGridData& x,
      const Integer    what = THEusualSuspects,
      const Integer    how = COMPUTEtheUsual);
    virtual void destroy(const Integer what = NOTHING);
    virtual void geometryHasChanged(const Integer what = ~NOTHING);
    void initialize();
//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((GenericGridData&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((GenericGridData&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new GenericGridData(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};

class GenericGrid:
  public ReferenceCounting {
  public:
//  Public constants:

    enum GridTypeEnum
    {
      structuredGrid=0,
      unstructuredGrid
    };

//  Constants to be ORed to form the first argument of update() and destroy():
    enum {
      NOTHING          = GenericGridData::NOTHING,
      THEusualSuspects = GenericGridData::THEusualSuspects,
      EVERYTHING       = GenericGridData::EVERYTHING
    };

//  Constants to be ORed to form the second argument of update():
    enum {
      COMPUTEnothing   = GenericGridData::COMPUTEnothing,
      COMPUTEtheUsual  = GenericGridData::COMPUTEtheUsual,
      COMPUTEfailed    = GenericGridData::COMPUTEfailed
    };
//
//  Public member functions for access to data:
//
//  This is a mask that indicates which geometrical data has been computed.
//  This must be reset to zero to invalidate the data when the geometry changes.
//
    inline const Integer& computedGeometry() const
      { return rcData->computedGeometry; }
//
//  Public member functions.
//
//  Default constructor.
//
//  Create a GenericGrid.
//
    GenericGrid();
//
//  Copy constructor.  (Does a deep copy by default.)
//
    GenericGrid(
      const GenericGrid& x,
      const CopyType     ct = DEEP);
//
//  Destructor.
//
    virtual ~GenericGrid();
//
//  Assignment operator.  (Does a deep copy.)
//
    GenericGrid& operator=(const GenericGrid& x);
//
//  Make a reference.  (Does a shallow copy.)
//
    void reference(const GenericGrid& x);
    void reference(GenericGridData& x);
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
    virtual Integer get(const GenericDataBase& db,
			const aString& name,
			bool getMapping=true );   // for AMR grids we may not get the mapping.

    virtual Integer put(GenericDataBase& db,
			const aString&   name,
			bool putMapping = true ) const;  // for AMR grids we may not save the mapping.
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
      GenericGrid&  x,
      const Integer what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual);
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
//  Initialize the GenericGrid.
//
    virtual void initialize();
//
//  Comparison function.  Returns true if x is the same grid as *this.
//
    inline Logical operator==(const GenericGrid& x) const
      { return x.rcData == rcData; }
    inline Logical operator!=(const GenericGrid& x) const
      { return x.rcData != rcData; }
//
//  Pointer to reference-counted data.
//
    typedef GenericGridData RCData;
    RCData* rcData; Logical isCounted;
    inline       GenericGridData* operator->()       { return  rcData; }
    inline const GenericGridData* operator->() const { return  rcData; }
    inline       GenericGridData& operator*()        { return *rcData; }
    inline const GenericGridData& operator*()  const { return *rcData; }
//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((GenericGrid&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((GenericGrid&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new GenericGrid(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};
//
// Stream output Operator.
//
ostream& operator<<(ostream& s, const GenericGrid& g);

#endif // _GenericGrid
