
namespace Geometry
   {
     class PolygonalMeshTypeInfo {};

     template <typename MeshTypeInfo> class MeshBase
        {
          MeshTypeInfo x;
        };

     template <class CoordinateSystem>
     class PolyMesh
        {
          public:
               class Zone;
        };
   }

template<typename MeshType>
class X2
   {
     public:
         MeshType x;
         typedef int Scalar;
         typedef int Zone;
   };

namespace Geometry
   {
#if 1
  // Now we define the class Zone defined within Geometry::PolyMesh
     template <class CoordinateSystem>
     class PolyMesh<CoordinateSystem>::Zone
        {
          public:
               typedef PolyMesh<CoordinateSystem> MeshType;
        };
#endif
   }


// X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar *sp;
// X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar sp_0;

X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Zone sp_1;

// X2<  Geometry::MeshBase < int > >::Scalar sp_1;

