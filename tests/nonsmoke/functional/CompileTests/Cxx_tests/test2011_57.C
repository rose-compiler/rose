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

class PolygonalMeshTypeInfo {};

template <typename MeshTypeInfo> class MeshBase
   {
     MeshTypeInfo x;
   };

X2<  MeshBase < PolygonalMeshTypeInfo > >::Scalar sp_a;

// Note that if this is will use different classes as template arguments if 
// the name qualification is not outoput, and the resulting code will compile.
X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar sp_b;
