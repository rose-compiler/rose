
namespace Geometry
   {
  // class PolygonalMeshTypeInfo {};

#if 1
     template <typename MeshTypeInfo> class MeshBase
        {
       // MeshTypeInfo x;
        };
#endif

     template <class CoordinateSystem>
     class PolyMesh
        {
          public:
            // Forward declaration oc class that will be defined in an alternative re-entrant namespace definition of: Geometry
               class Zone;
        };
   }

template<typename MeshType>
class X2
   {
     public:
//       MeshType x;
//       typedef int Scalar;
         typedef int Zone_typedef;
   };

#if 1
#if 0
namespace Geometry
   {
#if 1
  // Now we define the class Zone defined within Geometry::PolyMesh
  // This causes the symbol for Zone class to be placed into global scope (a mistake/bug).
     template <class CoordinateSystem>
     class PolyMesh<CoordinateSystem>::Zone
        {
          public:
//             typedef PolyMesh<CoordinateSystem> MeshType;
        };
#endif
   }
#else
template <class CoordinateSystem>
class Geometry::PolyMesh<CoordinateSystem>::Zone
   {
     public:
   };
#endif
#endif

// X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar *sp;
// X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Scalar sp_0;

// X2<  Geometry::MeshBase < int > >::Scalar sp_1;

// X2<  Geometry::MeshBase < Geometry::PolygonalMeshTypeInfo > >::Zone sp_1;

// This will be unparsed as "Zone sp_1;" if the PolyMesh<CoordinateSystem>::ZoneX 
// class is defined in the second re-entrant definition of the namespace Geometry.
// X2<  Geometry::MeshBase < float > >::Zone sp_1;

X2<  Geometry::PolyMesh < float >::Zone >::Zone_typedef sp_2;
