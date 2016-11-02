
// This test code does not work on older compilers (versions of g++ 4.2.4 or less at least).
#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ >= 4) )

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

#endif
