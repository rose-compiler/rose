namespace internal {

// Redundant nested namespace causes a problem in ROSE...
namespace internal {}

class GeneratedExtensionFinder
   {
     public:
          void Find();
   };

namespace 
   {
     const int FindRegisteredExtension();
   }  // namespace

void GeneratedExtensionFinder::Find() 
   {
  // We need to do the name qualification based on the structural position (set of scopes) 
  // of this function instead of the semantic position (set of scopes). 
     const int extension = FindRegisteredExtension();
   }

}  // namespace internal

