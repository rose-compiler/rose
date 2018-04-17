
#if 0
// Original Code:
//   resultptr = new Geometry::Field<Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo >,Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo >::FaceHandle,Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo >::VectorType >
//        ((Geometry::Field<Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo >,Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo >::FaceHandle,Geometry::MeshBase<Geometry::PolygonalMeshTypeInfo >::VectorType > &) result);

// Problem code (unparsed by ROSE):
//   resultptr = 
//     new Geometry::Field< Hydro< Geometry::MeshBase< Geometry::PolygonalMeshTypeInfo > > ::Mesh , Hydro< Geometry::MeshBase< Geometry::PolygonalMeshTypeInfo > > ::Face , Hydro< Geometry::MeshBase< Geometry::PolygonalMeshTypeInfo > > ::Vector >  
//          (result.SwigValueWrapper< Geometry::Field< Hydro< Geometry::MeshBase< Geometry::PolygonalMeshTypeInfo > > ::Mesh , Hydro< Geometry::MeshBase< Geometry::PolygonalMeshTypeInfo > > ::Face , Hydro< Geometry::MeshBase< Geometry::PolygonalMeshTypeInfo > > ::Vector >  > ());

#endif


#if 0
// Simpler version of problem
// Original Code:
//   resultptr = new Geometry::Field<x,y,z>::VectorType ((Geometry::Field<x,y,z> &) result);

// Problem code (unparsed by ROSE):
//   resultptr = new Geometry::Field<x,y,z> (result.SwigValueWrapper<x,y,z>());

#endif


template <typename T> class X
   {
     public:
          X ();
       // X ( const X & a );
          X ( X & a );
   };

void foo ()
   {
     X<int> xObject;
     X<int>* ptr = new X<int>( (X<int> &) xObject );
   }


