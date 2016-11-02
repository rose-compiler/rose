template <class T>
class NewLaserMaterialDataBase
   {
   };

namespace LRT_namespace
   {
     class LRTMeshTypes
        {
        };
   }

namespace NewLaserNamespace
   {
#if 0
  // DQ (8/19/2014): g++ reports this as a bug (so I moved the teamplte instantiation directive to the outer scope).
  // BUG: This is being unparsed as: "template class NewLaserMaterialDataBase < LRTMeshTypes >;"
     template class NewLaserMaterialDataBase<LRT_namespace::LRTMeshTypes>;
#endif

  // This works fine (because it is a variable declaration):
  // NewLaserMaterialDataBase<LRT_namespace::LRTMeshTypes> X;
   }

// This is a better place and yet we preserve the original bug (which failed to unparse name qualification for the template arguments).
template class NewLaserMaterialDataBase<LRT_namespace::LRTMeshTypes>;
