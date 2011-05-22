
template<typename MeshType>
class X1
   {
     public:
          typedef typename MeshType::ScalarType Scalar;
          typedef typename MeshType::ZoneHandle Zone;
   };


template<typename MeshType>
class X2: public X1<MeshType> 
   {
     public:
          typedef typename MeshType::ScalarType Scalar;
          typedef typename MeshType::ZoneHandle Zone;
   };

// Reproduction of the class and template structure within the "Geometry" namespace
namespace Geometry
   {
      struct Dim2
         {
           static int numberOfDimensions() { return 2; }
         };

     class Tensor2d;
     class Vector2d
        {
          public:
               typedef double    ScalarType;
               typedef double    InnerProductType;
               typedef Tensor2d  OuterProductType;
        };

     class XY
        {
          public:
               typedef Dim2                 ScalarType;

               typedef Vector2d             VectorType;
               typedef Tensor2d             TensorType;
               typedef Dim2                 DimensionType;
               enum { nDim = 2 };
        };

     template <class CoordinateSystem>
     class PolyMesh
        {
          public:

               typedef typename CoordinateSystem::ScalarType          ScalarType;
               typedef typename CoordinateSystem::TensorType          TensorType;
               typedef typename CoordinateSystem::VectorType          VectorType;
               typedef typename CoordinateSystem::VectorType          gvector;
               typedef typename CoordinateSystem::VectorType          point;
               typedef PolyMesh<CoordinateSystem>                     MeshType;

               typedef typename CoordinateSystem::ScalarType      Scalar;
               typedef typename CoordinateSystem::TensorType      Tensor;
               typedef typename CoordinateSystem::VectorType      Vector;
               typedef PolyMesh<CoordinateSystem>                 Mesh;
   
               typedef CoordinateSystem                           CoordinateSystemType;
               typedef typename CoordinateSystem::DimensionType            DimensionType;

               class Zone;
               class Face;
               class Side;
               class Edge;
               class Node;
               class Corner;

               typedef typename Mesh::Zone ZoneType;
               typedef typename Mesh::Face FaceType;
               typedef typename Mesh::Side SideType;
               typedef typename Mesh::Edge EdgeType;
               typedef typename Mesh::Node NodeType;
               typedef typename Mesh::Corner CornerType;
        };

     class SpaceBase {};

     template <typename MeshTypeInfo>
     class MeshBase: public SpaceBase
        {
          public:
               typedef MeshBase<MeshTypeInfo>                  BaseType;

               typedef typename MeshTypeInfo::CoreMesh          CoreMesh;
               typedef typename MeshTypeInfo::CoordinateSystem  CoordinateSystem;

            // Dimensional elements.
               typedef typename CoreMesh::ScalarType           ScalarType;
               typedef typename CoreMesh::VectorType           VectorType;
               typedef typename CoreMesh::TensorType           TensorType;
               typedef typename CoreMesh::VectorType           PositionType;
               typedef typename CoreMesh::DimensionType        DimensionType;

            // Data types.
               typedef typename MeshTypeInfo::ZoneHandle       ZoneHandle;
        };

     template <typename MeshTypeInfo>
     class ZoneBase
        {
          public:
               typedef ZoneBase<MeshTypeInfo>                  BaseType;

            // Core mesh type.
               typedef typename MeshTypeInfo::CoreMesh         CoreMesh;

            // Mesh types.
               typedef typename MeshTypeInfo::MeshType         MeshType;

            // Element types.
               typedef typename MeshTypeInfo::CornerHandle     CornerHandle;
               typedef typename MeshTypeInfo::EdgeHandle       EdgeHandle;
               typedef typename MeshTypeInfo::FaceHandle       FaceHandle;
               typedef typename MeshTypeInfo::NodeHandle       NodeHandle;
               typedef typename MeshTypeInfo::SideHandle       SideHandle;
               typedef typename MeshTypeInfo::ZoneHandle       ZoneHandle;

            // CoreMesh types.
               typedef typename CoreMesh::ScalarType     ScalarType;
               typedef typename CoreMesh::VectorType     VectorType;
               typedef typename CoreMesh::TensorType     TensorType;

          protected:
               typedef typename CoreMesh::ZoneType   CoreZoneType;
               typedef typename CoreMesh::ZoneType   CoreSideType;
        };

     class PolygonalMeshTypeInfo
        {
          public:
              typedef XY                                CoordinateSystem;
              typedef PolyMesh<XY>                      CoreMesh;
              typedef MeshBase<PolygonalMeshTypeInfo>   MeshType;
              typedef ZoneBase<PolygonalMeshTypeInfo>   ZoneHandle;
        };

     typedef MeshBase<PolygonalMeshTypeInfo> PolygonalMesh;

     class FieldBase 
        {
          public:
               virtual ~FieldBase() {};
        };

     template <typename Space, typename Centering, typename Value>
     class ReferenceField: public FieldBase
        {
          public:
               typedef Space                                         FieldSpaceType;
               typedef Space                                         SpaceType;
               typedef              Value                            value_type;
               typedef              Centering                        centering_type;
        };

     template <class SpaceType, class CenteringType, class ValueType>
     class Field : public ReferenceField<SpaceType, CenteringType, ValueType> 
        {
          public:
               typedef ReferenceField<SpaceType, CenteringType, ValueType> ReferenceFieldType;
               typedef typename ReferenceFieldType::value_type       value_type;
               typedef SpaceType FieldSpaceType;
               typedef CenteringType FieldCenteringType;
               typedef ValueType FieldValueType;
        };
   }

namespace Geometry
   {
     template <class CoordinateSystem>
     class PolyMesh<CoordinateSystem>::Zone
        {
          public:
               typedef PolyMesh<CoordinateSystem> MeshType;

               typedef typename PolyMesh<CoordinateSystem>::Face **           iterator;
               typedef typename PolyMesh<CoordinateSystem>::Face * const *    const_iterator;
               typedef typename PolyMesh<CoordinateSystem>::Side * const *    SideIterator;
               typedef typename PolyMesh<CoordinateSystem>::Corner **         CornerIterator;
               typedef typename PolyMesh<CoordinateSystem>::Corner * const *  const_CornerIterator;
        };
   }

// Simple initial test
X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar *sp;

#if 0
// DQ (5/15/2011): Testing...

// Target Example:
// Geometry::Field< Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo >,
//                  X1<Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo > >::Zone,
//                  X2<Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo > >::Scalar > *pointer;
Geometry::Field <Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo >,
                 X1<Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Zone,
                 X2<Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar > *sp2;
#endif


