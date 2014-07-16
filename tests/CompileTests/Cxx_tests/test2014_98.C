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
  // BUG: This is being unparsed as: "template class NewLaserMaterialDataBase < LRTMeshTypes >;"
     template class NewLaserMaterialDataBase<LRT_namespace::LRTMeshTypes>;

  // This works fine:
  // NewLaserMaterialDataBase<LRT_namespace::LRTMeshTypes> X;
   }
