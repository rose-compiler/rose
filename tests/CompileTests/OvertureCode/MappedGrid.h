#ifndef _MappedGrid
#define _MappedGrid

//
// Who to blame:  Geoff Chesshire
//

#include "GenericGrid.h"
#include "MappedGridFunction.h"
#include "MappingRC.h"
#include "Box.H"
#include "GCMath.h"

// #include "DataBaseAccessFunctions.h" // Cannot include this here.
int initializeMappingList();
int destructMappingList();

class AMR_ParentChildSiblingInfo;

//
//  Class for reference-counted data.
//
class MappedGridData:
  public GenericGridData {
  public:
    enum {
      THEmask                    = ENDtheGenericGridData,           // bit  0
      THEinverseVertexDerivative = THEmask                    << 1, // bit  1
      THEinverseCenterDerivative = THEinverseVertexDerivative << 1, // bit  2
      THEvertex                  = THEinverseCenterDerivative << 1, // bit  3
      THEcenter                  = THEvertex                  << 1, // bit  4
      THEcorner                  = THEcenter                  << 1, // bit  5
      THEvertexDerivative        = THEcorner                  << 1, // bit  6
      THEcenterDerivative        = THEvertexDerivative        << 1, // bit  7
      THEvertexJacobian          = THEcenterDerivative        << 1, // bit  8
      THEcenterJacobian          = THEvertexJacobian          << 1, // bit  9
      THEcellVolume              = THEcenterJacobian          << 1, // bit 10
      THEcenterNormal            = THEcellVolume              << 1, // bit 11
      THEcenterArea              = THEcenterNormal            << 1, // bit 12
      THEfaceNormal              = THEcenterArea              << 1, // bit 13
      THEfaceArea                = THEfaceNormal              << 1, // bit 14
      THEvertexBoundaryNormal    = THEfaceArea                << 1, // bit 15
      THEcenterBoundaryNormal    = THEvertexBoundaryNormal    << 1, // bit 16
      THEcenterBoundaryTangent   = THEcenterBoundaryNormal    << 1, // bit 17
      THEminMaxEdgeLength        = THEcenterBoundaryTangent   << 1, // bit 18
      THEboundingBox             = THEminMaxEdgeLength        << 1, // bit 19
      ENDtheMappedGridData       = THEboundingBox             << 1, // bit 20
      THEusualSuspects           = GenericGridData::THEusualSuspects
                                 | THEmask
                                 | THEvertex
                                 | THEcenter
                                 | THEvertexDerivative,
      EVERYTHING                 = GenericGridData::EVERYTHING
                                 | THEmask
                                 | THEinverseVertexDerivative
                                 | THEinverseCenterDerivative
                                 | THEvertex
                                 | THEcenter
                                 | THEcorner
                                 | THEvertexDerivative
                                 | THEcenterDerivative
                                 | THEvertexJacobian
                                 | THEcenterJacobian
                                 | THEcellVolume
                                 | THEcenterNormal
                                 | THEcenterArea
                                 | THEfaceNormal
                                 | THEfaceArea
                                 | THEvertexBoundaryNormal
                                 | THEcenterBoundaryNormal
                                 | THEcenterBoundaryTangent
                                 | THEminMaxEdgeLength
                                 | THEboundingBox,
      USEdifferenceApproximation = 1,                               // bit  0
      COMPUTEgeometry            = USEdifferenceApproximation << 1, // bit  1
      COMPUTEgeometryAsNeeded    = COMPUTEgeometry            << 1, // bit  2
      COMPUTEtheUsual            = GenericGridData::COMPUTEtheUsual
                                 | COMPUTEgeometryAsNeeded,
      ISinterpolationPoint       = INT_MIN,            // (sign bit) bit    31
      ISdiscretizationPoint      = 1 << 30,                       // bit    30
      ISghostPoint               = ISdiscretizationPoint    >> 1, // bit    29
      ISinteriorBoundaryPoint    = ISghostPoint             >> 1, // bit    28
      USESbackupRules            = ISinteriorBoundaryPoint  >> 1, // bit    27
      IShiddenByRefinement       = USESbackupRules          >> 1, // bit    26
      ISreservedBit2             = IShiddenByRefinement     >> 1, // bit    25
      ISreservedBit1             = ISreservedBit2           >> 1, // bit    24
      ISreservedBit0             = ISreservedBit1           >> 1, // bit    23
      GRIDnumberBits             = ISreservedBit0            - 1, // bits 0-22
      ISusedPoint                = ISinterpolationPoint
                                 | ISdiscretizationPoint
                                 | ISghostPoint
    };
    Integer                     numberOfDimensions;
    IntegerArray                boundaryCondition;              // ****** todo : clean up all these arrays****
    IntegerArray                boundaryDiscretizationWidth;
    RealArray                   boundingBox;
    RealArray                   gridSpacing;
    Logical                     isAllCellCentered;
    Logical                     isAllVertexCentered;
    LogicalArray                isCellCentered;
    IntegerArray                discretizationWidth;
    IntegerArray                indexRange;
    IntegerArray                extendedIndexRange;
    int                         extendedRange[2][3]; // include interp pts outside mixed bndry's
    IntegerArray                gridIndexRange;
    IntegerArray                dimension;
    IntegerArray                numberOfGhostPoints;
    Logical                     useGhostPoints;
    IntegerArray                isPeriodic;
    IntegerArray                sharedBoundaryFlag;
    RealArray                   sharedBoundaryTolerance;
    RealArray                   minimumEdgeLength;
    RealArray                   maximumEdgeLength;
    IntegerArray                I1array, I2array, I3array;
    Integer                     *I1, *I2, *I3;
    Partitioning_Type           partition;
    bool                        partitionInitialized;  // true when the partition has been specified.
    GenericGrid::GridTypeEnum   gridType;
    bool                        refinementGrid; // true if this is a refinement grid.

    enum BoundaryFlagEnum
    {
      branchCutPeriodicBoundary,
      periodicBoundary,
      interpolationBoundary,
      mixedPhysicalInterpolationBoundary,
      physicalBoundary
    };
    
    BoundaryFlagEnum boundaryFlag[2][3];

    IntegerMappedGridFunction*  mask;
    RealMappedGridFunction*     inverseVertexDerivative;
    // RealMappedGridFunction*     inverseVertexDerivative2D;
    // RealMappedGridFunction*     inverseVertexDerivative1D;
    RealMappedGridFunction*     inverseCenterDerivative;
    // RealMappedGridFunction*     inverseCenterDerivative2D;
    // RealMappedGridFunction*     inverseCenterDerivative1D;
    RealMappedGridFunction*     vertex;
    // RealMappedGridFunction*     vertex2D;
    // RealMappedGridFunction*     vertex1D;
    RealMappedGridFunction*     center;
    // RealMappedGridFunction*     center2D;
    // RealMappedGridFunction*     center1D;
    RealMappedGridFunction*     corner;
    // RealMappedGridFunction*     corner2D;
    // RealMappedGridFunction*     corner1D;
    RealMappedGridFunction*     vertexDerivative;
    // RealMappedGridFunction*     vertexDerivative2D;
    // RealMappedGridFunction*     vertexDerivative1D;
    RealMappedGridFunction*     centerDerivative;
    // RealMappedGridFunction*     centerDerivative2D;
    // RealMappedGridFunction*     centerDerivative1D;
    RealMappedGridFunction*     vertexJacobian;
    RealMappedGridFunction*     centerJacobian;
    RealMappedGridFunction*     cellVolume;
    RealMappedGridFunction*     centerNormal;
    // RealMappedGridFunction*     centerNormal2D;
    // RealMappedGridFunction*     centerNormal1D;
    RealMappedGridFunction*     centerArea;
    // RealMappedGridFunction*     centerArea2D;
    // RealMappedGridFunction*     centerArea1D;
    RealMappedGridFunction*     faceNormal;
    // RealMappedGridFunction*     faceNormal2D;
    // RealMappedGridFunction*     faceNormal1D;
    RealMappedGridFunction*     faceArea;
    // RealMappedGridFunction*     faceArea2D;
    // RealMappedGridFunction*     faceArea1D;
    RealMappedGridFunction*     vertexBoundaryNormal[3][2];
    RealMappedGridFunction*     centerBoundaryNormal[3][2];
    RealMappedGridFunction*     centerBoundaryTangent[3][2];
    MappingRC                   mapping;
    Box                         box;
    MappedGridData(const Integer numberOfDimensions_ = 0);
    MappedGridData(
      const MappedGridData& x,
      const CopyType        ct = DEEP);
    virtual ~MappedGridData();
    MappedGridData& operator=(const MappedGridData& x);
    void reference(const MappedGridData& x);
    virtual void breakReference();
    virtual void consistencyCheck() const;

    virtual Integer get(const GenericDataBase& db,
			const aString& name,
			bool getMapping=true );   // for AMR grids we may not get the mapping.

    virtual Integer put(GenericDataBase& db,
			const aString& name,
			bool putMapping = true   // for AMR grids we may not save the mapping.
			) const;

    inline Integer update(
      const Integer what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual)
      { return update(*this, what, how); }
    inline Integer update(
      MappedGridData& x,
      const Integer   what = THEusualSuspects,
      const Integer   how = COMPUTEtheUsual)
      { return update((GenericGridData&)x, what, how); }
    virtual void destroy(const Integer what = NOTHING);
    void specifyProcesses(const Range& range);
    void initialize(const Integer& numberOfDimensions_);

    // remove items from what that are not appropriate for the gridType:
    virtual int getWhatForGrid(const int what) const;  
    
    void initializePartition();  // initialize the partition object
    
  protected:
    virtual Integer update(
      GenericGridData& x,
      const Integer    what = THEusualSuspects,
      const Integer    how = COMPUTEtheUsual);
  private:

    Integer update1(
      MappedGridData& y,
      const Integer&  what,
      const Integer&  how,
      Integer&        computeNeeded);
    Integer update2(
      MappedGridData& y,
      const Integer&  what,
      const Integer&  how,
      Integer&        computeNeeded);
    Integer updateUnstructuredGrid(
      MappedGridData& y,
      const Integer&  what,
      const Integer&  how,
      Integer&        computeNeeded);

    Integer computeGeometry(
      const Integer& what,
      const Integer& how);
    Integer computeGeometryWithDifferences(
      const Integer& what,
      const Integer& how);
    Integer computeGeometryFromMapping(
      const Integer& what,
      const Integer& how);
    Integer computeUnstructuredGeometry(
      const Integer& what,
      const Integer& how);
//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((MappedGridData&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((MappedGridData&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new MappedGridData(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
};

class MappedGrid:
  public GenericGrid {
  public:
//  Public constants:

//  Constants to be ORed to form the first argument of update() and destroy():
    enum {
      THEmask                    = MappedGridData::THEmask,
      THEinverseVertexDerivative = MappedGridData::THEinverseVertexDerivative,
      THEinverseCenterDerivative = MappedGridData::THEinverseCenterDerivative,
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
      THEusualSuspects           = MappedGridData::THEusualSuspects,
      EVERYTHING                 = MappedGridData::EVERYTHING
    };

//  Constants to be ORed to form the second argument of update():
    enum {
      USEdifferenceApproximation = MappedGridData::USEdifferenceApproximation,
      COMPUTEgeometry            = MappedGridData::COMPUTEgeometry,
      COMPUTEgeometryAsNeeded    = MappedGridData::COMPUTEgeometryAsNeeded,
      COMPUTEtheUsual            = MappedGridData::COMPUTEtheUsual
    };

//  Mask bits to access data in mask.
    enum {
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

    // should be the same as in MappedGridData
    enum BoundaryFlagEnum
    {
      branchCutPeriodicBoundary         =MappedGridData::branchCutPeriodicBoundary,
      periodicBoundary                  =MappedGridData::periodicBoundary,
      interpolationBoundary             =MappedGridData::interpolationBoundary,
      mixedPhysicalInterpolationBoundary=MappedGridData::mixedPhysicalInterpolationBoundary,
      physicalBoundary                  =MappedGridData::physicalBoundary
    };

//  Public data.
    AMR_ParentChildSiblingInfo* parentChildSiblingInfo;

    AMR_ParentChildSiblingInfo* getParentChildSiblingInfo(){return parentChildSiblingInfo;}
    void setParentChildSiblingInfo(AMR_ParentChildSiblingInfo *ptr){parentChildSiblingInfo=ptr;}
//
//  Public member functions for access to data.
//
//  Boxlib box.
//
    inline const Box& box() const { return rcData->box; }
//
//  Boundary condition flags.
//
    inline const IntegerArray& boundaryCondition() const
      { return rcData->boundaryCondition; }
    inline const Integer& boundaryCondition(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->boundaryCondition(ks,kd); }

    MappedGrid::BoundaryFlagEnum boundaryFlag(int side,int axis ) const;
//
//  Boundary condition stencil.
//
    inline const IntegerArray& boundaryDiscretizationWidth() const
      { return rcData->boundaryDiscretizationWidth; }
    inline const Integer& boundaryDiscretizationWidth(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->boundaryDiscretizationWidth(ks,kd); }
//
//  Bounding box for all grid vertices.
//
    inline const RealArray& boundingBox() const
      { return rcData->boundingBox; }
    inline const Real& boundingBox(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->boundingBox(ks,kd); }
//
//  Grid spacing.
//
    inline const RealArray& gridSpacing() const
      { return rcData->gridSpacing; }
    inline const Real& gridSpacing(const Integer& kd) const
      { return rcData->gridSpacing(kd); }
//
//  Grid type, structured or unstructured.
//
    GenericGrid::GridTypeEnum getGridType() const
      { return rcData->gridType; }
  
//
//  get the name of the grid.
//
    aString getName() const;
//
//  Cell-centered in each direction.
//
    inline const LogicalArray& isCellCentered() const
      { return rcData->isCellCentered; }
    inline const Logical& isCellCentered(const Integer& kd) const
      { return rcData->isCellCentered(kd); }
//
//  Cell-centered in all directions.
//
    inline const Logical& isAllCellCentered() const
      { return rcData->isAllCellCentered; }
//
//  Vertex-centered in all directions.
//
    inline const Logical& isAllVertexCentered() const
      { return rcData->isAllVertexCentered; }

//  is the grid rectangular
    bool isRectangular() const;

    // is the grid a refinement grid:
    bool isRefinementGrid() const { return rcData->refinementGrid; } 
    bool& isRefinementGrid(){ return rcData->refinementGrid; } 

    // show which geometry arrays are built
    int displayComputedGeometry(FILE *file=stdout ) const;

    // return size of this object  
    virtual real sizeOf(FILE *file = NULL ) const;

//
//  Interior discretization stencil width.
//
    inline const IntegerArray& discretizationWidth() const
      { return rcData->discretizationWidth; }
    inline const Integer& discretizationWidth(const Integer& kd) const
      { return rcData->discretizationWidth(kd); }
//
//  Index range, computational points.
//
    inline const IntegerArray& indexRange() const
      { return rcData->indexRange; }
    inline const Integer& indexRange(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->indexRange(ks,kd); }
//
//  Index range plus interpolation points.
//
    inline const IntegerArray& extendedIndexRange() const
      { return rcData->extendedIndexRange; }
    inline const Integer& extendedIndexRange(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->extendedIndexRange(ks,kd); }
//
//  Index range of gridpoints.
//
    inline const IntegerArray& gridIndexRange() const
      { return rcData->gridIndexRange; }
    inline const Integer& gridIndexRange(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->gridIndexRange(ks,kd); }
//
//  Dimensions of grid arrays.
//
    inline const IntegerArray& dimension() const
      { return rcData->dimension; }
    inline const Integer& dimension(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->dimension(ks,kd); }
//
//  Range for all discretization, boundary and interpolation points.
//
    inline const int& extendedRange( int side, int axis ) const
      { return rcData->extendedRange[side][axis]; }
    IntegerArray extendedRange() const;
//
//  Number of ghost points.
//
    inline const IntegerArray& numberOfGhostPoints() const
      { return rcData->numberOfGhostPoints; }
    inline const Integer& numberOfGhostPoints(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->numberOfGhostPoints(ks,kd); }
//
//  Indicate whether ghost points may be used for interpolation.
//
    inline const Logical& useGhostPoints() const
      { return rcData->useGhostPoints; }
//
//  Grid periodicity.
//
    inline const IntegerArray& isPeriodic() const
      { return rcData->isPeriodic; }
    inline const Integer& isPeriodic(const Integer& kd) const
      { return rcData->isPeriodic(kd); }
//
//  The number of dimensions of the domain.
//
    inline const Integer& numberOfDimensions() const
      { return rcData->numberOfDimensions; }
//
//  Shared boundary flags.
//
    inline const IntegerArray& sharedBoundaryFlag() const
      { return rcData->sharedBoundaryFlag; }
    inline const Integer& sharedBoundaryFlag(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->sharedBoundaryFlag(ks,kd); }
//
//  Shared boundary tolerance.
//
    inline const RealArray& sharedBoundaryTolerance() const
      { return rcData->sharedBoundaryTolerance; }
    inline const Real& sharedBoundaryTolerance(
      const Integer& ks,
      const Integer& kd) const
      { return rcData->sharedBoundaryTolerance(ks,kd); }
//
//  Minimum grid cell-edge length.
//
    inline const RealArray& minimumEdgeLength() const
      { return rcData->minimumEdgeLength; }
    inline const Real& minimumEdgeLength(const Integer& kd) const
      { return rcData->minimumEdgeLength(kd); }
//
//  Maximum grid cell-edge length.
//
    inline const RealArray& maximumEdgeLength() const
      { return rcData->maximumEdgeLength; }
    inline const Real& maximumEdgeLength(const Integer& kd) const
      { return rcData->maximumEdgeLength(kd); }
//
// delta x for rectangular grids
//
    int getDeltaX( Real dx[3] ) const;
//
//  Indirect addressing for periodic grids.
//
    const Integer* I1() const { return rcData->I1; }
    const Integer* I2() const { return rcData->I2; }
    const Integer* I3() const { return rcData->I3; }
//
//  Public member functions for setting data.
//
    void setNumberOfDimensions(const Integer& numberOfDimensions_);

    void setBoundaryCondition(const Integer& ks,
			      const Integer& kd,
			      const Integer& boundaryCondition_);
    
    void setBoundaryFlag( int side, int axis, MappedGridData::BoundaryFlagEnum bc );
    
    void setBoundaryDiscretizationWidth(const Integer& ks,
					const Integer& kd,
					const Integer& boundaryDiscretizationWidth_);
    void setIsCellCentered(const Integer& kd,
                           const Logical& isCellCentered_);
    void setDiscretizationWidth(const Integer& kd,
				const Integer& discretizationWidth_);
    void setGridIndexRange(const Integer& ks,
                           const Integer& kd,
			   const Integer& gridIndexRange_);
    
    //  Use a given mapping.
    void setMapping(Mapping& x);
    void setMapping(MappingRC& x);

    void setNumberOfGhostPoints(const Integer& ks,
				const Integer& kd,
				const Integer& numberOfGhostPoints_);
    
    void setUseGhostPoints(const Logical& useGhostPoints_);

    void setIsPeriodic(const Integer& kd, const Mapping::periodicType& isPeriodic_);
    
    void setSharedBoundaryFlag(const Integer& ks,
                               const Integer& kd,
			       const Integer& sharedBoundaryFlag_);
    void setSharedBoundaryTolerance(const Integer& ks,
				    const Integer& kd,
				    const Real&    sharedBoundaryTolerance_);
//
//  Discretization point mask.
//
    inline IntegerMappedGridFunction& mask()
      { return *rcData->mask; }
    inline const IntegerMappedGridFunction& mask() const
      { return ((MappedGrid*)this)->mask(); }
//
//  Inverse derivative of the mapping at the vertices.
//
    inline RealMappedGridFunction&    inverseVertexDerivative()
      { return *rcData->inverseVertexDerivative; }
    inline const RealMappedGridFunction&    inverseVertexDerivative() const
      { return ((MappedGrid*)this)->inverseVertexDerivative(); }
// //
// //  Inverse derivative at the vertices, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction&    inverseVertexDerivative2D()
//       { return *rcData->inverseVertexDerivative2D; }
//     inline const RealMappedGridFunction&    inverseVertexDerivative2D() const
//       { return ((MappedGrid*)this)->inverseVertexDerivative2D(); }
// //
// //  Inverse derivative at the vertices, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction&    inverseVertexDerivative1D()
//       { return *rcData->inverseVertexDerivative1D; }
//     inline const RealMappedGridFunction&    inverseVertexDerivative1D() const
//       { return ((MappedGrid*)this)->inverseVertexDerivative1D(); }
//
//  Inverse derivative at the discretization centers.
//
    inline RealMappedGridFunction&    inverseCenterDerivative()
      { return *rcData->inverseCenterDerivative; }
    inline const RealMappedGridFunction&    inverseCenterDerivative() const
      { return ((MappedGrid*)this)->inverseCenterDerivative(); }
// //
// //  Inverse derivative at the discretization centers, for a two-dimensional grid
// //
//     inline RealMappedGridFunction&    inverseCenterDerivative2D()
//       { return *rcData->inverseCenterDerivative2D; }
//     inline const RealMappedGridFunction&    inverseCenterDerivative2D() const
//       { return ((MappedGrid*)this)->inverseCenterDerivative2D(); }
// //
// //  Inverse derivative at the discretization centers, for a one-dimensional grid
// //
//     inline RealMappedGridFunction&    inverseCenterDerivative1D()
//       { return *rcData->inverseCenterDerivative1D; }
//     inline const RealMappedGridFunction&    inverseCenterDerivative1D() const
//       { return ((MappedGrid*)this)->inverseCenterDerivative1D(); }
//
//  Vertex coordinates.
//
    inline RealMappedGridFunction& vertex()
      { return *rcData->vertex; }
    inline const RealMappedGridFunction& vertex() const
      { return ((MappedGrid*)this)->vertex(); }
// //
// //  Vertex coordinates, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& vertex2D()
//       { return *rcData->vertex2D; }
//     inline const RealMappedGridFunction& vertex2D() const
//       { return ((MappedGrid*)this)->vertex2D(); }
// //
// //  Vertex coordinates, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& vertex1D()
//       { return *rcData->vertex1D; }
//     inline const RealMappedGridFunction& vertex1D() const
//       { return ((MappedGrid*)this)->vertex1D(); }
//
//  Coordinates of discretization centers.
//
    inline RealMappedGridFunction& center()
      { return *rcData->center; }
    inline const RealMappedGridFunction& center() const
      { return ((MappedGrid*)this)->center(); }
// //
// //  Coordinates of discretization centers, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& center2D()
//       { return *rcData->center2D; }
//     inline const RealMappedGridFunction& center2D() const
//       { return ((MappedGrid*)this)->center2D(); }
// //
// //  Coordinates of discretization centers, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& center1D()
//       { return *rcData->center1D; }
//     inline const RealMappedGridFunction& center1D() const
//       { return ((MappedGrid*)this)->center1D(); }
//
//  Coordinates of control volume corners.
//
    inline RealMappedGridFunction& corner()
      { return *rcData->corner; }
    inline const RealMappedGridFunction& corner() const
      { return ((MappedGrid*)this)->corner(); }
// //
// //  Coordinates of control volume corners, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& corner2D()
//       { return *rcData->corner2D; }
//     inline const RealMappedGridFunction& corner2D() const
//       { return ((MappedGrid*)this)->corner2D(); }
// //
// //  Coordinates of control volume corners, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& corner1D()
//       { return *rcData->corner1D; }
//     inline const RealMappedGridFunction& corner1D() const
//       { return ((MappedGrid*)this)->corner1D(); }
//
//  Derivative of the mapping at the vertices.
//
    inline RealMappedGridFunction& vertexDerivative()
      { return *rcData->vertexDerivative; }
    inline const RealMappedGridFunction& vertexDerivative() const
      { return ((MappedGrid*)this)->vertexDerivative(); }
// //
// //  Derivative of the mapping at the vertices, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& vertexDerivative2D()
//       { return *rcData->vertexDerivative2D; }
//     inline const RealMappedGridFunction& vertexDerivative2D() const
//       { return ((MappedGrid*)this)->vertexDerivative2D(); }
// //
// //  Derivative of the mapping at the vertices, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& vertexDerivative1D()
//       { return *rcData->vertexDerivative1D; }
//     inline const RealMappedGridFunction& vertexDerivative1D() const
//       { return ((MappedGrid*)this)->vertexDerivative1D(); }
//
//  Derivative of the mapping at the discretization centers.
//
    inline RealMappedGridFunction& centerDerivative()
      { return *rcData->centerDerivative; }
    inline const RealMappedGridFunction& centerDerivative() const
      { return ((MappedGrid*)this)->centerDerivative(); }
// //
// //  Derivative at the discretization centers, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& centerDerivative2D()
//       { return *rcData->centerDerivative2D; }
//     inline const RealMappedGridFunction& centerDerivative2D() const
//       { return ((MappedGrid*)this)->centerDerivative2D(); }
// //
// //  Derivative at the discretization centers, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& centerDerivative1D()
//       { return *rcData->centerDerivative1D; }
//     inline const RealMappedGridFunction& centerDerivative1D() const
//       { return ((MappedGrid*)this)->centerDerivative1D(); }
//
//  Determinant of vertexDerivative.
//
    inline RealMappedGridFunction& vertexJacobian()
      { return *rcData->vertexJacobian; }
    inline const RealMappedGridFunction& vertexJacobian() const
      { return ((MappedGrid*)this)->vertexJacobian(); }
//
//  Determinant of centerDerivative.
//
    inline RealMappedGridFunction& centerJacobian()
      { return *rcData->centerJacobian; }
    inline const RealMappedGridFunction& centerJacobian() const
      { return ((MappedGrid*)this)->centerJacobian(); }
//
//  Cell Volume.
//
    inline RealMappedGridFunction& cellVolume()
      { return *rcData->cellVolume; }
    inline const RealMappedGridFunction& cellVolume() const
      { return ((MappedGrid*)this)->cellVolume(); }
//
//  Cell-center normal vector, normalized to cell-face area.
//
    inline RealMappedGridFunction& centerNormal()
      { return *rcData->centerNormal; }
    inline const RealMappedGridFunction& centerNormal() const
      { return ((MappedGrid*)this)->centerNormal(); }
// //
// //  Cell-center normal vector, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& centerNormal2D()
//       { return *rcData->centerNormal2D; }
//     inline const RealMappedGridFunction& centerNormal2D() const
//       { return ((MappedGrid*)this)->centerNormal2D(); }
// //
// //  Cell-center normal vector, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& centerNormal1D()
//       { return *rcData->centerNormal1D; }
//     inline const RealMappedGridFunction& centerNormal1D() const
//       { return ((MappedGrid*)this)->centerNormal1D(); }
//
//  Cell-center area (Length of cell-center normal vector).
//
    inline RealMappedGridFunction& centerArea()
      { return *rcData->centerArea; }
    inline const RealMappedGridFunction& centerArea() const
      { return ((MappedGrid*)this)->centerArea(); }
// //
// //  Cell-center area, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& centerArea2D()
//       { return *rcData->centerArea2D; }
//     inline const RealMappedGridFunction& centerArea2D() const
//       { return ((MappedGrid*)this)->centerArea2D(); }
// //
// //  Cell-center area, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& centerArea1D()
//       { return *rcData->centerArea1D; }
//     inline const RealMappedGridFunction& centerArea1D() const
//       { return ((MappedGrid*)this)->centerArea1D(); }
//
//  Cell-face normal vector, normalized to cell-face area.
//
    inline RealMappedGridFunction& faceNormal()
      { return *rcData->faceNormal; }
    inline const RealMappedGridFunction& faceNormal() const
      { return ((MappedGrid*)this)->faceNormal(); }
// //
// //  Cell-face normal vector, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& faceNormal2D()
//       { return *rcData->faceNormal2D; }
//     inline const RealMappedGridFunction& faceNormal2D() const
//       { return ((MappedGrid*)this)->faceNormal2D(); }
// //
// //  Cell-face normal vector, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& faceNormal1D()
//       { return *rcData->faceNormal1D; }
//     inline const RealMappedGridFunction& faceNormal1D() const
//       { return ((MappedGrid*)this)->faceNormal1D(); }
//
//  Cell-face area.
//
    inline RealMappedGridFunction& faceArea()
      { return *rcData->faceArea; }
    inline const RealMappedGridFunction& faceArea() const
      { return ((MappedGrid*)this)->faceArea(); }
// //
// //  Cell-face area, for a two-dimensional grid.
// //
//     inline RealMappedGridFunction& faceArea2D()
//       { return *rcData->faceArea2D; }
//     inline const RealMappedGridFunction& faceArea2D() const
//       { return ((MappedGrid*)this)->faceArea2D(); }
// //
// //  Cell-face area, for a one-dimensional grid.
// //
//     inline RealMappedGridFunction& faceArea1D()
//       { return *rcData->faceArea1D; }
//     inline const RealMappedGridFunction& faceArea1D() const
//       { return ((MappedGrid*)this)->faceArea1D(); }
//
//  Outward unit normal vectors at the vertices on each boundary.
//
    inline RealMappedGridFunction& vertexBoundaryNormal(
      const Integer& ks,
      const Integer& kd)
      { return *rcData->vertexBoundaryNormal[kd][ks]; }
    inline const RealMappedGridFunction& vertexBoundaryNormal(
      const Integer& ks,
      const Integer& kd) const
      { return ((MappedGrid*)this)->vertexBoundaryNormal(ks,kd); }
//
//  Outward unit normal vectors at the centers on each boundary.
//
    inline RealMappedGridFunction& centerBoundaryNormal(
      const Integer& ks,
      const Integer& kd)
      { return *rcData->centerBoundaryNormal[kd][ks]; }
    inline const RealMappedGridFunction& centerBoundaryNormal(
      const Integer& ks,
      const Integer& kd) const
      { return ((MappedGrid*)this)->centerBoundaryNormal(ks,kd); }
//
//  Unit tangent vectors at the centers on each boundary.
//
    inline RealMappedGridFunction& centerBoundaryTangent(
      const Integer& ks,
      const Integer& kd)
      { return *rcData->centerBoundaryTangent[kd][ks]; }
    inline const RealMappedGridFunction& centerBoundaryTangent(
      const Integer& ks,
      const Integer& kd) const
      { return ((MappedGrid*)this)->centerBoundaryTangent(ks,kd); }
//
//  Grid mapping.
//
    inline MappingRC& mapping() { return rcData->mapping; }
    inline const MappingRC& mapping() const { return rcData->mapping; }
//
//  Public member functions.
//
//  Default constructor.
//
//  If numberOfDimensions_==0 (e.g., by default) then create a null
//  MappedGrid.  Otherwise, create a MappedGrid with the given
//  number of dimensions.
//
    MappedGrid(const Integer numberOfDimensions_ = 0);
//
//  Copy constructor.  (Does a deep copy by default.)
//
    MappedGrid(
      const MappedGrid& x,
      const CopyType    ct = DEEP);
//
//  Constructor from a mapping.
//
    MappedGrid(Mapping& mapping);
    MappedGrid(MappingRC& mapping);
//
//  Destructor.
//
    virtual ~MappedGrid();
//
//  Assignment operator.  (Does a deep copy.)
//
    MappedGrid& operator=(const MappedGrid& x);
//
//  Make a reference.  (Does a shallow copy.)
//
    void reference(const MappedGrid& x);
    void reference(MappedGridData& x);
//
//  Use a given mapping.
//
    void reference(Mapping& x);
    void reference(MappingRC& x);
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
//
//  "Get" and "put" database operations.
//
    virtual Integer get(const GenericDataBase& db,
			const aString& name,
			bool getMapping=true );   // for AMR grids we may not get the mapping.

    virtual Integer put(GenericDataBase& db,
			const aString& name,
      			bool putMapping = true   // for AMR grids we may not save the mapping.
			) const;
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
      MappedGrid&   x,
      const Integer what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual)
      { return update((GenericGrid&)x, what, how); }
//
//  Destroy optional grid data.
//
    virtual void destroy(const Integer what = NOTHING);
#ifdef ADJUST_FOR_PERIODICITY
//
//  Adjust the inverted coordinates for periodicity.
//
    void adjustForPeriodicity(
      const RealArray&    r,
      const LogicalArray& whereMask);
#endif // ADJUST_FOR_PERIODICITY
//
//  Adjust the inverted coordinates of boundary points
//  in cases where the points lie on a shared boundary.
//
    void adjustBoundary(
      MappedGrid&      g2,
      const Integer&   ks1,
      const Integer&   kd1,
      const RealArray& r2,
      const LogicalArray& whereMask);
//
//  Compute the condition number of the mapping inverse.
//
//  The condition number is the max norm (max absolute row sum) of the matrix
//
//    [ 1/dr2   0   ] [ rx2 ry2 ] [ xr1 xs1 ] [ dr1  0  ]
//    [   0   1/ds2 ] [ sx2 sy2 ] [ yr1 ys1 ] [  0  ds1 ]
//
    void getInverseCondition(
      MappedGrid&         g2,
      const RealArray&    xr1,
      const RealArray&    rx2,
      const RealArray&    condition,
      const LogicalArray& whereMask);
//
//  Specify the set of processes over which MappedGridFunctions are distributed.
//  We now support only the specification of a contiguous range of process IDs.
//
    void specifyProcesses(const Range& range);
//
//  Initialize the MappedGrid with the given number of dimensions.
//
    virtual void initialize(
      const Integer& numberOfDimensions_);
//
//  Pointer to reference-counted data.
//
    typedef MappedGridData RCData;
    MappedGridData* rcData; Logical isCounted;
    inline       MappedGridData* operator->()       { return  rcData; }
    inline const MappedGridData* operator->() const { return  rcData; }
    inline       MappedGridData& operator*()        { return *rcData; }
    inline const MappedGridData& operator*()  const { return *rcData; }

  private:
    virtual Integer update(
      GenericGrid&  x,
      const Integer what = THEusualSuspects,
      const Integer how = COMPUTEtheUsual);

  protected:
//
//  The following functions are declared protected here in order to disallow
//  access to the corresponding functions in class GenericGrid.
//
    inline virtual void initialize() {
        cerr << "virtual void MappedGrid::initialize() must not be called!"
             << "It must have been called illegally through the base class GenericGridCollection."
             << endl;
        abort();
    }

//
//  Virtual member functions used only through class ReferenceCounting:
//
  private:
    inline virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((MappedGrid&)x); }
    inline virtual void reference(const ReferenceCounting& x)
      { reference((MappedGrid&)x); }
    inline virtual ReferenceCounting* virtualConstructor(
      const CopyType ct = DEEP) const
      { return new MappedGrid(*this, ct); }
    aString className;
  public:
    inline virtual aString getClassName() const { return className; }
    Integer updateMappedGridPointers(const Integer what);
};
//
// Stream output operator.
//
ostream& operator<<(ostream& s, const MappedGrid& g);

#endif // _MappedGrid
