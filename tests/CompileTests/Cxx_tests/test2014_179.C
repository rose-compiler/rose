
// namespace google {
// namespace protobuf {
namespace internal {

// Redundant nested namespace
namespace internal {}


}
// }
// }

namespace internal {

class GeneratedExtensionFinder
   {
     public:
          bool Find(int number);
   };
}


namespace internal {

namespace {

const int FindRegisteredExtension(int number) 
   {
     return 0;
   }
}  // namespace

bool GeneratedExtensionFinder::Find(int number) 
   {
  // We need to do the name qualification based on the structural position (set of scopes) 
  // of this function instead of the semantic position (set of scopes). 
     const int extension = FindRegisteredExtension(number);

     return false;
   }

}  // namespace internal

