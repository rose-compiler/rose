
namespace Geometry
   {
     class PolygonalMeshTypeInfo {};

     template <typename MeshTypeInfo> class MeshBase
        {
          MeshTypeInfo x;
        };

     template<typename MeshType>
     class X2
        {
          public:
              MeshType x;
              typedef int Scalar;
        };

     X2<  MeshBase < PolygonalMeshTypeInfo > >::Scalar sp;
   }

using Geometry::X2;

X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar sp;

