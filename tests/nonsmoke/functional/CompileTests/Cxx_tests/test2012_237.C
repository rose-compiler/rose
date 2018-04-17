template <typename MeshTypeInfo> class ZoneBase { int xyz; };

class PolygonalMeshTypeInfo
   {
     public:
          typedef ZoneBase<PolygonalMeshTypeInfo>   ZoneHandle;
   };
      
