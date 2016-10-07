
namespace Geometry
   {
     class PolygonalMeshTypeInfo {};

     template <typename MeshTypeInfo> class MeshBase
        {
          MeshTypeInfo x;
        };

   }

template<typename MeshType>
class X2
   {
     public:
         MeshType x;
         typedef int Scalar;
   };

X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar sp_b;

using Geometry::PolygonalMeshTypeInfo;
using Geometry::MeshBase;

X2<  MeshBase < PolygonalMeshTypeInfo > >::Scalar sp_a;
